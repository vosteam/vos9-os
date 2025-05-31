#include "../../include/kernel.h"
#include "../../include/vga.h"
#include "../../include/keyboard.h"
#include "../../include/mouse.h"
#include "../../include/string.h"
#include "../../include/memory.h"
#include "../../include/filesystem.h"
#include "../../include/drivers/sata.h"
#include "../../include/drivers/timer.h"
#include "../../include/shell.h"

// Simple delay function
void delay(uint32_t count) {
    for (volatile uint32_t i = 0; i < count; i++) {
        // Empty loop for delay
    }
}

// Demo function to show cursor movement capabilities
void cursor_demo(void) {
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_puts("\n=== Cursor Movement Demo ===\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Available controls:\n");
    vga_puts("- Arrow Keys: Move cursor up/down/left/right\n");
    vga_puts("- Backspace: Delete character and move cursor left\n");
    vga_puts("- Regular keys: Type characters\n\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("Try moving the cursor with arrow keys!\n");
    vga_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    vga_puts("Type some text and use backspace to edit.\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

// Main kernel function
void kernel_main(uint32_t magic, struct multiboot_info* mbi) {
    // Initialize drivers and subsystems
    vga_init();
    keyboard_init();
    // mouse_init();       // Mouse disabled for DOS-like experience
    memory_init();
    timer_init(1000);  // Initialize timer with 1000 Hz frequency
    filesystem_init();
    
    // Initialize SATA controller
    sata_init();
    
    // Display DOS-style boot message
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_clear();
    vga_puts("Starting MyOS-DOS...\n\n");
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("MyOS-DOS Version 1.0\n");
    vga_puts("Copyright (C) 2025 MyOS Corporation\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("====================================\n\n");
    
    // Check if we were loaded by a multiboot compliant bootloader
    if (magic == MULTIBOOT_BOOTLOADER_MAGIC) { // Use MULTIBOOT_BOOTLOADER_MAGIC
        vga_set_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
        vga_puts("Loaded by multiboot compliant bootloader\n");
        
        // Display memory information if available
        if (mbi && (mbi->flags & (1 << 0))) { // Check if mbi is not null and flags bit 0 is set
            vga_puts("Memory info: Lower mem: ");
            vga_put_dec(mbi->mem_lower); // Use vga_put_dec
            vga_puts(" KB, Upper mem: ");
            vga_put_dec(mbi->mem_upper); // Use vga_put_dec
            vga_puts(" KB\n");
        }
        if (mbi && (mbi->flags & (1 << 6))) { // Check if mbi is not null and flags bit 6 is set for mmap
            vga_puts("Memory map available (length: ");
            vga_put_dec(mbi->mmap_length);
            vga_puts(", address: ");
            vga_put_hex(mbi->mmap_addr);
            vga_puts(")\n");
            // Further parsing of mmap can be added here
        }

        vga_puts("\n");
    } else {
        vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
        vga_puts("Warning: Not loaded by multiboot bootloader (magic: ");
        vga_put_hex(magic); // Use vga_put_hex
        vga_puts(")\nExpected: ");
        vga_put_hex(MULTIBOOT_BOOTLOADER_MAGIC); // Use vga_put_hex
        vga_puts("\n\n");
    }
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Welcome to MyOS!\n");
    vga_puts("This is a simple operating system kernel.\n\n");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("Features:\n");
    vga_puts("- Modular architecture\n");
    vga_puts("- VGA text mode driver\n");
    vga_puts("- Keyboard input driver\n");
    vga_puts("- Mouse input driver\n");
    vga_puts("- Dynamic memory management\n");
    vga_puts("- SATA/AHCI disk support\n");
    vga_puts("- System timer (PIT)\n");
    vga_puts("- Simple filesystem\n");
    vga_puts("- Basic library functions\n");
    vga_puts("- Protected mode operation\n\n");
    
    // Display system status
    vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga_puts("System Status:\n");
    
    // Check SATA devices
    uint32_t sata_count = sata_detect_drives();
    vga_puts("SATA devices detected: ");
    vga_put_dec(sata_count);
    vga_puts("\n");
    
    // Display memory info
    vga_puts("Memory heap initialized: ");
    vga_put_hex(memory_get_heap_start());
    vga_puts(" - ");
    vga_put_hex(memory_get_heap_end());
    vga_puts("\n");
    
    // Display uptime
    vga_puts("System uptime: ");
    vga_put_dec(timer_get_uptime_seconds());
    vga_puts(" seconds\n\n");
    
    vga_set_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    vga_puts("Filesystem Test:\n");
    
    // Create a test file
    int result = fs_create_file("test.txt", 0);
    if (result == 0) {
        vga_puts("Created file: test.txt\n");
        
        // Open file for writing
        file_handle_t* handle = fs_open_file("test.txt", "w");
        if (handle) {
            // Write to file
            const char* test_data = "Hello from MyOS filesystem!";
            fs_write_file(handle, test_data, strlen(test_data));
            vga_puts("Wrote data to file\n");
            
            fs_close_file(handle);
        }
        
        // List files
        vga_puts("Files in filesystem:\n");
        fs_list_files();
    }
    vga_puts("\n");
    
    // Show cursor demo
    cursor_demo();
    
    // Initialize and start the shell
    shell_init();
    
    // Main kernel loop with shell
    while (1) {
        // Handle shell input and commands
        shell_run();
        
        // Small delay to reduce CPU usage
        delay(100);
    }
}
