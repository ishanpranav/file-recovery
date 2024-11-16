# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

# References:
#  - https://www.man7.org/linux/man-pages/man3/getopt.3.html

# getopt in <main.c>: _POSIX_C_SOURCE >= 2

CC=gcc
CFLAGS=-D_POSIX_C_SOURCE=2 -O3 -pedantic -std=c99 -Wall -Wextra

all: nyufile

nyufile: main.c fat32.h fat32_attributes.h fat32_boot_sector.h \
	fat32_directory_entry.h options.h volume
	$(CC) $(CFLAGS) *.o main.c -o nyufile

volume: volume.c volume.h
	$(CC) $(CFLAGS) -c volume.c

clean:
	rm -f *.o nyufile a.out
