#pragma once

void kabort()
{
    kprintf("\nKernel aborted.");
    kfprintf(klog, "Kernel aborted.\n");
    Halt();
}