#pragma once

void kputc(char c)
{
	currentStream = kstdout;
	outc(c);
	UpdateCursor();
}

void kputs(char* str)
{
	currentStream = kstdout;
	while(*str)
	{
		outc(*str);
		
		str++;
	}

	UpdateCursor();
}

void kprintf_d(int32_t val)
{
	if(val < 0)
	{
		val *= -1;
		outc('-');
	}
	if(val < 10)
	{
		outc(val + '0');
	}
	else
	{
		bool first0 = true;
		uint32_t div = 1000000000;
		for(uint8_t i = 0; i < 10; i++)
		{
			uint8_t digit = (val / div) % 10;
			if(digit)
				first0 = false;
			if(!first0)
				outc(digit + '0');
			div /= 10;
		}
	}
}

void kprintf_u(uint32_t val)
{
	if(val < 10)
	{
		outc(val + '0');
	}
	else
	{
		bool first0 = true;
		uint32_t div = 1000000000;
		for(uint8_t i = 0; i < 10; i++)
		{
			uint8_t digit = (val / div) % 10;
			if(digit)
				first0 = false;
			if(!first0)
				outc(digit + '0');
			div /= 10;
		}
	}
}

void kprintf_x(uint32_t val)
{
	if(val < 16)
	{
		outc(hex[val]);
	}
	else
	{
		bool first0 = true;
		uint32_t div = (1 << 28);
		for(uint8_t i = 0; i < 8; i++)
		{
			uint8_t digit = (val / div) & 0x0f;
			if(digit)
				first0 = false;
			if(!first0)
				outc(hex[digit]);
			div >>= 4;
		}
	}
}

void kprintf_lx(uint64_t val)
{
	if(val < 16)
	{
		outc(hex[val]);
	}
	else
	{
		bool first0 = true;
		uint8_t off = 60;
		for(uint8_t i = 0; i < 16; i++)
		{
			uint8_t digit = (val >> off) & 0x0f;
			if (digit)
				first0 = false;
			if (!first0)
				outc(hex[digit]);
			off -= 4;
		}
	}
}

void kprintf_X(uint32_t val)
{
	if(val < 16)
	{
		outc(HEX[val]);
	}
	else
	{
		bool first0 = true;
		uint32_t div = (1 << 28);
		for(uint8_t i = 0; i < 8; i++)
		{
			uint8_t digit = (val / div) & 0x0f;
			if(digit)
				first0 = false;
			if(!first0)
				outc(HEX[digit]);
			div >>= 4;
		}
	}
}

void kprintf_s(char* str)
{
	while(*str)
	{
		outc(*str);
		
		str++;
	}
}

void kfprintf(kFILE* file, char* fmt, ...)
{
	currentStream = file;

	uint32_t* arg = (uint32_t*)&fmt;
	arg++;

	bool next_as_long = false;

	while(*fmt)
	{
		if (*fmt == '%' || next_as_long)
		{
			fmt++;

			switch(*fmt) // TODO: Add %f
			{
			case '%':
				outc('%');
				arg--;

				break;

			case 'l':
				next_as_long = true;
				arg--;
				fmt--;
				break;

			case 's':
				kprintf_s((char*)(*(uint32_t*)arg));

				break;

			case 'c':
				outc(*(char*)arg);

				break;

			case 'd':
				kprintf_d(*(int32_t*)arg);

				break;

			case 'u':
				kprintf_u(*(uint32_t*)arg);

				break;

			case 'x':
				if (next_as_long)
				{
					next_as_long = false;
					kprintf_lx(*(uint64_t*)arg);
					arg++;
				}
				else
					kprintf_x(*(uint32_t*)arg);

				break;

			case 'X':
				kprintf_x(*(uint32_t*)arg);

				break;

			default:
				outc('%');
				outc(*fmt);
				arg--;
			}

			arg++;
		}
		else
			outc(*fmt);
		
		fmt++;
	}

	UpdateCursor();
}

void kprintf(char* fmt, ...)
{
	currentStream = kstdout;

	uint32_t* arg = (uint32_t*)&fmt;
	arg++;

	while(*fmt)
	{
		switch(*fmt)
		{
		case '%':
			fmt++;

			switch(*fmt) // TODO: Add %f
			{
			case '%':
				outc('%');
				arg--;

				break;

			case 's':
				kprintf_s((char*)(*(uint32_t*)arg));

				break;

			case 'c':
				outc(*(char*)arg);

				break;

			case 'd':
				kprintf_d(*(int32_t*)arg);

				break;

			case 'u':
				kprintf_u(*(uint32_t*)arg);

				break;

			case 'x':
				kprintf_x(*(uint32_t*)arg);

				break;

			case 'X':
				kprintf_x(*(uint32_t*)arg);

				break;

			default:
				outc('%');
				outc(*fmt);
				arg--;
			}

			arg++;

			break;

		default:
			outc(*fmt);
		}
		
		fmt++;
	}

	UpdateCursor();
}

void kgets(char* str)
{
	while(PS2_KB_GetKeyState('\n'));

	uint16_t cursor_start_pos = textCursor;
    uint8_t size = 0;
    while(true)
    {
        if(kb_text_key_down)
        {
            uint16_t key = PS2_KB_GetTextKey();
			if(key == '\n')
				break;
            if(!(key == '\b' && cursor_start_pos == textCursor))
            {
                if(key != '\b' && key != '\n' && key != KB_DELETE)
                {
                    str[size] = key;
                    size++;
                    kputc(key);
                }
                else if(key == '\b')
                {
                    size--;
                    kputc('\b');
                }
            }
        }
    }

    str[size] = '\0';

    kputc('\n');
}

