#pragma once

void reboot()
{
    uint8_t status;
    do
    {
        status = inb(PS2_STATUS_REGISTER);
    } while (status & (1 << 2));

    outb(PS2_COMMAND_REGISTER, 0xfe);   // Reset CPU

    Halt();
}