#pragma once

virtual_address_t PhysicalAddressToVirtual(physical_address_t address)
{
    //! Cant work because it needs the physical address to compute itself
    // for (uint32_t i = 0; i < 1024; i++)
    // {
    //     for (uint16_t j = 0; j < 1024; j++)
    //     {
    //         struct PageDirectory_Entry_4KB pde = page_directory[i];
    //         struct PageTable_Entry pte = ((struct PageTable_Entry*)(pde.address << 12))[j];
    //         physical_address_t pte_address = pte.address << 12;
    //         if (address >= pte_address && address < pte_address + 4096)
    //         {
    //             struct VirtualAddressLayout layout;
    //             layout.page_directory_entry = i;
    //             layout.page_table_entry = j;
    //             layout.page_offset = address & 0xfff;
    //             return *(virtual_address_t*)&layout;
    //         }
    //     }
    // }
    // return 0;

    if(address < 0x100000)
        return (physical_address_t)address;
    if(address >= 0x100000 && address < 0x100000 + (uint32_t)(-0xc0000000))
        return (physical_address_t)address + 0xc0000000;
    return 0;
}

physical_address_t VirtualAddressToPhysical(virtual_address_t address)
{
    // uint32_t _address = address;
    // struct VirtualAddressLayout layout = *(struct VirtualAddressLayout*)&_address;
    // struct PageDirectory_Entry_4KB pde = page_directory[layout.page_directory_entry];
    // struct PageTable_Entry pte = ((struct PageTable_Entry*)PhysicalAddressToVirtual((physical_address_t)pde.address << 12))[layout.page_table_entry];
    // return (pte.address << 12) | layout.page_offset;
    if(address >= 0xc0000000)
        return (physical_address_t)address - 0xc0000000;
    if(address < 0x100000)
        return (physical_address_t)address;
    return 0;
}

bool IsPageValid(physical_address_t address)
{
    if ((address & 0xfff) != 0) return false;

    for(uint32_t i = 0; i < multibootInfo->mmap_length; i += sizeof(multiboot_memory_map_t)) 
    {
        multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*)(multibootInfo->mmap_addr + i);

        physical_address_t addr = ((physical_address_t)mmmt->addr_high << 8) | mmmt->addr_low;
        uint32_t len = mmmt->len_low;
 
        if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) 
        {
            if(address >= addr && address + 4096 < addr + len)
                return true;
        }
    }
    return false;
}

bool DetectFirstAvailablePage()
{
    available_pages = 0;
    first_page_after_kernel = 0;
    memory_blocks_count = 0;

    for (uint32_t i = 0; i < multibootInfo->mmap_length; i += sizeof(multiboot_memory_map_t)) 
    {
        multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*)(multibootInfo->mmap_addr + i);

        physical_address_t addr = ((physical_address_t)mmmt->addr_high << 8) | mmmt->addr_low;
        uint32_t len = mmmt->len_low;
 
        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) 
        {
            if (usableMemoryAddress >= addr && usableMemoryAddress < addr + len)
            {
                physical_address_t address = (usableMemoryAddress + 4096) & 0xfffff000;
                if(IsPageValid(address))
                {
                    first_page_after_kernel = address;
                    return true;
                }
            }
            else if (usableMemoryAddress <= addr)
            {
                physical_address_t address = (addr + 4096) & 0xfffff000;
                if(IsPageValid(address))
                {
                    first_page_after_kernel = address;
                    return true;
                }
            }
        }
    }

    return false;
}

