// recover_contiguous_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <string.h>
#include "fat32_boot_sector.h"
#include "utility.h"
#include "volume_root_iterator.h"

static uint32_t math_ceildiv(uint32_t a, uint32_t b)
{
    return (a + b - 1) / b;
}

void recover_contiguous_utility(
    FILE* output,
    Volume volume,
    char* recover,
    UTILITY_UNUSED char* sha1)
{
    struct VolumeRootIterator it;

    volume_root_begin(&it, volume);

    VolumeFindResult find = volume_root_single(&it, recover);
    char* message = volume_find_result_to_string(find);

    fprintf(output, "%s: %s\n", recover, message);

    if (find != VOLUME_FIND_RESULT_OK)
    {
        return;
    }

    *it.entry->name = *recover;

    if (!it.entry->fileSize)
    {
        return;
    }

    Fat32BootSector bootSector = volume->data;
    uint32_t lo = it.entry->firstClusterLo;
    uint32_t hi = it.entry->firstClusterHi;
    uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);
    uint32_t eof = 0x0ffffff8;
    uint32_t bytesPerCluster = bootSector->sectorsPerCluster;

    bytesPerCluster *= bootSector->bytesPerSector;

    uint32_t clusters = math_ceildiv(it.entry->fileSize, bytesPerCluster);
    uint32_t lastCluster = firstCluster + clusters - 1;

    for (uint32_t fat = 0; fat < bootSector->fats; fat++)
    {
        uint32_t fatSector = bootSector->reservedSectors;

        fatSector += fat * bootSector->sectorsPerFat;

        uint32_t fatStartByte = fatSector * bootSector->bytesPerSector;

        for (uint32_t cluster = firstCluster; cluster < lastCluster; cluster++)
        {
            //  FATOffset = N * 4;

            uint32_t fatOffset = cluster * 4;
            uint32_t fatByte = fatStartByte + fatOffset;
            uint32_t nextCluster = cluster + 1;

            memcpy((char*)volume->data + fatByte, &nextCluster, sizeof(nextCluster));
        }

        memcpy((char*)volume->data + fatStartByte + lastCluster * 4, &eof, sizeof(eof));
    }
}
