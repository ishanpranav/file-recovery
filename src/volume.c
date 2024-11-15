// volume.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://man7.org/linux/man-pages/man2/close.2.html
//  - https://www.man7.org/linux/man-pages/man3/fstat.3p.html
//  - https://www.man7.org/linux/man-pages/man2/mmap.2.html
//  - https://www.man7.org/linux/man-pages/man2/open.2.html
//  - https://www.man7.org/linux/man-pages/man3/stat.3type.html

#include "volume.h"

bool volume(Volume instance, char* path)
{
    return false; // TODO: not implemented
}

void finalize_volume(Volume instance)
{

}

/*static char* main_map()
{    
    int descriptor = open(disk, O_RDONLY);

    if (descriptor == -1)
    {
        main_fail(app);
    }

    struct stat status;

    if (fstat(descriptor, &status) == -1)
    {
        perror(app);
        close(descriptor);
        exit(EXIT_FAILURE);
    }

    if (close(descriptor) == -1)
    {
        main_fail(app);
    }
}*/
