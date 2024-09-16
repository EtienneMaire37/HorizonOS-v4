#pragma once

#define STACK_SIZE 16 * KB

struct Task
{
    char name[64];
    struct IntRegisters registers;
    uint8_t stack[STACK_SIZE];
    struct Task* previousTask;
    struct Task* nextTask;
};

#define TASK_SWITCH_DELAY 40

uint8_t taskSwitchCounter = TASK_SWITCH_DELAY;
bool multitaskingEnabled = false;

struct Task* firstTask;
struct Task* currentTask;

struct Task CreateTask(char* name, uint32_t address)
{
    struct Task task;
    kmemcpy(&task.name[0], name, min(63, kstrlen(name)));
    task.name[63] = 0;

    task.registers.cs = CODE_SEGMENT;
    task.registers.eip = address;
    task.registers.eflags = (1 << 9);  // Interrupt enable

    task.registers.ebp = (uint32_t)&task.stack[STACK_SIZE - 1];
    task.registers.esp = task.registers.ebp;

    return task;
}  

void AddTask(struct Task* task)
{
    firstTask->previousTask->nextTask = task;
    task->nextTask = firstTask;
    firstTask->previousTask = task;
}

void InitMultitasking(struct Task* task)
{
    firstTask = task;
    firstTask->nextTask = firstTask->previousTask = firstTask;
}

void EnableMultitasking()
{
    currentTask = firstTask;
    multitaskingEnabled = true;

    ((void (*)())firstTask->registers.eip)();
}