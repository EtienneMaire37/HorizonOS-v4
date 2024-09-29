#pragma once

void reset()
{
    // ! Keyboard method 
//     uint8_t status;
//     do
//     {
//         status = inb(PS2_STATUS_REGISTER);
//     } while (status & (1 << 2));

//     outb(PS2_COMMAND_REGISTER, 0xfe);   // Reset CPU

//     Halt();

    // ! Triple fault method
    LOG("WARN", "Rebooting...");

    _idtr.size = 0;
    LoadIDT();

    EnableInterrupts();
}