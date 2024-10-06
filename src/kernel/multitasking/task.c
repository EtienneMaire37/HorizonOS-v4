#pragma once

struct Task CreateTask(char* name, virtual_address_t entry, uint8_t ring)
{
    if (ring != 0b11) ring = 0b00;

    struct Task task;
    task.ring = ring;
    kmemcpy(&task.name[0], name, min(63, kstrlen(name) + 1));
    task.name[63] = 0;

    task.registers.cs = TaskCodeSegment(task);
    task.registers.ds = TaskDataSegment(task);
    task.registers.eip = entry;
    task.registers.eflags = (1 << 9);  // Interrupt enable

    task.page_directory_ptr = AllocatePage();
    InitPageDirectory(task.page_directory_ptr);
    task.registers.cr3 = (uint32_t)VirtualAddressToPhysical((virtual_address_t)task.page_directory_ptr);

    for (uint16_t i = 768; i < 1023; i++)
        AddPageTable(task.page_directory_ptr, i, (struct PageTable_Entry*)(uint32_t)VirtualAddressToPhysical((virtual_address_t)&page_table_768_1023[(i - 768) * 1024]), PAGING_SUPERVISOR_LEVEL, true);  
    
    struct PageTable_Entry* pt_0 = AllocatePage();
    for(uint16_t i = 0; i < 256; i++)
        SetPage(pt_0, i, i * 4096, PAGING_SUPERVISOR_LEVEL, true);
    for(uint16_t i = 256; i < 1024; i++)
        RemovePage(pt_0, i);

    AddPageTable(task.page_directory_ptr, 0, (struct PageTable_Entry*)(uint32_t)VirtualAddressToPhysical((virtual_address_t)pt_0), PAGING_SUPERVISOR_LEVEL, true);  
 
    task.registers.ebp = (uint32_t)&task.stack[STACK_SIZE - 1];
    task.registers.esp = task.registers.ebp;
    task.registers.currEsp = task.registers.ebp;

    return task;
}  

struct Task LoadTaskFromInitrd(char* filename, uint8_t ring)
{
    LOG("INFO", "Loading \"%s\"", filename);
    struct Task task = CreateTask(filename, 0, ring);

    INITRD_FILE* f = Initrd_GetFileInfo(filename);
    if(!f) 
    {
        LOG("ERROR", "Couldn't find file");
        kabort();
    }
    uint8_t* stream = (uint8_t*)(&(f[1]));

    struct ELF32_Header* header = (struct ELF32_Header*)stream;

    bool supported = true;

    LOG("INFO", "ELF Version: %u", header->elf_version);
    LOG("INFO", "ABI: %s", elf_abi[header->abi]);
    LOG("INFO", "Architecture: %ubit", header->architecture == ELF32_ARCHITECTURE_32 ? 32 : 64);
    LOG("INFO", "Type: %s", elf_type[header->type % 5]);
    LOG("INFO", "CPU: %s", elf_machine[header->machine]);
    LOG("INFO", "Endianness: %s", header->endianness == ELF32_LITTLE_ENDIAN ? "Little Endian" : "Big Endian");
    LOG("INFO", "Entry point: 0x%x", header->entry);

    supported &= header->elf[0] == 0x7f;
    supported &= header->elf[1] == 0x45;
    supported &= header->elf[2] == 0x4c;
    supported &= header->elf[3] == 0x46;
    supported &= header->elf_version == 1;
    supported &= header->abi == ELF32_ABI_SYSTEM_V;
    supported &= header->architecture == ELF32_ARCHITECTURE_32;
    // supported &= header->type == ELF32_TYPE_EXECUTABLE; 
    supported &= header->machine == ELF32_MACHINE_X86;
    supported &= header->endianness == ELF32_LITTLE_ENDIAN;
    // supported &= header->entry != 0;

    if(!supported) 
    {
        LOG("ERROR", "ELF file not supported");
        kabort();
    }

    LOG("INFO", "Program header : %u entries", header->ph_num);

    for (uint16_t i = 0; i < header->ph_num; i++)
    {
        struct ELF32_ProgramHeader_Entry* ph = &((struct ELF32_ProgramHeader_Entry*)&stream[header->ph_off])[i];

        LOG("INFO", "Section : off: 0x%x ; vaddr: 0x%x", ph->seg_off, ph->seg_vaddr);
    }

    LOG("INFO", "Section header : %u entries", header->sh_num);

    struct ELF32_SectionHeader_Entry* shstrtab = &((struct ELF32_SectionHeader_Entry*)&stream[header->sh_off])[header->sh_str_idx]; 

    for (uint16_t i = 0; i < header->sh_num; i++)
    {
        struct ELF32_SectionHeader_Entry* sh = &((struct ELF32_SectionHeader_Entry*)&stream[header->sh_off])[i];

        LOG("INFO", "Section %s : off: 0x%x ; addr: 0x%x", (char*)&stream[shstrtab->offset + sh->name], sh->offset, sh->address);
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