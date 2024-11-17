// options.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

/**
 * Specifies a command-line option. This enumeration supports a bitwise
 * combination of its member values.
 */
enum Options
{
    /** No options. */
    OPTIONS_NONE = 0x0,
    
    /** Print the file system information. */
    OPTIONS_INFORMATION = 0x1,

    /** List the root directory. */
    OPTIONS_LIST = 0x2,

    /** Recover a contiguous file. */
    OPTIONS_RECOVER_CONTIGUOUS = 0x4,

    /** Recover a possibly non-contiguous file. */
    OPTIONS_RECOVER_NON_CONTIGUOUS = 0x8,

    /** Recover a file. */
    OPTIONS_RECOVER =
        OPTIONS_RECOVER_CONTIGUOUS | OPTIONS_RECOVER_NON_CONTIGUOUS,

    /** The SHA1 digest. */
    OPTIONS_SHA1 = 0x10
};

/**
 * Specifies a command-line option. This enumeration supports a bitwise
 * combination of its member values.
 */
typedef enum Options Options;
