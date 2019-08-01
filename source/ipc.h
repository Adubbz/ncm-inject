#pragma once

#include "ncm.h"

extern u64 g_start_log_timer_s;

Result OpenLocationResolver(void* _this, void** out, FsStorageId storage_id);