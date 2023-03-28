#######################################
#
# Modul: Makefile
#
# Author: Michal Zapletal
# Email:  xzaple41@stud.fit.vutbr.cz
#
# Makefile
# Copyright (C) 2023, Zapletal Michal
#
#######################################
PROGRAM_NAME = ipkcpc
COMPILER = gcc
CFLAGS = -g -std=c11 -Wall -Wextra -Werror -pedantic
WINDOWS_LIB = -lwsock32 -lws2_32

$(PROGRAM_NAME): $(PROGRAM_NAME).c
	$(COMPILER) $(CFLAGS) $(PROGRAM_NAME).c -o $(PROGRAM_NAME)

run:
	make && ./$(PROGRAM_NAME)

all:
	$(COMPILER) $(CFLAGS) $(PROGRAM_NAME).c -o $(PROGRAM_NAME) $(WINDOWS_LIB)

debug:
	$(COMPILER) $(CFLAGS) -D DEBUG $(PROGRAM_NAME).c -o $(PROGRAM_NAME)

debug-win:
	$(COMPILER) $(CFLAGS) -D DEBUG $(PROGRAM_NAME).c -o $(PROGRAM_NAME) $(WINDOWS_LIB)

clean:
	rm $(PROGRAM_NAME)