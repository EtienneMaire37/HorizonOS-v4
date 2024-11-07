ENTRY(_start)

INPUT
(
	"src/tasks/bin/hello_world.o"
)

OUTPUT_FORMAT(elf32-i386)
OUTPUT("src/initrd/hello_world.elf")
 
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