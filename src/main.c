// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/getopt.3.html
//  - https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "options.h"
#include "volume.h"
#define main_fail(app) do                                                      \
{                                                                              \
    perror((app));                                                             \
    exit(EXIT_FAILURE);                                                        \
}                                                                              \
while (false)
#define main_fail_usage(app) do                                                \
{                                                                              \
    printf(                                                                    \
        "Usage: %s disk <options>\n"                                           \
        "  -i                     Print the file system information.\n"        \
        "  -l                     List the root directory.\n"                  \
        "  -r filename [-s sha1]  Recover a contiguous file.\n"                \
        "  -R filename -s sha1    Recover a possibly non-contiguous file.\n",  \
        (app));                                                                \
    exit(EXIT_FAILURE);                                                        \
}                                                                              \
while (false)
#define main_assert_not_option(arg, app) do                                    \
{                                                                              \
    if (*(arg) == '-')                                                         \
    {                                                                          \
        main_fail_usage((app));                                                \
    }                                                                          \
}                                                                              \
while (false)

int main(int count, char* args[])
{
    if (count < 1)
    {
        return EXIT_FAILURE;
    }

    char* app = args[0];

    if (count < 2)
    {
        main_fail_usage(app);
    }

    struct Volume disk;

    if (!volume(&disk, args[1]))
    {
        main_fail(app);
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

            main_assert_not_option(recover, app);
            break;

        case 'R':
            options |= OPTIONS_RECOVER_NON_CONTIGUOUS;
            recover = optarg;

            main_assert_not_option(recover, app);
            break;

        case 's':
            options |= OPTIONS_SHA1;
            sha1 = optarg;

            main_assert_not_option(sha1, app);
            break;

        default: main_fail_usage(app);
        }
    }

    if (options == OPTIONS_NONE ||
        (options & OPTIONS_RECOVER) == OPTIONS_RECOVER ||
        (options & OPTIONS_INFORMATION && options != OPTIONS_INFORMATION) ||
        (options & OPTIONS_LIST && options != OPTIONS_LIST) ||
        (options & OPTIONS_SHA1 && !(options & OPTIONS_RECOVER)) ||
        (options & OPTIONS_RECOVER_NON_CONTIGUOUS && !(options & OPTIONS_SHA1)))
    {
        main_fail_usage(app);
    }

    printf("options: 0x%x, recover: %s, sha1: %s\n", options, recover, sha1);

    return EXIT_SUCCESS;
}
