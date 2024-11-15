// main.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdio.h>

static void main_print_usage(FILE* output)
{
    fprintf(output, 
        "Usage: ./nyufile disk <options>\n"
        "  -i                     Print the file system information.\n"
        "  -l                     List the root directory.\n"
        "  -r filename [-s sha1]  Recover a contiguous file.\n"
        "  -R filename -s sha1    Recover a possibly non-contiguous file.\n");
}

int main()
{
    main_print_usage(stdout);
    
    return 0;
}
