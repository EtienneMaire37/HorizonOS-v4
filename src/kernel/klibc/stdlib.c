#pragma once

void kabort()
{
    kprintf("\nKernel aborted.");
    LOG("ERROR", "Kernel aborted")
    Halt();
}