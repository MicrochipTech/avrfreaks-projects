/*
 *  setuart.c
 *
 *  Enable/disable MODEN control line of tty device
 *
 *  Copyright (C) 2008 Roger Huang (roger.huang@yia.idv.tw)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <asm/ioctls.h>


int search_bit(char *name)
{
	struct uart_bit {
		char *name;
		int bit;
	} *s, bits[] = {
			{"rts", TIOCM_RTS},
			{"dtr", TIOCM_DTR},
			{NULL, -1},
		};

	for (s = bits; s->name != NULL; s++) {
		if (strcmp(name, s->name) == 0)
			break;
	}

	return s->bit;
}

int set_uart(int cmd, int *flag)
{
	int ret = 0;

	if (flag)
		ret = ioctl(0, cmd, flag);

	return ret;
}

const char *program_name;
void usage(FILE *stream, int exit_code)
{
	fprintf(stream, "Usage: %s options\n"
			"\tThe utility set/get uart moden line for stdin\n\n"
			"\t--help\tThis is usage\n"
			"\t[-]rts\tEnable/disable RTS\n"
			"\t[-]dtr\tEnable/disable DTR\n",
			program_name);
	exit(exit_code);
}

int main(int argc, char *argv[])
{
	int i, j;
	int set = 0, clear = 0;
	char *s;

	program_name = argv[0];

	for (i = 1; i < argc; i++) {
		int *flag;

		if (strcmp("--help", argv[i]) == 0)
			usage(stdout, EXIT_SUCCESS);

		flag = &set;
		s = argv[i];
		if (s[0] == '-') {
			flag = &clear;
			s++;
		}

		j = search_bit(s);
		if (j < 0)
			usage(stdout, EXIT_FAILURE);

		*flag |= j;
	}

	if (set_uart(TIOCMBIC, &clear) < 0)
		perror("TIOCMBIC,");
	if (set_uart(TIOCMBIS, &set) < 0)
		perror("TIOCMBIS");

	return EXIT_SUCCESS;
}

