// recover_fragmented_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References
//  - Microsoft Extensible Firmware Initiative FAT32 File System Specification
//  - Heap's algorithm: https://en.wikipedia.org/wiki/Heap%27s_algorithm
//  - Permutation - Generation in lexicographic order: https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// - https://docs.openssl.org/1.0.2/man3/sha/

#include <string.h>
#include "fat32_boot_sector.h"
#include "next_permutation.h"
#include "utility.h"
#include "volume_root_iterator.h"
#define COMBINATORIAL_SEARCH_N 20
#define COMBINATORIAL_SEARCH_K 5

static VolumeFindResult combinatorial_search(
    VolumeRootIterator* iterator,
    unsigned char sha1[SHA_DIGEST_LENGTH])
{
    Volume* volume = iterator->instance;
    VolumeRootIterator it;
    bool isFirstCluster[COMBINATORIAL_SEARCH_N] = { 0 };

    for (volume_root_begin(&it, volume); !it.end; volume_root_next(&it))
    {
        uint32_t lo = it.entry->firstClusterLo;
        uint32_t hi = it.entry->firstClusterHi;
        uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);

        if (firstCluster - 2 < COMBINATORIAL_SEARCH_N)
        {
            isFirstCluster[firstCluster - 2] = true;
        }
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

    printf("length = %u\n", length);

    uint32_t bytesPerCluster = iterator->bytesPerCluster;
    int k = volume_clusters(iterator->entry->fileSize, bytesPerCluster);

    printf("k = %u\n", k);

    int max = COMBINATORIAL_SEARCH_N - k;
    int difference = length - k;

    if (difference < max)
    {
        max = difference;
    }

    printf("max = %u\n", max);

    for (int i = 0; i < max; i++)
    {
        uint32_t permutation[COMBINATORIAL_SEARCH_K - 1];
        uint32_t hi = iterator->entry->firstClusterHi;
        uint32_t lo = iterator->entry->firstClusterLo;
        uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);
        uint32_t* p = candidates + i;

        memcpy(permutation, p, (k - 1) * sizeof * p);

        int c = 0;
        do
        {
            printf("%d ", firstCluster);
            for (int j = 0; j < k - 1; j++) {
                printf("%d ", permutation[j]);
            }
            printf("\n");
            c++;

            SHA_CTX context;

            SHA1_Init(&context);

            uint8_t* data;
            
            data = volume_root_data(&it, firstCluster);
            
            SHA1_Update(&context, data, iterator->bytesPerCluster);
        
            for (int j = 0; j < k - 2; j++)
            {
                data = volume_root_data(&it, permutation[j]);

                SHA1_Update(&context, data, iterator->bytesPerCluster);
            }
    
            data = volume_root_data(&it, permutation[k - 2]);

            uint32_t remainder = iterator->entry->fileSize;

            remainder -= iterator->bytesPerCluster * (k - 1);

            SHA1_Update(&context, data, remainder);

            unsigned char digest[SHA_DIGEST_LENGTH];

            SHA1_Final(digest, &context);

            for (int j = 0; j < SHA_DIGEST_LENGTH; j++) {
                printf("%02x ", digest[j]);
            }
            printf("\n");

            if (memcmp(digest, sha1, SHA_DIGEST_LENGTH) == 0)
            {
                return VOLUME_FIND_RESULT_SHA1_FOUND;
            }
        } 
        while (next_permutation(permutation, k - 1));
    }

    return VOLUME_FIND_RESULT_NOT_FOUND;
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

    volume_root_begin(&it, volume);

    find = volume_root_first_free(&it, recover, NULL);

    if (!volume_find_result_is_ok(find))
    {
        goto recover_fragmented_utility_exit;
    }

    find = combinatorial_search(&it, sha1);

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
