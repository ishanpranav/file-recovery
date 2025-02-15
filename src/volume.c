// volume.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://man7.org/linux/man-pages/man2/close.2.html
//  - https://www.man7.org/linux/man-pages/man3/fstat.3p.html
//  - https://www.man7.org/linux/man-pages/man2/mmap.2.html
//  - https://www.man7.org/linux/man-pages/man2/open.2.html
//  - https://www.man7.org/linux/man-pages/man3/stat.3type.html
//  - https://docs.openssl.org/1.0.2/man3/sha
//  - Microsoft Extensible Firmware Initiative FAT32 File System Specification

#include <openssl/sha.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fat32_attributes.h"
#include "fat32_boot_sector.h"
#include "volume_root_iterator.h"

// From specification:
//   If(FATContent >= 0x0FFFFFF8)
//        IsEOF = TRUE;

#define volume_is_eof(value) ((value) >= 0x0ffffff8)

bool volume(Volume* instance, const char* path)
{
    bool result = false;
    int descriptor = open(path, O_RDWR);

    if (descriptor == -1)
    {
        goto volume_exit;
    }

    struct stat status;

    if (fstat(descriptor, &status) == -1)
    {
        goto volume_exit_open;
    }

    void* data = mmap(
        NULL,
        status.st_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        descriptor,
        0);

    if (data == MAP_FAILED)
    {
        goto volume_exit_open;
    }

    instance->size = status.st_size;
    instance->data = data;
    result = true;

volume_exit_open:
    close(descriptor);

volume_exit:
    return result;
}

void volume_display_name(char buffer[13], uint8_t name[11])
{
    if (*name == 0x20)
    {
        *buffer = '\0';

        return;
    }

    char* end = buffer;

    memcpy(end, name, 8);

    end += 7;

    if (*buffer == 0x05)
    {
        *buffer = (char)0xe5;
    }

    while (end > buffer && (*end == '\0' || *end == 0x20))
    {
        end--;
    }

    end++;
    *end = '.';

    char* extension = end;

    end++;

    memcpy(end, name + 8, 3);

    end += 2;

    while (end >= extension && (*end == '\0' || *end == 0x20))
    {
        end--;
    }

    if (end == extension && *end == '.')
    {
        end--;
    }

    end++;
    *end = '\0';
}

uint32_t volume_clusters(uint32_t fileSize, uint32_t bytesPerCluster)
{
    return (fileSize + bytesPerCluster - 1) / bytesPerCluster;
}

static void volume_root_reset_offset(VolumeRootIterator* iterator)
{
    Fat32BootSector* bootSector = iterator->instance->data;

    // From specification:
    //   FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;

    uint32_t sector = (iterator->cluster - 2) * bootSector->sectorsPerCluster;

    sector += iterator->firstDataSector;

    uint8_t* data = iterator->instance->data;

    data += sector * bootSector->bytesPerSector;
    iterator->offset = 0;
    iterator->data = data;
}

void volume_root_begin(VolumeRootIterator* iterator, Volume* instance)
{
    iterator->instance = instance;

    Fat32BootSector* bootSector = instance->data;

    // From specification:

    //   RootDirSectors =
    //     ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;

    uint32_t rootSectors = bootSector->rootEntries;

    rootSectors *= sizeof(Fat32DirectoryEntry);
    rootSectors += bootSector->bytesPerSector - 1;
    rootSectors /= bootSector->bytesPerSector;

    // From specification:

    //   FirstDataSector =
    //     BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;

    uint32_t firstDataSector = bootSector->reservedSectors;

    firstDataSector += bootSector->fats * bootSector->sectorsPerFat;
    firstDataSector += rootSectors;

    uint32_t bytesPerCluster = bootSector->sectorsPerCluster;

    bytesPerCluster *= bootSector->bytesPerSector;
    iterator->cluster = bootSector->rootCluster;
    iterator->firstDataSector = firstDataSector;
    iterator->bytesPerCluster = bytesPerCluster;

    volume_root_reset_offset(iterator);

    iterator->entry = (Fat32DirectoryEntry*)(iterator->data + iterator->offset);
    iterator->end = volume_is_eof(iterator->cluster);
}

