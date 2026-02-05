; OM3 OS - Interactive Version
; ----------------------------
bits 16
org 0x7c00

start:
    ; 1. Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; 2. Set Video Mode (Text Mode 80x25, 16 colors)
    ; This clears the screen and ensures we are in color mode.
    mov ah, 0x00    ; BIOS function: Set Video Mode
    mov al, 0x03    ; Mode 3: 80x25 Color Text
    int 0x10        ; Call Video Interrupt

    ; 3. Print the prompt
    mov si, msg_prompt
    call print_string

main_loop:
    ; 4. Wait for a keystroke
    mov ah, 0x00    ; BIOS function: Read Key
    int 0x16        ; Call Keyboard Interrupt
    ; Result: AL now holds the ASCII character, AH holds the Scan Code.

    ; 5. Print the character you just typed
    mov ah, 0x0E    ; BIOS function: Teletype Output
    int 0x10        ; Call Video Interrupt

    jmp main_loop   ; Jump back to wait for the next key

; --- Function: Print String ---
print_string:
    mov ah, 0x0e
.loop:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .loop
.done:
    ret

; --- Data ---
msg_prompt db 'OM3 OS v0.1 - Type something: ', 0

; --- Boot Sector Magic ---
times 510-($-$$) db 0
dw 0xaa55