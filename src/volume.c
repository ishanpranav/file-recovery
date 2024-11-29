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
        buffer[0] = (char)0xe5;
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

static void volume_root_reset_offset(VolumeRootIterator iterator)
{
    Fat32BootSector bootSector = iterator->instance->data;

    // From specification:
    //   FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;

    uint32_t sector = (iterator->cluster - 2) * bootSector->sectorsPerCluster;

    sector += iterator->firstDataSector;

    uint8_t* data = iterator->instance->data;

    data += sector * bootSector->bytesPerSector;
    iterator->offset = 0;
    iterator->data = data;
}

void volume_root_begin(VolumeRootIterator iterator, Volume instance)
{
    iterator->instance = instance;

    Fat32BootSector bootSector = instance->data;

    // From specification:

    //   RootDirSectors =
    //     ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;

    uint32_t rootSectors = bootSector->rootEntries;

    rootSectors *= sizeof(struct Fat32DirectoryEntry);
    rootSectors += bootSector->bytesPerSector - 1;
    rootSectors /= bootSector->bytesPerSector;

    // From specification:

    //   FirstDataSector =
    //     BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;

    uint32_t firstDataSector = bootSector->reservedSectors;

    firstDataSector += bootSector->fats * bootSector->sectorsPerFat;
    firstDataSector += rootSectors;

    uint32_t bytesPerCluster = bootSector->sectorsPerCluster;

    bytesPerCluster *= bootSector->bytesPerSector;
    iterator->cluster = bootSector->rootCluster;
    iterator->firstDataSector = firstDataSector;
    iterator->bytesPerCluster = bytesPerCluster;

    volume_root_reset_offset(iterator);

    iterator->entry = (Fat32DirectoryEntry)(iterator->data + iterator->offset);
    iterator->end = fat32_is_eof(iterator->cluster);
}

void volume_root_next(VolumeRootIterator iterator)
{
    uint32_t step = sizeof(struct Fat32DirectoryEntry);

    if (iterator->offset < iterator->bytesPerCluster - step)
    {
        iterator->offset += step;

        uint8_t* pointer = iterator->data + iterator->offset;

        iterator->entry = (Fat32DirectoryEntry)pointer;
        iterator->end = false;

        return;
    }

    if (fat32_is_eof(iterator->cluster))
    {
        iterator->end = true;

        return;
    }

    // From specification:
    //   FATOffset = N * 4;

    uint32_t fatOffset = iterator->cluster * 4;

    // From specification:
    //   ThisFATSecNum = BPB_ResvdSecCnt + (FATOffset / BPB_BytsPerSec);

    Fat32BootSector bootSector = iterator->instance->data;
    uint32_t fatSector = bootSector->reservedSectors;

    fatSector += fatOffset / bootSector->bytesPerSector;

    uint32_t fatEntryOffset = fatOffset % bootSector->bytesPerSector;

    // From specification:
    //   FAT32ClusEntryVal =
    //     (*((DWORD *) &SecBuff[ThisFATEntOffset])) & 0x0FFFFFFF;

    uint8_t* buffer = iterator->instance->data;

    buffer += fatSector * bootSector->bytesPerSector;
    iterator->cluster = *(uint32_t*)(buffer + fatEntryOffset) & 0x0fffffff;

    volume_root_reset_offset(iterator);

    iterator->entry = (Fat32DirectoryEntry)(iterator->data + iterator->offset);
    iterator->end = fat32_is_eof(iterator->cluster);
}

bool volume_root_first(VolumeRootIterator iterator, const char* fileName)
{
    for (; !iterator->end; volume_root_next(iterator))
    {
        if (iterator->entry->attributes & FAT32_ATTRIBUTES_DIRECTORY ||
            !(fat32_directory_entry_is_end_free(iterator->entry) &&
                fat32_directory_entry_is_mid_free(iterator->entry)))
        {
            continue;
        }

        char buffer[13];

        volume_get_display_name(buffer, iterator->entry->name);

        if (strcmp((char*)(iterator->entry->name + 1), fileName + 1) == 0)
        {
            return true;
        }
    }

    return false;
}

VolumeFindResult volume_root_single(
    VolumeRootIterator iterator,
    const char* fileName)
{
    if (!volume_root_first(iterator, fileName))
    {
        return VOLUME_FIND_RESULT_NOT_FOUND;
    }

    struct VolumeRootIterator copy = *iterator;

    if (volume_root_first(&copy, fileName))
    {
        return VOLUME_FIND_RESULT_MULTIPLE_FOUND;
    }

    return VOLUME_FIND_RESULT_OK;
}

void finalize_volume(Volume instance)
{
    munmap(instance->data, instance->size);
}
