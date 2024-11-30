// recover_contiguous_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <string.h>
#include "fat32_boot_sector.h"
#include "utility.h"
#include "volume_root_iterator.h"

void recover_contiguous_utility(
    FILE* output,
    Volume* volume,
    const char* recover,
    unsigned char sha1[SHA_DIGEST_LENGTH])
{
    VolumeRootIterator it;

    volume_root_begin(&it, volume);

    VolumeFindResult find = volume_root_single_free(&it, recover, sha1);
    const char* message = volume_find_result_to_string(find);

    fprintf(output, "%s: %s\n", recover, message);

    if (!volume_find_result_is_ok(find))
    {
        return;
    }

    uint32_t lo = it.entry->firstClusterLo;
    uint32_t hi = it.entry->firstClusterHi;
    uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);
    uint32_t clusters = volume_clusters(it.entry->fileSize, it.bytesPerCluster);
    
    volume_root_restore_contiguous(&it, firstCluster, clusters);
}
