// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/getopt.3.html
//  - https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fat32_boot_sector.h"
#include "options.h"
#include "volume_root_iterator.h"

static void main_print_usage(char* app)
{
    printf(
        "Usage: %s disk <options>\n"
        "  -i                     Print the file system information.\n"
        "  -l                     List the root directory.\n"
        "  -r filename [-s sha1]  Recover a contiguous file.\n"
        "  -R filename -s sha1    Recover a possibly non-contiguous file.\n",
        app);
}

int main(int count, char* args[])
{
    int result = EXIT_FAILURE;

    if (count < 1)
    {
        goto main_exit;
    }

    char* app = args[0];

    if (count < 2)
    {
        main_print_usage(app);

        goto main_exit;
    }

    char* path = args[1];

    if (path[0] == '-')
    {
        main_print_usage(app);

        goto main_exit;
    }

    int option;
    char* recover = NULL;
    char* sha1 = NULL;
    Options options = OPTIONS_NONE;

    while ((option = getopt(count - 1, args + 1, ":ilr:R:s:")) != -1)
    {
        switch (option)
        {
        case 'i':
            options |= OPTIONS_INFORMATION;
            break;

        case 'l':
            options |= OPTIONS_LIST;
            break;

        case 'r':
            options |= OPTIONS_RECOVER_CONTIGUOUS;
            recover = optarg;

            if (recover[0] == '-')
            {
                main_print_usage(app);

                goto main_exit_volume;
            }
            break;

        case 'R':
            options |= OPTIONS_RECOVER_NON_CONTIGUOUS;
            recover = optarg;

            if (recover[0] == '-')
            {
                main_print_usage(app);

                goto main_exit_volume;
            }
            break;

        case 's':
            options |= OPTIONS_SHA1;
            sha1 = optarg;

            if (sha1[0] == '-')
            {
                main_print_usage(app);

                goto main_exit_volume;
            }
            break;

        default:
            main_print_usage(app);

            goto main_exit_volume;
        }
    }

    if (options == OPTIONS_NONE ||
        (options & OPTIONS_RECOVER) == OPTIONS_RECOVER ||
        (options & OPTIONS_INFORMATION && options != OPTIONS_INFORMATION) ||
        (options & OPTIONS_LIST && options != OPTIONS_LIST) ||
        (options & OPTIONS_SHA1 && !(options & OPTIONS_RECOVER)) ||
        (options & OPTIONS_RECOVER_NON_CONTIGUOUS && !(options & OPTIONS_SHA1)))
    {
        main_print_usage(app);

        goto main_exit_volume;
    }

    struct Volume disk;

    if (!volume(&disk, path))
    {
        perror(app);

        goto main_exit;
    }

    Fat32BootSector bootSector = disk.data;

    if (options & OPTIONS_INFORMATION)
    {
        printf(
            "Number of FATs = %" PRId8 "\n"
            "Number of bytes per sector = %" PRId8 "\n"
            "Number of sectors per cluster = %" PRId8 "\n"
            "Number of reserved sectors = %" PRId8 "\n",
            bootSector->fats,
            bootSector->bytesPerSector,
            bootSector->sectorsPerCluster,
            bootSector->reservedSectors);
    }

    if (options & OPTIONS_LIST)
    {
        // uint32_t entries = 0;
        struct VolumeRootIterator it;

        volume_begin(&it, &disk);
        // for (volume_begin(&it, &disk); volume_next(&it); entries++)
        // {
        //     printf("entry\n");
        // }
    }

    result = EXIT_SUCCESS;

main_exit_volume:
    finalize_volume(&disk);

main_exit:
    return result;
}
