#pragma once

#define STACK_SIZE (16 * KB)

struct Task
{
    char name[64];
    struct IntRegisters registers;
    uint8_t stack[STACK_SIZE];
    uint8_t ring;

    struct Task* previousTask;
    struct Task* nextTask;
};

#define TASK_SWITCH_DELAY 40

uint8_t taskSwitchCounter = TASK_SWITCH_DELAY;
uint16_t taskCount = 0;
bool multitaskingEnabled = false;

struct Task* firstTask;
struct Task* currentTask;

#define TaskCodeSegment(__task) (__task).ring == 0b11 ? USER_CODE_SEGMENT | 0b11 : KERNEL_CODE_SEGMENT
#define TaskDataSegment(__task) (__task).ring == 0b11 ? USER_DATA_SEGMENT | 0b11 : KERNEL_DATA_SEGMENT

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

void DeleteTask(struct Task* task)
{
    task->previousTask->nextTask = task->nextTask;
    task->nextTask->previousTask = task->previousTask;

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
    currentTask = firstTask;
    multitaskingEnabled = true;

    LOG("INFO", "Switched to task \"%s\"", currentTask->name);
    ((void (*)())currentTask->registers.eip)();
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

void DeleteCurrentTask(struct IntRegisters* params)
{
    LOG("INFO", "Killing task \"%s\"", currentTask->name);

    // currentTask = currentTask->previousTask;
    // *params = currentTask->registers;
    // DeleteTask(currentTask->nextTask);
    // TaskSwitch(params);

    DeleteTask(currentTask);
    TaskSwitch(params);
}