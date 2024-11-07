#include <stddef.h>

void PDCLIB_putchar(char c) 
{
    asm volatile (
        "movb %0, %%al\n\t"
        "int $0xFF"
        :
        : "r" (c)
        : "al"
    );
}

size_t PDCLIB_write(const void * buffer, size_t size, size_t count, void * stream) 
{
    const char * buf = (const char *)buffer;
    size_t total = size * count;
    
    for (size_t i = 0; i < total; i++) {
        PDCLIB_putchar(buf[i]);
    }
    
    return count;
}

int PDCLIB_flushbuffer( void * stream ) 
{
    return 0;
}

void * PDCLIB_open(const char * filename, unsigned int mode) 
{
    return NULL; 
}

int PDCLIB_close(void * stream) 
{
    return -1; 
}

size_t PDCLIB_read(void * buffer, size_t size, size_t count, void * stream) 
{
    return 0; 
}

int PDCLIB_seek(void * stream, long offset, int whence) 
{
    return -1; 
}

long PDCLIB_tell(void * stream) 
{
    return -1; 
}
