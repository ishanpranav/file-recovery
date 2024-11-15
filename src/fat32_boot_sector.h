// fat32_boot_sector.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References
//   - Microsoft Extensible Firmware Initiative FAT32 File System Specification

#include <stdint.h>
#pragma pack(push, 1)

/** Represents a Microsoft FAT32 boot sector. */
struct Fat32BootSector
{
    /**
     * Specifies the jump instruction to boot code. This field has two allowed
     * forms: `0xeb 0x?? 0x90` and `0xe9 0x?? 0x??`. Note `0x??` indicates any
     * 8-bit. This forms a three-byte Intel x86 unconditional branch (jump)
     * instruction that jumps to the start of the operating system bootstrap
     * code. This code typically occupies the rest of sector 0 of the volume
     * following the BIOS parameter block and possibly other sectors. Either of
     * these forms is acceptable. `0xeB 0x?? 0x90` is the more frequently used
     * format.
     * 
     * This corresponds to the `BS_jmpBoot` field.
     */
    uint8_t jumpBoot[3];

    /** The string `“MSWIN4.1”`. This corresponds to the `BS_OEMName` field. */
    uint8_t oemName[8];

    /**
     * Specifies the count of bytes per sector: `512`, `1024`, `2048` or `4096`.
     * If maximum compatibility with old implementations is desired, only the
     * value `512` should be used.
     * 
     * This corresponds to the `BPB_BytsPerSec` field.
     */
    uint16_t bytesPerSector;

    /**
     * Specifies the number of sectors per allocation unit. This value must be
     * a power of 2 that is greater than 0: `1`, `2`, `4`, `8`, `16`, `32`,
     * `64`, or `128`. A value should never be used that results in a "bytes per
     * cluster" value (`bytesPerSector` * `sectorsPerCluster`) greater than
     * `32768`.
     * 
     * This corresponds to the `BPB_SecPerClus` field.
     */
    uint8_t sectorsPerCluster;

    /**
     * Specifies the number of reserved sectors. This field must not be `0`.
     * Typically this is `32`.
     * 
     * This corresponds to the `BPB_RsvdSecCnt` field.
     */
    uint16_t reservedSectors;

    /**
     * Specifies the count of FAT data structures on the volume. This field must
     * be `2`.
     * 
     * This corresponds to the `BPB_NumFATs` field.
     */
    uint8_t fats;

    /**
     * Specifies the count of directory entries in the root directory. This
     * field must be `0`.
     * 
     * This corresponds to the `BPB_RootEntCnt` field.
     */
    uint16_t rootEntries;

    /**
     * Deprecated. This field must be `0`.
     * 
     * This corresponds to the `BPB_TotSec16` field.
     */
    uint16_t totalSectors16;

    /**
     * Specifies the media type: `0xf0`, `0xf8`, `0xf9`, `0xfA`, `0xfB`,
     * `0xfc`, `0xfd`, or `0xfe`.
     * 
     * This corresponds to the `BPB_Media` field.
     */
    uint8_t media;

    /**
     * Deprecated. This field must be `0`.
     *
     * This corresponds to the `BPB_FATSz16` field. 
     */
    uint16_t fatSize16;

    /**
     * Specifies the "sectors per track" for the storage device.
     * 
     * This corresponds to the `BPB_SecPerTrk` field.
     */
    uint16_t sectorsPerTrack;

    /**
     * Specifies the number of heads for the storage device.
     * 
     * This corresponds to the `BPB_NumHeads` field.
     */
    uint16_t heads;

    /**
     * Specifies the count of hidden sectors preceding the partition that
     * contains this FAT volume.
     * 
     * This corresponds to the `BPB_HiddSec` field.
    */
    uint32_t hiddenSectors;
    
    /**
     * Specifies the count of all sectors in all four regions of the volume.
     * 
     * This corresponds to the `BPB_TotSec32` field.
     */
    uint32_t totalSectors; 

    /**
     * Specifies the number of sectors occupied by one FAT.
     * 
     * This corresponds to the `BPB_FATSz32` field.
     */
    uint32_t fatSize;

    /**
     * Specifies a bitwise combination of flags. Bits 0 to 3: specifies the
     * zero-based number of the active FAT. This value is only valid if
     * mirroring is disabled. Bits 4 to 6: reserved. Bit 7: `0` if the FAT is
     * mirrored; otherwise, `1`. Bits 8 to 15: reserved.
     * 
     * This corresponds to the `BPB_ExtFlags` field.
     */
    uint16_t extendedFlags;

    /**
     * Specifies the file system version number. The high byte is the major
     * revision number. The low byte is the minor revision number.
     * 
     * This corresponds to the `BPB_FSVer` field.
     */
    uint16_t fileSystemVersion;

    /**
     * Specifies the cluster number of the first cluster in the root directory.
     * Typically this is `2`.
     * 
     * This corresponds to the `BPB_RootClus` field.
     */
    uint32_t rootCluster;

    /**
     * Specifies the sector number of the FSINFO structure in the reserved area
     * of the FAT32 volume. Typically this is `1`.
     * 
     * This corresponds to the `BPB_FSInfo` field.
     */
    uint16_t fileSystemInfoSector;

    /**
     * Specifies the sector number of the backup boot sector in the reserved
     * area of the FAT32 volume. Typically this is `6`.
     * 
     * This corresponds to the `BPB_BkBootSec` field.
     */
    uint16_t backupBootSector;
    
    /** Reserved. This corresponds to the `BPB_Reserved` field. */
    uint8_t reserved[12];

    /**
     * Specifies the drive number. This corresponds to the `BS_DrvNum` field.
     */
    uint8_t driveNumber;
    
    /** Reserved. This corresponds to the `BS_Reserved1` field. */
    uint8_t reserved1;

    /**
     * Specifies the extended boot signature: `0x29`.
     * 
     * This corresponds to the `BS_BootSig` field.
     */
    uint8_t bootSignature;

    /**
     * Specifies the volume serial number.
     * 
     * This corresponds to the `BS_VolID` field.
     */
    uint32_t volumeId;

    /**
     * Specifies the volume label.
     * 
     * This corresponds to the `BS_VolLab` field.
     */
    uint8_t volumeLabel[11];

    /**
     * The string `"FAT32   "`. This corresponds to the `BS_FilSysType` field.
     */
    uint8_t fileSystemType[8];
};

/** Represents a Microsoft FAT32 boot sector. */
typedef struct Fat32BootSector* Fat32BootSector;

#pragma pack(pop)
