#include "../include/filesystem.h"
#include "../include/memory.h"
#include "../include/vga.h"
#include "../include/string.h"

static file_entry_t file_table[MAX_FILES];
static file_handle_t file_handles[MAX_FILES];
static int fs_initialized = 0;

int filesystem_init(void) {
    vga_puts("Initializing file system...\n");
    
    // Clear file table
    for (int i = 0; i < MAX_FILES; i++) {
        file_table[i].used = 0;
        file_handles[i].open = 0;
    }
    
    // Create some default files
    fs_create_file("readme.txt", FILE_ATTR_READ | FILE_ATTR_WRITE);
    fs_create_file("system.log", FILE_ATTR_READ | FILE_ATTR_WRITE);
    fs_create_file("config.sys", FILE_ATTR_READ | FILE_ATTR_WRITE);
    
    fs_initialized = 1;
    vga_puts("File system initialized with ");
    vga_put_dec(MAX_FILES);
    vga_puts(" file slots\n");
    
    return 1;
}

int fs_create_file(const char *name, uint32_t attributes) {
    if (!fs_initialized) return 0;
    
    // Find free slot
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].used) {
            strncpy(file_table[i].name, name, MAX_FILENAME - 1);
            file_table[i].name[MAX_FILENAME - 1] = '\0';
            file_table[i].size = 0;
            file_table[i].attributes = attributes;
            file_table[i].sector = 100 + i; // Simple sector allocation
            file_table[i].used = 1;
            return 1;
        }
    }
    
    return 0; // No free slots
}

int fs_delete_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            file_table[i].used = 0;
            return 1;
        }
    }
    return 0;
}

file_handle_t* fs_open_file(const char *name, const char *mode) {
    // Find file
    file_entry_t *file = NULL;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            file = &file_table[i];
            break;
        }
    }
    
    if (!file) return NULL;
    
    // Find free handle
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_handles[i].open) {
            file_handles[i].file = file;
            file_handles[i].position = 0;
            file_handles[i].open = 1;
            return &file_handles[i];
        }
    }
    
    return NULL;
}

void fs_close_file(file_handle_t *handle) {
    if (handle) {
        handle->open = 0;
    }
}

int fs_read_file(file_handle_t *handle, void *buffer, uint32_t size) {
    if (!handle || !handle->open) return 0;
    
    uint32_t bytes_to_read = size;
    if (handle->position + size > handle->file->size) {
        bytes_to_read = handle->file->size - handle->position;
    }
    
    // Simulate reading from storage
    // In real implementation, this would read from SATA drive
    
    handle->position += bytes_to_read;
    return bytes_to_read;
}

int fs_write_file(file_handle_t *handle, const void *buffer, uint32_t size) {
    if (!handle || !handle->open) return 0;
    if (!(handle->file->attributes & FILE_ATTR_WRITE)) return 0;
    
    // Simulate writing to storage
    handle->file->size = handle->position + size;
    handle->position += size;
    
    return size;
}

void fs_list_files(void) {
    vga_puts("File listing:\n");
    vga_puts("Name                Size    Attributes\n");
    vga_puts("--------------------------------\n");
    
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].used) {
            vga_puts(file_table[i].name);
            
            // Pad to column
            int len = strlen(file_table[i].name);
            for (int j = len; j < 20; j++) {
                vga_putchar(' ');
            }
            
            vga_put_dec(file_table[i].size);
            vga_puts("    ");
            
            if (file_table[i].attributes & FILE_ATTR_READ) vga_putchar('R');
            if (file_table[i].attributes & FILE_ATTR_WRITE) vga_putchar('W');
            if (file_table[i].attributes & FILE_ATTR_EXEC) vga_putchar('X');
            if (file_table[i].attributes & FILE_ATTR_DIR) vga_putchar('D');
            
            vga_putchar('\n');
        }
    }
}

int fs_file_exists(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}
