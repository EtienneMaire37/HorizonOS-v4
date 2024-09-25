bits 32
section .text

global LoadPageDirectory
LoadPageDirectory:
    push ebp
    mov ebp, esp

    mov eax, [esp + 8]
    mov cr3, eax

    mov esp, ebp
    pop ebp
    ret 

global EnablePaging
EnablePaging:
    mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax
    ret

extern _kernelStart  
extern HigherHalf
global JumpToHigherHalf
JumpToHigherHalf:
    add esp, 4

    sub esp, _kernelStart
    add esp, 0xc0000000

    sub ebp, _kernelStart
    add ebp, 0xc0000000

    mov eax, HigherHalf
    sub eax, _kernelStart
    add eax, 0xc0000000
    
    jmp eax