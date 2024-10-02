#pragma once

physical_address_t first_page_after_kernel = 0;
uint32_t available_pages = 0;

uint32_t memory_blocks_count = 0;

uint8_t* memory_bitmap = NULL;

struct UsableMemoryBlock
{
    physical_address_t address;
    uint32_t page_count;
} __attribute__((packed));

struct UsableMemoryBlock usable_memory_layout[512];

virtual_address_t PhysicalAddressToVirtual(physical_address_t address);
physical_address_t VirtualAddressToPhysical(virtual_address_t address);
bool IsPageValid(physical_address_t address);
bool DetectFirstAvailablePage();
void DetectRemainingPages();
physical_address_t GetPageAddress(uint32_t index);
uint32_t GetPageIndex(physical_address_t address);
void PFA_SetPage(uint32_t index, bool isFree);
bool PFA_GetPage(uint32_t index);
void SetupMemAllocations();
void* AllocatePage();
void FreePage(void* page);