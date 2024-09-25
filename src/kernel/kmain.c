#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "multiboot.h"
multiboot_info_t* multibootInfo;

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)
#define TB (1024 * GB)

#define CODE_SEGMENT 0x08
#define DATA_SEGMENT 0x10

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
#include "klibc/reboot.h"

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

uint32_t page_table_0[1024] __attribute__((aligned(4096)));
uint32_t page_table_768[1024] __attribute__((aligned(4096)));

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

    if (kernelSize >= 0x400000)
    {
        LOG("CRITICAL", "Kernel is too big (max 4MB)"); 
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
    SetupGDTEntry(&GDT[1], 0, 0xfffff, 0x9a, 0xc);  // Kernel mode code segment
    SetupGDTEntry(&GDT[2], 0, 0xfffff, 0x92, 0xc);  // Kernel mode data segment
    InstallGDT();
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

    // *((uint8_t*)0xb000000) = 0; // PAGE FAULT

    // LOG("INFO", "Setting up paging"); 

    // // Map kernel to higher half (0xC0000000)

    // InitPageDirectory();

    // for (uint16_t i = 0; i < 256; i++)                                          // First megabyte
    //     SetPage(&page_table_0[0], i, i * 4096, PAGING_SUPERVISOR_LEVEL, true);
    // for (uint16_t i = 256; i < 1024; i++)                                       // First 4 megabytes
    //     SetPage(&page_table_0[0], i, i * 4096, PAGING_SUPERVISOR_LEVEL, true);

    // for (uint32_t i = 0; i < 1024; i++)                                         // 0xC0000000 - 0xC03FFFFF
    //     SetPage(&page_table_768[0], i, i * 4096 + (uint32_t)&_kernelStart, PAGING_SUPERVISOR_LEVEL, true);

    // AddPageTable(0, &page_table_0[0], PAGING_SUPERVISOR_LEVEL, true);
    // AddPageTable(768, &page_table_768[0], PAGING_SUPERVISOR_LEVEL, true);

    // ReloadPageDirectory();
    // EnablePaging();

    // while(true);

    // JumpToHigherHalf();
}

void HigherHalf()
{
    for (uint16_t i = 256; i < 1024; i++)                                       // 0x100000 - 0x3fffff
        RemovePage(&page_table_0[0], i);

    ReloadPageDirectory();

    EnableInterrupts(); 
    LOG("INFO", "Enabled interrupts"); 

    // void A()
    // {
    //     while(true) *(char*)(0xb8000) = 'A';
    // }

    // void B()
    // {
    //     while(true) *(char*)(0xb8000) = 'B';
    // }

    // struct Task taskA = CreateTask("Task A", (uint32_t)&A), taskB = CreateTask("Task B", (uint32_t)&B);
    // InitMultitasking(&taskA);
    // AddTask(&taskB);
    // EnableMultitasking();

    while(true);
}