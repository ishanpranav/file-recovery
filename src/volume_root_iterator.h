// volume_root_iterator.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include "volume.h"

struct VolumeRootIterator
{
    Volume instance;
};

typedef struct VolumeRootIterator* VolumeRootIterator;

void volume_begin(VolumeRootIterator iterator, Volume instance);
bool volume_next(VolumeRootIterator iterator);
