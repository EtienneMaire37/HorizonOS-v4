bits 32
section .text

global LoadGDT
extern _gdtr
; void LoadGDT()
LoadGDT:
    lgdt  [_gdtr]
    jmp 0x08:.reloadSeg
.reloadSeg:
    mov   ax, 0x10
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
 
    ret