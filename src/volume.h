// volume.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <sys/types.h>
#include <stdbool.h>

/** Represents a FAT32 disk image. */
struct Volume
{
    off_t size;
    void* data;
};

/** Represents a FAT32 disk image. */
typedef struct Volume* Volume;

/**
 * Initializes an instance of the `Volume` struct.
 * 
 * @param instance the `Volume` instance
 * @param path     a pointer to a zero-terminated string containing the path to
 *                 the disk image
 * @return `true` if the operation succeeded; otherwise `false`. When `false`,
 *         `errno` is assigned to indicate the error.
 */
bool volume(Volume instance, char* path);

/**
 * Frees all resources.
 * 
 * @param instance the `Volume` instance. This method corrupts the `instance`
 *                 argument.
 */
void finalize_volume(Volume instance);
