[org 0x7c00]
[bits 16]

start:
    ; [CHECKPOINT 1] Print 'A' (Alive)
    mov ah, 0x0e
    mov al, 'A'
    int 0x10

    ; 1. Load the Kernel from Disk
    mov bx, 0x1000      ; Destination address
    mov ah, 0x02        ; Read sectors
    mov al, 5           ; Read 5 sectors (make sure kernel isn't bigger than this!)
    mov ch, 0           ; Cylinder 0
    mov dh, 0           ; Head 0
    mov cl, 2           ; Sector 2
    int 0x13            ; Call Disk Interrupt
    
    jc disk_error       ; If Carry Flag is set, jump to error

    ; [CHECKPOINT 2] Print 'B' (Binary Loaded)
    mov ah, 0x0e
    mov al, 'B'
    int 0x10

    ; 2. Switch to 32-bit Protected Mode
    cli                 
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 0x1         
    mov cr0, eax
    
    jmp CODE_SEG:init_pm 

disk_error:
    mov ah, 0x0e
    mov al, 'E'         ; Print 'E' for Error
    int 0x10
    jmp $

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov ebp, 0x90000
    mov esp, ebp

    ; [CHECKPOINT 3] We can't use BIOS print here easily, 
    ; but if we jump and crash, we will see A and B on screen.
    call 0x1000
    jmp $

; --- GDT and Magic ---
gdt_start:
    dq 0x0
gdt_code:
    dw 0xffff, 0x0, 0x9a00, 0x00cf
gdt_data:
    dw 0xffff, 0x0, 0x9200, 0x00cf
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

times 510-($-$$) db 0
dw 0xaa55