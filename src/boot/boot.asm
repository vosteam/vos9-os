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

    ; Load kernel from disk
    mov ax, 0x1000      ; Segment for kernel loading
    mov es, ax
    mov bx, 0           ; Offset
    mov ah, 0x02        ; Read sectors function
    mov al, 10          ; Number of sectors to read
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Sector 2 (after bootloader)
    mov dh, 0           ; Head 0
    mov dl, 0x00        ; Floppy disk (changed from 0x80)
    int 0x13

    jc disk_error       ; If read error

    ; Switch to protected mode
    cli                 ; Disable interrupts
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:protected_mode

disk_error:
    mov si, disk_error_msg
    call print_string
    hlt

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

welcome_msg db 'MyOS Bootloader v1.0', 13, 10, 'Loading kernel...', 13, 10, 0
disk_error_msg db 'Disk read error!', 13, 10, 0

; GDT (Global Descriptor Table)
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0

    ; Code segment
    dw 0xFFFF       ; Limit (low)
    dw 0x0000       ; Base (low)
    db 0x00         ; Base (middle)
    db 10011010b    ; Access
    db 11001111b    ; Granularity
    db 0x00         ; Base (high)

    ; Data segment
    dw 0xFFFF       ; Limit (low)
    dw 0x0000       ; Base (low)
    db 0x00         ; Base (middle)
    db 10010010b    ; Access
    db 11001111b    ; Granularity
    db 0x00         ; Base (high)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                ; GDT address

[BITS 32]
protected_mode:
    ; Setup segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Jump to kernel
    call 0x10000

    ; Infinite loop
hang:
    hlt
    jmp hang

; Fill to 510 bytes
times 510-($-$$) db 0

; Boot signature
dw 0xAA55
