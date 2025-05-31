#include "../../include/kernel.h"
#include "../../include/vga.h"
#include "../../include/keyboard.h"
#include "../../include/string.h"
#include "../../include/memory.h"
#include "../../include/filesystem.h"
#include "../../include/drivers/timer.h"
#include "../../include/shell.h"

// Simple delay function
void delay(uint32_t count) {
    for (volatile uint32_t i = 0; i < count; i++) {
        // Empty loop for delay
    }
}

// Main kernel function
void kernel_main(uint32_t magic, struct multiboot_info* mbi) {
    // Initialize basic drivers
    vga_init();
    keyboard_init();
    memory_init();
    timer_init(1000);
    filesystem_init();
    
    // Clear screen and display DOS-style boot message
    vga_clear();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    // ASCII Art Logo
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("        ██╗   ██╗ ██████╗ ███████╗ █████╗       ██████╗  ██████╗ ███████╗\n");
    vga_puts("        ██║   ██║██╔═══██╗██╔════╝██╔══██╗      ██╔══██╗██╔═══██╗██╔════╝\n");
    vga_puts("        ██║   ██║██║   ██║███████╗╚██████║█████╗██║  ██║██║   ██║███████╗\n");
    vga_puts("        ╚██╗ ██╔╝██║   ██║╚════██║ ╚═══██║╚════╝██║  ██║██║   ██║╚════██║\n");
    vga_puts("         ╚████╔╝ ╚██████╔╝███████║ █████╔╝      ██████╔╝╚██████╔╝███████║\n");
    vga_puts("          ╚═══╝   ╚═════╝ ╚══════╝ ╚════╝       ╚═════╝  ╚═════╝ ╚══════╝\n");
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("                              Retro Operating System\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("                                 Version 1.0\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("                         © 2025 vosteam Organization\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    // DOS-style startup message
    vga_puts("Starting VOS9-DOS...\n");
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("VOS9-DOS Version 1.0\n");
    vga_puts("Copyright (C) 2025 vosteam Organization\n\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    // Check multiboot
    if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
        vga_puts("System loaded successfully.\n");
        if (mbi && (mbi->flags & (1 << 0))) {
            vga_puts("Memory: ");
            vga_put_dec(mbi->mem_lower + mbi->mem_upper);
            vga_puts(" KB available\n");
        }
    }
    
    vga_puts("\n");
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("Ready.\n\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    // Initialize and start the shell
    shell_init();
    
    // Main kernel loop
    while (1) {
        shell_run();
        delay(100);
    }
}
