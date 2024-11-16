// fat32.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References
//   - Microsoft Extensible Firmware Initiative FAT32 File System Specification

// From specification:
//   If(FATContent >= 0x0FFFFFF8)
//        IsEOF = TRUE;

/**
 * Determines whether a given value is an end of file (EOF) or indicator. An EOF
 * indicator may be used to indicate the end of a cluster chain.
 * 
 * @param value the value to test.
 * @return `true` if `value` is an EOF indicator; otherwise, `false`.
 */
#define fat32_is_eof(value) ((value) >= 0x0ffffff8)
