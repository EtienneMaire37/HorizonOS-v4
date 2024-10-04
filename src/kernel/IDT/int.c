#pragma once

void kernelPanic(struct IntRegisters params)
{
    DisableInterrupts();

    textColor = BG_BLUE;
    ClearScreen(' ');

    textCursor = 4 + 2 * 80;
    UpdateCursor();
    HideCursor();

    textColor = (FG_LIGHTRED | BG_BLUE);
    kfprintf(kstdout, "Kernel panic\n\n\t");

    textColor = (FG_WHITE | BG_BLUE);

    kfprintf(kstdout, "Exception number: %u\n\n\t", params.interruptNumber);
    kfprintf(kstdout, "Error:       %s\n\t", errorString[params.interruptNumber]);
    kfprintf(kstdout, "Error code:  0x%x\n\n\t", params.errorCode);

    kfprintf(kstdout, "cr2:  0x%x\n", params.cr2);

    // LOG("ERROR", "Kernel panic : Exception number : %u ; Error : %s ; Error code = 0x%x", params.interruptNumber, errorString[params.interruptNumber], params.errorCode);

    Halt();
}

#define ReturnFromISR() { return TaskDataSegment(*currentTask); }

uint32_t InterruptHandler(struct IntRegisters params)
{
    if(params.interruptNumber < 32)            // Fault
    {
        LOG("ERROR", "Fault : Exception number : %u ; Error : %s ; Error code = 0x%x ; cr2 = 0x%x", params.interruptNumber, errorString[params.interruptNumber], params.errorCode, params.cr2);

        if (currentTask->ring == 0b00)
            kernelPanic(params);
        else
            KillCurrentTask(&params);
    }
    else if(params.interruptNumber < 32 + 16)  // IRQ
    {
        uint8_t irqNumber = params.interruptNumber - 32;

        if(irqNumber == 7 && !(PIC_GetISR() >> 7))
            ReturnFromISR();
        if(irqNumber == 15 && !(PIC_GetISR() >> 15))
        {
            outb(PIC1_CMD, PIC_EOI);
	        io_wait();
            ReturnFromISR();
        }
        
        // LOG("INFO", "Interrupt %u handled", params.interruptNumber);

        switch (irqNumber)
        {
        case 0:
            HandleIRQ0();
            
            if (multitaskingEnabled)
                TaskSwitch(&params);

            break;

        case 1:
            HandleIRQ1();
            break;

        default:
            break;
        }

        PIC_SendEOI(irqNumber);
    }
    else if (params.interruptNumber == 0xff) // System call
    {
        kputc((char)params.eax);
    }
    ReturnFromISR();
}