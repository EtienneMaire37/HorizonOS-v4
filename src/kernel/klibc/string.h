#pragma once

void* kmemchr(const void* str, int c, size_t n);
int kmemcmp(const void* str1, const void* str2, size_t n);
void* kmemcpy(void* dest, const void* src, size_t n);
// void *memmove(void *dest, const void *src, size_t n);
void* kmemset(void* ptr, int value, size_t count);
int kstrcmp(const char* str1, const char* str2);
uint32_t kstrlen(char* str);