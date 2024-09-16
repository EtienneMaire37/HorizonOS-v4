#pragma once

#define PIT_CHANNEL_0_DATA  0x40
#define PIT_CHANNEL_1_DATA  0x41
#define PIT_CHANNEL_2_DATA  0x42
#define PIT_MODE_CMD        0x43

uint32_t globalTimer = 0; // in milliseconds seconds

void HandleIRQ0();
void PIT_Channel0_SetFrequency(uint32_t frequency);
void ksleep(uint32_t milliseconds);