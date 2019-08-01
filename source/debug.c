#include "debug.h"

#include "utils/result.h"
#include "ncm.h"

void log(const char* msg) {
    size_t msg_len = strlen(msg);
    FsFile file;

    if (R_FAILED(OpenFile(&file, "dbg:/cmds.log", FsOpenMode_Write | FsOpenMode_Append))) {
        return;
    }

    size_t file_size = 0;

    if (R_FAILED(GetFileSize(&file_size, file))) {
        CloseFile(file);
        return;
    }

    WriteFile(file, file_size, msg, msg_len, FsWriteOption_Flush);
    CloseFile(file);
}