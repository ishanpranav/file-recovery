// fat32_directory.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdint.h>
#pragma pack(push, 1)

/**
 * Determines whether a given entry is a mid-directory free entry (`0xe5`). To
 * test if a directory is free, test both `fat32_directory_entry_is_mid_free`
 * and `fat32_directory_entry_is_end_free`.
 * 
 * @param value a pointer to the value to test.
 * @return `true` if `value` is an `0xe5` free entry; otherwise, `false`.
 */
#define fat32_directory_entry_is_mid_free(value) (*(value)->name == 0xe5)

/**
 * Determines whether a given entry is an end-directory free entry (`0x00`). To
 * test if a directory is free, test both `fat32_directory_entry_is_mid_free`
 * and `fat32_directory_entry_is_end_free`.
 * 
 * @param value a pointer to the value to test.
 * @return `true` if `value` is an `0x00` free entry; otherwise, `false`.
 */
#define fat32_directory_entry_is_end_free(value) (!*(value)->name)

/**
 * Gets the 32-bit cluster number of the first cluster in an entry's cluster
 * chain.
 * 
 * @param lo the lower 16 bits of the first cluster number.
 * @param hi the upper 16 bits of the first cluster number.
 * @return the 32-bit concatenation of `lo` an `hi`.
 */
#define fat32_directory_entry_first_cluster(lo, hi) (((hi) << 16) | (lo))

/** Represents a Microsoft FAT32 directory entry. */
struct Fat32DirectoryEntry
{
    /**
     * Specifies the directory short name.
     * 
     * This corresponds to the `DIR_Name` field.
     */
    uint8_t name[11];
    
    /**
     * Specifies the file attributes. This field is a bitwise combination of
     * attribute flags.
     * 
     * This corresponds to the `DIR_Attr` field.
     */
    uint8_t attributes;

    /** Reserved. This corresponds to the `DIR_NTRes` field. */
    uint8_t ntReserved;

    /**
     * Specifies the count of tenths of a second of the creation time.
     * 
     * This corresponds to the `DIR_CrtTimeTenth` field.
     */
    uint8_t  createdTimeTenth;

    /**
     * Specifies the created time.
     * 
     * This corresponds to the `DIR_CrtTime` field.
     */
    uint16_t createdTime;

    /**
     * Specifies the created time.
     * 
     * This corresponds to the `DIR_CrtDate` field.
     */
    uint16_t createdDate;
    
    /**
     * Specifies the last accessed date.
     * 
     * This corresponds to the `DIR_LstAccDate` field.
     */
    uint16_t lastAccessedDate;

    /**
     * Specifies the high two bytes of this entry's first cluster number.
     *
     * This corresponds to the `DIR_FstClusHI` field.
     */
    uint16_t firstClusterHi;
    
    /**
     * Specifies the written time.
     * 
     * This corresponds to the `DIR_WrtDate` field.
     */
    uint16_t writtenTime;
    
    /**
     * Specifies the written date.
     * 
     * This corresponds to the `DIR_WrtDate` field.
     */
    uint16_t writtenDate;

    /**
     * Specifies the low two bytes of this entry's first cluster number.
     * 
     * This corresponds to the `DIR_FstClusLO` field.
     */
    uint16_t firstClusterLo;

    /**
     * Specifies the file size in bytes.
     * 
     * This corresponds to the `DIR_FileSize` field.
     */
    uint32_t fileSize;
};

/** Represents a Microsoft FAT32 directory entry. */
typedef struct Fat32DirectoryEntry Fat32DirectoryEntry;

#pragma pack(pop)
