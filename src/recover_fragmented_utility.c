// recover_fragmented_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References
//  - Microsoft Extensible Firmware Initiative FAT32 File System Specification
//  - Heap's algorithm: https://en.wikipedia.org/wiki/Heap%27s_algorithm
//  - Permutation - Generation in lexicographic order: https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
// - https://docs.openssl.org/1.0.2/man3/sha/

#include <string.h>
#include "next_permutation.h"
#include "utility.h"
#include "volume_root_iterator.h"
#define COMBINATORIAL_SEARCH_N 20
#define COMBINATORIAL_SEARCH_K 5

static VolumeFindResult combinatorial_search(
    uint32_t results[COMBINATORIAL_SEARCH_K],
    uint32_t clusters,
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

    // printf("length = %u\n", length);

    int k = clusters;

    // printf("k = %u\n", k);

    int max = COMBINATORIAL_SEARCH_N - k;
    int difference = length - k;

    if (difference < max)
    {
        max = difference;
    }

    // printf("max = %u\n", max);

    for (int i = 0; i < max; i++)
    {
        uint32_t permutation[COMBINATORIAL_SEARCH_K - 1];
        uint32_t hi = iterator->entry->firstClusterHi;
        uint32_t lo = iterator->entry->firstClusterLo;
        uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);
        uint32_t* p = candidates + i;

        memcpy(permutation, p, (k - 1) * sizeof * p);

        do
        {
            // printf("%d ", firstCluster);
            // for (int j = 0; j < k - 1; j++) {
            //     printf("%d ", permutation[j]);
            // }
            // printf("\n");
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

            if (k > 2)
            {
                data = volume_root_data(&it, permutation[k - 2]);
            
                uint32_t remainder = iterator->entry->fileSize;

                remainder -= iterator->bytesPerCluster * (k - 1);

                SHA1_Update(&context, data, remainder);
            }

            unsigned char digest[SHA_DIGEST_LENGTH];

            SHA1_Final(digest, &context);

            // for (int j = 0; j < SHA_DIGEST_LENGTH; j++) {
            //     printf("%02x ", digest[j]);
            // }
            // printf("\n");

            if (memcmp(digest, sha1, SHA_DIGEST_LENGTH) == 0)
            {
                *results = firstCluster;
                results++;

                memcpy(results, permutation, (k - 1) * sizeof * results);

                return VOLUME_FIND_RESULT_SHA1_FOUND;
            }
        } while (next_permutation(permutation, k - 1));
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

    Fat32BootSector* bootSector = volume->data;
    uint32_t clusters;

    if (volume_find_result_is_ok(find))
    {
        *it.entry->name = *recover;

        if (!it.entry->fileSize)
        {
            goto recover_fragmented_utility_exit;
        }

        uint32_t lo = it.entry->firstClusterLo;
        uint32_t hi = it.entry->firstClusterHi;
        uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);

        clusters = volume_clusters(it.entry->fileSize, it.bytesPerCluster);

        recover_contiguous(bootSector, firstCluster, clusters);
    }

    volume_root_begin(&it, volume);

    find = volume_root_first_free(&it, recover, NULL);

    if (!volume_find_result_is_ok(find))
    {
        goto recover_fragmented_utility_exit;
    }

    uint32_t results[COMBINATORIAL_SEARCH_K];

    clusters = volume_clusters(it.entry->fileSize, it.bytesPerCluster);
    find = combinatorial_search(results, clusters, &it, sha1);

    if (!volume_find_result_is_ok(find))
    {
        goto recover_fragmented_utility_exit;
    }

    *it.entry->name = *recover;

    for (uint32_t fat = 0; fat < bootSector->fats; fat++)
    {
        // From specification:
        //   BPB_ResvdSecCnt + (BPB_NumFATs * FATSz)

        uint32_t fatSector = bootSector->reservedSectors;

        fatSector += fat * bootSector->sectorsPerFat;

        uint32_t fatStartByte = fatSector * bootSector->bytesPerSector;
        uint32_t* fatData = (uint32_t*)((uint8_t*)volume->data + fatStartByte);

        for (uint32_t i = 0; i < clusters - 1; i++)
        {
            fatData[results[i]] = results[i + 1];
        }

        fatData[results[clusters - 1]] = VOLUME_EOF;
    }

recover_fragmented_utility_exit:
    {
        const char* message = volume_find_result_to_string(find);

        fprintf(output, "%s: %s\n", recover, message);
    }
}
