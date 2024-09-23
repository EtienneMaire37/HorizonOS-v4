ENTRY(_start)

INPUT
(
	"bin/kernelentry.o"
 	"bin/kmain.o"
	"bin/gdt.o"
	"bin/idt.o"
	"bin/paging.o"
)

OUTPUT_FORMAT(elf32-i386)
OUTPUT("bin/kernel.elf")
 
SECTIONS
{
	. = 0x100000;
 
	.text : ALIGN(0x1000)
	{
		_kernelStart = .;
		*(.multiboot)
		*(.text)
	}
 
	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}

	.data : ALIGN(0x1000)
	{
		*(.data)
	}

	.bss : ALIGN(0x1000)
	{
		*(.bss)
		_kernelEnd = .;
	}
}