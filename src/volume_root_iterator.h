// volume_root_iterator.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include "fat32_directory_entry.h"
#include "volume.h"
#include "volume_find_result.h"

/** Iterates over the entries in the root directory of a volume. */
struct VolumeRootIterator
{
    bool end;
    uint32_t cluster;
    uint32_t firstDataSector;
    uint32_t offset;
    uint32_t bytesPerCluster;
    uint8_t* data;
    Fat32DirectoryEntry entry;
    Volume instance;
};

/** Iterates over the entries in the root directory of a volume. */
typedef struct VolumeRootIterator* VolumeRootIterator;

/**
 * Provides an iterator over the entries in the root directories of a volume.
 * 
 * @param iterator when this method returns, contains the iterator. This
 *                 argument is passed uninitialized.
 * @param instance the volume whose root directory entries to iterate.
*/
void volume_root_begin(VolumeRootIterator iterator, Volume instance);

/**
 * Advances the iterator to the next directory entry.
 * 
 * @param iterator the iterator.
 */
void volume_root_next(VolumeRootIterator iterator);

/**
 * 
 * @param iterator
 * @param fileName
 * @param sha1
 * @return
 */
bool volume_root_first_free(
    VolumeRootIterator iterator,
    const char* fileName,
    unsigned char sha1[SHA_DIGEST_LENGTH]);

/**
 * 
 * @param result
 * @param iterator
 * @param fileName
 * @param sha1
 * @return
*/
VolumeFindResult volume_root_single_free(
    VolumeRootIterator iterator,
    const char* fileName,
    unsigned char sha1[SHA_DIGEST_LENGTH]);
