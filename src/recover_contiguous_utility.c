// recover_contiguous_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <string.h>
#include "fat32_attributes.h"
#include "utility.h"
#include "volume_root_iterator.h"

void recover_contiguous_utility(
    FILE* output,
    Volume volume,
    char* recover,
    UTILITY_UNUSED char* sha1)
{
    bool candidate = false;
    struct VolumeRootIterator it;

    for (volume_root_begin(&it, volume); !it.end; volume_root_next(&it))
    {
        if (it.entry->attributes & FAT32_ATTRIBUTES_DIRECTORY ||
            !(fat32_directory_entry_is_end_free(it.entry) &&
                fat32_directory_entry_is_mid_free(it.entry)))
        {
            continue;
        }

        char buffer[13];

        volume_get_display_name(buffer, it.entry->name);

        if (strcmp((char*)(it.entry->name + 1), recover + 1) != 0)
        {
            continue;
        }

        if (candidate)
        {
            fprintf(output, "%s: multiple candidates found\n", recover);

            return;
        }

        candidate = true;
    }

    if (!candidate)
    {
        printf("%s: file not found\n", recover);

        return;
    }

    printf("%s: successfully recovered\n", recover);
}
