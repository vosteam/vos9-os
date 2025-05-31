#include "../include/shell.h"
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/string.h"
#include "../include/memory.h"
#include "../include/filesystem.h"
#include "../include/drivers/timer.h"

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
    // Check if keyboard data is available
    if (inb(0x64) & 0x01) {
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
                    // Ignore arrow keys and other special keys for DOS-like experience
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
                    // Add character to buffer (convert to uppercase for DOS feel)
                    char c = event.ascii;
                    if (c >= 'a' && c <= 'z') {
                        c = c - 'a' + 'A';
                    }
                    shell_buffer[shell_buffer_pos++] = c;
                    vga_putchar(c);
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
    
    // Command not found - DOS style error
    vga_puts("Bad command or file name\n");
}

// DOS-style command implementations
void cmd_help(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    vga_puts("For more information on a specific command, type HELP command-name\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        vga_puts(commands[i].name);
        vga_puts("    ");
        if ((i + 1) % 6 == 0) vga_puts("\n"); // New line every 6 commands
    }
    vga_puts("\n");
}

void cmd_cls(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_clear();
}

void cmd_echo(int argc, char* argv[]) {
    if (argc < 2) {
        vga_puts("ECHO is on.\n");
        return;
    }
    
    for (int i = 1; i < argc; i++) {
        vga_puts(argv[i]);
        if (i < argc - 1) vga_puts(" ");
    }
    vga_puts("\n");
}

void cmd_time(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts("Current time is ");
    uint32_t seconds = timer_get_uptime_seconds();
    uint32_t hours = seconds / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t secs = seconds % 60;
    
    vga_put_dec(hours);
    vga_puts(":");
    if (minutes < 10) vga_puts("0");
    vga_put_dec(minutes);
    vga_puts(":");
    if (secs < 10) vga_puts("0");
    vga_put_dec(secs);
    vga_puts(" (uptime)\n");
}

void cmd_mem(int argc, char* argv[]) {
    (void)argc; (void)argv;
    uint32_t total = (memory_get_heap_end() - memory_get_heap_start()) / 1024;
    
    vga_put_dec(total);
    vga_puts(" KB total conventional memory\n");
    vga_put_dec(total);
    vga_puts(" KB available to MyOS-DOS\n");
    vga_puts("\nLargest executable program size is ");
    vga_put_dec(total);
    vga_puts(" KB\n");
}

void cmd_dir(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_puts(" Volume in drive C has no label\n");
    vga_puts(" Volume Serial Number is 1234-5678\n\n");
    vga_puts(" Directory of C:\\\n\n");
    
    // List files
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
    if (argc < 3) {
        vga_puts("Insufficient parameters\n");
        return;
    }
    vga_puts("File copy not implemented\n");
}

void cmd_del(int argc, char* argv[]) {
    if (argc < 2) {
        vga_puts("Required parameter missing\n");
        return;
    }
    
    if (fs_delete_file(argv[1]) == 0) {
        // File deleted successfully - no message in DOS style
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
    while(1); // Infinite loop to simulate halt
}
