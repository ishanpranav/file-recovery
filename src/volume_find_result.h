// volume_find_result.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

/**
 * Determine whether a given result indicates a successful search.
 *
 * @param value the value to test
 * @return `true` if `value` indicates that a match was found; otherwise,
 *         `false`.
 */
#define volume_find_result_is_ok(value) (value < 2)

/** Specifies the result of a volume search operation. */
enum VolumeFindResult
{
    /** A single candidate was discovered. */
    VOLUME_FIND_RESULT_NAME_FOUND = 0,

    /** At least one candidate was discovered with a matching SHA-1 hash. */
    VOLUME_FIND_RESULT_SHA1_FOUND = 1,

    /** No candidate was discovered. */
    VOLUME_FIND_RESULT_NOT_FOUND,

    /** Multiple candidates were discovered. */
    VOLUME_FIND_RESULT_MULTIPLE_FOUND,

    /** The number of volume find result enumeration members. */
    VOLUME_FIND_RESULT_COUNT
};

/** Specifies the result of a volume search operation. */
typedef enum VolumeFindResult VolumeFindResult;

/**
 * Returns a string representation of the search result.
 * 
 * @param value the value to convert.
 * @return a pointer to a zero-terminated string representing `value`. This
 *         value should not be modified or passed as an argument to `free`.
 */
const char* volume_find_result_to_string(VolumeFindResult value);
