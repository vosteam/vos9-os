#ifndef KERNEL_H
#define KERNEL_H

// Basic type definitions
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t; // In 32-bit, size_t is typically uint32_t

// Signed type definitions
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

#define NULL ((void*)0)

// Multiboot constants
#define MULTIBOOT_HEADER_MAGIC         0x1BADB002 // Magic number for the Multiboot header
#define MULTIBOOT_BOOTLOADER_MAGIC   0x2BADB002 // Magic number passed by the bootloader in eax

// VGA parameters
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// VGA colors
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
};

// Legacy kernel functions (for backward compatibility)
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color);
void terminal_clear(void);

// I/O ports
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Multiboot header constants
#define MULTIBOOT_MAGIC 0x1BADB002
#define MULTIBOOT_FLAGS 0x00000003

// Multiboot information structure
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    // Symbol table info (ELF format)
    struct {
        uint32_t num;
        uint32_t size;
        uint32_t addr;
        uint32_t shndx;
    } elf_sec;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    // Drive info, config table, boot loader name, APM table, VBE info can be added here
};

// Kernel functions
void kernel_main(uint32_t magic, struct multiboot_info* mbi);
void delay(uint32_t count);

#endif
