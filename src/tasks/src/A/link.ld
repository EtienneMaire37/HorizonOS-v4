ENTRY(_start)

INPUT
(
	"src/tasks/bin/A.o"
)

OUTPUT_FORMAT(elf32-i386)
OUTPUT("src/tasks/bin/A.elf")
 
SECTIONS
{
	. = 0x400000;

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