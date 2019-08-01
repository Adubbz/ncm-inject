#include "utils/types.h"

#pragma once

typedef enum {
    ServiceType_Uninitialized,      ///< Uninitialized service.
    ServiceType_Normal,             ///< Normal service.
    ServiceType_Domain,             ///< Domain.
    ServiceType_DomainSubservice,   ///< Domain subservice;
    ServiceType_Override,           ///< Service overriden in the homebrew environment.
} ServiceType;

typedef struct {
    Handle handle;
    u32 object_id;
    ServiceType type;
} Service;

typedef enum {
    ContentStorageId_NandSystem = 0,
    ContentStorageId_NandUser   = 1,
    ContentStorageId_SdCard     = 2,
} ContentStorageId;

typedef enum
{
    FsOpenMode_Read   = BIT(0), ///< Open for reading.
    FsOpenMode_Write  = BIT(1), ///< Open for writing.
    FsOpenMode_Append = BIT(2), ///< Append file.
} FsOpenMode;

typedef enum
{
    FsWriteOption_None  = 0,      ///< No option.
    FsWriteOption_Flush = BIT(0), ///< Forces a flush after write.
} FsWriteOption;

typedef struct {
    void* handle;
} FileHandle;

typedef enum
{
    FsStorageId_None       = 0,
    FsStorageId_Host       = 1,
    FsStorageId_GameCard   = 2,
    FsStorageId_NandSystem = 3,
    FsStorageId_NandUser   = 4,
    FsStorageId_SdCard     = 5,
} FsStorageId;

_Static_assert(sizeof(FileHandle) == 0x8, "FileHandle definition!");

// FS functions
typedef Result (*mount_content_storage_t)(const char* mount, ContentStorageId content_storage_id);
typedef Result (*create_file_t)(const char* path, size_t size);
typedef Result (*open_file_t)(FileHandle* out_file, const char* path, u32 open_mode);
typedef Result (*close_file_t)(FileHandle file);
typedef Result (*write_file_t)(FileHandle file, u64 offset, const void* buf, size_t size, u32 option);
typedef Result (*get_file_size_t)(u64* out_size, FileHandle file);

// impl functions
typedef Result (*open_location_resolver_impl_t)(void* _this, void** out, u8 storage_id);

// FS functions
extern mount_content_storage_t MountContentStorage;
extern create_file_t CreateFile;
extern open_file_t OpenFile;
extern close_file_t CloseFile;
extern write_file_t WriteFile;
extern get_file_size_t GetFileSize;

// impl functions
extern open_location_resolver_impl_t OpenLocationResolverImpl;