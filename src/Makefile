# Makefile
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

CC=clang
CFLAGS=-O3 -pedantic -std=c99 -Wall -Wextra

all: nyufile

nyufile: main.c
	$(CC) $(CFLAGS) main.c -o nyufile

clean:
	rm -f *.o nyufile a.out
