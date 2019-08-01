#include "ipc.h"

#include "debug.h"
#include "nx/counter.h"

u64 g_start_log_timer_s = 0;

Result OpenLocationResolver(void* _this, void** out, FsStorageId storage_id) {
    if (armTicksToNs(armGetSystemTick()) / 1e+9 > g_start_log_timer_s + 3) {
        flush_debug_log();
    }

    return OpenLocationResolverImpl(((u8*)_this) + 0x8, out, storage_id);
}