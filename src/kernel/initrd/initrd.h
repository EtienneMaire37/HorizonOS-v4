#pragma once

virtual_address_t initrd_address;
uint32_t initrd_size;

typedef struct TAR_Header INITRD_FILE;

uint32_t Initrd_GetFileSize(struct TAR_Header* header);
void Initrd_ListFiles();
INITRD_FILE* Initrd_GetFileInfo(char* file);