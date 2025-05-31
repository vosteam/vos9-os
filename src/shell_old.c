#include "../include/shell.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/string.h"
#include "../include/memory.h"
#include "../include/filesystem.h"
#include "../include/drivers/timer.h"
#include "../include/games.h"

// Forward declarations of command functions
void cmd_help(int argc, char* argv[]);
void cmd_cls(int argc, char* argv[]);
void cmd_echo(int argc, char* argv[]);
void cmd_time(int argc, char* argv[]);
void cmd_mem(int argc, char* argv[]);
void cmd_dir(int argc, char* argv[]);
void cmd_type(int argc, char* argv[]);
void cmd_copy(int argc, char* argv[]);
void cmd_del(int argc, char* argv[]);
void cmd_ver(int argc, char* argv[]);
void cmd_exit(int argc, char* argv[]);

static char shell_buffer[SHELL_BUFFER_SIZE];
static int shell_buffer_pos = 0;
static char* shell_argv[SHELL_MAX_ARGS];

// Command table - DOS style
static const shell_command_t commands[] = {
    {"help", "Display available commands", cmd_help},
    {"cls", "Clear the screen", cmd_cls},
    {"echo", "Display text", cmd_echo},
    {"time", "Display system time", cmd_time},
    {"mem", "Display memory information", cmd_mem},
    {"dir", "List files and directories", cmd_dir},
    {"type", "Display file contents", cmd_type},
    {"copy", "Copy files", cmd_copy},
    {"del", "Delete a file", cmd_del},
    {"ver", "Display version information", cmd_ver},
    {"exit", "Exit the system", cmd_exit},
    {NULL, NULL, NULL} // End marker
};

void shell_init(void) {
    shell_buffer_pos = 0;
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("C:\\>");
}

void shell_run(void) {
    // This will be called from the main kernel loop
    if (inb(0x64) & 0x01) { // Check if keyboard data is available
        key_event_t event = keyboard_get_event();
        
        if (event.type == KEY_EVENT_PRESS) {
            if (event.special) {
                switch (event.scancode) {
                    case KEY_BACKSPACE:
                        if (shell_buffer_pos > 0) {
                            shell_buffer_pos--;
                            shell_buffer[shell_buffer_pos] = '\0';
                            vga_backspace();
                        }
                        break;
                    // Ignore arrow keys for DOS-like experience
                    default:
                        break;
                }
            } else if (event.ascii != 0) {
                if (event.ascii == '\n') {
                    // Process command
                    vga_putchar('\n');
                    shell_buffer[shell_buffer_pos] = '\0';
                    if (shell_buffer_pos > 0) {
                        shell_process_input(shell_buffer);
                    }
                    shell_buffer_pos = 0;
                    vga_puts("C:\\>");
                } else if (shell_buffer_pos < SHELL_BUFFER_SIZE - 1) {
                    // Add character to buffer
                    shell_buffer[shell_buffer_pos++] = event.ascii;
                    vga_putchar(event.ascii);
                }
            }
        }
    }
}

void shell_process_input(const char* input) {
    int argc = 0;
    shell_parse_command(input, shell_argv, &argc);
    
    if (argc > 0) {
        shell_execute_command(argc, shell_argv);
    }
}

void shell_parse_command(const char* input, char* argv[], int* argc) {
    static char cmd_buffer[SHELL_BUFFER_SIZE];
    int i = 0, arg_start = 0;
    *argc = 0;
    
    // Copy input to buffer
    while (input[i] && i < SHELL_BUFFER_SIZE - 1) {
        cmd_buffer[i] = input[i];
        i++;
    }
    cmd_buffer[i] = '\0';
    
    // Parse arguments
    i = 0;
    while (cmd_buffer[i] && *argc < SHELL_MAX_ARGS - 1) {
        // Skip whitespace
        while (cmd_buffer[i] == ' ' || cmd_buffer[i] == '\t') {
            i++;
        }
        
        if (cmd_buffer[i] == '\0') break;
        
        // Mark start of argument
        arg_start = i;
        
        // Find end of argument
        while (cmd_buffer[i] && cmd_buffer[i] != ' ' && cmd_buffer[i] != '\t') {
            i++;
        }
        
        // Null-terminate argument
        if (cmd_buffer[i]) {
            cmd_buffer[i] = '\0';
            i++;
        }
        
        argv[*argc] = &cmd_buffer[arg_start];
        (*argc)++;
    }
    
    argv[*argc] = NULL;
}

