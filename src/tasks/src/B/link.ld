ENTRY(_start)

INPUT
(
	"src/tasks/bin/B.o"
)

OUTPUT_FORMAT(elf32-i386)
OUTPUT("src/tasks/bin/B.elf")
 
SECTIONS
{
	. = 0x100000;

	.text :
	{
		*(.text)
	}
 
	.rodata :
	{
		*(.rodata)
	}

	.data :
	{
		*(.data)
	}

	.bss :
	{
		*(.bss)
	}
}