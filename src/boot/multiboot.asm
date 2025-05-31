; Multiboot header
MULTIBOOT_MAGIC     equ 0x1BADB002
MULTIBOOT_FLAGS     equ 0x00000003
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .text
global _start
extern kernel_main

_start:
    ; Setup stack
    mov esp, stack_top
    
    ; Push multiboot info structure pointer
    push ebx
    ; Push multiboot magic number
    push eax
    
    ; Call main kernel function
    call kernel_main
    
    ; If kernel returns, hang
.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384  ; 16KB stack
stack_top:
