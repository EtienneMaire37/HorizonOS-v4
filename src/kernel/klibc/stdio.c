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

void kprintf_x(uint64_t val, uint8_t padding)
{
	bool first0 = true;
	uint8_t off = 60;
	for(uint8_t i = 0; i < 16; i++)
	{
		uint8_t digit = (val >> off) & 0x0f;
		if (digit || (off >> 2) <= padding)
			first0 = false;
		if (!first0)
			outc(hex[digit]);
		off -= 4;
	}
}

void kprintf_X(uint64_t val, uint8_t padding)
{
	bool first0 = true;
	uint8_t off = 60;
	for(uint8_t i = 0; i < 16; i++)
	{
		uint8_t digit = (val >> off) & 0x0f;
		if (digit || (off >> 2) <= padding)
			first0 = false;
		if (!first0)
			outc(HEX[digit]);
		off -= 4;
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
	uint8_t padding = 0;

	while(*fmt)
	{
		if (*fmt == '%' || next_as_long || padding != 0)
		{
			fmt++;

			if(*fmt >= '0' && *fmt <= '9')
			{
				padding = (10 * (*fmt - '0')) + *(fmt + 1) - '0';
				arg--;
			}
			else	
			{
				switch(*fmt)
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
						kprintf_x(*(uint64_t*)arg, padding - 1);
						arg++;
					}
					else
						kprintf_x((uint64_t)*(uint32_t*)arg, padding - 1);
					padding = 0;

					break;

				case 'X':
					if (next_as_long)
					{
						next_as_long = false;
						kprintf_X(*(uint64_t*)arg, padding - 1);
						arg++;
					}
					else
						kprintf_X((uint64_t)*(uint32_t*)arg, padding - 1);
					padding = 0;

					break;

				default:
					outc('%');
					outc(*fmt);
					arg--;
				}
			}

			arg++;
		}
		else
			outc(*fmt);
		
		fmt++;
	}

	UpdateCursor();
}

#define kprintf(...) kfprintf(kstdout, __VA_ARGS__)