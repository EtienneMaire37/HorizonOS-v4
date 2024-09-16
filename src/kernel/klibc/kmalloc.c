#pragma once

uint32_t totalBlocks;
uint32_t blockSize;
uint32_t bitmapSize;
uint32_t freeMemAddress;

void* mapLinearAddressToAvailableMemory(uint32_t address)
{
    if(address < 0x78200)   // Lower conventional memory
        return (void*)(address + 0x7e00);

    return NULL;
}

uint32_t mapAvailableMemoryToLinearAddress(void* ptr)
{
    uint32_t address = (uint32_t)ptr;
    if(address >= 0x7e00 && address < 0x80000)   // Lower conventional memory
        return address - 0x7e00;

    return 0;
}

bool isPtrAvailable(void* ptr)
{
    for(uint32_t i = 0; i < multibootInfo->mmap_length; i += sizeof(multiboot_memory_map_t)) 
    {
        multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*)(multibootInfo->mmap_addr + i);

        uint64_t addr = (mmmt->addr_high << 8) | mmmt->addr_low;
        uint64_t len = (mmmt->len_high) | mmmt->len_low;
 
        if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) 
        {
            if((uint32_t)ptr >= addr && (uint32_t)ptr < addr + len)
                return true;
        }
    }

    return false;
}

void initMemAlloc(uint16_t _blockSize)
{
    blockSize = _blockSize;
    totalBlocks = availableMem / blockSize;
    bitmapSize = totalBlocks / 8;

    for(uint32_t i = 0; i < bitmapSize; i++)
        *(uint8_t*)mapLinearAddressToAvailableMemory(i) = 0xff;        // Every block is free
    for(uint32_t i = bitmapSize; i < 2 * bitmapSize; i++)
        *(uint8_t*)mapLinearAddressToAvailableMemory(i) = 0xff;        // Every block is the first one

    freeMemAddress = 2 * bitmapSize;
}

void setBlockFree(uint32_t blockNumber, bool isFree)
{
    uint8_t* blockByte = mapLinearAddressToAvailableMemory(blockNumber / 8);
    uint8_t blockBit = 7 - (blockNumber % 8);
    
    *blockByte &= (0xff ^ (1 << blockBit));
    *blockByte |= (isFree << blockBit);
}

void setBlockAsFirstBlock(uint32_t blockNumber, bool isFirstBlock)
{
    uint8_t* blockByte = mapLinearAddressToAvailableMemory((blockNumber / 8) + bitmapSize);
    uint8_t blockBit = 7 - (blockNumber % 8);
    
    *blockByte &= (0xff ^ (1 << blockBit));
    *blockByte |= (isFirstBlock << blockBit);
}

bool isBlockFree(uint32_t blockNumber)
{
    uint8_t* blockByte = mapLinearAddressToAvailableMemory(blockNumber / 8);
    uint8_t blockBit = 7 - (blockNumber % 8);
    
    return (*blockByte >> blockBit) & 1;
}

bool isBlockAFirstBlock(uint32_t blockNumber)
{
    uint8_t* blockByte = mapLinearAddressToAvailableMemory((blockNumber / 8) + bitmapSize);
    uint8_t blockBit = 7 - (blockNumber % 8);
    
    return (*blockByte >> blockBit) & 1;
}

void printMemState(uint32_t block)
{
    textCursor = 0;
    HideCursor();
    for(uint32_t i = block; (i < block + 80 * 25) && (i < totalBlocks); i++)
    {
        if(isBlockFree(i))
        {
            textColor = BG_GREEN | FG_BLACK;//BG_LIGHTGREEN | FG_BLACK;

            outc('/');
        }
        else
        {
            textColor = BG_BLUE | FG_BLACK;

            if(isBlockAFirstBlock(i))
                outc('#');
            else
                outc('+');
        }
    }

    while(true);
}

void* kmalloc(size_t size)
{
    if(size == 0)
        return NULL;

    uint32_t numBlocks = kceil(size / (float)blockSize);
    uint32_t blockNum = 0;
    uint32_t totalContiguousBlocks = 0;
    
    for(uint32_t i = 0; i < totalBlocks; i++)
    {
        if(isBlockFree(i))
        {
            totalContiguousBlocks++;
        }
        else
        {
            totalContiguousBlocks = 0;
            blockNum = i + 1;
        }
        
        if(totalContiguousBlocks >= numBlocks)
        {
            for(uint32_t j = 0; j < numBlocks; j++)
                setBlockAsFirstBlock(j + blockNum, false);
            setBlockAsFirstBlock(blockNum, true);
            for(uint32_t j = 0; j < numBlocks; j++)
                setBlockFree(j + blockNum, false);

            // printf("\nBlock: %d, %d blocks\n", blockNum, numBlocks);
            // printf("Address: 0x%x\n\n", mapLinearAddressToAvailableMemory(blockNum * blockSize + freeMemAddress));

            return mapLinearAddressToAvailableMemory(blockNum * blockSize + freeMemAddress); 
        }
    }

    return NULL;
}

void* kcalloc(size_t nitems, size_t size)
{
    void* ptr = kmalloc(nitems * size);
    kmemset(ptr, 0, nitems * size);
    return ptr;
}

void kfree(void* ptr)
{
    uint32_t address = mapAvailableMemoryToLinearAddress(ptr);
    uint32_t blockNum = (address - freeMemAddress) / blockSize;

    do
    {
        setBlockFree(blockNum, true);
        blockNum++;
    } while (!isBlockAFirstBlock(blockNum));
}