#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "multiboot.h"
multiboot_info_t* multibootInfo;

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)
#define TB (1024 * GB)

extern uint8_t stack_top;

extern uint8_t _kernelStart;
extern uint8_t _kernelEnd;
uint32_t kernelSize;

uint32_t availableMem = 0;

extern void _halt();
void Halt();

#define EnableInterrupts()  asm("sti");
#define DisableInterrupts() asm("cli");

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#include "IO/io.h"
#include "IO/ps2.h"
#include "debug/out.h"

#include "GDT/gdt.h"
#include "IDT/idt.h"
#include "IDT/int.h"
#include "IDT/PIC.h"
#include "IO/ps2kb.h"
#include "IO/textio.h"
#include "PIT/pit.h"

#include "klibc/math.h"
#include "klibc/stdio.h"
#include "klibc/stdlib.h"
#include "klibc/string.h"

#include "paging/paging.h"
#include "multitasking/task.h"

#include "klibc/reset.h"

// ---------------------------------------------------------------

#include "GDT/gdt.c"
#include "IDT/idt.c"
#include "IDT/int.c"
#include "IDT/PIC.c"
#include "IO/ps2kb.c"
#include "IO/textio.c"
#include "PIT/pit.c"

#include "klibc/math.c"
#include "klibc/stdio.c"
#include "klibc/stdlib.c"
#include "klibc/string.c"

// #include "klibc/kmalloc.c"

void Halt()
{
    LOG("WARN", "Kernel halted");
    _halt();
}

struct PageTable_Entry page_table_0[1024] __attribute__((aligned(4096)));
struct PageTable_Entry page_table_768_1023[256][1024] __attribute__((aligned(4096)));

void kernel(multiboot_info_t* _multibootInfo, uint32_t magicNumber)
{
    textColor = FG_WHITE | BG_BLACK;
    textCursor = 0;
    _kstdin.stream = STDIN_STREAM;
    _kstdout.stream = STDOUT_STREAM;
    _kstderr.stream = STDERR_STREAM;
    _klog.stream = LOG_STREAM;

    multibootInfo = _multibootInfo;

    kernelSize = &_kernelEnd - &_kernelStart;

    ClearScreen(' ');
    ResetCursor();

    LOG("INFO", "Kernel loaded at address 0x%x - 0x%x (%u bytes long)", &_kernelStart, &_kernelEnd, kernelSize); 

    uint32_t maxKernelSize = (~(uint32_t)&_kernelStart) + 1;
    if (kernelSize >= maxKernelSize)
    {
        LOG("CRITICAL", "Kernel is too big (max %uB)", maxKernelSize); 
        kabort();
    }

    kprintf("Detecting available memory...");

    if(magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) 
    {
        LOG("CRITICAL", "Invalid multiboot magic number (%x)", magicNumber);
        kabort();
    }
    if(!((multibootInfo->flags >> 6) & 1)) 
    {
        LOG("CRITICAL", "Invalid memory map");
        kabort();
    }

    for(uint32_t i = 0; i < multibootInfo->mmap_length; i += sizeof(multiboot_memory_map_t)) 
    {
        multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*)(multibootInfo->mmap_addr + i);

        if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) 
        {
            availableMem += mmmt->len_low;
        }
    }

    LOG("INFO", "Detected %u bytes of usable memory", availableMem); 

    kprintf(" | Done (%u bytes found)\n", availableMem);

    kprintf("Loading a GDT...");
    kmemset(&GDT[0], 0, sizeof(struct GDT_Entry));   // NULL Descriptor
    SetupGDTEntry(&GDT[1], 0, 0xfffff, 0b10011010, 0b1100);  // Kernel mode code segment
    SetupGDTEntry(&GDT[2], 0, 0xfffff, 0b10010010, 0b1100);  // Kernel mode data segment
    SetupGDTEntry(&GDT[3], 0, 0xfffff, 0b11111010, 0b1100);  // User mode code segment
    SetupGDTEntry(&GDT[4], 0, 0xfffff, 0b11110010, 0b1100);  // User mode data segment
    
    kmemset(&TSS, 0, sizeof(struct TSS_Entry));
    TSS.ss0 = KERNEL_DATA_SEGMENT;
    TSS.esp0 = (uint32_t)&stack_top;
    SetupGDTEntry(&GDT[5], (uint32_t)&TSS, sizeof(struct TSS_Entry), 0b10000000 | TSS_TYPE_32BIT_TSS_AVL, 0b1100);  // TSS
    InstallGDT();
    LoadTSS();
    kprintf(" | Done\n");

    LOG("INFO", "Loaded the GDT"); 

    kprintf("Loading an IDT...");
    InstallIDT();
    kprintf(" | Done\n");

    LOG("INFO", "Loaded the IDT"); 

    kprintf("Initializing the PIC...");
    PIC_Remap(32, 32 + 8);
    kprintf(" | Done\n");

    LOG("INFO", "Initialized the PIC"); 

    kprintf("Initializing the PIT...");
    PIT_Channel0_SetFrequency(1000);
    kprintf(" | Done\n");

    LOG("INFO", "Initialized the PIT"); 

    kprintf("Initializing the keyboard...");
    kb_layout = KB_AZERTY;
    PS2_KB_Init();
    PS2_KB_ResetKeyboard();
    PS2_KB_SetScancodeSet(2);
    kprintf(" | Done\n");

    LOG("INFO", "Initialized the keyboard"); 

    LOG("INFO", "Setting up paging"); 

    for (uint16_t i = 256; i < 1024; i++)
        RemovePage(&page_table_0[0], i);

    for (uint16_t i = 1; i < 256; i++)
        for (uint16_t j = 0; j < 1024; j++)
            SetPage(&page_table_768_1023[i][0], j, (j + 4096 * i) * 4096, PAGING_SUPERVISOR_LEVEL, true);

    AddPageTable(1023, (struct PageTable_Entry*)&page_directory, PAGING_SUPERVISOR_LEVEL, true);    // Setup recursive mapping

    ReloadPageDirectory();

    EnableInterrupts(); 
    LOG("INFO", "Enabled interrupts");  

    void A()
    {
        while(true) *(char*)(0xb8000) = 'A';
    }

    void B()
    {
        DisableInterrupts(); // * GPF
        while(true) *(char*)(0xb8000) = 'B';
    }

    void C()
    {
        while(true) *(char*)(0xb8000) = 'C';
    }

    struct Task taskA = CreateTask("Task A", (uint32_t)&A, 0b00), taskB = CreateTask("Task B", (uint32_t)&B, 0b11), taskC = CreateTask("Task C", (uint32_t)&C, 0b00);
    InitMultitasking(&taskA);
    AddTask(&taskB);
    AddTask(&taskC);
    EnableMultitasking();
}