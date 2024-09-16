bits 32
section .text

global LoadIDT
extern _idtr
; void LoadIDT()
LoadIDT:
    lidt [_idtr]
    ret

%macro INT_ERROR_CODE 1
INT_%1:
    cli
    push %1

    jmp _InterruptHandler
%endmacro

%macro INT_NO_ERROR_CODE 1
INT_%1:
    cli
    push 0     ; dummy error code
    push %1

    jmp _InterruptHandler
%endmacro

extern InterruptHandler

; ISRs
INT_NO_ERROR_CODE 0
INT_NO_ERROR_CODE 1
INT_NO_ERROR_CODE 2
INT_NO_ERROR_CODE 3
INT_NO_ERROR_CODE 4
INT_NO_ERROR_CODE 5
INT_NO_ERROR_CODE 6
INT_NO_ERROR_CODE 7
INT_ERROR_CODE    8
INT_NO_ERROR_CODE 9
INT_ERROR_CODE    10
INT_ERROR_CODE    11
INT_ERROR_CODE    12
INT_ERROR_CODE    13
INT_ERROR_CODE    14
INT_NO_ERROR_CODE 15
INT_NO_ERROR_CODE 16
INT_ERROR_CODE    17
INT_NO_ERROR_CODE 18
INT_NO_ERROR_CODE 19
INT_NO_ERROR_CODE 20
INT_NO_ERROR_CODE 21
INT_NO_ERROR_CODE 22
INT_NO_ERROR_CODE 23
INT_NO_ERROR_CODE 24
INT_NO_ERROR_CODE 25
INT_NO_ERROR_CODE 26
INT_NO_ERROR_CODE 27
INT_NO_ERROR_CODE 28
INT_NO_ERROR_CODE 29
INT_ERROR_CODE    30
INT_NO_ERROR_CODE 31

; IRQs
%assign i 32
    %rep    16
        INT_NO_ERROR_CODE i
    %assign i i+1 
    %endrep

; All the other interrupts
%assign i 48
    %rep    (256 - 48)
        INT_NO_ERROR_CODE i
    %assign i i+1 
    %endrep

global INTTable
INTTable:
    %assign i 0 
    %rep    256
        dd INT_%+i
    %assign i i+1 
    %endrep

intEnd:
    add esp, 8
    iret 

_InterruptHandler:
    pushad
    call InterruptHandler
    popad
    jmp intEnd