#pragma once

char hex[16] = "0123456789abcdef";
char HEX[16] = "0123456789ABCDEF";

typedef enum stream_t
{
    STDIN_STREAM,
    STDOUT_STREAM,
    STDERR_STREAM,
    LOG_STREAM,
    FILE_STREAM
} stream_t;

typedef struct kFILE
{
    stream_t stream;
} kFILE;

kFILE _kstdin;
kFILE _kstdout;
kFILE _kstderr;
kFILE _klog;

kFILE* kstdin = &_kstdin;
kFILE* kstdout = &_kstdout;
kFILE* kstderr = &_kstderr;
kFILE* klog = &_klog;

kFILE* currentStream, *currentInterruptStream;

void kputc(char c);
void kputs(char* str);

void kprintf_d(int32_t val);
void kprintf_u(uint32_t val);
void kprintf_x(uint32_t val);
void kprintf_X(uint32_t val);
void kfprintf(kFILE* file, char* fmt, ...);
void kgets(char* str);

extern void kprintf(char* fmt, ...);