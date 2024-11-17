// volume.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#ifndef VOLUME_H
#define VOLUME_H
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

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
 * @param instance the `Volume` instance.
 * @param path     a pointer to a zero-terminated string containing the path to
 *                 the disk image.
 * @return `true` if the operation succeeded; otherwise `false`. When `false`,
 *         `errno` is assigned to indicate the error.
 */
bool volume(Volume instance, char* path);

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
void finalize_volume(Volume instance);

#endif
