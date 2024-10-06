#pragma once

struct Task CreateTask(char* name, virtual_address_t entry, uint8_t ring)
{
    if (ring != 0b11) ring = 0b00;

    struct Task task;
    task.ring = ring;
    // kmemcpy(&task.name[0], name, min(63, kstrlen(name) + 1));
    // task.name[63] = 0;
    task.name = name;

    task.registers.cs = TaskCodeSegment(task);
    task.registers.ds = TaskDataSegment(task);
    task.registers.eip = entry;
    task.registers.eflags = (1 << 9);  // Interrupt enable

    CreateNewVirtualAddressSpace(&task);

    task.registers.ebp = (uint32_t)&task.stack[STACK_SIZE - 1];
    task.registers.esp = task.registers.ebp;
    task.registers.currEsp = task.registers.ebp;

    return task;
}  

struct Task LoadTaskFromInitrd(char* filename, uint8_t ring)
{
    LOG("INFO", "Loading \"%s\"", filename);
    struct Task task = CreateTask(filename, 0, ring);
    // LOG("INFO", "Loading \"%s\"", filename);

    INITRD_FILE* f = Initrd_GetFileInfo(filename);
    if(!f) 
    {
        LOG("ERROR", "   Couldn't find file \"%s\"", filename);
        kabort();
    }
    uint8_t* stream = (uint8_t*)(&(f[1]));

    struct ELF32_Header* header = (struct ELF32_Header*)stream;

    bool supported = true;

    LOG("INFO", "   ELF Version: %u", header->elf_version);
    LOG("INFO", "   ABI: %s", elf_abi[header->abi]);
    LOG("INFO", "   Architecture: %ubit", header->architecture == ELF32_ARCHITECTURE_32 ? 32 : 64);
    LOG("INFO", "   Type: %s", elf_type[header->type % 5]);
    LOG("INFO", "   CPU: %s", elf_machine[header->machine]);
    LOG("INFO", "   Endianness: %s", header->endianness == ELF32_LITTLE_ENDIAN ? "Little Endian" : "Big Endian");
    LOG("INFO", "   Entry point: 0x%x", header->entry);

    supported &= header->elf[0] == 0x7f;
    supported &= header->elf[1] == 0x45;
    supported &= header->elf[2] == 0x4c;
    supported &= header->elf[3] == 0x46;
    supported &= header->elf_version == 1;
    supported &= header->abi == ELF32_ABI_SYSTEM_V;
    supported &= header->architecture == ELF32_ARCHITECTURE_32;
    supported &= header->type == ELF32_TYPE_EXECUTABLE; 
    supported &= header->machine == ELF32_MACHINE_X86;
    supported &= header->endianness == ELF32_LITTLE_ENDIAN;
    supported &= header->entry != 0;

    if(!supported) 
    {
        LOG("ERROR", "   ELF file not supported");
        kabort();
    }

    LOG("INFO", "   Program header : %u entries", header->ph_num);

    for (uint16_t i = 0; i < header->ph_num; i++)
    {
        struct ELF32_ProgramHeader_Entry* ph = &((struct ELF32_ProgramHeader_Entry*)&stream[header->ph_off])[i];

        LOG("INFO", "       Section : off: 0x%x ; vaddr: 0x%x", ph->seg_off, ph->seg_vaddr);
    }

    LOG("INFO", "   Section header : %u entries", header->sh_num);

    struct ELF32_SectionHeader_Entry* shstrtab = &((struct ELF32_SectionHeader_Entry*)&stream[header->sh_off])[header->sh_str_idx]; 

    for (uint16_t i = 0; i < header->sh_num; i++)
    {
        struct ELF32_SectionHeader_Entry* sh = &((struct ELF32_SectionHeader_Entry*)&stream[header->sh_off])[i];

        if (sh->type != ELF32_SH_TYPE_NULL)
        {
            LOG("INFO", "       Section %s : off: 0x%x ; addr: 0x%x ; allocated : %s", (char*)&stream[shstrtab->offset + sh->name], sh->offset, sh->address, (sh->flags & ELF32_SH_FLAG_ALLOC) != 0 ? "true" : "false");
            if (sh->flags & ELF32_SH_FLAG_ALLOC)
            {
                if (sh->address & 0xfff)
                {
                    LOG("ERROR", "Kernel doesn't support non page aligned sections for now");
                    kabort();
                }

                uint32_t pages = (sh->size + 4095) / 4096;
                uint32_t size = sh->size;
                for(uint32_t j = 0; j < pages; j++, size -= 4096)
                {
                    uint32_t address = sh->address + 4096 * j;
                    struct VirtualAddressLayout layout = *(struct VirtualAddressLayout*)&address;
                    void* page = CreateNewPage(&task, layout.page_directory_entry, layout.page_table_entry, ring == 0 ? PAGING_SUPERVISOR_LEVEL : PAGING_USER_LEVEL);
                    kmemcpy(page, &stream[sh->offset + j * 4096], size & 0xfff);
                }
            }
        }
    }

