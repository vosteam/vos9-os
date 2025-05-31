#include "../include/memory.h"
#include "../include/vga.h"
#include "../include/string.h"

static mem_block_t *heap_start = NULL;
static size_t total_memory = HEAP_SIZE;
static size_t used_memory = 0;

void memory_init(void) {
    vga_puts("Initializing memory manager...\n");
    
    // Initialize the heap
    heap_start = (mem_block_t*)HEAP_START;
    heap_start->address = (void*)(HEAP_START + sizeof(mem_block_t));
    heap_start->size = HEAP_SIZE - sizeof(mem_block_t);
    heap_start->free = 1;
    heap_start->next = NULL;
    
    vga_puts("Memory: Heap initialized at 0x");
    vga_put_hex(HEAP_START);
    vga_puts(", size: ");
    vga_put_dec(HEAP_SIZE / 1024);
    vga_puts(" KB\n");
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    
    mem_block_t *current = heap_start;
    
    while (current) {
        if (current->free && current->size >= size) {
            // Split block if it's much larger than needed
            if (current->size > size + sizeof(mem_block_t) + 16) {
                mem_block_t *new_block = (mem_block_t*)((char*)current->address + size);
                new_block->address = (char*)new_block + sizeof(mem_block_t);
                new_block->size = current->size - size - sizeof(mem_block_t);
                new_block->free = 1;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            
            current->free = 0;
            used_memory += current->size;
            return current->address;
        }
        current = current->next;
    }
    
    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;
    
    mem_block_t *current = heap_start;
    
    while (current) {
        if (current->address == ptr) {
            current->free = 1;
            used_memory -= current->size;
            
            // Merge with next block if it's free
            if (current->next && current->next->free) {
                current->size += current->next->size + sizeof(mem_block_t);
                current->next = current->next->next;
            }
            
            return;
        }
        current = current->next;
    }
}

void memory_print_stats(void) {
    vga_puts("Memory Statistics:\n");
    vga_puts("  Total: ");
    vga_put_dec(total_memory / 1024);
    vga_puts(" KB\n");
    vga_puts("  Used: ");
    vga_put_dec(used_memory / 1024);
    vga_puts(" KB\n");
    vga_puts("  Free: ");
    vga_put_dec((total_memory - used_memory) / 1024);
    vga_puts(" KB\n");
}

size_t memory_get_free(void) {
    return total_memory - used_memory;
}

size_t memory_get_used(void) {
    return used_memory;
}

uint32_t memory_get_heap_start(void) {
    return HEAP_START;
}

uint32_t memory_get_heap_end(void) {
    return HEAP_START + HEAP_SIZE;
}
