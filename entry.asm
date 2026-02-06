[bits 32]
[extern kmain]     ; Look for C kernel function

global start
global _start       ; Export BOTH names so the linker definitely finds one

start:
_start:
    call kmain     ; Jump to C
    jmp $           ; Hang if C returns