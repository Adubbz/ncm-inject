#pragma once

#include "debug.h"
#include "nx/services/fs.h"
#include "nx/types.h"

// lr manager functions
Result LocationResolverManagerImpl__OpenLocationResolver(void* _this, void** out, FsStorageId storage_id);

Result LocationResolverManagerImpl__OpenLocationResolverResult(Result rc, ...);
Result LocationResolverManagerImpl__OpenRegisteredLocationResolverResult(Result rc, ...);
Result LocationResolverManagerImpl__RefreshLocationResolverResult(Result rc, ...);
Result LocationResolverManagerImpl__OpenAddOnContentLocationResolverResult(Result rc, ...);

// registered location resolver functions
Result RegisteredLocationResolverImpl__ResolveProgramPathResult(Result rc, ...);
Result RegisteredLocationResolverImpl__RegisterProgramPathResult(Result rc, ...);
Result RegisteredLocationResolverImpl__UnregisterProgramPathResult(Result rc, ...);
Result RegisteredLocationResolverImpl__RedirectProgramPathResult(Result rc, ...);
Result RegisteredLocationResolverImpl__ResolveHtmlDocumentPathResult(Result rc, ...);
Result RegisteredLocationResolverImpl__RegisterHtmlDocumentPathResult(Result rc, ...);
Result RegisteredLocationResolverImpl__UnregisterHtmlDocumentPathResult(Result rc, ...);
Result RegisteredLocationResolverImpl__RedirectHtmlDocumentPathResult(Result rc, ...);
Result RegisteredLocationResolverImpl__RefreshResult(Result rc, ...);

// content location resolver functions
Result ContentLocationResolverImpl__ResolveProgramPath(void* _this, char* out, u64 title_id);
Result ContentLocationResolverImpl__ResolveDataPath(void* _this, char* out, u64 title_id);

Result ContentLocationResolverImpl__ResolveProgramPathResult(Result rc, ...);
Result ContentLocationResolverImpl__RedirectProgramPathResult(Result rc, ...);
Result ContentLocationResolverImpl__ResolveApplicationControlPathResult(Result rc, ...);
Result ContentLocationResolverImpl__ResolveApplicationHtmlDocumentPathResult(Result rc, ...);
Result ContentLocationResolverImpl__ResolveDataPathResult(Result rc, ...);
Result ContentLocationResolverImpl__RedirectApplicationControlPathResult(Result rc, ...);
Result ContentLocationResolverImpl__RedirectApplicationHtmlDocumentPathResult(Result rc, ...);
Result ContentLocationResolverImpl__ResolveApplicationLegalInformationPathResult(Result rc, ...);
Result ContentLocationResolverImpl__RedirectApplicationLegalInformationPathResult(Result rc, ...);
Result ContentLocationResolverImpl__RefreshResult(Result rc, ...);
Result ContentLocationResolverImpl__RedirectApplicationProgramPathResult(Result rc, ...);
Result ContentLocationResolverImpl__ClearApplicationRedirectionResult(Result rc, ...);
Result ContentLocationResolverImpl__EraseProgramRedirectionResult(Result rc, ...);
Result ContentLocationResolverImpl__EraseApplicationControlRedirectionResult(Result rc, ...);
Result ContentLocationResolverImpl__EraseApplicationHtmlDocumentRedirectionResult(Result rc, ...);
Result ContentLocationResolverImpl__EraseApplicationLegalInformationRedirectionResult(Result rc, ...);
Result ContentLocationResolverImpl__ResolveProgramPathForDebugResult(Result rc, ...);
Result ContentLocationResolverImpl__RedirectProgramPathForDebugResult(Result rc, ...);
Result ContentLocationResolverImpl__RedirectApplicationProgramPathForDebugResult(Result rc, ...);
Result ContentLocationResolverImpl__EraseProgramRedirectionForDebugResult(Result rc, ...);

// impl functions
typedef Result (*open_location_resolver_impl_t)(void* _this, void** out, u8 storage_id);
extern open_location_resolver_impl_t OpenLocationResolverImpl;

// content location resolver functions
typedef Result (*resolve_program_path_impl_t)(void* _this, char* out, u64 title_id);
typedef Result (*resolve_data_path_impl_t)(void* _this, char* out, u64 title_id);

extern resolve_program_path_impl_t ResolveProgramPathImpl;
extern resolve_data_path_impl_t ResolveDataPathImpl;