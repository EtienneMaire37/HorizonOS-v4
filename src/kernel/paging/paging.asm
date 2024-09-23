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