void shell_execute_command(int argc, char* argv[]) {
    if (argc == 0) return;
    
    const char* cmd_name = argv[0];
    
    // Find and execute command
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(cmd_name, commands[i].name) == 0) {
            commands[i].function(argc, argv);
            return;
        }
    }
    
    // Command not found
    vga_puts("Bad command or file name\n");
}

void shell_print_prompt(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts(SHELL_PROMPT);
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void shell_clear_buffer(void) {
    shell_buffer_pos = 0;
    for (int i = 0; i < SHELL_BUFFER_SIZE; i++) {
        shell_buffer[i] = '\0';
    }
}

// Built-in commands implementation
void cmd_help(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    vga_puts("For more information on a specific command, type HELP command-name\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        vga_puts(commands[i].name);
        vga_puts("    ");
    }
    vga_puts("\n");
}

void cmd_cls(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_clear();
}

void cmd_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        vga_puts(argv[i]);
        if (i < argc - 1) vga_puts(" ");
    }
    vga_puts("\n");
}

void cmd_time(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts("Current time is ");
    vga_put_dec(timer_get_uptime_seconds());
    vga_puts(" seconds since boot\n");
}

void cmd_mem(int argc, char* argv[]) {
    (void)argc; (void)argv;
    uint32_t total = (memory_get_heap_end() - memory_get_heap_start()) / 1024;
    vga_put_dec(total);
    vga_puts(" KB total conventional memory\n");
    vga_put_dec(total);
    vga_puts(" KB available to MyOS-DOS\n");
}

void cmd_dir(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts(" Directory of C:\\\n\n");
    fs_list_files();
    vga_puts("               1 File(s)          0 bytes\n");
    vga_puts("               0 Dir(s)   ");
    vga_put_dec((memory_get_heap_end() - memory_get_heap_start()) / 1024);
    vga_puts(" KB free\n");
}

void cmd_type(int argc, char* argv[]) {
    if (argc < 2) {
        vga_puts("Required parameter missing\n");
        return;
    }
    
    file_handle_t* handle = fs_open_file(argv[1], "r");
    if (handle) {
        vga_puts("[File content display not implemented]\n");
        fs_close_file(handle);
    } else {
        vga_puts("File not found\n");
    }
}

void cmd_copy(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts("Insufficient parameters\n");
}

void cmd_del(int argc, char* argv[]) {
    if (argc < 2) {
        vga_puts("Required parameter missing\n");
        return;
    }
    
    if (fs_delete_file(argv[1]) == 0) {
        // File deleted successfully
    } else {
        vga_puts("File not found\n");
    }
}

void cmd_ver(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts("MyOS-DOS Version 1.0\n");
}

void cmd_exit(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts("Thank you for using MyOS-DOS\n");
    vga_puts("System halted.\n");
    // In a real system, this would halt the machine
}

void cmd_clear(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_clear_screen();
}

void cmd_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        vga_puts(argv[i]);
        if (i < argc - 1) vga_puts(" ");
    }
    vga_puts("\n");
}

void cmd_date(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts("System uptime: ");
    vga_put_dec(timer_get_uptime_seconds());
    vga_puts(" seconds (");
    vga_put_dec(timer_get_ticks());
    vga_puts(" ticks)\n");
}

void cmd_mem(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts("Memory Information:\n");
    vga_puts("Heap start: ");
    vga_put_hex(memory_get_heap_start());
    vga_puts("\n");
    vga_puts("Heap end: ");
    vga_put_hex(memory_get_heap_end());
    vga_puts("\n");
    vga_puts("Available memory: ");
    vga_put_dec((memory_get_heap_end() - memory_get_heap_start()) / 1024);
    vga_puts(" KB\n");
}

void cmd_ls(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts("Files in filesystem:\n");
    fs_list_files();
}

