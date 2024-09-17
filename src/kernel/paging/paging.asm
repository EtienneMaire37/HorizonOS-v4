bits 32

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
    push ebp
    mov ebp, esp

    mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax
    
    mov esp, ebp
    pop ebp
    ret