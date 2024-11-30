// recover_contiguous_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References
//  - Microsoft Extensible Firmware Initiative FAT32 File System Specification

#include <string.h>
#include "fat32.h"
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
    const char* recover,
    unsigned char sha1[SHA_DIGEST_LENGTH])
{
    struct VolumeRootIterator it;

    volume_root_begin(&it, volume);

    VolumeFindResult find = volume_root_single_free(&it, recover, sha1);
    char* message = volume_find_result_to_string(find);

    fprintf(output, "%s: %s\n", recover, message);

    if (!volume_find_result_is_ok(find))
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
    uint32_t bytesPerCluster = bootSector->sectorsPerCluster;

    bytesPerCluster *= bootSector->bytesPerSector;

    uint32_t clusters = math_ceildiv(it.entry->fileSize, bytesPerCluster);
    uint32_t lastCluster = firstCluster + clusters - 1;

    for (uint32_t fat = 0; fat < bootSector->fats; fat++)
    {
        // From specification:
        //   BPB_ResvdSecCnt + (BPB_NumFATs * FATSz)

        uint32_t fatSector = bootSector->reservedSectors;

        fatSector += fat * bootSector->sectorsPerFat;

        uint32_t fatStartByte = fatSector * bootSector->bytesPerSector;
        uint32_t* fatData = (uint32_t*)((uint8_t*)volume->data + fatStartByte);

        for (uint32_t cluster = firstCluster; cluster < lastCluster; cluster++)
        {
            fatData[cluster] = cluster + 1;
        }

        fatData[lastCluster] = FAT32_EOF;
    }
}
