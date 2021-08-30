/*
 * Copyright (C) 2005-2020 Darron Broad
 * All rights reserved.
 *
 * This file is part of Pickle Microchip PIC ICSP.
 *
 * Pickle Microchip PIC ICSP is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * Pickle Microchip PIC ICSP is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Pickle Microchip PIC ICSP. If not, see http://www.gnu.org/licenses/
 */

#include "VERSION"

#include "pctrl.h"

/*
 * pctrl session
 */
struct pickle p = {0};

static void
usage(char *execname, char *msg)
{
	printf("USAGE: %s RUN|STOP|RESTORE [BEFORE][AFTER][DURATION]\n", execname);
	printf("Control master clear.\n\n");

	if (msg)
		printf("Error: %s.\n\n", msg);

	printf("FILES:\n"
		" %s\n"
		"\t\tConfiguration file.\n\n", p.dotfile);

	printf("ENVIRONMENT:\n"
		" PICKLE\n"
		"\t\tConfiguration file.\n\n");

	printf("COMMANDS:\n"
		" RUN\t\tRaise master clear to take the device out of reset.\n"
		" STOP\t\tLower master clear to put the device into reset.\n"
		" RESTORE\tLower then raise master clear to reset the device.\n\n");

	printf("ARGUMENTS:\n"
		" BEFORE\t\tDelay in milliseconds before command.\n"
		" AFTER\t\tDelay in milliseconds after command.\n"
		" DURATION\tDuration of reset for RESTORE command.\n\n");

	printf("DEFAULTS:\n"
		" BEFORE\t\t%dms.\n"
		" AFTER\t\t%dms.\n"
		" DURATION\t%dms.\n\n",
		BEFORE / 1000, AFTER / 1000, DURATION / 1000);

	printf("EXAMPLES:\n"
		" pctrl RUN 10\n"
		"\t\tWait 10ms then raise master clear to take the device out of reset.\n"
		" pctrl STOP 0 10\n"
		"\t\tLower master clear to put the device into reset then wait 10ms.\n"
		" pctrl RESTORE 0 0 100\n"
		"\t\tLower for 100ms then raise master clear to reset the device.\n"

		"\n");

	printf("VERSION:\n %s\n", VERSION);

	if (msg)
		io_exit(EX_USAGE);
	io_exit(EX_OK);
}

int
main(int argc, char *argv[])
{
	char *execdup = NULL, *execname = NULL;
	uint32_t before = BEFORE, after = AFTER, duration = DURATION;

	/* Get exec name */
	execdup = (char *)strdup(argv[0]);
	if (execdup == NULL) {
		printf("%s: fatal error: strdup failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}
	execname = basename(execdup);
	if (execname == NULL) {
		printf("%s: fatal error: basename failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}

	/* Get configuration */
	getconf();

	/* Determine back-end */
	if (io_backend() == 0)
		usage(execname, io_error());

	/* Open device */
	if (io_open() < 0)
		usage(execname, io_error());

	/* Reset uid */
	if (getuid() != geteuid()) {
		if (setuid(getuid()) < 0) {
			printf("%s: fatal error: setuid failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		}
	}

	/* Check args */
	if (argc < 2)
		usage(execname, "Missing arg");
	if (argc > 5)
		usage(execname, "Too many args");

	if (strcasecmp(argv[1], "RUN") == 0)
		printf("RUN\n");
	else if (strcasecmp(argv[1], "STOP") == 0)
		printf("STOP\n");
	else if (strcasecmp(argv[1], "RESTORE") == 0)
		printf("RESTORE\n");
	else
		usage(execname, "Invalid command");

	/* Get args */
	if (argc > 2)
		before = 1000 * strtoul(argv[2], NULL, 0);
	if (argc > 3)
		after = 1000 * strtoul(argv[3], NULL, 0);
	if (argc > 4)
		duration = 1000 * strtoul(argv[4], NULL, 0);

	/*
	 * Perform operation
	 */

	/* BEFORE */	
	io_usleep(before);

	/* RUN/STOP RESTORE */
	if (strcasecmp(argv[1], "RUN") == 0)
		io_close(HIGH);
	else if (strcasecmp(argv[1], "STOP") == 0)
		io_close(LOW);
	else if (strcasecmp(argv[1], "RESTORE") == 0) {
		io_set_vpp(LOW);
		/* DURATION */
		io_usleep(duration);
		io_close(HIGH);
	}

	/* AFTER */	
	io_usleep(after);

	io_exit(EX_OK);
}
