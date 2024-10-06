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

#define ELF32_ARCHITECTURE_32   1
#define ELF32_ARCHITECTURE_64   2

#define ELF32_LITTLE_ENDIAN     1
#define ELF32_BIG_ENDIAN        2

#define ELF32_ABI_SYSTEM_V      0

#define ELF32_TYPE_NONE         0
#define ELF32_TYPE_RELOCATABLE  1
#define ELF32_TYPE_EXECUTABLE   2
#define ELF32_TYPE_SHARED       3
#define ELF32_TYPE_CORE         4

#define ELF32_MACHINE_X86       3