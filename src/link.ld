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

	.multiboot : ALIGN(0x1000)
	{
		*(.multiboot.data)
		*(.multiboot.text)
	}
	
	. += 0xC0000000;
 
	.text ALIGN(0x1000) : AT (ADDR (.text) - 0xC0000000)
	{
		_kernelStart = .;
		*(.text)
	}
 
	.rodata ALIGN(0x1000) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	.data ALIGN(0x1000) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss ALIGN(0x1000) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(.bss)
		_kernelEnd = .;
	}
}