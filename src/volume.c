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
#include <fcntl.h>
#include <unistd.h>
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
#include <assert.h>
#include <stdio.h>
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
    uint32_t sector = (cluster - 2) * bootSector->sectorsPerCluster;

    sector += firstDataSector;

    // uint32_t bytesPerCluster = bootSector->sectorsPerCluster;

    // bytesPerCluster *= bootSector->bytesPerSector;

    uint8_t* data = instance->data;

    data += sector * bootSector->bytesPerSector;

    Fat32DirectoryEntry entry = (Fat32DirectoryEntry)data;

    printf("'%s'\n", entry->name);
}

bool volume_next(VolumeRootIterator iterator)
{
    return false; // TODO: implement volume_next
}

void finalize_volume(Volume instance)
{
    munmap(instance->data, instance->size);
}
