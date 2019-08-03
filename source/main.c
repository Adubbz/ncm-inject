/*
 * Copyright (c) 2019 m4xw <m4x@m4xw.net>
 * Copyright (c) 2019 Atmosphere-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "ncm.h"
#include "nx/result.h"
#include "nx/smc.h"
#include "nx/arm/counter.h"
#include "nx/kernel/svc.h"
#include "nx/services/fs.h"
#include "nx/services/sm.h"
#include "utils/ams_fatal.h"

#define NCM_OFFSET_810_RTLD                0x5B4
#define NCM_OFFSET_810_RTLD_DESTINATION    0x9C

// Prototypes
void __init();
void __initheap(void);
void setup_hooks(void);
void __libc_init_array(void);
void hook_function(uintptr_t source, uintptr_t target);

void *__stack_top;
uintptr_t text_base;
char inner_heap[INNER_HEAP_SIZE];
size_t inner_heap_size = INNER_HEAP_SIZE;
extern char _start;
extern char __argdata__;

// Defined by linkerscript
#define INJECTED_SIZE ((uintptr_t)&__argdata__ - (uintptr_t)&_start)
#define INJECT_OFFSET(type, offset) (type)(text_base + INJECTED_SIZE + offset)

#define GENERATE_ADD(register, register_target, value) (0x91000000 | value << 10 | register << 5 | register_target)
#define GENERATE_ADRP(register, page_addr) (0x90000000 | ((((page_addr) >> 12) & 0x3) << 29) | ((((page_addr) >> 12) & 0x1FFFFC) << 3) | ((register) & 0x1F))
#define GENERATE_BRANCH(source, destination) (0x14000000 | ((((destination) - (source)) >> 2) & 0x3FFFFFF))
#define GENERATE_NOP() (0xD503201F)

#define INJECT_HOOK(offset, destination) hook_function(INJECT_OFFSET(uintptr_t, offset), (uintptr_t)&destination)
#define INJECT_HOOK_RELATIVE(offset, relative_destination) hook_function(INJECT_OFFSET(uintptr_t, offset), INJECT_OFFSET(uintptr_t, offset) + relative_destination)
#define INJECT_NOP(offset) write_nop(INJECT_OFFSET(uintptr_t, offset))

void __initheap(void)
{
    void *addr = inner_heap;
    size_t size = inner_heap_size;

    /* Newlib Heap Management */
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    fake_heap_start = (char *)addr;
    fake_heap_end = (char *)addr + size;
}

void hook_function(uintptr_t source, uintptr_t target)
{
    u32 branch_opcode = GENERATE_BRANCH(source, target);
    smcWriteAddress32((void *)source, branch_opcode);
}

void write_nop(uintptr_t source)
{
    smcWriteAddress32((void *)source, GENERATE_NOP());
}

void write_adrp_add(int reg, uintptr_t pc, uintptr_t add_rel_offset, intptr_t destination)
{
    uintptr_t add_opcode_location = pc + add_rel_offset;

    intptr_t offset = (destination & 0xFFFFF000) - (pc & 0xFFFFF000);
    uint32_t opcode_adrp = GENERATE_ADRP(reg, offset);
    uint32_t opcode_add = GENERATE_ADD(reg, reg, (destination & 0x00000FFF));

    smcWriteAddress32((void *)pc, opcode_adrp);
    smcWriteAddress32((void *)add_opcode_location, opcode_add);
}

void on_content_manager_created(void)
{
    if (R_FAILED(smInitialize())) {
        fatal_abort(Fatal_SmInitFailed);
    }

    if (R_FAILED(fsInitialize())) {
        fatal_abort(Fatal_FsInitFailed);
    }

    clear_log();
}

