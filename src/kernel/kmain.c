#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "multiboot.h"
multiboot_info_t* multibootInfo;

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)
#define TB (1024 * GB)

typedef uint64_t physical_address_t;
typedef uint32_t virtual_address_t;

extern uint8_t stack_top;

extern uint8_t _kernelStart;
extern uint8_t _kernelEnd;
physical_address_t usableMemoryAddress;
uint32_t kernelSize;

uint32_t availableMem = 0;

extern void _halt();
void Halt();

#define EnableInterrupts()  asm("sti");
#define DisableInterrupts() asm("cli");

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

physical_address_t VirtualAddressToPhysical(virtual_address_t address);
virtual_address_t PhysicalAddressToVirtual(physical_address_t address);

#include "IO/io.h"
#include "PS2/ps2.h"
#include "debug/out.h"

#include "GDT/gdt.h"
#include "IDT/idt.h"
#include "IDT/int.h"
#include "IDT/PIC.h"
#include "PS2/ps2kb.h"
#include "IO/textio.h"
#include "PIT/pit.h"

#include "mmanager/page_frame_allocator.h"

#include "klibc/math.h"
#include "klibc/stdio.h"
#include "klibc/stdlib.h"
#include "klibc/string.h"

#include "paging/paging.h"
#include "multitasking/task.h"

#include "klibc/reset.h"

#include "files/tar.h"
#include "files/elf.h"

#include "initrd/initrd.h"

// ---------------------------------------------------------------

struct PageTable_Entry page_table_0[1024] __attribute__((aligned(4096)));
struct PageTable_Entry page_table_768_1023[256 * 1024] __attribute__((aligned(4096)));

// ---------------------------------------------------------------

#include "GDT/gdt.c"
#include "IDT/idt.c"
#include "IDT/int.c"
#include "IDT/PIC.c"
#include "PS2/ps2kb.c"
#include "IO/textio.c"
#include "PIT/pit.c"

#include "klibc/math.c"
#include "klibc/stdio.c"
#include "klibc/stdlib.c"
#include "klibc/string.c"

#include "paging/paging.c"
#include "multitasking/task.c"

#include "mmanager/page_frame_allocator.c"

#include "initrd/initrd.c"

