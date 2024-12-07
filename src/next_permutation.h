// next_permutation.h
// Copyright (c) 2024 Ishan Pranav
// Licensed under the MIT license.

#include <stdbool.h>

/**
 * Permutes a range of algorithms into the next lexicographical permutation.
 * 
 * @param items  a pointer to the first element to permute.
 * @param length the number of elements to permute.
 * @return `true` if there are more permutations; otherwise, `false`.
 */
bool next_permutation(uint32_t* items, int length);
