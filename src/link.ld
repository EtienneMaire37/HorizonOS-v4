ENTRY(_start)

INPUT
(
	"bin/kernelentry.o"
 	"bin/kmain.o"
	"bin/gdt.o"
	"bin/idt.o"
	"bin/paging.o"
)
 
SECTIONS
{
	. = 0x100000;
	_kernelStart = .;
 
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
 
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	_kernelEnd = .;
}