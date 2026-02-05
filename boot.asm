[org 0x7c00]
KERNEL_OFFSET equ 0x1000

    mov [BOOT_DRIVE], dl ; Remember which drive we booted from

    ; 1. Set up stack
    mov bp, 0x9000
    mov sp, bp

    ; 2. Print "Loading..."
    mov bx, MSG_REAL_MODE
    call print_string

    ; 3. Load Kernel from Disk
    call load_kernel

    ; 4. Switch to Protected Mode
    call switch_to_pm

    jmp $

; --- DISK ROUTINE ---
load_kernel:
    mov bx, KERNEL_OFFSET ; Destination address
    mov dh, 50            ; READ 50 SECTORS (Expanded from default!)
    mov dl, [BOOT_DRIVE]
    mov ah, 0x02          ; BIOS Read Sector Function
    mov al, dh            ; Read DH sectors
    mov ch, 0x00          ; Cylinder 0
    mov dh, 0x00          ; Head 0
    mov cl, 0x02          ; Start reading from Sector 2 (Sector 1 is Bootloader)
    int 0x13              ; BIOS Interrupt

    jc disk_error         ; Jump if error (Carry Flag set)
    
    ; Check if we actually read the expected number of sectors
    cmp al, 50            ; Did we read 50?
    jne disk_error
    ret

disk_error:
    mov bx, MSG_DISK_ERROR
    call print_string
    jmp $

; --- PRINT STRING (Real Mode) ---
print_string:
    pusha
    mov ah, 0x0e
.loop:
    mov al, [bx]
    cmp al, 0
    je .done
    int 0x10
    add bx, 1
    jmp .loop
.done:
    popa
    ret

; --- VARIABLES ---
BOOT_DRIVE      db 0
MSG_REAL_MODE   db "Loading OM3 OS (50 sectors)...", 0
MSG_DISK_ERROR  db "Disk Read Error!", 0

; --- GDT (Global Descriptor Table) ---
gdt_start:
gdt_null: ; Mandatory null descriptor
    dd 0x0
    dd 0x0
gdt_code: ; Code segment descriptor
    dw 0xffff    ; Limit (bits 0-15)
    dw 0x0       ; Base (bits 0-15)
    db 0x0       ; Base (bits 16-23)
    db 10011010b ; 1st flags, type flags
    db 11001111b ; 2nd flags, Limit (bits 16-19)
    db 0x0       ; Base (bits 24-31)
gdt_data: ; Data segment descriptor
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; --- SWITCH TO PM ---
switch_to_pm:
    cli                    ; 1. Disable interrupts
    lgdt [gdt_descriptor]  ; 2. Load GDT descriptor
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax           ; 3. Enable Protected Mode bit in CR0
    jmp CODE_SEG:init_pm   ; 4. Far jump to 32-bit code

[bits 32]
init_pm:
    mov ax, DATA_SEG       ; 5. Update segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000       ; 6. Update stack position
    mov esp, ebp

    call KERNEL_OFFSET     ; 7. Jump to our C Kernel!
    jmp $

; --- PADDING ---
times 510-($-$$) db 0
dw 0xaa55