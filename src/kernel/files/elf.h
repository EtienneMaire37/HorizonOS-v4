#pragma once

struct ELF32_Header
{
	uint8_t		elf[4]; // 7f 45 4c 46
    uint8_t     architecture;   // 1: 32 ; 2: 64
    uint8_t     endianness;     // 1: little endian ; 2: big endian
    uint8_t     elf_version;    // 1 for the original elf version
    uint8_t     abi;            // 0: System V ; 3: Linux
    uint8_t     abi_version;
    uint8_t     padding[7];

	uint16_t	type;
	uint16_t	machine;    // 3: x86
	uint32_t	version;
	uint32_t	entry;      // address of entry point or 0
	uint32_t	ph_off;     // program header offset
	uint32_t	sh_off;     // section header offset
	uint32_t	flags;      
	uint16_t	eh_size;    // elf header size
	uint16_t	ph_ent_size;// program header entry size  
	uint16_t	ph_num;     // number of entries in the program header table
	uint16_t	sh_ent_size;// section header entry size  
	uint16_t	sh_num;     // number of entries in the section header table
	uint16_t	sh_str_idx; // index of the section header table entry that contains the section names
};

struct ELF32_ProgramHeader_Entry
{
	uint32_t type;
    uint32_t seg_off;   // offset of the segment in the file image
    uint32_t seg_vaddr; // virtual address of the segment in memory
    uint32_t seg_paddr; // * unused
    uint32_t seg_filesz;// size in bytes of the segment in the file
    uint32_t seg_memsz; // size in bytes of the segment in memory
    uint32_t flags;
    uint32_t align;     // 0|1: no alignment
};

struct ELF32_SectionHeader_Entry
{
	uint32_t name;  // offset to the name of the section
    uint32_t type;
    uint32_t flags;
    uint32_t address;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t addr_align;
    uint32_t entry_size;
};

#define ELF32_ARCHITECTURE_32       1
#define ELF32_ARCHITECTURE_64       2

#define ELF32_LITTLE_ENDIAN         1
#define ELF32_BIG_ENDIAN            2

#define ELF32_ABI_SYSTEM_V          0

#define ELF32_TYPE_NONE             0
#define ELF32_TYPE_RELOCATABLE      1
#define ELF32_TYPE_EXECUTABLE       2
#define ELF32_TYPE_SHARED           3
#define ELF32_TYPE_CORE             4

#define ELF32_MACHINE_X86           3


#define ELF32_PH_TYPE_NULL          0
#define ELF32_PH_TYPE_LOADABLE      1
#define ELF32_PH_TYPE_DYNAMIC       2
#define ELF32_PH_TYPE_INTERPRETER   3
#define ELF32_PH_TYPE_NOTE          4
#define ELF32_PH_TYPE_PHTABLE       6
#define ELF32_PH_TYPE_THREAD_LOCAL  7

#define ELF32_PH_FLAG_EXECUTABLE    1
#define ELF32_PH_FLAG_WRITEABLE     2
#define ELF32_PH_FLAG_READABLE      4

char* elf_type[5] = 
{
    "Unknown", "Relocatable file", "Executable file", "Shared object", "Core file"
};

char* elf_abi[256] = 
{
    "System V", "HP-UX", "NetBSD", "Linux", "GNU Hurd", "", "Solaris", "AIX", "IRIX", "FreeBSD", "Tru64", "Novell Modesto", "OpenBSD", "OpenVMS", "NonStop Kernel", "AROS",
    "FenixOS", "Nuxi CloudABI", "Stratus Technologies OpenVOS", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""    
};

char* elf_machine[259] = 
{
    "No specific instruction set", "AT&T WE 32100", "SPARC", "x86", "Motorola 68000 (M68k)", "Motorola 88000 (M88k)", "Intel MCU", "Intel 80860", "MIPS", "IBM System/370", "MIPS RS3000 Little-endian", "", "", "", "", "Hewlett-Packard PA-RISC",
    "", "", "", "Intel 80960", "PowerPC", "PowerPC", "S390", "IBM SPU/SPC", "", "", "", "", "", "", "", "",
    "", "", "", "", "NEC V800", "Fujitsu FR20", "TRW RH-32", "Motorola RCE", "ARM", "Digital Alpha", "SuperH", "SPARC Version 9", "Siemens TriCore embedded processor", "Argonaut RISC Core", "Hitachi H8/300", "Hitachi H8/300H",
    "", "", "IA-64", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "" 
};