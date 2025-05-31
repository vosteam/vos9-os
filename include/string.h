#ifndef STRING_H
#define STRING_H

#include "kernel.h"

// String manipulation functions
size_t strlen(const char* str);
int strcmp(const char* str1, const char* str2);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
void* memset(void* ptr, int value, size_t num);
void* memcpy(void* dest, const void* src, size_t num);

#endif
