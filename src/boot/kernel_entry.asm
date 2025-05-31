[BITS 32]

section .text
    global _start
    extern kernel_main

_start:
    ; Setup stack
    mov esp, stack_top
    
    ; Call main kernel function
    call kernel_main
    
    ; If kernel returns, hang
.hang:
    hlt
    jmp .hang

section .bss
    resb 8192    ; 8KB stack
stack_top:
