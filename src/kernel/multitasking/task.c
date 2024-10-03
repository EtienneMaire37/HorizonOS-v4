#pragma once

struct Task CreateTask(char* name, virtual_address_t address, uint8_t ring)
{
    if (ring != 0b11) ring = 0b00;
    struct Task task;
    task.ring = ring;
    kmemcpy(&task.name[0], name, min(63, kstrlen(name) + 1));
    task.name[63] = 0;

    task.registers.cs = TaskCodeSegment(task);
    task.registers.ds = TaskDataSegment(task);
    task.registers.eip = address;
    task.registers.eflags = (1 << 9);  // Interrupt enable

    task.page_directory_ptr = AllocatePage();
    InitPageDirectory(task.page_directory_ptr);
    task.registers.cr3 = (uint32_t)VirtualAddressToPhysical((virtual_address_t)task.page_directory_ptr);

    for (uint16_t i = 768; i < 1023; i++)
        AddPageTable(task.page_directory_ptr, i, (struct PageTable_Entry*)(uint32_t)VirtualAddressToPhysical((virtual_address_t)&page_table_768_1023[(i - 768) * 1024]), PAGING_SUPERVISOR_LEVEL, true);  
    AddPageTable(task.page_directory_ptr, 0, (struct PageTable_Entry*)(uint32_t)VirtualAddressToPhysical((virtual_address_t)&page_table_0), PAGING_SUPERVISOR_LEVEL, true);  
 
    task.registers.ebp = (uint32_t)&task.stack[STACK_SIZE - 1];
    task.registers.esp = task.registers.ebp;
    task.registers.currEsp = task.registers.ebp;

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