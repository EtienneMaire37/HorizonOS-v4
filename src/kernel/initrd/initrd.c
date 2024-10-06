#pragma once

uint32_t Initrd_GetFileSize(struct TAR_Header* header)
{
    uint32_t size = 0;
    uint32_t count = 1073741824;    // 8^10

    for (uint8_t j = 0; j < 10; j++, count >>= 3)
        size += ((header->size[j] - '0') * count);

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
        LOG("INFO", "   File %u : \"%s\" | size : %uB", count, &header->filename[0], fileSize);
        uint32_t blocks = (fileSize + 511) / 512;
        header = &header[blocks + 1];
        count++;
    }
}

INITRD_FILE* Initrd_GetFileInfo(char* filename)
{
    struct TAR_Header* header = (struct TAR_Header*)initrd_address;
    while (header->filename[0] != 0)
    {
        if (kstrcmp(filename, &header->filename[0]) == 0)
            return header;
        // LOG("INFO", "   File : \"%s\" \"%s\"", filename, &header->filename[0]);
        uint32_t fileSize = Initrd_GetFileSize(header);
        uint32_t blocks = (fileSize + 511) / 512;
        header = &header[blocks + 1];
    }
    return NULL;
}