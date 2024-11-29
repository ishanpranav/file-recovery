// recover_contiguous_utility.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <string.h>
#include "utility.h"
#include "volume_root_iterator.h"

void recover_contiguous_utility(
    FILE* output,
    Volume volume,
    char* recover,
    UTILITY_UNUSED char* sha1)
{
    struct VolumeRootIterator it;
    
    volume_root_begin(&it, volume);

    VolumeFindResult find = volume_root_single(&it, recover);
    char* message = volume_find_result_to_string(find);

    if (!find)
    {
        fprintf(output, "%s: %s\n", recover, message);

        return;
    }

    fprintf(output, "%s: %s\n", recover, message);
}
