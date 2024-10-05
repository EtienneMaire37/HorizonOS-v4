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

    char linked_file[100];
    char ustar[6];
    char ustar_version[2];
    char owner_user_name[32];
    char owner_group_name[32];
    char device_major_number[8];
    char device_minor_number[8];
    char filename_prefix[155];

    uint8_t padding[12];
};

#define TAR_NORMAL_FILE_0       '0' 
#define TAR_NORMAL_FILE_1       '\0' 
#define TAR_HARD_LINK           '1' 
#define TAR_SYMBOLIC_LINK       '2' 
#define TAR_CHARACTER_DEVICE    '3' 
#define TAR_BLOCK_DEVICE        '4' 
#define TAR_DIRECTORY           '5' 
#define TAR_NAMED_PIPE          '6' 

typedef struct TAR_Header INITRD_FILE;

uint32_t Initrd_GetFileSize(struct TAR_Header* header);
void Initrd_ListFiles();