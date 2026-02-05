; boot.asm - Loads C kernel and jumps to 32-bit mode
[org 0x7c00]
[bits 16]

start:
    ; 1. Load the Kernel from Disk
    ; We are going to read 5 sectors just to be safe
    mov bx, 0x1000      ; Load kernel to address 0x1000
    mov ah, 0x02        ; BIOS Read Sector Function
    mov al, 5           ; Read 5 sectors
    mov ch, 0           ; Cylinder 0
    mov dh, 0           ; Head 0
    mov cl, 2           ; Start reading from Sector 2 (Sector 1 is bootloader)
    int 0x13            ; Call Disk Interrupt

    ; 2. Switch to 32-bit Protected Mode
    cli                 ; Disable interrupts (critical!)
    lgdt [gdt_descriptor] ; Load the Global Descriptor Table
    
    mov eax, cr0
    or eax, 0x1         ; Set the first bit of Control Register 0
    mov cr0, eax
    
    jmp CODE_SEG:init_pm ; Far jump to flush the CPU pipeline

[bits 32]
init_pm:
    ; 3. Set up segment registers for 32-bit mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov ebp, 0x90000    ; Update stack position
    mov esp, ebp

    call 0x1000         ; Jump to our C kernel!
    jmp $

; --- GDT (Global Descriptor Table) ---
; This is boilerplate magic required for 32-bit mode.
gdt_start:
    dq 0x0              ; Null Descriptor
gdt_code:               ; Code Segment
    dw 0xffff, 0x0, 0x9a00, 0x00cf
gdt_data:               ; Data Segment
    dw 0xffff, 0x0, 0x9200, 0x00cf
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

times 510-($-$$) db 0
dw 0xaa55