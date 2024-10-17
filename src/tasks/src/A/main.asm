bits 32

section .text

global _start
_start:
    mov al, "A"
.loop:
    int 0xff
    jmp .loop