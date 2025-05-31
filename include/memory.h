#ifndef MEMORY_H
#define MEMORY_H

#include "kernel.h"

// Memory management constants
#define PAGE_SIZE           4096
#define HEAP_START          0x100000  // 1MB
#define HEAP_SIZE           0x400000  // 4MB
#define MAX_ALLOCATIONS     1024

// Memory block structure
typedef struct mem_block {
    void *address;
    size_t size;
    int free;
    struct mem_block *next;
} mem_block_t;

// Function declarations
void memory_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);
void memory_print_stats(void);
size_t memory_get_free(void);
size_t memory_get_used(void);
uint32_t memory_get_heap_start(void);
uint32_t memory_get_heap_end(void);

#endif
