#pragma once

#include "nx/fs.h"
#include "utils/types.h"

Result OpenLocationResolver(void* _this, void** out, FsStorageId storage_id);

// impl functions
typedef Result (*open_location_resolver_impl_t)(void* _this, void** out, u8 storage_id);

extern open_location_resolver_impl_t OpenLocationResolverImpl;