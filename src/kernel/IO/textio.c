#pragma once

void ShowCursor(uint8_t scanlineStart, uint8_t scanlineEnd)
{
	outb(0x3d4, 0x0a);
	outb(0x3d5, (inb(0x3d5) & 0xc0) | scanlineStart);
 
	outb(0x3d4, 0x0b);
	outb(0x3d5, (inb(0x3d5) & 0xe0) | scanlineEnd);
}

void HideCursor()
{
	outb(0x3d4, 0x0a);
	outb(0x3d5, 0x20);
}

void ResetCursor()
{
	ShowCursor(14, 15);
}

void SetCursorPos(uint16_t pos)
{
	outb(0x3d4, 0x0f);
	outb(0x3d5, (pos & 0xff));
	outb(0x3d4, 0x0e);
	outb(0x3d5, ((pos >> 8) & 0xff));
}

void UpdateCursor()
{
	// textCursor %= 80 * 25;
	SetCursorPos(textCursor);
}

void ClearScreen(char c)
{
	for(uint16_t i = 0; i < 80 * 25; i++)
	{
        VRAM[i]._char = c;
		VRAM[i].color = textColor;
	}

	// textCursor = 0;
	// UpdateCursor();
}

void outc(char c)
{
	switch(currentStream->stream)
	{
	case LOG_STREAM:
		Debug_putc(c);
		break;
		
	case STDOUT_STREAM:
	{
		switch(c)
		{
		case '\n':
			textCursor += 80;

		case '\r':
			textCursor = (int)(textCursor / 80) * 80;

			break;

		case '\b':
			textCursor--;
			outc(' ');
			textCursor--;

			break;

		case '\t':
		{
			// for(uint8_t i = 0; i < TAB_LENGTH; i++)
			uint8_t firstTabX = (textCursor % 80) / TAB_LENGTH;
			while(firstTabX == (textCursor % 80) / TAB_LENGTH)
				outc(' ');

			break;
		}

		default:
			VRAM[textCursor]._char = c;
			VRAM[textCursor].color = textColor;

			textCursor++;
		}

		while((textCursor / 80) >= 25)	// Last line
		{
			kmemcpy(&VRAM[0], &VRAM[80], 80 * 25 * sizeof(struct Character));
			
			for(uint8_t i = 0; i < 80; i++)
			{
				VRAM[24 * 80 + i]._char = ' ';
				VRAM[24 * 80 + i].color = FG_WHITE | BG_BLACK;
			}

			textCursor -= 80;
		}

		break;
	}
	default:
		;
	}
}