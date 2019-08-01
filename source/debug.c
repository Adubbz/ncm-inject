#include <string.h>

#include "debug.h"

#include "utils/fatal.h"
#include "utils/result.h"
#include "ncm.h"

#define TMP_LOG_MAX 0x10000

char tmp_debug_log[TMP_LOG_MAX] = {0};

void debug_log(const char* msg) {
    size_t cur_tmp_log_size = strlen(tmp_debug_log);
    strncat(tmp_debug_log, msg, TMP_LOG_MAX-cur_tmp_log_size);
}

void flush_debug_log(void) {
    Result rc = 0;
    FileHandle file;
    size_t tmp_log_sz = strlen(tmp_debug_log);

    if (tmp_log_sz == 0) {
        fatal_abort(Fatal_BadLogMessage);
    }

    if (R_FAILED(rc = OpenFile(&file, "dbg:/cmds.log", FsOpenMode_Write | FsOpenMode_Append))) {
        fatal_abort(Fatal_MountFailed);
    }

    size_t file_size = 0;

    if (R_FAILED(rc = GetFileSize(&file_size, file))) {
        CloseFile(file);
        fatal_abort(Fatal_GetFileSizeFailed);
    }

    if (R_FAILED(rc = WriteFile(file, file_size, tmp_debug_log, tmp_log_sz, FsWriteOption_None))) {
        //CloseFile(file);
        fatal_abort(rc);
    }

    CloseFile(file);

    memset(tmp_debug_log, '\0', TMP_LOG_MAX);
}