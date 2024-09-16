#pragma once

void PS2_KB_Init()
{
    kmemset(&kb_state, false, 128);
}

bool PS2_KB_ResetKeyboard()
{
	PS2_Wait();
	outb(PS2_DATA_PORT, 0xff);
    io_wait();
	if(inb(PS2_DATA_PORT) == KB_RESEND)
		return PS2_KB_ResetKeyboard();
	return (inb(PS2_DATA_PORT) == 0xaa);
}

void PS2_KB_SetScancodeSet(uint8_t set)
{
	PS2_Wait();
	outb(PS2_DATA_PORT, 0xf0);
    io_wait();
	outb(PS2_DATA_PORT, set);
    io_wait();
	if(inb(PS2_DATA_PORT) == KB_RESEND)
		PS2_KB_SetScancodeSet(set);
}

uint8_t PS2_KB_GetScancodeSet()
{
	PS2_Wait();
	outb(PS2_DATA_PORT, 0xf0);
    io_wait();
	outb(PS2_DATA_PORT, 0);
	if(inb(PS2_DATA_PORT) == KB_RESEND)
		return PS2_KB_GetScancodeSet();
	uint8_t r = inb(PS2_DATA_PORT);
	return r == 0x43 ? KB_SCANCODE_SET_1 : (r == 0x41 ? KB_SCANCODE_SET_2 : KB_SCANCODE_SET_3 /*3f*/);
}

void PS2_KB_SetScancodeKeyState(uint8_t scancode, bool extended, bool state)
{
    if(extended)
        kb_state_extended[scancode] = state;
    else
        kb_state[scancode] = state;
}

bool PS2_KB_GetScancodeKeyState(uint8_t scancode, bool extended)
{
    if(extended)
        return kb_state_extended[scancode];
    else
        return kb_state[scancode];
}

void PS2_KB_SetKeyState(uint16_t key, bool state)
{
	for(uint8_t i = 0; i < 128; i++)
	{
        if(kb_layout == KB_AZERTY)
        {
            if(kb_azerty_set2[i] == key)
                PS2_KB_SetScancodeKeyState(i, false, state);
        }
        // else if(kb_layout == KB_QWERTY)
        // {
        //     if(kb_qwerty_set2[i] == key)
        //         PS2_KB_SetScancodeKeyState(i, false, state);
        // }
	}

    for(uint8_t i = 0; i < 128; i++)
	{
        if(kb_layout == KB_AZERTY)
        {
            if(kb_azerty_set2_extended[i] == key)
                PS2_KB_SetScancodeKeyState(i, true, state);
        }
        // else if(kb_layout == KB_QWERTY)
        // {
        //     if(kb_qwerty_set2_extended[i] == key)
        //         PS2_KB_SetScancodeKeyState(i, true, state);
        // }
	}
}

bool PS2_KB_GetKeyState(uint16_t key)
{
	bool r = false;
	for(uint8_t i = 0; i < 128; i++)
	{
		if(kb_layout == KB_AZERTY)
        {
            if(kb_azerty_set2[i] == key)
                r |= PS2_KB_GetScancodeKeyState(i, false);
        }
        // else if(kb_layout == KB_QWERTY)
        // {
        //     if(kb_qwerty_set2[i] == key)
        //         r |= PS2_KB_GetScancodeKeyState(i, false);
        // }
	}

    for(uint8_t i = 0; i < 128; i++)
	{
		if(kb_layout == KB_AZERTY)
        {
            if(kb_azerty_set2_extended[i] == key)
                r |= PS2_KB_GetScancodeKeyState(i, true);
        }
        // else if(kb_layout == KB_QWERTY)
        // {
        //     if(kb_qwerty_set2_extended[i] == key)
        //         r |= PS2_KB_GetScancodeKeyState(i, true);
        // }
	}

	return r;
}

uint16_t PS2_KB_GetKey(uint8_t scancode, bool extended)
{
    if(kb_layout == KB_AZERTY)
    {
        if(extended)
            return kb_azerty_set2_extended[scancode];
        return kb_azerty_set2[scancode];
    }
    return 0;
}

uint16_t PS2_KB_GetFormattedKey(uint8_t scancode, bool extended)
{
    if(kb_layout == KB_AZERTY)
    {
        if(extended)
            return kb_azerty_set2_extended[scancode];
        if(kb_capsLock ^ (PS2_KB_GetKeyState(KB_LSHIFT) || PS2_KB_GetKeyState(KB_RSHIFT)))
        {
            if(PS2_KB_GetKeyState(KB_ALT_GR))
                return 0;
            return kb_azerty_set2_caps[scancode];
        }
        else
        {
            if(PS2_KB_GetKeyState(KB_ALT_GR))
                return kb_azerty_set2_altgr[scancode];
            return kb_azerty_set2[scancode];
        }
    }
    return 0;
}

uint16_t PS2_KB_GetTextKey()
{
    kb_text_key_down = false;

    return PS2_KB_GetFormattedKey(kb_text_key_scancode, kb_text_key_extended);
}

bool PS2_KB_AnyKeyPressed()
{
    for(uint8_t i = 0; i < 128; i++)
        if(PS2_KB_GetScancodeKeyState(i, false)) return true;
    for(uint8_t i = 0; i < 128; i++)
        if(PS2_KB_GetScancodeKeyState(i, true)) return true;
    return false;
}

void HandleIRQ1()
{
    uint8_t scancode = inb(PS2_DATA_PORT);

    if(scancode == 0xe0)
    {
        kb_extendedScancode = true;
        return;
    }

    if(scancode >> 7)   // Key released
    {
        PS2_KB_SetScancodeKeyState(scancode - 0x80, kb_extendedScancode, false);
        kb_text_key_down = false;
    }
    else                // Key pressed
    {
        uint16_t key = PS2_KB_GetKey(scancode, kb_extendedScancode);
        PS2_KB_SetScancodeKeyState(scancode, kb_extendedScancode, true);
        if(key == KB_CAPS_LOCK)
        {
            kb_capsLock ^= 1;
            // PS2_KB_SetLEDState(KB_LED_CAPS_LOCK * kb_capsLock);
        }
        else if(key == KB_NUMLOCK)
            kb_numLock ^= 1;
        else if(key == KB_SCROLLLOCK)
            kb_scrollLock ^= 1;
        else if(key < 256 || (kb_numLock && (key == KB_NUMPAD_1 || key == KB_NUMPAD_2 || key == KB_NUMPAD_3 || 
                                                       key == KB_NUMPAD_4 || key == KB_NUMPAD_5 || key == KB_NUMPAD_6 || 
                                                       key == KB_NUMPAD_7 || key == KB_NUMPAD_8 || key == KB_NUMPAD_9 || 
                                                       key == KB_NUMPAD_0)))
        {
            kb_text_key_scancode = scancode;
            kb_text_key_extended = kb_extendedScancode;
            kb_text_key_down = true;
        }
    }

    kb_extendedScancode = false;
}