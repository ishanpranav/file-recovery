// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/getopt.3.html

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "options.h"
#define main_assert_not_option(arg, args) do                                   \
{                                                                              \
    if (*(arg) == '-')                                                         \
    {                                                                          \
        main_print_usage((args));                                              \
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
}                                                                              \
while (0);

static void main_print_usage(char* args[])
{
    printf(
        "Usage: %s disk <options>\n"
        "  -i                     Print the file system information.\n"
        "  -l                     List the root directory.\n"
        "  -r filename [-s sha1]  Recover a contiguous file.\n"
        "  -R filename -s sha1    Recover a possibly non-contiguous file.\n",
        args[0]);
}

int main(int count, char* args[])
{
    if (count < 2)
    {
        main_print_usage(args);

        return EXIT_FAILURE;
    }

    char* disk = args[1];

    main_assert_not_option(disk, args);

    int option;
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
        
            main_assert_not_option(optarg, args);
            break;

        case 'R':
            options |= OPTIONS_RECOVER_NON_CONTIGUOUS;

            main_assert_not_option(optarg, args);
            break;

        case 's':
            options |= OPTIONS_SHA1;

            main_assert_not_option(optarg, args);
            break;

        default:
            main_print_usage(args);

            return EXIT_FAILURE;
        }
    }

    printf("options: 0x%x\n", options);

    if (options == OPTIONS_NONE ||
        (options & OPTIONS_RECOVER) == OPTIONS_RECOVER ||
        (options & OPTIONS_INFORMATION && options != OPTIONS_INFORMATION) ||
        (options & OPTIONS_LIST && options != OPTIONS_LIST) ||
        (options & OPTIONS_SHA1 && !(options & OPTIONS_RECOVER)) ||
        (options & OPTIONS_RECOVER_NON_CONTIGUOUS && !(options & OPTIONS_SHA1)))
    {
        main_print_usage(args);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
