// volume_find_result.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

/** Specifies the result of a volume search operation. */
enum VolumeFindResult
{
    /** A single candidate was discovered. */
    VOLUME_FIND_RESULT_OK = 0,

    /** No candidate was discovered. */
    VOLUME_FIND_RESULT_NOT_FOUND,

    /** Multiple candidates were discovered. */
    VOLUME_FIND_RESULT_MULTIPLE_FOUND,
    
    /** At least one candidate was discovered with a matching SHA-1 hash. */
    VOLUME_FIND_RESULT_SHA1_FIND,

    /** The number of volume find result enumeration members. */
    VOLUME_FIND_RESULT_COUNT
};

/** Specifies the result of a volume search operation. */
typedef enum VolumeFindResult VolumeFindResult;

/**
 * 
 * @param value
 * @return 
 */
char* volume_find_result_to_string(VolumeFindResult value);
