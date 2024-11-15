// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - https://www.man7.org/linux/man-pages/man3/getopt.3.html

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
    int option;

    while ((option = getopt(count, args, "ilr:R:s:")) != -1)
    {
        switch (option)
        {
        case 'i':
            break;

        case 'l': break;
        case 'r': break;
        case 'R': break;
        case 's': break;

        default:
            main_print_usage(args);
            return EXIT_FAILURE;
        }
    }
        
    if (optind >= count)
    {
        main_print_usage(args);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
