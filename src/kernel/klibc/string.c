#pragma once

void* kmemchr(const void* str, int c, size_t n)
{
    void* out = NULL;
    for(size_t i = 0; i < n; i++)
    {
        if(((uint8_t*)str)[i] == (uint8_t)c)
        {
            out = &((uint8_t*)str)[i];
            break;
        }
    }
    return out;
}

int kmemcmp(const void* str1, const void* str2, size_t n)
{
    for(uint64_t i = 0; i < n; i++)
    {
        if(((uint8_t*)str1)[i] < ((uint8_t*)str2)[i])
            return -1;
        if(((uint8_t*)str1)[i] > ((uint8_t*)str2)[i])
            return 1;
    }

    return 0; 
}

void* kmemcpy(void* dest, const void* src, size_t n)
{
    for(size_t i = 0; i < n; i++)
        // ((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
        *(uint8_t*)(dest + i) = *(uint8_t*)(src + i);

    return dest;
}

// void *memmove(void *dest, const void *src, size_t n);

void* kmemset(void* ptr, int value, size_t count)
{
    for(size_t i = 0; i < count; i++)
        ((uint8_t*)ptr)[i] = (uint8_t)value;

    return ptr;
}

int kstrcmp(const char* str1, const char* str2)
{
    while(*str1 != 0 && *str2 != 0)
    {
        if (*str1 != *str2)
            return *str1 - *str2;

        str1++;
        str2++;
    }

    return 0; 
}

uint32_t kstrlen(char* str)
{
    uint32_t len = 0;

    while(*str)
    {
        len++;
        str++;
    }

    return len;
}