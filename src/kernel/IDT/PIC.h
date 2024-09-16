#pragma once

#define PIC1		    0x20
#define PIC2		    0xA0
#define PIC1_CMD	    (PIC1 + 0)
#define PIC1_DATA	    (PIC1 + 1)
#define PIC2_CMD    	(PIC2 + 0)
#define PIC2_DATA	    (PIC2 + 1)

#define PIC_EOI		    0x20

#define PIC_READ_IRR                0x0a
#define PIC_READ_ISR                0x0b
 
void PIC_SendEOI(uint8_t irq);
void PIC_Disable();
void PIC_Remap(uint8_t master_offset, uint8_t slave_offset);
void PIC_SetMask(uint16_t mask);
uint16_t PIC_GetRegister(int32_t OCW3);
uint16_t PIC_GetIRR();
uint16_t PIC_GetISR();