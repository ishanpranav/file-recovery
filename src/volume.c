// volume.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://man7.org/linux/man-pages/man2/close.2.html
//  - https://www.man7.org/linux/man-pages/man3/fstat.3p.html
//  - https://www.man7.org/linux/man-pages/man2/mmap.2.html
//  - https://www.man7.org/linux/man-pages/man2/open.2.html
//  - https://www.man7.org/linux/man-pages/man3/stat.3type.html

#include <sys/mman.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "fat32.h"
#include "fat32_attributes.h"
#include "fat32_boot_sector.h"
#include "fat32_directory_entry.h"
#include "volume_root_iterator.h"

bool volume(Volume instance, char* path)
{
    bool result = false;
    int descriptor = open(path, O_RDWR);

    if (descriptor == -1)
    {
        goto volume_exit;
    }

    struct stat status;

    if (fstat(descriptor, &status) == -1)
    {
        goto volume_exit_open;
    }

    void* data = mmap(
        NULL,
        status.st_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        descriptor,
        0);

    if (data == MAP_FAILED)
    {
        goto volume_exit_open;
    }

    instance->size = status.st_size;
    instance->data = data;
    result = true;

volume_exit_open:
    close(descriptor);

volume_exit:
    return result;
}

void volume_get_display_name(char buffer[13], uint8_t name[11])
{
    char* end = buffer;

    memcpy(end, name, 8);

    end += 7;

    if (buffer[0] == 0x05)
    {
        buffer[0] = 0xe5;
    }
    
    while (end > buffer && (*end == '\0' || *end == 0x20))
    {
        end--;
    }

    end++;
    *end = '.';

    char* extension = end;

    end++;

    memcpy(end, name + 8, 3);

    end += 2;

    while (end >= extension && (*end == '\0' || *end == 0x20))
    {
        end--;
    }

    if (end == extension && *end == '.')
    {
        end--;
    }

    end++;
    *end = '\0';
}

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
void volume_begin(VolumeRootIterator iterator, Volume instance)
{
    Fat32BootSector bootSector = instance->data;

    // From specification:

    //   RootDirSectors =
    //     ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;

    uint32_t rootSectors = (bootSector->rootEntries * 32);

    rootSectors += bootSector->bytesPerSector - 1;
    rootSectors /= bootSector->bytesPerSector;

    // From specification:

    //   FirstDataSector =
    //     BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;

    uint32_t firstDataSector = bootSector->reservedSectors;

    firstDataSector += bootSector->fats * bootSector->sectorsPerFat;
    firstDataSector += rootSectors;

    // From specification:
    //   FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;

    uint32_t cluster = bootSector->rootCluster;
    uint32_t entries = 0;

    while (!fat32_is_eof(cluster))
    {
        uint32_t sector = (cluster - 2) * bootSector->sectorsPerCluster;

        sector += firstDataSector;

        uint8_t* data = instance->data;
        
        data += sector * bootSector->bytesPerSector;

        char buffer[13];
        uint32_t bytesPerCluster = bootSector->sectorsPerCluster;
        
        bytesPerCluster *= bootSector->bytesPerSector;

        for (uint32_t offset = 0; offset < bytesPerCluster; offset += sizeof(struct Fat32DirectoryEntry))
        {
            Fat32DirectoryEntry entry = (Fat32DirectoryEntry)(data + offset);

            if (fat32_directory_entry_is_mid_free(entry))
            {
                continue;
            }

            if (fat32_directory_entry_is_end_free(entry))
            {
                goto volume_begin_exit;
            }

            volume_get_display_name(buffer, entry->name);

            uint32_t firstCluster = entry->firstClusterHi << 16;
            
            firstCluster |= entry->firstClusterLo;
            entries++;

            if (entry->attributes & FAT32_ATTRIBUTES_DIRECTORY)
            {
                printf("%s/ (starting cluster = %" PRIu32 ")\n",
                    buffer, firstCluster);

                continue;
            }
            
            printf("%s (size = %" PRIu32, buffer, entry->fileSize);
            
            if (entry->fileSize)
            {
                printf(", starting cluster = %" PRIu32, firstCluster);
            }

            printf(")\n");
        }

        // From specification:
        //   FATOffset = N * 4;

        uint32_t fatOffset = cluster * 4;

        // From specification:
        //   ThisFATSecNum = BPB_ResvdSecCnt + (FATOffset / BPB_BytsPerSec);

        uint32_t fatSector = bootSector->reservedSectors;

        fatSector += fatOffset / bootSector->bytesPerSector;

        uint32_t fatEntryOffset = fatOffset % bootSector->bytesPerSector;

        // From specification:
        //   FAT32ClusEntryVal =
        //     (*((DWORD *) &SecBuff[ThisFATEntOffset])) & 0x0FFFFFFF;

        uint8_t* sectorBuffer = instance->data;

        sectorBuffer += fatSector * bootSector->bytesPerSector;
        cluster = *(uint32_t*)(sectorBuffer + fatEntryOffset) & 0x0fffffff;
    }

volume_begin_exit:
    printf("Total number of entries = %" PRIu32 "\n", entries);
}

bool volume_next(VolumeRootIterator iterator)
{
    return false; // TODO: implement volume_next
}

void finalize_volume(Volume instance)
{
    munmap(instance->data, instance->size);
}
