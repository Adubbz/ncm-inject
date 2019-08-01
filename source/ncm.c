#include "ncm.h"

open_location_resolver_impl_t OpenLocationResolverImpl;

Result OpenLocationResolver(void* _this, void** out, FsStorageId storage_id) {
    return OpenLocationResolverImpl(((u8*)_this) + 0x8, out, storage_id);
}