void setup_hooks(void)
{
    // rtld
    INJECT_HOOK_RELATIVE(NCM_OFFSET_810_RTLD, NCM_OFFSET_810_RTLD_DESTINATION);
    
    // content manager creation hook
    INJECT_HOOK(0x35e58, on_content_manager_created);

    // location resolver manager command hooks
    INJECT_HOOK(0x9820, LocationResolverManagerImpl__OpenLocationResolver);

    // content location resolver command hooks
    INJECT_HOOK(0x2bca0, ContentLocationResolverImpl__ResolveProgramPath);
    INJECT_HOOK(0x2bd20, ContentLocationResolverImpl__ResolveDataPath);

    // location resolver manager result hooks
    INJECT_HOOK(0x987c, LocationResolverManagerImpl__OpenRegisteredLocationResolverResult);
    INJECT_HOOK(0x9898, LocationResolverManagerImpl__RefreshLocationResolverResult);
    INJECT_HOOK(0x98cc, LocationResolverManagerImpl__OpenAddOnContentLocationResolverResult);

    // registered location resolver hooks
    INJECT_HOOK(0x2bfb8, RegisteredLocationResolverImpl__ResolveProgramPathResult);
    INJECT_HOOK(0x2bfd8, RegisteredLocationResolverImpl__RegisterProgramPathResult);
    INJECT_HOOK(0x2bff8, RegisteredLocationResolverImpl__UnregisterProgramPathResult);
    INJECT_HOOK(0x2c018, RegisteredLocationResolverImpl__RedirectProgramPathResult);
    INJECT_HOOK(0x2c038, RegisteredLocationResolverImpl__ResolveHtmlDocumentPathResult);
    INJECT_HOOK(0x2c058, RegisteredLocationResolverImpl__RegisterHtmlDocumentPathResult);
    INJECT_HOOK(0x2c078, RegisteredLocationResolverImpl__UnregisterHtmlDocumentPathResult);
    INJECT_HOOK(0x2c098, RegisteredLocationResolverImpl__RedirectHtmlDocumentPathResult);
    INJECT_HOOK(0x2c0b8, RegisteredLocationResolverImpl__RefreshResult);

    // content location resolver result hooks
    INJECT_HOOK(0x2bcb8, ContentLocationResolverImpl__ResolveProgramPathResult);
    INJECT_HOOK(0x2bcd8, ContentLocationResolverImpl__RedirectProgramPathResult);
    INJECT_HOOK(0x2bcf8, ContentLocationResolverImpl__ResolveApplicationControlPathResult);
    INJECT_HOOK(0x2bd18, ContentLocationResolverImpl__ResolveApplicationHtmlDocumentPathResult);
    INJECT_HOOK(0x2bd38, ContentLocationResolverImpl__ResolveDataPathResult);
    INJECT_HOOK(0x2bd58, ContentLocationResolverImpl__RedirectApplicationControlPathResult);
    INJECT_HOOK(0x2bd78, ContentLocationResolverImpl__RedirectApplicationHtmlDocumentPathResult);
    INJECT_HOOK(0x2bd98, ContentLocationResolverImpl__ResolveApplicationLegalInformationPathResult);
    INJECT_HOOK(0x2bdb8, ContentLocationResolverImpl__RedirectApplicationLegalInformationPathResult);
    INJECT_HOOK(0x2bdd8, ContentLocationResolverImpl__RefreshResult);
    INJECT_HOOK(0x2bdf8, ContentLocationResolverImpl__RedirectApplicationProgramPathResult);
    INJECT_HOOK(0x2be18, ContentLocationResolverImpl__ClearApplicationRedirectionResult);
    INJECT_HOOK(0x2be38, ContentLocationResolverImpl__EraseProgramRedirectionResult);
    INJECT_HOOK(0x2be58, ContentLocationResolverImpl__EraseApplicationControlRedirectionResult);
    INJECT_HOOK(0x2be78, ContentLocationResolverImpl__EraseApplicationHtmlDocumentRedirectionResult);
    INJECT_HOOK(0x2be98, ContentLocationResolverImpl__EraseApplicationLegalInformationRedirectionResult);
    INJECT_HOOK(0x2beb8, ContentLocationResolverImpl__ResolveProgramPathForDebugResult);
    INJECT_HOOK(0x2bed8, ContentLocationResolverImpl__RedirectProgramPathForDebugResult);
    INJECT_HOOK(0x2bef8, ContentLocationResolverImpl__RedirectApplicationProgramPathForDebugResult);
    INJECT_HOOK(0x2bf18, ContentLocationResolverImpl__EraseProgramRedirectionForDebugResult);
}

void populate_function_pointers(void)
{
    OpenLocationResolverImpl = INJECT_OFFSET(open_location_resolver_impl_t, 0x2b030);
    ResolveProgramPathImpl = INJECT_OFFSET(resolve_program_path_impl_t, 0x2c400);
    ResolveDataPathImpl = INJECT_OFFSET(resolve_data_path_impl_t, 0x2c620);
}

// inject main func
void __init()
{
    // Call constructors.
    __libc_init_array();

    MemoryInfo meminfo;
    u32 pageinfo;
    svcQueryMemory(&meminfo, &pageinfo, (u64)&_start);

    text_base = meminfo.addr;

    setup_hooks();
    populate_function_pointers();
}
