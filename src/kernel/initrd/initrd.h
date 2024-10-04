#pragma once

virtual_address_t initrd_address;
uint32_t initrd_size;

struct TAR_Header
{
    char filename[100];
    uint8_t mode[8];
    uint8_t uid[8];
    uint8_t gid[8];
    uint8_t size[12];
    uint8_t time[12];
    uint8_t checksum[8];
    uint8_t typeflag;

    uint8_t padding[512 - 157];
};

uint32_t Initrd_GetFileSize(struct TAR_Header* header);
void Initrd_ListFiles();