// volume_root_iterator.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include "fat32_directory_entry.h"
#include "volume.h"
#include "volume_find_result.h"

/** Iterates over the entries in the root directory of a volume. */
struct VolumeRootIterator
{
    /** `true` if there are no more entries; otherwise, `false`. */
    bool end;

    /** Specifies the current cluster number. */
    uint32_t cluster;
    
    /** Specifies the sector number of the first data sector. */
    uint32_t firstDataSector;

    /** Specifies the current byte offset within the volume. */
    uint32_t offset;

    /** Specifies the number of bytes per cluster. */
    uint32_t bytesPerCluster;

    /** The sector data. */
    uint8_t* data;

    /** The current entry. */
    Fat32DirectoryEntry* entry;

    /** The volume instance. */
    Volume* instance;
};

/** Iterates over the entries in the root directory of a volume. */
typedef struct VolumeRootIterator VolumeRootIterator;

/**
 * Provides an iterator over the entries in the root directories of a volume.
 * 
 * @param iterator when this method returns, contains the iterator. This
 *                 argument is passed uninitialized.
 * @param instance the volume whose root directory entries to iterate.
*/
void volume_root_begin(VolumeRootIterator* iterator, Volume* instance);

/**
 * Advances the iterator to the next directory entry.
 * 
 * @param iterator the iterator.
 */
void volume_root_next(VolumeRootIterator* iterator);

/**
 * Advances the iterator to the next directory entry that is a free file stored
 * contiguously whose name matches the given file name and whose SHA-1 digest
 * matches the given digest, if any. If there is no match, the position of the
 * iterator is not specified.
 * 
 * @param iterator the iterator.
 * @param fileName a pointer to a zero-terminated string containing the file
 *                 name to match. Note that the first character is ignored in
 *                 the comparison.
 * @param sha1     the SHA-1 digest to match, or `VOLUME_SHA1_NONE`.
 * @return `VOLUME_FIND_RESULT_NAME_FOUND` if the iterator points to an entry
 *         with a matching name, `VOLUME_FIND_RESULT_SHA1_FOUND` if the
 *         iterator points to an entry with a matching digest, or
 *         `VOLUME_FIND_RESULT_NOT_FOUND` if there is no match.
 */
VolumeFindResult volume_root_first_free(
    VolumeRootIterator* iterator,
    const char* fileName,
    unsigned char sha1[SHA_DIGEST_LENGTH]);

/**
 * Advances the iterator to the single unique directory entry that is a free
 * file stored contiguously whose name matches the given file and whose SHA-1
 * digest matches the given digest, if any. If there is no match, the position
 * of the iterator is not specified. If there are multiple matches, the iterator
 * points to the second match.
 * 
 * @param iterator the iterator.
 * @param fileName a pointetr to a zero-terminated string containing the file
 *                 name to match. Note that the first character is ignored in
 *                 the comparison.
 * @param sha1     the SHA-1 digest to match, or `VOLUME_SHA1_NONE`.
 * @return If the iterator points to an entry with a matching name, then
 *         `VOLUME_FIND_RESULT_NAME_FOUND`; if the iterator points to an entry
 *         with a matching digest, then `VOLUME_FIND_RESULT_SHA1_FOUND`; if the
 *         iterator points to the second matching entry, then
 *         `VOLUME_FIND_RESULT_MULTIPLE_FOUND`; otherwise,
 *         `VOLUME_FIND_RESULT_NOT_FOUND` if there is no match.
*/
VolumeFindResult volume_root_single_free(
    VolumeRootIterator* iterator,
    const char* fileName,
    unsigned char sha1[SHA_DIGEST_LENGTH]);
