// list_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <inttypes.h>
#include "fat32_attributes.h"
#include "utility.h"
#include "volume_root_iterator.h"

void list_utility(
    FILE* output,
    Volume volume,
    UTILITY_UNUSED const char* recover,
    UTILITY_UNUSED unsigned char sha1[SHA_DIGEST_LENGTH])
{
    uint32_t entries = 0;
    struct VolumeRootIterator it;

    for (volume_root_begin(&it, volume); !it.end; volume_root_next(&it))
    {
        if (fat32_directory_entry_is_end_free(it.entry))
        {
            break;
        }

        if (fat32_directory_entry_is_mid_free(it.entry) ||
            it.entry->attributes & FAT32_ATTRIBUTES_HIDDEN)
        {
            continue;
        }

        char buffer[13];

        volume_get_display_name(buffer, it.entry->name);
        fprintf(output, "%s", buffer);

        uint32_t lo = it.entry->firstClusterLo;
        uint32_t hi = it.entry->firstClusterHi;
        uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);

        entries++;

        if (it.entry->attributes & FAT32_ATTRIBUTES_DIRECTORY)
        {
            fprintf(output, "/ (starting cluster = %" PRIu32, firstCluster);
        }
        else
        {
            fprintf(output, " (size = %" PRIu32, it.entry->fileSize);

            if (it.entry->fileSize)
            {
                fprintf(output, ", starting cluster = %" PRIu32, firstCluster);
            }
        }

        fprintf(output, ")\n");
    }

    fprintf(output, "Total number of entries = %" PRIu32 "\n", entries);
}
