#pragma once

void InitPageDirectory(struct PageDirectory_Entry_4KB* pd)
{
    for(uint16_t i = 0; i < 1024; i++)
        pd[i].present = 0;
}

void InitPageTable(struct PageTable_Entry* pt)
{
    for(uint16_t i = 0; i < 1024; i++)
        pt[i].present = 0;
}

void AddPageTable(struct PageDirectory_Entry_4KB* pd, uint16_t index, physical_address_t pt_address, bool user_supervisor, bool read_write)
{
    pd[index].page_size = 0;
    pd[index].cache_disable = 0;
    pd[index].write_through = 0;
    pd[index].address = (((uint32_t)pt_address) >> 12);
    pd[index].user_supervisor = user_supervisor;
    pd[index].read_write = read_write;
    pd[index].present = 1;
} 

void RemovePageTable(struct PageDirectory_Entry_4KB* pd, uint16_t index)
{
    pd[index].page_size = 0;
    pd[index].present = 0;
} 

void RemovePage(struct PageTable_Entry* pt, uint16_t index)
{
    pt[index].present = 0;
} 

void SetPage(struct PageTable_Entry* pt, uint16_t index, physical_address_t address, bool user_supervisor, bool read_write)
{
    pt[index].global = 0;
    pt[index].cache_disable = 0;
    pt[index].write_through = 0;
    pt[index].dirty = 0;
    pt[index].address = (address >> 12);
    pt[index].page_attribute_table = 0;
    pt[index].user_supervisor = user_supervisor;
    pt[index].read_write = read_write;
    pt[index].present = 1;
} 

void SetPageByAddress(struct PageDirectory_Entry_4KB* pd, virtual_address_t vaddress, physical_address_t paddress, bool user_supervisor, bool read_write)
{

}

void RemovePageByAddress(struct PageDirectory_Entry_4KB* pd, virtual_address_t vaddress)
{
    struct VirtualAddressLayout* layout = (struct VirtualAddressLayout*)&vaddress;
    RemovePage((struct PageTable_Entry*)(pd[layout->page_directory_entry].address << 12), layout->page_table_entry);
}