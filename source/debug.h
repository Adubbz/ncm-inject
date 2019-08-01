#pragma once

#include "nx/fs.h"

extern FsFileSystem g_nand_fs;

void debug_log(const char* msg);
void flush_debug_log(void);