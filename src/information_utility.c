// information_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <inttypes.h>
#include "utility.h"
#include "volume.h"

void information_utility(
    FILE* output,
    Volume* volume,
    UTILITY_UNUSED const char* recover,
    UTILITY_UNUSED unsigned char sha1[SHA_DIGEST_LENGTH])
{
    Fat32BootSector* bootSector = volume->data;

    fprintf(
        output,
        "Number of FATs = %" PRIu8 "\n"
        "Number of bytes per sector = %" PRIu8 "\n"
        "Number of sectors per cluster = %" PRIu8 "\n"
        "Number of reserved sectors = %" PRIu8 "\n",
        bootSector->fats,
        bootSector->bytesPerSector,
        bootSector->sectorsPerCluster,
        bootSector->reservedSectors);   
}
