#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "kernel.h"

// File system constants
#define MAX_FILES           64
#define MAX_FILENAME        32
#define MAX_FILE_SIZE       4096
#define FS_SECTOR_SIZE      512

// File attributes
#define FILE_ATTR_READ      0x01
#define FILE_ATTR_WRITE     0x02
#define FILE_ATTR_EXEC      0x04
#define FILE_ATTR_DIR       0x08

// File structure
typedef struct {
    char name[MAX_FILENAME];
    uint32_t size;
    uint32_t attributes;
    uint32_t sector;
    int used;
} file_entry_t;

// File handle
typedef struct {
    file_entry_t *file;
    uint32_t position;
    int open;
} file_handle_t;

// Function declarations
int filesystem_init(void);
int fs_create_file(const char *name, uint32_t attributes);
int fs_delete_file(const char *name);
file_handle_t* fs_open_file(const char *name, const char *mode);
void fs_close_file(file_handle_t *handle);
int fs_read_file(file_handle_t *handle, void *buffer, uint32_t size);
int fs_write_file(file_handle_t *handle, const void *buffer, uint32_t size);
void fs_list_files(void);
int fs_file_exists(const char *name);

#endif
