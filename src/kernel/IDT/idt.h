#pragma once

#define ISR_TASK_GATE           0b0101
#define ISR_INTERRUPT_GATE_16   0b0110 
#define ISR_TRAP_GATE_16        0b0111 
#define ISR_INTERRUPT_GATE_32   0b1110 
#define ISR_TRAP_GATE_32        0b1111

struct IDT_Descriptor
{
    uint16_t size;      // The size of the table in bytes subtracted by 1
    uint32_t address;   // The linear address of the IDT (not the physical address, paging applies).
} __attribute__((__packed__));

struct IDT_Descriptor _idtr;

struct IDT_Entry
{   
    uint16_t offset_lo           : 16;
    uint16_t segment_selector    : 16;
    uint8_t reserved             : 8;
    uint8_t gate_type            : 4;
    uint8_t zero                 : 1;
    uint8_t DPL                  : 2;
    uint8_t present              : 1;
    uint16_t offset_hi           : 16;
} __attribute__((__packed__));

struct IDT_Entry IDT[256];

extern void LoadIDT();
extern uint32_t INTTable[256];  // ptrs to the isrs

void SetupIDTEntry(struct IDT_Entry* entry, uint16_t segment, uint32_t offset, uint8_t privilege, uint8_t type);
void InstallIDT();