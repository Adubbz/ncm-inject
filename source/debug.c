#include <string.h>

#include "debug.h"

#include "utils/fatal.h"
#include "utils/result.h"
#include "ncm.h"

FsFileSystem g_nand_fs;

void debug_log(const char* msg) {
    Result rc = 0;
    FsFile file;
    size_t msg_size = strlen(msg);

    if (msg == NULL || msg_size == 0) {
        fatal_abort(Fatal_BadLogMessage);
    }

    if (R_FAILED(fsFsOpenFile(&g_nand_fs, "/cmds.log", FS_OPEN_WRITE | FS_OPEN_APPEND, &file))) {
        fatal_abort(Fatal_OpenLogFailed);
    }

    size_t file_size = 0;

    if (R_FAILED(rc = fsFileGetSize(&file, &file_size))) {
        fsFileClose(&file);
        fatal_abort(Fatal_GetFileSizeFailed);
    }

    if (R_FAILED(rc = fsFileWrite(&file, file_size, msg, msg_size, FS_WRITEOPTION_FLUSH))) {
        fsFileClose(&file);
        fatal_abort(rc);
    }

    fsFileClose(&file);
}