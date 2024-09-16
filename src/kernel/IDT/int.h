#pragma once

struct IntRegisters
{
    uint32_t edi, esi, ebp;
    uint32_t esp;   // ESP before pushing the other GP registers
    uint32_t ebx, edx, ecx, eax;
    uint32_t interruptNumber, errorCode;
    uint32_t eip;
    uint16_t cs;
    uint32_t eflags;
};

char* errorString[32] = 
{
    "DIVISION_BY_ZERO__ERROR",
    "DEBUG",
    "NON-MASKABLE_INTERRUPT",
    "BREAKPOINT",
    "OVERFLOW",
    "BOUND_RANGE_EXCEEDED",
    "INVALID_OPCODE",
    "DEVICE_NOT_AVAILABLE",
    "DOUBLE_FAULT",
    "COPROCESSOR_SEGMENT_OVERRUN",
    "INVALID_TSS",
    "SEGMENT_NOT_PRESENT",
    "STACK_SEGMENT_FAULT",
    "GENERAL_PROTECTION_FAULT",
    "PAGE_FAULT",
    "",
    "X87_FLOATING_POINT_EXCEPTION",
    "ALIGNMENT_CHECK",
    "MACHINE_CHECK",
    "SIMD_FLOATING_POINT_EXCEPTION",
    "VIRTUALIZATION_EXCEPTION",
    "CONTROL_PROTECTION_EXCEPTION",
    "",
    "",
    "",
    "",
    "",
    "",
    "HYPERVISOR_INJECTION_EXCEPTION",
    "VMM_COMMUNICATION_EXCEPTION",
    "SECURITY_EXCEPTION",
    ""
};

void kernelPanic(uint8_t intNb, uint8_t errorCode);
void InterruptHandler(struct IntRegisters params);