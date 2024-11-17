# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

# References:
#  - https://www.man7.org/linux/man-pages/man3/getopt.3.html

# getopt in <main.c>: _POSIX_C_SOURCE >= 2

CC=clang
CFLAGS=-D_POSIX_C_SOURCE=2 -O3 -pedantic -std=c99 -Wall -Wextra

all: nyufile

nyufile: main.c fat32.h fat32_attributes.h fat32_boot_sector.h \
	fat32_directory_entry.h options.h information_utility list_utility \
	recover_contiguous_utility volume
	$(CC) $(CFLAGS) *.o main.c -o nyufile

information_utility: information_utility.c utility.h
	$(CC) $(CFLAGS) -c information_utility.c
	
list_utility: list_utility.c utility.h
	$(CC) $(CFLAGS) -c list_utility.c
	
recover_contiguous_utility: recover_contiguous_utility.c utility.h
	$(CC) $(CFLAGS) -c recover_contiguous_utility.c
	
# recover_non_contiguous_utility: recover_non_contiguous_utility.c utility.h
# 	$(CC) $(CFLAGS) -c recover_non_contiguous_utility.c
	
volume: volume.c volume.h
	$(CC) $(CFLAGS) -c volume.c

clean:
	rm -f *.o nyufile a.out
