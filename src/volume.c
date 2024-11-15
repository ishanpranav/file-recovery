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

    Fat32BootSector bootSector = mmap(
        NULL,
        status.st_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        descriptor,
        0);

    if (bootSector == MAP_FAILED)
    {
        goto volume_exit_open;
    }

    instance->size = status.st_size;
    instance->bootSector = bootSector;
    result = true;

volume_exit_open:
    close(descriptor);

volume_exit:
    return result;
}

void volume_begin(VolumeRootIterator iterator, Volume instance)
{
    // From specification:

    //   RootDirSectors =
    //     ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;
    
    uint32_t rootDirectorySectors;

    Fat32BootSector bootSector = instance->bootSector;
    uint32_t firstDataSector = bootSector->reservedSectors;

    firstDataSector += (bootSector.fats * bootSector->fatSize);
    firstDataSector += rootDirectorySectors;

    //   FirstDataSector =
    //     BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;

    // TODO: implement volume_begin
}

bool volume_next(VolumeRootIterator iterator)
{
    return false; // TODO: implement volume_next
}

void finalize_volume(Volume instance)
{
    munmap(instance->bootSector, instance->size);
}
