section .text

global _start
_start:
    mov eax, 0
    mov al, "A"
.loop:
    int 0xff
    jmp .loop