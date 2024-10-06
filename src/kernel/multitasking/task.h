#pragma once

#define STACK_SIZE (16 * KB)

struct Task
{
    char* name;
    struct IntRegisters registers;
    uint8_t stack[STACK_SIZE];
    struct PageDirectory_Entry_4KB* page_directory_ptr;
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
struct Task firstTaskTrampoline;

#define TaskCodeSegment(__task)         (__task).ring == 0b11 ? USER_CODE_SEGMENT | 0b11 : KERNEL_CODE_SEGMENT
#define TaskDataSegment(__task)         (__task).ring == 0b11 ? USER_DATA_SEGMENT | 0b11 : KERNEL_DATA_SEGMENT
#define TaskPagingPrivilege(__task)     (__task).ring == 0b11 ? PAGING_USER_LEVEL : PAGING_SUPERVISOR_LEVEL

struct Task CreateTask(char* name, virtual_address_t address, uint8_t ring);
void AddTask(struct Task* task);
void KillTask(struct Task* task);
void InitMultitasking(struct Task* task);
void EnableMultitasking();
void TaskSwitch(struct IntRegisters* params);
void FreeVirtualAddressSpace(struct Task* task);
void CreateNewVirtualAddressSpace(struct Task* task);
void CreateNewPageTable(struct Task* task, uint16_t index, uint8_t user_supervisor);
void* CreateNewPage(struct Task* task, uint16_t page_table_index, uint16_t page_index, uint8_t user_supervisor);
void KillCurrentTask(struct IntRegisters* params);