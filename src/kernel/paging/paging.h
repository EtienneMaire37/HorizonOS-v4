#pragma once

#define PAGING_SUPERVISOR_LEVEL     0
#define PAGING_USER_LEVEL           1

struct PageDirectory_Entry_4KB
{
    uint32_t address : 20;  // High bits
    uint8_t avl_0 : 4;
    uint8_t page_size : 1; // 0 for 4 KB entry
    uint8_t avl_1 : 1;
    uint8_t accessed : 1;
    uint8_t cache_disable : 1;
    uint8_t write_through : 1;
    uint8_t user_supervisor : 1; // 0: Supervisor ; 1: User
    uint8_t read_write : 1;     // 0: Read only ; 1: Read-write
    uint8_t present : 1;
};

struct PageTable_Entry
{
    uint32_t address : 20;  // High bits
    uint8_t avl_0 : 3;
    uint8_t global : 1; 
    uint8_t page_attribute_table : 1;
    uint8_t dirty : 1;
    uint8_t accessed : 1;
    uint8_t cache_disable : 1;
    uint8_t write_through : 1;
    uint8_t user_supervisor : 1; 
    uint8_t read_write : 1;    
    uint8_t present : 1;
};

struct PageDirectory_Entry_4KB  page_directory[1024] __attribute__((aligned(4096)));
struct PageTable_Entry          page_table_0[1024] __attribute__((aligned(4096)));

void InitPageDirectory()
{
    for(uint16_t i = 0; i < 1024; i++)
    {
        page_directory[i].page_size = 0;
        page_directory[i].present = 0;
    }
}

void AddPageTable(uint16_t index, struct PageTable_Entry* entry, bool user_supervisor, bool read_write)
{
    page_directory[index].page_size = 0;
    page_directory[index].cache_disable = 0;
    page_directory[index].write_through = 0;
    page_directory[index].address = (((uint32_t)&entry) >> 12);
    page_directory[index].user_supervisor = user_supervisor;
    page_directory[index].read_write = read_write;
    page_directory[index].present = 1;
} 

void SetPage(struct PageTable_Entry* pt, uint16_t index, uint32_t address, bool user_supervisor, bool read_write)
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

extern void LoadPageDirectory(struct PageDirectory_Entry_4KB* first_entry);
extern void EnablePaging(); 