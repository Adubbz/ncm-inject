#include "ncm.h"

open_location_resolver_impl_t OpenLocationResolverImpl;

resolve_program_path_impl_t ResolveProgramPathImpl;
resolve_data_path_impl_t ResolveDataPathImpl;

Result LocationResolverManagerImpl__OpenLocationResolver(void* _this, void** out, FsStorageId storage_id) {
    Result rc = 0;
    rc = OpenLocationResolverImpl(((u8*)_this) + 0x8, out, storage_id);
    debug_log("%s -> 0x%x\n", "LocationResolverManagerImpl__OpenLocationResolver", rc);
    debug_log("    storage_id: 0x%x\n", storage_id);
    return rc;
}

Result ContentLocationResolverImpl__ResolveProgramPath(void* _this, char* out, u64 title_id) {
    Result rc = 0;
    rc = ResolveProgramPathImpl(((u8*)_this) + 0x8, out, title_id);
    debug_log("%s -> 0x%x\n", "ContentLocationResolverImpl__ResolveProgramPath", rc);
    debug_log("    title_id: 0x%lx\n", title_id);
    debug_log("    path: %s\n", out);
    return rc;
}


Result ContentLocationResolverImpl__ResolveDataPath(void* _this, char* out, u64 title_id) {
    Result rc = 0;
    rc = ResolveDataPathImpl(((u8*)_this) + 0x8, out, title_id);
    debug_log("%s -> 0x%x\n", "ContentLocationResolverImpl__ResolveDataPath", rc);
    debug_log("    title_id: 0x%lx\n", title_id);
    debug_log("    path: %s\n", out);
    return rc;
}

#define DEFINE_RESULT_LOG(name) \
    Result name##Result(Result rc, ...) { \
        debug_log("%s -> 0x%x\n", __FUNCTION__, rc); \
        return rc; \
    }

// lr manager functions
DEFINE_RESULT_LOG(LocationResolverManagerImpl__OpenLocationResolver)
DEFINE_RESULT_LOG(LocationResolverManagerImpl__OpenRegisteredLocationResolver)
DEFINE_RESULT_LOG(LocationResolverManagerImpl__RefreshLocationResolver)
DEFINE_RESULT_LOG(LocationResolverManagerImpl__OpenAddOnContentLocationResolver)

// registered location resolver functions
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__ResolveProgramPath)
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__RegisterProgramPath)
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__UnregisterProgramPath)
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__RedirectProgramPath)
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__ResolveHtmlDocumentPath)
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__RegisterHtmlDocumentPath)
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__UnregisterHtmlDocumentPath)
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__RedirectHtmlDocumentPath)
DEFINE_RESULT_LOG(RegisteredLocationResolverImpl__Refresh)

// content location resolver functions
DEFINE_RESULT_LOG(ContentLocationResolverImpl__ResolveProgramPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__RedirectProgramPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__ResolveApplicationControlPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__ResolveApplicationHtmlDocumentPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__ResolveDataPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__RedirectApplicationControlPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__RedirectApplicationHtmlDocumentPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__ResolveApplicationLegalInformationPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__RedirectApplicationLegalInformationPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__Refresh)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__RedirectApplicationProgramPath)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__ClearApplicationRedirection)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__EraseProgramRedirection)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__EraseApplicationControlRedirection)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__EraseApplicationHtmlDocumentRedirection)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__EraseApplicationLegalInformationRedirection)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__ResolveProgramPathForDebug)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__RedirectProgramPathForDebug)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__RedirectApplicationProgramPathForDebug)
DEFINE_RESULT_LOG(ContentLocationResolverImpl__EraseProgramRedirectionForDebug)