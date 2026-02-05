[org 0x7c00]
[bits 16]

start:
    ; [Checkpoint] A for Alive
    mov ah, 0x0e
    mov al, 'A'
    int 0x10

    ; 1. Load the Kernel (Read 10 sectors to be safe)
    mov bx, 0x1000      ; Load address
    mov ah, 0x02        ; Read function
    mov al, 10          ; Sectors to read
    mov ch, 0           ; Cylinder 0
    mov dh, 0           ; Head 0
    mov cl, 2           ; Sector 2
    int 0x13            ; Call BIOS
    jc disk_error

    ; [Checkpoint] B for Binary Loaded
    mov ah, 0x0e
    mov al, 'B'
    int 0x10

    ; 2. Switch to 32-bit Protected Mode
    cli                 ; Disable interrupts
    lgdt [gdt_descriptor] ; Load GDT
    
    mov eax, cr0
    or eax, 0x1         ; Set protected mode bit
    mov cr0, eax
    
    jmp CODE_SEG:init_pm ; Far jump to flush CPU

disk_error:
    mov ah, 0x0e
    mov al, 'E'
    int 0x10
    jmp $

[bits 32]
init_pm:
    ; 3. Set up Data Segments
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov ebp, 0x90000
    mov esp, ebp

    ; --- [DEBUG] FORCE PRINT 'P' ---
    ; We write directly to Video Memory (0xb8000)
    ; This proves 32-bit mode is working WITHOUT needing C code.
    mov byte [0xb8000], 'P'
    mov byte [0xb8001], 0x4F    ; Red background, White text

    ; 4. Jump to C Kernel
    call 0x1000
    jmp $

; --- GDT (The Global Descriptor Table) ---
gdt_start:
    dq 0x0

gdt_code:
    dw 0xFFFF    ; Limit (0-15)
    dw 0x0       ; Base (0-15)
    db 0x0       ; Base (16-23)
    db 10011010b ; Access Byte (Present, Ring 0, Code, Exec/Read)
    db 11001111b ; Flags (4KB Granularity, 32-bit) + Limit (16-19)
    db 0x0       ; Base (24-31)

gdt_data:
    dw 0xFFFF    ; Limit (0-15)
    dw 0x0       ; Base (0-15)
    db 0x0       ; Base (16-23)
    db 10010010b ; Access Byte (Present, Ring 0, Data, Read/Write)
    db 11001111b ; Flags (4KB Granularity, 32-bit) + Limit (16-19)
    db 0x0       ; Base (24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

times 510-($-$$) db 0
dw 0xaa55