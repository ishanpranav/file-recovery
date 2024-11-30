// recover_fragmented_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References
//  - Microsoft Extensible Firmware Initiative FAT32 File System Specification

#include <string.h>
#include "fat32_boot_sector.h"
#include "utility.h"
#include "volume_root_iterator.h"

#define COMBINATORIAL_SEARCH_N 20
#define COMBINATORIAL_SEARCH_K 5

static bool combinatorial_search(
    VolumeRootIterator* iterator,
    const char* fileName,
    unsigned char sha1[SHA_DIGEST_LENGTH])
{
    VolumeRootIterator it;
    Volume* volume = iterator->instance;
    bool isFirstCluster[COMBINATORIAL_SEARCH_N] = { 0 };

    for (volume_root_begin(&it, volume); !it.end; volume_root_next(&it))
    {
        uint32_t lo = it.entry->firstClusterLo;
        uint32_t hi = it.entry->firstClusterHi;
        uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);

        isFirstCluster[firstCluster - 2] = true;
    }
    
    uint32_t candidates[COMBINATORIAL_SEARCH_N];
    uint32_t length = 0;
    
    for (uint32_t cluster = 2; cluster < COMBINATORIAL_SEARCH_N + 2; cluster++)
    {
        if (!isFirstCluster[cluster - 2])
        {
            candidates[length] = cluster;
            length++;
        }
    }

    VolumeFindResult result = VOLUME_FIND_RESULT_COUNT;

    for (;;)
    {
        VolumeFindResult find;

        find = volume_root_first_free(iterator, fileName, NULL);

        if (!volume_find_result_is_ok(find))
        {
            return false;
        }

        char buffer[13];

        volume_display_name(buffer, iterator->entry->name);
        printf("so far candidate '%s'\n", buffer);

        if (iterator->end)
        {
            return false;
        }

        volume_root_next(iterator);
    }
}

void recover_fragmented_utility(
    FILE* output,
    Volume* volume,
    const char* recover,
    unsigned char sha1[SHA_DIGEST_LENGTH])
{
    VolumeRootIterator it;
    VolumeFindResult find;

    volume_root_begin(&it, volume);

    VolumeRootIterator copy = it;

    find = volume_root_first_free(&it, recover, sha1);

    if (volume_find_result_is_ok(find))
    {
        uint32_t lo = it.entry->firstClusterLo;
        uint32_t hi = it.entry->firstClusterHi;
        uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);
        uint32_t clusters = volume_clusters(
            it.entry->fileSize,
            it.bytesPerCluster);

        volume_root_restore_contiguous(&it, firstCluster, clusters);

        goto recover_fragmented_utility_exit;
    }

    find = volume_root_fragmented(&copy, recover, sha1);

    if (!volume_find_result_is_ok(find))
    {
        goto recover_fragmented_utility_exit;
    }

    char buffer[13];

    volume_display_name(buffer, copy.entry->name);
    fprintf(output, "legit candidate '%s'\n", buffer);

recover_fragmented_utility_exit:
    {
        const char* message = volume_find_result_to_string(find);

        fprintf(output, "%s: %s\n", recover, message);
    }

    // volume_root_begin(&it, volume);

    // VolumeFindResult find;

    // for (;;)
    // {
    //     find = volume_root_first_free(&it, recover, NULL);

    //     if (!volume_find_result_is_ok(find))
    //     {
    //         break;
    //     }

    //     char buffer[13];

    //     volume_display_name(buffer, it.entry->name);
    //     fprintf(output, "Candidate '%s'\n", buffer);

    //     if (it.end)
    //     {
    //         break;
    //     }

    //     volume_root_next(&it);
    // }

    // if ()
    // {
    // }

    // VolumeFindResult first = volume_root_first_free(iterator, fileName, sha1);

    // if (first == VOLUME_FIND_RESULT_NOT_FOUND || iterator->end)
    // {
    //     return first;
    // }

    // VolumeRootIterator copy = *iterator;

    // volume_root_next(&copy);

    // VolumeFindResult second = volume_root_first_free(&copy, fileName, sha1);

    // if (second != VOLUME_FIND_RESULT_NOT_FOUND)
    // {
    //     return VOLUME_FIND_RESULT_MULTIPLE_FOUND;
    // }
}
