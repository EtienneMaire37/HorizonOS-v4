#pragma once

void kernelPanic(uint8_t intNb, uint8_t errorCode)
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
    kfprintf(kstdout, "Exception number: %u\n\n\t", intNb);
    kfprintf(kstdout, "Error:       %s\n\t", errorString[intNb]);
    kfprintf(kstdout, "Error code:  0x%x\n\n\t", errorCode);

    LOG("ERROR", "Kernel panic : Exception number : %u ; Error : %s ; Error code = %u", intNb, errorString[intNb], errorCode);

    Halt();
}

void InterruptHandler(struct IntRegisters params)
{
    if(params.interruptNumber < 32)            // ISR
    {
        kernelPanic(params.interruptNumber, params.errorCode);
        return;
    }
    else if(params.interruptNumber < 32 + 16)  // IRQ
    {
        uint8_t irqNumber = params.interruptNumber - 32;

        if(irqNumber == 7 && !(PIC_GetISR() >> 7))
            return;
        if(irqNumber == 15 && !(PIC_GetISR() >> 15))
        {
            outb(PIC1_CMD, PIC_EOI);
	        io_wait();
            return;
        }
        
        // LOG("INFO", "Interrupt %u handled", params.interruptNumber);

        switch (irqNumber)
        {
        case 0:
            HandleIRQ0();
            
            if (multitaskingEnabled)
            {
                taskSwitchCounter--;
                if(!taskSwitchCounter)
                {
                    taskSwitchCounter = TASK_SWITCH_DELAY;
                    currentTask->registers = params;
                    currentTask = currentTask->nextTask;
                    params = currentTask->registers;

                    LOG("INFO", "Switched to task \"%s\"", currentTask->name);
                }
            }

            break;

        case 1:
            HandleIRQ1();
            break;

        default:
            break;
        }

        PIC_SendEOI(irqNumber);

        return;
    }
}