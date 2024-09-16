#pragma once

void HandleIRQ0()
{
    globalTimer++;
}

void PIT_Channel0_SetFrequency(uint32_t frequency)
{
    uint32_t divisor = 1193180 / frequency;
    outb(PIT_MODE_CMD, 0x36);            
    io_wait();
    outb(PIT_CHANNEL_0_DATA, divisor & 0xff);   
    io_wait();
    outb(PIT_CHANNEL_0_DATA, divisor >> 8);   
    io_wait(); 
}

void ksleep(uint32_t milliseconds)
{
    uint32_t targetTimer = globalTimer + milliseconds;

    while(targetTimer > globalTimer);
}