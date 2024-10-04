section .text

global _start
_start:
    mov eax, 0
    mov al, "B"
.loop:
    int 0xff
    jmp .loop