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
#include "nx/counter.h"
#include "nx/fs.h"
#include "nx/svc.h"
#include "nx/sm.h"
#include "nx/smc.h"
#include "utils/fatal.h"
#include "utils/result.h"

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
    Result rc = 0;

    if (R_FAILED(smInitialize())) {
        fatal_abort(Fatal_SmInitFailed);
    }

    if (R_FAILED(fsInitialize())) {
        fatal_abort(Fatal_FsInitFailed);
    }

    if (R_FAILED(fsOpenContentStorageFileSystem(&g_nand_fs, FS_CONTENTSTORAGEID_NandSystem))) {
        fatal_abort(Fatal_LogMountFailed);
    }

    fsFsDeleteFile(&g_nand_fs, "/cmds.log");
    if (R_FAILED(rc = fsFsCreateFile(&g_nand_fs, "/cmds.log", 0, 0)) && rc != 0x402) {
        fatal_abort(Fatal_CreateLogFailed);
    }

    debug_log("test1\n");
    debug_log("test2\n");
}

void setup_hooks(void)
{
    // rtld
    INJECT_HOOK_RELATIVE(NCM_OFFSET_810_RTLD, NCM_OFFSET_810_RTLD_DESTINATION);
    
    // content manager creation hook
    INJECT_HOOK(0x35e58, on_content_manager_created);

    // location resolver manager command hooks
    INJECT_HOOK(0x9820, OpenLocationResolver);
}

void populate_function_pointers(void)
{
    OpenLocationResolverImpl = INJECT_OFFSET(open_location_resolver_impl_t, 0x2b030);
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