    if (header->entry != 0)
        task.registers.eip = header->entry;

    return task;
}

void AddTask(struct Task* task)
{
    firstTask->previousTask->nextTask = task;
    task->previousTask = firstTask->previousTask;
    task->nextTask = firstTask;
    firstTask->previousTask = task;

    taskCount++;
}

void KillTask(struct Task* task)
{
    task->previousTask->nextTask = task->nextTask;
    task->nextTask->previousTask = task->previousTask;

    FreeVirtualAddressSpace(task);

    taskCount--;
}

void InitMultitasking(struct Task* task)
{
    firstTask = task;
    firstTask->nextTask = firstTask->previousTask = firstTask;

    taskCount = 1;
}

void EnableMultitasking()
{
    firstTaskTrampoline.nextTask = firstTask;
    currentTask = &firstTaskTrampoline;
    multitaskingEnabled = true;

    while(true);
}

void TaskSwitch(struct IntRegisters* params)
{
    taskSwitchCounter--;
    if(!taskSwitchCounter)
    {
        taskSwitchCounter = TASK_SWITCH_DELAY;
        currentTask->registers = *params;
        currentTask = currentTask->nextTask;
        *params = currentTask->registers;
        
        TSS.esp0 = currentTask->registers.esp - sizeof(struct IntRegisters);

        LOG("INFO", "Switched to task \"%s\"", currentTask->name);
    }
}

void KillCurrentTask(struct IntRegisters* params)
{
    LOG("INFO", "Killing task \"%s\"", currentTask->name);

    KillTask(currentTask);
    TaskSwitch(params);
}

void CreateNewVirtualAddressSpace(struct Task* task)
{
    task->page_directory_ptr = AllocatePage();
    InitPageDirectory(task->page_directory_ptr);
    task->registers.cr3 = (uint32_t)VirtualAddressToPhysical((virtual_address_t)task->page_directory_ptr);

    for (uint16_t i = 768; i < 1023; i++)
        AddPageTable(task->page_directory_ptr, i, VirtualAddressToPhysical((virtual_address_t)&page_table_768_1023[(i - 768) * 1024]), PAGING_SUPERVISOR_LEVEL, true);  

    physical_address_t pt_0_address = AllocatePhysicalPage();
    struct PageTable_Entry* pt_0 = (struct PageTable_Entry*)PhysicalAddressToVirtual(pt_0_address);
    for(uint16_t i = 0; i < 256; i++)
        SetPage(pt_0, i, i * 4096, PAGING_SUPERVISOR_LEVEL, true);
    for(uint16_t i = 256; i < 1024; i++)
        RemovePage(pt_0, i);

    AddPageTable(task->page_directory_ptr, 0, pt_0_address, PAGING_SUPERVISOR_LEVEL, true);  
}

void FreeVirtualAddressSpace(struct Task* task)
{
    for(uint16_t i = 0; i < 768; i++)
    {
        struct PageDirectory_Entry_4KB pde = task->page_directory_ptr[i];
        if ((*(uint32_t*)&pde) & 1)
        {
            struct PageTable_Entry* page_table = (struct PageTable_Entry*)PhysicalAddressToVirtual((*(uint32_t*)&pde) & 0xfffff000);
            for(uint16_t j = (i == 0 ? 256 : 0); j < 1024; j++)
            {
                struct PageTable_Entry pte = page_table[j];
                if ((*(uint32_t*)&pte) & 1)
                {
                    FreePhysicalPage(((*(uint32_t*)&pte) & 0xfffff000));
                }
            }
            FreePhysicalPage(((*(uint32_t*)&pde) & 0xfffff000));
        }
    }

    FreePage(task->page_directory_ptr);
}

void CreateNewPageTable(struct Task* task, uint16_t index, uint8_t user_supervisor)
{
    if (task->page_directory_ptr[index].present == 1)
    {
        LOG("CRITICAL", "Kernel tried to allocate an already existing page table");
        kabort();
    }
    physical_address_t pt_address = AllocatePhysicalPage();
    struct PageTable_Entry* pt = (struct PageTable_Entry*)PhysicalAddressToVirtual(pt_address);
    InitPageTable(pt);
    AddPageTable(task->page_directory_ptr, index, pt_address, user_supervisor, true);  
}

void* CreateNewPage(struct Task* task, uint16_t page_table_index, uint16_t page_index, uint8_t user_supervisor)
{
    if (task->page_directory_ptr[page_table_index].present == 0)
        CreateNewPageTable(task, page_table_index, PAGING_USER_LEVEL);

    struct PageTable_Entry* page_table = (struct PageTable_Entry*)PhysicalAddressToVirtual(task->page_directory_ptr[page_table_index].address << 12);
    if (page_table[page_index].present == 1)
    {
        LOG("CRITICAL", "Kernel tried to allocate an already existing page");
        kabort();
    }
    physical_address_t page_address = AllocatePhysicalPage();
    SetPage(page_table, page_index, page_address, user_supervisor, true);  
    return (struct PageTable_Entry*)PhysicalAddressToVirtual(page_address);
}