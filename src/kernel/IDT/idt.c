#pragma once

void SetupIDTEntry(struct IDT_Entry* entry, uint16_t segment, uint32_t offset, uint8_t privilege, uint8_t type)
{
    entry->segment_selector = segment;

    entry->offset_lo = (offset & 0xffff);
    entry->offset_hi = ((offset >> 16) & 0xffff);

    entry->DPL = privilege;
    entry->gate_type = type;

    entry->present = 1; // Gate is present
    entry->zero = 0;
}

void InstallIDT()
{
    for(uint8_t i = 0; i < 32; i++)
        SetupIDTEntry(&IDT[i], 0x08, INTTable[i], 0, ISR_TRAP_GATE_32);

    for(uint16_t i = 32; i < 256; i++)
        SetupIDTEntry(&IDT[i], 0x08, INTTable[i], 0, ISR_INTERRUPT_GATE_32);

    _idtr.size = sizeof(IDT) - 1;
    _idtr.address = (uint32_t)&IDT;

    PIC_Disable();

    LoadIDT();
}