#pragma once

uint32_t Initrd_GetFileSize(struct TAR_Header* header)
{
    uint32_t size = 0;
    uint32_t count = 1;

    for (uint8_t j = 11; j > 0; j--, count *= 8)
        size += ((header->size[j - 1] - '0') * count);

    return size;
}

void Initrd_ListFiles()
{
    uint8_t count = 0;
    struct TAR_Header* header = (struct TAR_Header*)initrd_address;
    LOG("INFO", "Initrd files : ");
    while (header->filename[0] != 0)
    {
        uint32_t fileSize = Initrd_GetFileSize(header);
        LOG("INFO", "File %u : %s | size : %uB", count, &header->filename[0], fileSize);
        uint32_t blocks = (fileSize + 511) / 512;
        header = &header[blocks + 1];
        count++;
    }
}

INITRD_FILE* Initrd_GetFileInfo(char* file)
{
    struct TAR_Header* header = (struct TAR_Header*)initrd_address;
    while (header->filename[0] != 0)
    {
        if (kstrcmp(file, header->filename) == 0)
            return header;
        uint32_t fileSize = Initrd_GetFileSize(header);
        uint32_t blocks = (fileSize + 511) / 512;
        header = &header[blocks + 1];
    }
    return NULL;
}