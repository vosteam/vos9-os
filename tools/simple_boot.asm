[BITS 16]           ; 16-bit mode
[ORG 0x7C00]        ; Bootloader loads at 0x7C00

start:
    ; Setup segments
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Clear screen
    mov ax, 0x0003
    int 0x10

    ; Print welcome message
    mov si, welcome_msg
    call print_string

    ; Print system info
    mov si, system_msg
    call print_string

    ; Simple shell
shell_loop:
    mov si, prompt_msg
    call print_string
    
    ; Read character
    mov ah, 0x00
    int 0x16
    
    ; Echo character
    mov ah, 0x0E
    int 0x10
    
    ; Check for enter
    cmp al, 0x0D
    je newline
    jmp shell_loop

newline:
    mov si, newline_msg
    call print_string
    jmp shell_loop

print_string:
    mov ah, 0x0E
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    ret

welcome_msg db 'MyOS v1.0 - Simple Operating System', 13, 10, 0
system_msg db 'Features: 16-bit mode, basic shell, keyboard input', 13, 10, 0
prompt_msg db 13, 10, 'MyOS> ', 0
newline_msg db 13, 10, 0

; Fill to 510 bytes
times 510-($-$$) db 0

; Boot signature
dw 0xAA55
