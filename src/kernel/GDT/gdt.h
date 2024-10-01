#pragma once

struct GDT_Descriptor
{
    uint16_t size;      // The size of the table in bytes subtracted by 1
    uint32_t address;   // The linear address of the GDT (not the physical address, paging applies).
} __attribute__((__packed__));

struct GDT_Descriptor _gdtr;

struct GDT_Entry
{   
    uint16_t limit_lo   : 16;
    uint16_t base_lo    : 16;
    uint8_t base_mid    : 8;
    uint8_t access_byte : 8;
    uint8_t limit_hi    : 4;
    uint8_t flags       : 4;
    uint8_t base_hi     : 8;
} __attribute__((__packed__));

struct TSS_Entry 
{
	uint32_t link;
	uint32_t esp0;     // The stack pointer to load when changing to kernel mode.
	uint32_t ss0;      // The stack segment to load when changing to kernel mode.
    
	uint32_t esp1; 
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((__packed__));

#define TSS_TYPE_16BIT_TSS_AVL  0x1
#define TSS_TYPE_16BIT_TSS_BSY  0x3
#define TSS_TYPE_32BIT_TSS_AVL  0x9
#define TSS_TYPE_32BIT_TSS_BSY  0xB
#define TSS_TYPE_LDT            0x2

#define KERNEL_CODE_SEGMENT 	0x08
#define KERNEL_DATA_SEGMENT 	0x10
#define USER_CODE_SEGMENT   	0x18
#define USER_DATA_SEGMENT   	0x20
#define TSS_SEGMENT   			0x28

struct GDT_Entry GDT[6];
struct TSS_Entry TSS;

extern void LoadGDT();
extern void LoadTSS();

void SetupGDTEntry(struct GDT_Entry* entry, physical_address_t base, uint32_t limit, uint8_t access_byte, uint8_t flags);
void InstallGDT();