#pragma once

void Debug_putc(char c)
{
    outb(0xe9, c);
}

#define LOG(severity, ...)   { kfprintf(klog, "0000-00-00 \t 00:00:00,000 \t %s \t", severity); kfprintf(klog, __VA_ARGS__); kfprintf(klog, "\n"); } 