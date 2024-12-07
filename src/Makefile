# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

# References:
#  - https://www.man7.org/linux/man-pages/man3/getopt.3.html

# getopt in <main.c>: _POSIX_C_SOURCE >= 2

CC=gcc
CFLAGS=-D_POSIX_C_SOURCE=2 -O3 -lcrypto -pedantic -std=c99 -Wall -Wextra

all: nyufile

nyufile: main.c fat32_attributes.h fat32_boot_sector.h fat32_directory_entry.h \
	options.h information_utility list_utility next_permutation \
	recover_contiguous_utility recover_fragmented_utility volume \
	volume_find_result
	$(CC) $(CFLAGS) *.o main.c -o nyufile

information_utility: information_utility.c utility.h
	$(CC) $(CFLAGS) -c information_utility.c
	
list_utility: list_utility.c utility.h
	$(CC) $(CFLAGS) -c list_utility.c
	
next_permutation: next_permutation.c next_permutation.h
	$(CC) $(CFLAGS) -c next_permutation.c

recover_contiguous_utility: recover_contiguous_utility.c utility.h
	$(CC) $(CFLAGS) -c recover_contiguous_utility.c
	
recover_fragmented_utility: recover_fragmented_utility.c utility.h
	$(CC) $(CFLAGS) -c recover_fragmented_utility.c
	
volume: volume.c volume.h
	$(CC) $(CFLAGS) -c volume.c

volume_find_result: volume_find_result.c volume_find_result.h
	$(CC) $(CFLAGS) -c volume_find_result.c
	
clean:
	rm -f *.o nyufile a.out
