// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/getopt.3.html
//  - https://www.man7.org/linux/man-pages/man3/sscanf.3.html
//  - https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html
//  - https://stackoverflow.com/questions/3408706/hexadecimal-string-to-byte-array-in-c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fat32_attributes.h"
#include "fat32_boot_sector.h"
#include "options.h"
#include "utility.h"
#include "volume_root_iterator.h"

static const Utility UTILITIES_BY_OPTIONS[] =
{
    [OPTIONS_INFORMATION] = information_utility,
    [OPTIONS_LIST] = list_utility,
    [OPTIONS_RECOVER_CONTIGUOUS] = recover_contiguous_utility,
    [OPTIONS_RECOVER_FRAGMENTED] = recover_fragmented_utility
};

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

    char* app = *args;

    if (count < 2)
    {
        main_print_usage(app);

        goto main_exit;
    }

    char* path = args[1];

    if (*path == '-')
    {
        main_print_usage(app);

        goto main_exit;
    }

    int option;
    char* recover = NULL;
    char* sha1String = "";
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

            if (*recover == '-')
            {
                main_print_usage(app);

                goto main_exit;
            }
            break;

        case 'R':
            options |= OPTIONS_RECOVER_FRAGMENTED;
            recover = optarg;

            if (*recover == '-')
            {
                main_print_usage(app);

                goto main_exit;
            }
            break;

        case 's':
            options |= OPTIONS_SHA1;
            sha1String = optarg;

            if (*sha1String == '-')
            {
                main_print_usage(app);

                goto main_exit;
            }
            break;

        default:
            main_print_usage(app);

            goto main_exit;
        }
    }

    if (options == OPTIONS_NONE ||
        (options & OPTIONS_RECOVER) == OPTIONS_RECOVER ||
        (options & OPTIONS_INFORMATION && options != OPTIONS_INFORMATION) ||
        (options & OPTIONS_LIST && options != OPTIONS_LIST) ||
        (options & OPTIONS_SHA1 && !(options & OPTIONS_RECOVER)) ||
        (options & OPTIONS_RECOVER_FRAGMENTED && !(options & OPTIONS_SHA1)))
    {
        main_print_usage(app);

        goto main_exit;
    }

    Volume disk;

    if (!volume(&disk, path))
    {
        perror(app);

        goto main_exit;
    }

    int i = 0;
    unsigned char sha1[SHA_DIGEST_LENGTH] = VOLUME_SHA1_NONE;
    
    while (i < SHA_DIGEST_LENGTH && 
        sscanf(sha1String + 2 * i, "%2hhx", sha1 + i) == 1)
    {
        i++;
    }

    for (Options mask = 0x8; mask; mask >>= 1)
    {
        if (options & mask)
        {
            UTILITIES_BY_OPTIONS[mask](stdout, &disk, recover, sha1);
        }
    }

    result = EXIT_SUCCESS;

    finalize_volume(&disk);

main_exit:
    return result;
}
