// fat32_attributes.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References
//   - Microsoft Extensible Firmware Initiative FAT32 File System Specification

/** Specifies the file attributes of a directory entry. */
enum Fat32Attributes
{
    /** No attributes. */
    FAT32_ATTRIBUTES_NONE = 0x00,

    /** Writes to the file should fail. */
    FAT32_ATTRIBUTES_READ_ONLY = 0x01,

    /** Normal directory listings should not show this file. */
    FAT32_ATTRIBUTES_HIDDEN = 0x02,

    /** This is an operating system file. */
    FAT32_ATTRIBUTES_SYSTEM = 0x04,

    /**
     * There should only be one directory entry on the volume that has this
     * attribute set, and that file must be in the root directory. This name of
     * this file is actually the label for the volume. No data clusters are
     * allocated to the volume label file.
     */
    FAT32_ATTRIBUTES_VOLUME_ID = 0x08,

    /** This file is actually a container for other files. */
    FAT32_ATTRIBUTES_DIRECTORY = 0x10,

    /** Supports backup utilities. */
    FAT32_ATTRIBUTES_ARCHIVE = 0x20,

    /** The directory entry is part of a long name entry for another file. */
    FAT32_ATTRIBUTES_LONG_NAME =
        FAT32_ATTRIBUTES_READ_ONLY |
        FAT32_ATTRIBUTES_HIDDEN |
        FAT32_ATTRIBUTES_SYSTEM |
        FAT32_ATTRIBUTES_VOLUME_ID
};

/** Specifies the file attributes of a directory entry. */
typedef enum Fat32Attributes Fat32Attributes;
