// option.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

/**
 * Specifies a command-line option. This enumeration supports a bitwise
 * combination of its member values.
 */
enum Options
{
    OPTIONS_NONE = 0x0,
    OPTIONS_INFORMATION = 0x1,
    OPTIONS_LIST = 0x2,
    OPTIONS_RECOVER_CONTIGUOUS = 0x4,
    OPTIONS_RECOVER_NON_CONTIGUOUS = 0x8,
    OPTIONS_RECOVER =
        OPTIONS_RECOVER_CONTIGUOUS | OPTIONS_RECOVER_NON_CONTIGUOUS,
    OPTIONS_SHA1 = 0x10
};

/**
 * Specifies a command-line option. This enumeration supports a bitwise
 * combination of its member values.
 */
typedef enum Options Options;