void cmd_cat(int argc, char* argv[]) {
    if (argc < 2) {
        vga_puts("Usage: cat <filename>\n");
        return;
    }
    
    file_handle_t* handle = fs_open_file(argv[1], "r");
    if (handle) {
        vga_puts("File contents: ");
        vga_puts(argv[1]);
        vga_puts("\n");
        // In a real implementation, we would read and display file contents
        vga_puts("[File reading not fully implemented yet]\n");
        fs_close_file(handle);
    } else {
        vga_puts("File not found: ");
        vga_puts(argv[1]);
        vga_puts("\n");
    }
}

void cmd_touch(int argc, char* argv[]) {
    if (argc < 2) {
        vga_puts("Usage: touch <filename>\n");
        return;
    }
    
    int fd = fs_create_file(argv[1], 0); // 0 = default attributes
    if (fd >= 0) {
        vga_puts("Created file: ");
        vga_puts(argv[1]);
        vga_puts("\n");
        fs_close_file((file_handle_t*)fd); // Simplified for demo
    } else {
        vga_puts("Failed to create file: ");
        vga_puts(argv[1]);
        vga_puts("\n");
    }
}

void cmd_rm(int argc, char* argv[]) {
    if (argc < 2) {
        vga_puts("Usage: rm <filename>\n");
        return;
    }
    
    if (fs_delete_file(argv[1]) == 0) {
        vga_puts("Deleted file: ");
        vga_puts(argv[1]);
        vga_puts("\n");
    } else {
        vga_puts("Failed to delete file: ");
        vga_puts(argv[1]);
        vga_puts("\n");
    }
}

void cmd_sysinfo(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga_puts("=== MyOS System Information ===\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    vga_puts("OS Name: MyOS v2.0 Enhanced\n");
    vga_puts("Architecture: x86 (32-bit)\n");
    vga_puts("Features:\n");
    vga_puts("  - Interactive Shell\n");
    vga_puts("  - Dynamic Memory Management\n");
    vga_puts("  - SATA/AHCI Support\n");
    vga_puts("  - Simple Filesystem\n");
    vga_puts("  - Cursor Navigation\n");
    vga_puts("  - System Timer\n");
    
    vga_puts("\nHardware:\n");
    vga_puts("SATA devices: ");
    vga_put_dec(sata_detect_drives());
    vga_puts("\n");
    
    vga_puts("Uptime: ");
    vga_put_dec(timer_get_uptime_seconds());
    vga_puts(" seconds\n");
}

void cmd_cursor(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    uint8_t x, y;
    vga_get_cursor_position(&x, &y);
    vga_puts("Cursor position: (");
    vga_put_dec(x);
    vga_puts(", ");
    vga_put_dec(y);
    vga_puts(")\n");
}

void cmd_mouse(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    mouse_state_t* state = mouse_get_state();
    if (!state->initialized) {
        vga_puts("Mouse not initialized!\n");
        return;
    }
    
    vga_puts("Mouse status:\n");
    vga_puts("Position: (");
    vga_put_dec(state->x);
    vga_puts(", ");
    vga_put_dec(state->y);
    vga_puts(")\n");
    
    vga_puts("Buttons: ");
    if (state->buttons & MOUSE_LEFT_BUTTON) vga_puts("L ");
    if (state->buttons & MOUSE_RIGHT_BUTTON) vga_puts("R ");
    if (state->buttons & MOUSE_MIDDLE_BUTTON) vga_puts("M ");
    if (state->buttons == 0) vga_puts("None");
    vga_puts("\n");
    
    vga_puts("Use mouse to move cursor around the screen!\n");
}

void cmd_reboot(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
    vga_puts("Reboot command received!\n");
    vga_puts("(This is a simulation - real reboot not implemented)\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("System would restart now...\n");
}

// Function to handle mouse input in shell
void shell_handle_mouse_input(void) {
    mouse_state_t* state = mouse_get_state();
    if (!state->initialized) {
        return;
    }
    
    // Только обрабатываем мышь без спама в консоль
    // Клики можно обрабатывать по требованию через команду mouse
}
