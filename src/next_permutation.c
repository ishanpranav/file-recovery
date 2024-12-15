// next_permutation.c
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

// References:
//  - Permutation - Generation in lexicographic order: https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order

#include <stdint.h>
#include "next_permutation.h"

static void swap(uint32_t* left, uint32_t* right)
{
    uint32_t item = *left;

    *left = *right;
    *right = item;
}

static void reverse(uint32_t* left, uint32_t* right)
{
    while (left < right)
    {
        swap(left, right);

        left++;
        right--;
    }
}

bool next_permutation(uint32_t* items, int length)
{
    int i = length - 2;

    while (i >= 0 && items[i] >= items[i + 1])
    {
        i--;
    }

    if (i < 0)
    {
        return false;
    }

    int j = length - 1;

    while (items[j] <= items[i])
    {
        j--;
    }

    swap(items + i, items + j);
    reverse(items + i + 1, items + length - 1);

    return true;
}
