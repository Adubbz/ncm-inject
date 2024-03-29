/**
 * @file wait.h
 * @brief User mode synchronization primitive waiting operations.
 * @author plutoo
 * @copyright libnx Authors
 */
#pragma once
#include "mutex.h"
#include "svc.h"

// Implementation details.

typedef struct Waitable Waitable;
typedef struct WaitableMethods WaitableMethods;
typedef struct WaitableNode WaitableNode;

struct WaitableNode {
    WaitableNode* prev;
    WaitableNode* next;
};

struct Waitable {
    const WaitableMethods* vt;
    WaitableNode list;
    Mutex mutex;
};

typedef enum {
    WaiterType_Handle,
    WaiterType_HandleWithClear,
    WaiterType_Waitable,
} WaiterType;

// User-facing API starts here.

/// Waiter structure, representing any generic waitable synchronization object; both kernel-mode and user-mode.
typedef struct {
    WaiterType type;

    union {
        Handle handle;
        Waitable* waitable;
    };
} Waiter;

/// Creates a \ref Waiter for a kernel-mode \ref Handle.
static inline Waiter waiterForHandle(Handle h)
{
    Waiter wait_obj;
    wait_obj.type = WaiterType_Handle;
    wait_obj.handle = h;
    return wait_obj;
}

/**
 * @brief Waits for an arbitrary number of generic waitable synchronization objects, optionally with a timeout.
 * @param[out] idx_out Variable that will received the index of the signalled object.
 * @param[in] objects Array containing \ref Waiter structures.
 * @param[in] num_objects Number of objects in the array.
 * @param[in] timeout Timeout (in nanoseconds).
 * @return Result code.
 * @note The number of objects must not be greater than \ref MAX_WAIT_OBJECTS. This is a Horizon kernel limitation.
 */
Result waitObjects(s32* idx_out, const Waiter* objects, s32 num_objects, u64 timeout);

/**
 * @brief Waits for an arbitrary number of kernel synchronization objects, optionally with a timeout. This function replaces \ref svcWaitSynchronization.
 * @param[out] idx_out Variable that will received the index of the signalled object.
 * @param[in] handles Array containing handles.
 * @param[in] num_handles Number of handles in the array.
 * @param[in] timeout Timeout (in nanoseconds).
 * @return Result code.
 * @note The number of objects must not be greater than \ref MAX_WAIT_OBJECTS. This is a Horizon kernel limitation.
 */
Result waitHandles(s32* idx_out, const Handle* handles, s32 num_handles, u64 timeout);

/**
 * @brief Helper macro for \ref waitObjects that accepts \ref Waiter structures as variadic arguments instead of as an array.
 * @param[out] idx_out The index of the signalled waiter.
 * @param[in] timeout Timeout (in nanoseconds).
 * @note The number of objects must not be greater than \ref MAX_WAIT_OBJECTS. This is a Horizon kernel limitation.
 */
#define waitMulti(idx_out, timeout, ...) ({ \
    Waiter __objects[] = { __VA_ARGS__ }; \
    waitObjects((idx_out), __objects, sizeof(__objects) / sizeof(Waiter), (timeout)); \
})

/**
 * @brief Helper macro for \ref waitHandles that accepts handles as variadic arguments instead of as an array.
 * @param[out] idx_out The index of the signalled handle.
 * @param[in] timeout Timeout (in nanoseconds).
 * @note The number of objects must not be greater than \ref MAX_WAIT_OBJECTS. This is a Horizon kernel limitation.
 */
#define waitMultiHandle(idx_out, timeout, ...) ({ \
    Handle __handles[] = { __VA_ARGS__ }; \
    waitHandles((idx_out), __handles, sizeof(__handles) / sizeof(Handle), (timeout)); \
})

/**
 * @brief Waits on a single generic waitable synchronization object, optionally with a timeout.
 * @param[in] w \ref Waiter structure.
 * @param[in] timeout Timeout (in nanoseconds).
 */
static inline Result waitSingle(Waiter w, u64 timeout)
{
    s32 idx;
    return waitObjects(&idx, &w, 1, timeout);
}

/**
 * @brief Waits for a single kernel synchronization object, optionally with a timeout.
 * @param[in] h \ref Handle of the object.
 * @param[in] timeout Timeout (in nanoseconds).
 */
static inline Result waitSingleHandle(Handle h, u64 timeout)
{
    s32 idx;
    return waitHandles(&idx, &h, 1, timeout);
}

typedef struct WaiterNode WaiterNode;

struct WaiterNode {
    WaitableNode node;
    Waitable* parent;
    Handle thread;
    s32* idx_out;
    s32 idx;
};

struct WaitableMethods {
    bool (* beginWait)(Waitable* ww, WaiterNode* w, u64 cur_tick, u64* next_tick);
    Result (* onTimeout)(Waitable* ww, u64 old_tick);
    Result (* onSignal)(Waitable* ww);
};

static inline void _waitableInitialize(Waitable* ww, const WaitableMethods* vt)
{
    mutexInit(&ww->mutex);
    ww->vt = vt;
    ww->list.next = &ww->list;
    ww->list.prev = &ww->list;
}

static inline void _waitableSignalAllListeners(Waitable* ww)
{
    WaitableNode* node = &ww->list;
    WaitableNode* end = node;

    while (node->next != end) {
        node = node->next;
        WaiterNode* w = (WaiterNode*) node;

        // Try to swap -1 => idx on the waiter thread.
        // If another waitable signals simultaneously only one will win the race and insert its own idx.
        s32 minus_one = -1;
        bool sent_idx = __atomic_compare_exchange_n(
            w->idx_out, &minus_one, w->idx, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

        if (sent_idx)
            svcCancelSynchronization(w->thread);
    }
}

static inline void _waiterNodeInitialize(
    WaiterNode* w, Waitable* parent, Handle thread,
    s32 idx, s32* idx_out)
{
    // Initialize WaiterNode fields
    w->parent = parent;
    w->thread = thread;
    w->idx = idx;
    w->idx_out = idx_out;
}

static inline void _waiterNodeAdd(WaiterNode* w)
{
    // Add WaiterNode to the parent's linked list
    w->node.next = w->parent->list.next;
    w->parent->list.next = &w->node;
    w->node.prev = &w->parent->list;
}

static inline void _waiterNodeRemove(WaiterNode* w)
{
    mutexLock(&w->parent->mutex);
    w->node.prev->next = w->node.next;
    w->node.next->prev = w->node.prev;
    mutexUnlock(&w->parent->mutex);
}