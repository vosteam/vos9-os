#ifndef SHELL_H
#define SHELL_H

#include "kernel.h"

// Shell constants
#define SHELL_BUFFER_SIZE 256
#define SHELL_MAX_ARGS 16

// Command structure
typedef struct {
    const char* name;
    const char* description;
    void (*function)(int argc, char* argv[]);
} shell_command_t;

// Shell functions
void shell_init(void);
void shell_run(void);
void shell_process_input(const char* input);
void shell_parse_command(const char* input, char* argv[], int* argc);
void shell_execute_command(int argc, char* argv[]);

// DOS-style commands
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

#endif
