bits 32

section .text

global _start
_start:
    mov al, "B"
.loop:
    int 0xff
    jmp .loop