#pragma once

#define KB_ESCAPE      27
#define KB_BACKSPACE   '\b'
#define KB_RETURN      '\n'
#define KB_LCTRL       (0x0100 + 0)
#define KB_LSHIFT      (0x0100 + 1)
#define KB_RSHIFT      (0x0100 + 2)
#define KB_ALT         (0x0100 + 3)
#define KB_CAPS_LOCK   (0x0100 + 4)
#define KB_F1          (0x0100 + 5)
#define KB_F2          (0x0100 + 6)
#define KB_F3          (0x0100 + 7)
#define KB_F4          (0x0100 + 8)
#define KB_F5          (0x0100 + 9)
#define KB_F6          (0x0100 + 10)
#define KB_F7          (0x0100 + 11)
#define KB_F8          (0x0100 + 12)
#define KB_F9          (0x0100 + 13)
#define KB_F10         (0x0100 + 14)
#define KB_F11         (0x0100 + 15)
#define KB_F12         (0x0100 + 16)
#define KB_NUMLOCK     (0x0100 + 17)
#define KB_SCROLLLOCK  (0x0100 + 18)
#define KB_INSERT      (0x0100 + 27)
#define KB_DELETE      127
#define KB_RCTRL       (0x0100 + 29)
#define KB_é           ((uint8_t)130)
#define KB_è           ((uint8_t)138)
#define KB_ç           ((uint8_t)135)
#define KB_à           ((uint8_t)133)
#define KB_ù           ((uint8_t)151)
#define KB_DEGREE           ((uint8_t)248)
#define KB_POUND           ((uint8_t)156)
#define KB_SECTION           ((uint8_t)21)
#define KB_µ                 ((uint8_t)230)
// #define KB_É           ((uint8_t)130)
// #define KB_È           ((uint8_t)138)
// #define KB_Ç           ((uint8_t)135)
// #define KB_À           ((uint8_t)133)
// #define KB_Ù           ((uint8_t)151)
#define KB_²            ((uint8_t)253)

// Extended keys
#define KB_HOME        (0x0100 + 30)
#define KB_UP          (0x0100 + 31)
#define KB_LEFT        (0x0100 + 32)
#define KB_RIGHT       (0x0100 + 33)
#define KB_DOWN        (0x0100 + 34)
#define KB_PAGEUP      (0x0100 + 35)
#define KB_PAGEDOWN    (0x0100 + 36)
#define KB_END         (0x0100 + 37)

#define KB_ALT_GR      (0x0100 + 39)

#define KB_NUMPAD_0    (0x0100 + '0') // 48
#define KB_NUMPAD_1    (0x0100 + '1')
#define KB_NUMPAD_2    (0x0100 + '2')
#define KB_NUMPAD_3    (0x0100 + '3')
#define KB_NUMPAD_4    (0x0100 + '4')
#define KB_NUMPAD_5    (0x0100 + '5')
#define KB_NUMPAD_6    (0x0100 + '6')
#define KB_NUMPAD_7    (0x0100 + '7')
#define KB_NUMPAD_8    (0x0100 + '8')
#define KB_NUMPAD_9    (0x0100 + '9') // 57

#define KB_LED_SCROLL_LOCK (1 << 0)
#define KB_LED_NUM_LOCK    (1 << 1)
#define KB_LED_CAPS_LOCK   (1 << 2)

#define KB_ACK      0xfa
#define KB_RESEND   0xfe

#define KB_SCANCODE_SET_1   1
#define KB_SCANCODE_SET_2   2
#define KB_SCANCODE_SET_3   3