void Halt()
{
    LOG(WARNING, "Kernel halted");
    _halt();
}

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
    usableMemoryAddress = VirtualAddressToPhysical((virtual_address_t)&_kernelEnd);

    ClearScreen(' ');
    ResetCursor();

    LOG(INFO, "Kernel loaded at address 0x%x - 0x%x (%u bytes long)", &_kernelStart, &_kernelEnd, kernelSize); 

    uint32_t maxKernelSize = (uint32_t)(-(uint32_t)&_kernelStart);
    if (kernelSize >= maxKernelSize)
    {
        LOG(CRITICAL, "Kernel is too big (max %uB)", maxKernelSize); 
        kabort();
    }

    kprintf("Detecting available memory...");

    if(magicNumber != MULTIBOOT_BOOTLOADER_MAGIC) 
    {
        LOG(CRITICAL, "Invalid multiboot magic number (%x)", magicNumber);
        kabort();
    }
    if(!((multibootInfo->flags >> 6) & 1)) 
    {
        LOG(CRITICAL, "Invalid memory map");
        kabort();
    }

    LOG(INFO, "Memory map:");
    for (uint32_t i = 0; i < multibootInfo->mmap_length; i += sizeof(multiboot_memory_map_t)) 
    {
        multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*)(multibootInfo->mmap_addr + i);
        physical_address_t addr = ((physical_address_t)mmmt->addr_high << 8) | mmmt->addr_low;
        uint32_t len = mmmt->len_low;
        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) 
        {
            LOG(INFO, "   Memory block : address : 0x%lx ; length : %u", addr, len);
            availableMem += len;
        }   
    }

    LOG(INFO, "Detected %u bytes of usable memory", availableMem); 

    kprintf(" | Done (%u bytes found)\n", availableMem);

    multiboot_module_t* initrd = (multiboot_module_t*)multibootInfo->mods_addr;
    initrd_address = PhysicalAddressToVirtual((physical_address_t)initrd->mod_start);
    initrd_size = initrd->mod_end - initrd->mod_start;

    LOG(DEBUG, "Initrd loaded at address 0x%x (%u bytes long)", initrd_address, initrd_size);

    if (initrd->mod_start + initrd_size > usableMemoryAddress)
        usableMemoryAddress = initrd->mod_start + initrd_size;

    kprintf("Loading a GDT...");
    kmemset(&GDT[0], 0, sizeof(struct GDT_Entry));   // NULL Descriptor
    SetupGDTEntry(&GDT[1], 0, 0xfffff, 0b10011011, 0b1100);  // Kernel mode code segment
    SetupGDTEntry(&GDT[2], 0, 0xfffff, 0b10010011, 0b1100);  // Kernel mode data segment
    SetupGDTEntry(&GDT[3], 0, 0xfffff, 0b11111011, 0b1100);  // User mode code segment
    SetupGDTEntry(&GDT[4], 0, 0xfffff, 0b11110011, 0b1100);  // User mode data segment
    
    kmemset(&TSS, 0, sizeof(struct TSS_Entry));
    TSS.iopb = sizeof(struct TSS_Entry);
    TSS.ss0 = KERNEL_DATA_SEGMENT;
    TSS.esp0 = (uint32_t)&stack_top;
    SetupGDTEntry(&GDT[5], (uint32_t)&TSS, sizeof(struct TSS_Entry), 0b10000000 | TSS_TYPE_32BIT_TSS_AVL, 0b0100);  // TSS
    InstallGDT();
    LoadTSS();
    kprintf(" | Done\n");

    LOG(DEBUG, "Loaded the GDT"); 

    kprintf("Loading an IDT...");
    InstallIDT();
    kprintf(" | Done\n");

    LOG(DEBUG, "Loaded the IDT"); 

    kprintf("Initializing the PIC...");
    PIC_Remap(32, 32 + 8);
    kprintf(" | Done\n");

    LOG(DEBUG, "Initialized the PIC"); 

    kprintf("Initializing the PIT...");
    PIT_Channel0_SetFrequency(1000);
    kprintf(" | Done\n");

    LOG(DEBUG, "Initialized the PIT"); 

    kprintf("Initializing the keyboard...");
    kb_layout = KB_AZERTY;
    PS2_KB_Init();
    PS2_KB_ResetKeyboard();
    PS2_KB_SetScancodeSet(2);
    kprintf(" | Done\n");

    LOG(DEBUG, "Initialized the keyboard"); 

    LOG(DEBUG, "Setting up paging"); 

    for (uint16_t i = 256; i < 1024; i++)
        RemovePage(&page_table_0[0], i);

    for (uint16_t i = 1; i < 256; i++)
    {
        for (uint16_t j = 0; j < 1024; j++)
        {
            struct VirtualAddressLayout layout;
            layout.page_directory_entry = i + 768;
            layout.page_table_entry = j;
            layout.page_offset = 0;
            uint32_t address = *(uint32_t*)&layout + 0xc0000000;
            SetPage(&page_table_768_1023[i * 1024], j, address, PAGING_SUPERVISOR_LEVEL, true);
        }
    }

    for (uint16_t i = 769; i < 1023; i++)
        AddPageTable(page_directory, i, VirtualAddressToPhysical((virtual_address_t)&page_table_768_1023[(i - 768) * 1024]), PAGING_SUPERVISOR_LEVEL, true);  

    AddPageTable(page_directory, 1023, VirtualAddressToPhysical((virtual_address_t)&page_directory), PAGING_SUPERVISOR_LEVEL, true);    // Setup recursive mapping

    ReloadPageDirectory();

    EnableInterrupts(); 
    LOG(DEBUG, "Enabled interrupts");  

    if (!DetectFirstAvailablePage())
    {
        LOG(CRITICAL, "Not enough memory to run HorizonOS");
        kabort();
    }

    DetectRemainingPages();

    LOG(DEBUG, "First page after kernel : 0x%x", first_page_after_kernel);  
    LOG(INFO, "Available pages : %u", available_pages);  
    
    kprintf("Usable memory : %u bytes\n", available_pages * 4096);
    LOG(INFO, "Usable memory : %u bytes", available_pages * 4096);  

    LOG(INFO, "Usable memory layout :");  
    for (uint16_t i = 0; i < memory_blocks_count; i++)
        LOG(INFO, "Block : 0x%lx ; %u pages", usable_memory_layout[i].address, usable_memory_layout[i].page_count);  

    SetupMemAllocations();

    Initrd_ListFiles();

    struct Task taskA = LoadTaskFromInitrd("src/initrd/A.elf", 0b00, true);
    struct Task taskB = LoadTaskFromInitrd("src/initrd/B.elf", 0b11, true);
    
    InitMultitasking(&taskA);
    AddTask(&taskB);

    struct PageTable_Entry* APageTable1 = (struct PageTable_Entry*)PhysicalAddressToVirtual(taskA.page_directory_ptr[1].address << 12);
    struct PageTable_Entry* BPageTable1 = (struct PageTable_Entry*)PhysicalAddressToVirtual(taskB.page_directory_ptr[1].address << 12);
    
    LOG(DEBUG, "A pd ring : %u", (taskA.page_directory_ptr[1].user_supervisor * 3));
    LOG(DEBUG, "A pt ring : %u", (APageTable1[0].user_supervisor * 3));
    LOG(DEBUG, "B pd ring : %u", (taskB.page_directory_ptr[1].user_supervisor * 3));
    LOG(DEBUG, "A pt ring : %u", (APageTable1[0].user_supervisor * 3));

    LOG(DEBUG, "A pde 1 : 0x%x", *(uint32_t*)&taskA.page_directory_ptr[1]);
    LOG(DEBUG, "B pde 1 : 0x%x", *(uint32_t*)&taskB.page_directory_ptr[1]);

    LOG(DEBUG, "A pte 1024 : 0x%x", *(uint32_t*)&APageTable1[0]);
    LOG(DEBUG, "B pte 1024 : 0x%x", *(uint32_t*)&BPageTable1[0]);
    
    EnableMultitasking();
}