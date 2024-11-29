// volume_find_result.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stddef.h>
#include "volume_find_result.h"

static char* VOLUME_FIND_RESULT_STRINGS[VOLUME_FIND_RESULT_COUNT] =
{
    [VOLUME_FIND_RESULT_OK] = "successfully recovered",
    [VOLUME_FIND_RESULT_NOT_FOUND] = "file not found",
    [VOLUME_FIND_RESULT_MULTIPLE_FOUND] = "multiple candidates found"
};

char* volume_find_result_to_string(VolumeFindResult value)
{
    if (value < 0 || value >= VOLUME_FIND_RESULT_COUNT)
    {
        return NULL;
    }

    return VOLUME_FIND_RESULT_STRINGS[value];
}
