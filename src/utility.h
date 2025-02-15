// utility.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <openssl/sha.h>
#include <stdio.h>
#include "fat32_boot_sector.h"
#include "volume.h"
#ifdef __GNUC__
#define UTILITY_UNUSED __attribute__ ((unused))
#else
#define UTILITY_UNUSED
#endif

/** Represents a file-system utility. */
typedef void (*Utility)(
    FILE* output, 
    Volume* volume, 
    const char* recover, 
    unsigned char sha1[SHA_DIGEST_LENGTH]);

/**
 * Prints the file system information.
 *
 * @param output  the output stream.
 * @param volume  the FAT32 disk image.
 * @param recover unused.
 * @param sha1    unused.
 */
void information_utility(
    FILE* output,
    Volume* volume,
    const char* recover,
    unsigned char sha1[SHA_DIGEST_LENGTH]);

/**
 * Lists the entries in the root directory.
 *
 * @param output  the output stream.
 * @param volume  the FAT32 disk image.
 * @param recover unused.
 * @param sha1    unused.
 */
void list_utility(
    FILE* output,
    Volume* volume,
    const char* recover,
    unsigned char sha1[SHA_DIGEST_LENGTH]);

/**
 * 
 * @param bootSector
 * @param firstCluster
 * @param clusters
 */
void recover_contiguous(
    Fat32BootSector* bootSector, 
    uint32_t firstCluster, 
    uint32_t clusters);

/**
 * Recovers a contiguous file.
 *
 * @param output  the output stream.
 * @param volume  the FAT32 disk image.
 * @param recover a pointer to a zero-terminated string containing the name of
 *                the file to recover.
 * @param sha1    the SHA1 hash digest of the file, or `NULL`.
 */
void recover_contiguous_utility(
    FILE* output,
    Volume* volume,
    const char* recover,
    unsigned char sha1[SHA_DIGEST_LENGTH]);

/**
 * Recovers a fragmented (non-contiguous) file.
 *
 * @param output  the output stream.
 * @param volume  the FAT32 disk image.
 * @param recover a pointer to a zero-terminated string containing the name of
 *                the file to recover.
 * @param sha1    the SHA1 hash digest of the file.
 */
void recover_fragmented_utility(
    FILE* output,
    Volume* volume,
    const char* recover,
    unsigned char sha1[SHA_DIGEST_LENGTH]);