void DetectRemainingPages()
{
    available_pages = 1;
    memory_blocks_count = 0;
    bool same_memory_block = false;
    physical_address_t current_page = first_page_after_kernel;

    for (uint32_t i = 0; i < multibootInfo->mmap_length;) 
    {
        multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*)(multibootInfo->mmap_addr + i);

        physical_address_t addr = ((physical_address_t)mmmt->addr_high << 8) | mmmt->addr_low;
        uint32_t len = mmmt->len_low;   // Cant have 64b blocks in a 32b system
 
        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) 
        {
            if (current_page < addr)
                current_page = (addr + 4096) & 0xfffff000;

            if (current_page >= addr && current_page + 4096 < addr + len)
            {
                if (IsPageValid(current_page))
                {
                    available_pages++;
                    if (!same_memory_block)
                    {
                        usable_memory_layout[memory_blocks_count].address = current_page;
                        usable_memory_layout[memory_blocks_count].page_count = 1;
                        same_memory_block = true;
                    }
                    else
                        usable_memory_layout[memory_blocks_count].page_count++;
                }
            }
            current_page += 4096;
            if (current_page >= addr + len)
            {
                same_memory_block = false;
                if (usable_memory_layout[memory_blocks_count].page_count != 0)
                    memory_blocks_count++;
                i += sizeof(multiboot_memory_map_t);
            }
        }
        else
        {
            same_memory_block = false;
            i += sizeof(multiboot_memory_map_t);
        }
    }

    if (usable_memory_layout[0].address == first_page_after_kernel + 4096)
        usable_memory_layout[0].address = first_page_after_kernel;
    usable_memory_layout[0].page_count++;
}

physical_address_t GetPageAddress(uint32_t index)
{
    for (uint16_t i = 0; i < memory_blocks_count; i++)
    {
        if (index < usable_memory_layout[i].page_count)
            return usable_memory_layout[i].address + 4096 * index;
        index -= usable_memory_layout[i].page_count;
    }
    return 0;
}

uint32_t GetPageIndex(physical_address_t address)
{
    if ((address & 0xfff) != 0) return 0;
    for (uint16_t i = 0; i < memory_blocks_count; i++)
    {
        if (address >= usable_memory_layout[i].address && address < usable_memory_layout[i].address + 4096 * usable_memory_layout[i].page_count)
            return (usable_memory_layout[i].address - address) / 4096;
    }
    return 0;   // Invalid since it references the bitmap
}

void PFA_SetPage(uint32_t index, bool isFree)
{
    if (index >= available_pages)
        return;

    physical_address_t page = GetPageAddress(index / (4096 * 8));
    uint16_t offset = (index / 8) % 4096;
    uint8_t bit = index % 8;

    if (isFree)
        *(uint8_t*)PhysicalAddressToVirtual(page + offset) |= (1 << bit);
    else
        *(uint8_t*)PhysicalAddressToVirtual(page + offset) &= ~(uint8_t)(1 << bit);
}

bool PFA_GetPage(uint32_t index)
{
    if (index >= available_pages)
        return false;

    physical_address_t page = GetPageAddress(index / (4096 * 8));
    uint16_t offset = (index / 8) % 4096;
    uint8_t bit = index % 8;

    return (*(uint8_t*)PhysicalAddressToVirtual(page + offset) >> bit) & 1;
}

void SetupMemAllocations()
{
    memory_bitmap = (uint8_t*)PhysicalAddressToVirtual(first_page_after_kernel);
    uint8_t bitmap_length = ((available_pages + 7) / 8 + 4095) / 4096; // * In pages
    // LOG(INFO, "Bitmap length : %u pages", bitmap_length);
    for (uint32_t i = 0; i < bitmap_length; i++)
        PFA_SetPage(i, false);
    for (uint32_t i = bitmap_length; i < available_pages; i++)
        PFA_SetPage(i, true);
}

physical_address_t AllocatePhysicalPage()
{
    for (uint32_t i = 0; i < available_pages; i++)
    {
        if (PFA_GetPage(i)) // Page is free
        {
            PFA_SetPage(i, false);
            return GetPageAddress(i);
        }
    }
    return 0;
}

void* AllocatePage()
{
    return (void*)PhysicalAddressToVirtual(AllocatePhysicalPage());
}

void FreePhysicalPage(physical_address_t address)
{
    uint32_t index = GetPageIndex(address);
    if (PFA_GetPage(index))
    {
        LOG(CRITICAL, "Kernel tried to free an already free page");
        kabort();
    }
    PFA_SetPage(index, true);
}

void FreePage(void* page)
{
    FreePhysicalPage(VirtualAddressToPhysical((virtual_address_t)page));
}