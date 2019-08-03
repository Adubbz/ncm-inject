// Copyright 2018 plutoo
#include "../types.h"
#include "../result.h"
#include "svc.h"
#include "condvar.h"
#include "../internal.h"

Result condvarWaitTimeout(CondVar* c, Mutex* m, u64 timeout) {
    Result rc;

    rc = svcWaitProcessWideKeyAtomic((u32*)m, c, getThreadVars()->handle, timeout);

    // On timeout, we need to acquire it manually.
    if (rc == 0xEA01)
        mutexLock(m);

    return rc;
}
