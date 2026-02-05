[bits 32]
[extern _kmain]     ; Look for the C function
global start

start:
    ; [DEBUG] Print 'E' (Entry) in Green next to the 'P'
    ; Video memory 0xb8002 is the 2nd character slot
    mov byte [0xb8002], 'E'
    mov byte [0xb8003], 0x2F  ; 2 = Green Background, F = White Text

    call _kmain     ; Attempt to call C
    jmp $