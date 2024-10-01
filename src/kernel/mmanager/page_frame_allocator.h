#pragma once

physical_address_t first_page_after_kernel = 0;
uint32_t available_pages = 0;

uint32_t memory_blocks_count = 0;

struct UsableMemoryBlock
{
    physical_address_t address;
    uint32_t page_count;
} __attribute__((packed));

struct UsableMemoryBlock usable_memory_layout[512];

virtual_address_t PhysicalAddressToVirtual(physical_address_t address)
{
    // ! A trick that only works for the higher half addresses
    // TODO: Make it actually compute the address
    return address + 0xc0000000;
}

physical_address_t VirtualAddressToPhysical(virtual_address_t address)
{
    uint32_t _address = address;
    struct VirtualAddressLayout layout = *(struct VirtualAddressLayout*)&_address;
    struct PageDirectory_Entry_4KB pde = page_directory[layout.page_directory_entry];
    struct PageTable_Entry pte = ((struct PageTable_Entry*)(pde.address << 12))[layout.page_table_entry];
    return (pte.address << 12) | layout.page_offset;
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
            if (kEndAddress >= addr && kEndAddress < addr + len)
            {
                physical_address_t address = (kEndAddress + 4096) & 0xfffff000;
                if(IsPageValid(address))
                {
                    first_page_after_kernel = address;
                    return true;
                }
                return false;
            }
            else if (kEndAddress <= addr)
            {
                physical_address_t address = (addr + 4096) & 0xfffff000;
                if(IsPageValid(address))
                {
                    first_page_after_kernel = address;
                    return true;
                }
                return false;
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
            // memory_blocks_count++;
            i += sizeof(multiboot_memory_map_t);
        }
    }

    if (usable_memory_layout[0].address == first_page_after_kernel + 4096)
        usable_memory_layout[0].address = first_page_after_kernel;
    usable_memory_layout[0].page_count++;
}