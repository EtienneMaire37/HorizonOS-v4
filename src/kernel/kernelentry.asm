bits 32

ALIGNMENT equ 1
MEMINFO equ  (1 << 1)   
FLAGS equ (ALIGNMENT | MEMINFO)
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

%macro CALL_LOWER_HALF 1
	mov edx, %1
    sub edx, 0xc0000000
	call edx
%endmacro

%macro DEBUG_A 0
	mov al, 'A'
 	mov ah, 0x0f
 	mov [0xb8000], ax
 	jmp $
%endmacro

section .multiboot.data
dd MAGIC
dd FLAGS
dd CHECKSUM

section .multiboot.text
extern kernel
extern page_directory
extern page_table_0
extern page_table_768_1023
global _start
_start:
	cli

	mov [MAGIC_NUMBER], eax
	mov [MULTIBOOT_INFO], ebx

	mov ebp, stack_top
    sub ebp, 0xc0000000
	mov esp, stack_top
    sub esp, 0xc0000000

	xor ebx, ebx
	mov ecx, 1024
	.loopPT0:
		mov edx, 4096
		mov eax, ebx
		mul edx
		or eax, 0b111

		mov [page_table_0 + 4 * ebx - 0xc0000000], eax

		inc ebx
		loop .loopPT0

	xor ebx, ebx
	mov ecx, 1024
	.loopPT768:
		mov edx, 4096
		mov eax, ebx
		mul edx
		or eax, 0b111

		mov [page_table_768_1023 + 4 * ebx - 0xc0000000], eax

		inc ebx
		loop .loopPT768

	mov eax, page_table_0
	sub eax, 0xc0000000
	and eax, 0xfffff000
	or eax, 0b111

	mov [page_directory - 0xc0000000], eax

	mov eax, page_table_768_1023
	sub eax, 0xc0000000
	and eax, 0xfffff000
	or eax, 0b111

	mov [page_directory - 0xc0000000 + 4 * 768], eax

	mov ecx, page_directory
    sub ecx, 0xc0000000
	mov cr3, ecx

	mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax

	mov ebp, stack_top
	mov esp, ebp

	mov eax, [MAGIC_NUMBER]
	push eax ; magic number
	mov ebx, [MULTIBOOT_INFO]
	push ebx ; multiboot info

	call kernel
	cli

	add esp, 8 

	jmp _halt

MAGIC_NUMBER: dd 0
MULTIBOOT_INFO: dd 0

section .text

global _halt
_halt:
.loop:
	cli
  	hlt
	jmp .loop

global stack_top
section .data
times 16384 dd 0
stack_top: