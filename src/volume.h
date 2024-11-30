// volume.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#ifndef VOLUME_H
#define VOLUME_H
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

/** Specifies the SHA-1 hash digest length. */
#define SHA_DIGEST_LENGTH 20

/** Specifies the null or missing SHA-1 hash digest. */
#define VOLUME_SHA1_NONE "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

// From specification:
//   Microsoft operating system FAT drivers use the EOC value `0x0FFF` for FAT12
//   `0xFFFF` for FAT16, and `0x0FFFFFFF` for FAT32 when they set the contents
//   of a cluster to the EOC mark.

/** Specifies the value used to indicate the end of a cluster chain. */
#define VOLUME_EOF 0x0fffffff

/** Represents a FAT32 disk image. */
struct Volume
{
    off_t size;
    void* data;
};

/** Represents a FAT32 disk image. */
typedef struct Volume Volume;

/**
 * Initializes an instance of the `Volume` struct.
 * 
 * @param instance the `Volume` instance.
 * @param path     a pointer to a zero-terminated string containing the path to
 *                 the disk image.
 * @return `true` if the operation succeeded; otherwise `false`. When `false`,
 *         `errno` is assigned to indicate the error.
 */
bool volume(Volume* instance, const char* path);

/**
 * Converts a short directory entry name to a short display name.
 * 
 * @param buffer when this method returns, contains the short display name as a
 *               zero-terminated string. This argument is passed uninitialized.
 * @param name   the directory entry name.
 */
void volume_get_display_name(char buffer[13], uint8_t name[11]);

/**
 * Frees all resources.
 * 
 * @param instance the `Volume` instance. This method corrupts the `instance`
 *                 argument.
 */
void finalize_volume(Volume* instance);

#endif