uint16_t kb_azerty_set2[128] =
{
    0,  KB_ESCAPE, '&', KB_é, '\"', '\'', '(', '-', KB_è, '_', KB_ç, KB_à, ')', '=', '\b', '\t', 
    'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n', KB_LCTRL, 'q', 's', 
    'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', KB_ù, KB_², KB_LSHIFT, '*', 'w', 'x', 'c', 'v', 
    'b', 'n', ',', ';', ':', '!', KB_RSHIFT, '*', KB_ALT, ' ', KB_CAPS_LOCK, KB_F1, KB_F2, KB_F3, KB_F4, KB_F5, 
    KB_F6, KB_F7, KB_F8, KB_F9, KB_F10, KB_NUMLOCK, KB_SCROLLLOCK, KB_NUMPAD_7, KB_NUMPAD_8, KB_NUMPAD_9, '-', KB_NUMPAD_4, KB_NUMPAD_5, KB_NUMPAD_6, '+', KB_NUMPAD_1, 
    KB_NUMPAD_2, KB_NUMPAD_3, KB_NUMPAD_0, '.', 0, 0, '<', KB_F11, KB_F12, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};	

uint16_t kb_azerty_set2_caps[128] =
{
    0,  KB_ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', KB_DEGREE, '+', '\b', '\t', 
    'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 0, KB_POUND, '\n', KB_LCTRL, 'Q', 'S', 
    'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', '%', 0, KB_LSHIFT, KB_µ, 'W', 'X', 'C', 'V', 
    'B', 'N', '?', '.', '/', KB_SECTION, KB_RSHIFT, '*', KB_ALT, ' ', KB_CAPS_LOCK, KB_F1, KB_F2, KB_F3, KB_F4, KB_F5, 
    KB_F6, KB_F7, KB_F8, KB_F9, KB_F10, KB_NUMLOCK, KB_SCROLLLOCK, KB_NUMPAD_7, KB_NUMPAD_8, KB_NUMPAD_9, '-', KB_NUMPAD_4, KB_NUMPAD_5, KB_NUMPAD_6, '+', KB_NUMPAD_1, 
    KB_NUMPAD_2, KB_NUMPAD_3, KB_NUMPAD_0, KB_DELETE, 0, 0, '>', KB_F11, KB_F12, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};	

uint16_t kb_azerty_set2_altgr[128] = 
{
    0,  0, 0, '~', '#', '{', '[', '|', '`', '\\', '^', '@', ']', '}', '\b', '\t', 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, KB_CAPS_LOCK, KB_F1, KB_F2, KB_F3, KB_F4, KB_F5, 
    KB_F6, KB_F7, KB_F8, KB_F9, KB_F10, KB_NUMLOCK, KB_SCROLLLOCK, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, KB_F11, KB_F12, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint16_t kb_azerty_set2_extended[128] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\n', KB_RCTRL, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, '/', 0, 0, KB_ALT_GR, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, KB_UP, 0, 0, KB_LEFT, 0, KB_RIGHT, 0, 0, 
    KB_DOWN, 0, 0, KB_DELETE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

bool kb_state[128] = { false }, kb_state_extended[128] = { false };

enum KB_LAYOUT
{
    //KB_QWERTY,
    KB_AZERTY
};

bool kb_capsLock = false, kb_numLock = false, kb_scrollLock = false;
enum KB_LAYOUT kb_layout = KB_AZERTY;
bool kb_extendedScancode = false;

uint8_t kb_text_key_scancode = 0;
bool kb_text_key_extended = false;
bool kb_text_key_down = false;

void PS2_KB_Init();
bool PS2_KB_ResetKeyboard();
void PS2_KB_SetScancodeSet(uint8_t set);
uint8_t PS2_KB_GetScancodeSet();
void PS2_KB_SetScancodeKeyState(uint8_t scancode, bool extended, bool state);
bool PS2_KB_GetScancodeKeyState(uint8_t scancode, bool extended);
void PS2_KB_SetKeyState(uint16_t key, bool state);
bool PS2_KB_GetKeyState(uint16_t key);
uint16_t PS2_KB_GetKey(uint8_t scancode, bool extended);
uint16_t PS2_KB_GetFormattedKey(uint8_t scancode, bool extended);
uint16_t PS2_KB_GetTextKey();
bool PS2_KB_AnyKeyPressed();
void HandleIRQ1();