void volume_root_next(VolumeRootIterator* iterator)
{
    uint32_t step = sizeof(Fat32DirectoryEntry);

    if (iterator->offset < iterator->bytesPerCluster - step)
    {
        iterator->offset += step;

        uint8_t* pointer = iterator->data + iterator->offset;

        iterator->entry = (Fat32DirectoryEntry*)pointer;
        iterator->end = false;

        return;
    }

    if (volume_is_eof(iterator->cluster))
    {
        iterator->end = true;

        return;
    }

    // From specification:
    //   FATOffset = N * 4;

    uint32_t fatOffset = iterator->cluster * 4;

    // From specification:
    //   ThisFATSecNum = BPB_ResvdSecCnt + (FATOffset / BPB_BytsPerSec);

    Fat32BootSector* bootSector = iterator->instance->data;
    uint32_t fatSector = bootSector->reservedSectors;

    fatSector += fatOffset / bootSector->bytesPerSector;

    uint32_t fatEntryOffset = fatOffset % bootSector->bytesPerSector;

    // From specification:

    //   FAT32ClusEntryVal =
    //     (*((DWORD *) &SecBuff[ThisFATEntOffset])) & 0x0FFFFFFF;

    uint8_t* buffer = iterator->instance->data;

    buffer += fatSector * bootSector->bytesPerSector;
    iterator->cluster = *(uint32_t*)(buffer + fatEntryOffset) & 0x0fffffff;

    volume_root_reset_offset(iterator);

    iterator->entry = (Fat32DirectoryEntry*)(iterator->data + iterator->offset);
    iterator->end = volume_is_eof(iterator->cluster);
}

uint8_t* volume_root_data(VolumeRootIterator* iterator, uint32_t cluster)
{
    Fat32BootSector* bootSector = iterator->instance->data;
    uint8_t* result = iterator->instance->data;

    result += iterator->firstDataSector * bootSector->bytesPerSector;
    result += (cluster - 2) * iterator->bytesPerCluster;

    return result;
}

VolumeFindResult volume_root_first_free(
    VolumeRootIterator* iterator,
    const char* fileName,
    unsigned char sha1[SHA_DIGEST_LENGTH])
{
    if (*fileName == '\0')
    {
        return VOLUME_FIND_RESULT_NOT_FOUND;
    }

    for (; !iterator->end; volume_root_next(iterator))
    {
        if (iterator->entry->attributes & FAT32_ATTRIBUTES_DIRECTORY ||
            iterator->entry->attributes & FAT32_ATTRIBUTES_VOLUME_ID ||
            iterator->entry->attributes & FAT32_ATTRIBUTES_LONG_NAME ||
            !(fat32_directory_entry_is_end_free(iterator->entry) ||
                fat32_directory_entry_is_mid_free(iterator->entry)))
        {
            continue;
        }

        char buffer[13];

        volume_display_name(buffer, iterator->entry->name);

        if (*buffer != '\0' && strcmp(buffer + 1, fileName + 1) == 0)
        {
            if (!sha1)
            {
                return VOLUME_FIND_RESULT_NAME_FOUND;
            }

            unsigned char digest[SHA_DIGEST_LENGTH];
            uint32_t hi = iterator->entry->firstClusterHi;
            uint32_t lo = iterator->entry->firstClusterLo;
            uint32_t firstCluster = fat32_directory_entry_first_cluster(lo, hi);
            uint8_t* data = volume_root_data(iterator, firstCluster);
            
            SHA1(data, iterator->entry->fileSize * sizeof * data, digest);

            if (memcmp(digest, sha1, SHA_DIGEST_LENGTH) == 0)
            {
                return VOLUME_FIND_RESULT_SHA1_FOUND;
            }
        }
    }

    return VOLUME_FIND_RESULT_NOT_FOUND;
}

VolumeFindResult volume_root_single_free(
    VolumeRootIterator* iterator,
    const char* fileName,
    unsigned char sha1[SHA_DIGEST_LENGTH])
{
    VolumeFindResult first = volume_root_first_free(iterator, fileName, sha1);

    if (first == VOLUME_FIND_RESULT_NOT_FOUND || iterator->end)
    {
        return first;
    }

    VolumeRootIterator copy = *iterator;

    volume_root_next(&copy);

    VolumeFindResult second = volume_root_first_free(&copy, fileName, sha1);

    if (second != VOLUME_FIND_RESULT_NOT_FOUND)
    {
        return VOLUME_FIND_RESULT_MULTIPLE_FOUND;
    }

    return first;
}

void finalize_volume(Volume* instance)
{
    munmap(instance->data, instance->size);
}
