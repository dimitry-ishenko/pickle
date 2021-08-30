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

#include "pio.h"

/*
 * pio session
 */
struct pickle p = {0};

extern int io_stop;

static void
usage(char *execname, char *msg)
{
	printf("USAGE: %s COMMAND [ARG]\n", execname);
	printf("ICSPIO operations.\n\n");

	if (msg)
		printf("Error: %s.\n\n", msg);

	printf("FILES:\n"
		" %s\n"
		"\t\tConfiguration.\n\n", p.dotfile);

	printf("ENVIRONMENT:\n"
		" PICKLE\n"
		"\t\tConfiguration file.\n\n");

	printf("COMMANDS:\n"
		" LED\t\t0x00..0x3F\t\tSet Velleman K8048 LEDs.\n"
		" SWITCH\t\t\t\t\tGet Velleman K8048 switches.\n"
		" SLEEP\t\t\t\t\tSleep until watchdog time-out.\n"
		" WATCHDOG\tE|D\t\t\tSet watchdog enable/disable.\n"
		" CLOCK\t\t0..7\t\t\tSet internal RC clock divider.\n"
		" DIRECTION\tA..D|G 0x00..0xFF\tSet port data direction.\n"
		" OUTPUT\t\tA..D|G 0x00..0xFF\tSet port data output.\n"
		" INPUT\t\tA..D|G\t\t\tGet port data input, output on stdout.\n"
		" ANALOG\t\t0..N|D\t\t\tSet analog channel enable/disable.\n"
		" SAMPLE\t\t\t\t\tGet analog channel input.\n"
		" EEREAD\t\t0x00..0xFF\t\tGet data EEPROM.\n"
		" EEWRITE\t0x00..0xFF 0x00..0xFF\tSet data EEPROM.\n"
		" READ\t\t0x0000..0xFFFF\t\tGet program flash.\n"
		" VREF\t\t0..15|D\t\t\tSet vref voltage level or disable.\n"
		" 8\t\t0..0xFF\t\t\tSend an 8-bit arg & get an 8-bit answer.\n"
		" 16\t\t0..0xFFFF\t\tSend a 16-bit arg & get an 8-bit answer.\n"
		" 24\t\t0..0xFFFFFF\t\tSend a 24-bit arg & get an 8-bit answer.\n"
		" 32\t\t0..0xFFFFFFFF\t\tSend a 32-bit arg & get an 8-bit answer.\n"
		" TEST\t\t0..0xFF\t\t\tSend an 8-bit test arg & get no reply.\n"
		" ERROR\t\t\t\t\tGet last firmware error.\n"
		"\n");

	printf("EXAMPLES:\n"
		" pio WATCHDOG D\n"
		"\t\tTurn off watchdog.\n"
		" pio DIRECTION C 0xF0\n"
		"\t\tSet port C bits 0..3 as output and bits 4..7 as input.\n"
		" pio OUTPUT C 0x0A\n"
		"\t\tSet port C bits 0..3 to 0x0A.\n\n");

	printf("VERSION:\n %s\n", VERSION);

	if (msg)
		io_exit(EX_USAGE);
	io_exit(EX_OK);
}

/*
 * Get 8-bit byte arg
 *
 * Arg may be 0..255
 */
static uint8_t
getbytearg(char *execname, char *args)
{
	uint32_t argn = strtoul(args, NULL, 0);

	if (argn <= 255)
		return argn;

	usage(execname, "Invalid arg [8-bit byte]");
	return 0; /* Not reached */
}

/*
 * Get 16-bit word arg
 *
 * Arg may be 0..65535
 */
static uint16_t
getshortarg(char *execname, char *args)
{
	uint32_t argn = strtoul(args, NULL, 0);

	if (argn <= 65535)
		return argn;

	usage(execname, "Invalid arg [16-bit word]");
	return 0; /* Not reached */
}

/*
 * Get 24-bit arg
 *
 * Arg may be 0..0xFFFFFF
 */
static uint32_t
get24arg(char *execname, char *args)
{
	uint32_t argn = strtoul(args, NULL, 0);

	if (argn <= 0xFFFFFF)
		return argn;

	usage(execname, "Invalid arg [24-bit word]");
	return 0; /* Not reached */
}

/*
 * Get 32-bit arg
 *
 * Arg may be 0..0xFFFFFFFF
 */
static uint32_t
getintarg(char *execname, char *args)
{
	uint32_t argn = strtoul(args, NULL, 0);

	return argn;
}

/*
 * Get port arg
 *
 * Arg may be G (GPIO), A .. D
 */
static uint8_t
getportarg(char *execname, char *args)
{
	char p = toupper((int)args[0]);

	if (p == 'G')
		return 0;

	if (p >= 'A' && p <= 'D')
		return p - 'A' + 1;

		usage(execname, "Invalid arg [port]");
	return 0; /* Not reached */
}

int
main(int argc, char *argv[])
{
	char *execdup = NULL, *execname = NULL;

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

	/* Raise priority */
	setpriority(PRIO_PROCESS, 0, -20);

	/* Reset uid */
	if (getuid() != geteuid()) {
		if (setuid(getuid()) < 0) {
			printf("%s: fatal error: setuid failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		}
	}

	/* Check args */
	if (argc < 2)
		usage(execname, "Missing arg(s)");

	/*
	 * Perform operation
	 */

	int err, retry = 10;
	uint8_t cmd[STRLEN];

	/*
	 * ICSPIO
	 */
	if (strcasecmp(argv[1], "LED") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_LED;
		cmd[1] = getbytearg(execname, argv[2]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: LED(0x%02X)\n",
					__func__, cmd[1]);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "SWITCH") == 0)
	{
		if (argc > 2)
			usage(execname, "Too many args");

		cmd[0] = CMD_SWITCH;

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: SWITCH()\n",
					__func__);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 0, &res, 1);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%02X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "SLEEP") == 0)
	{
		if (argc > 2)
			usage(execname, "Too many args");

		cmd[0] = CMD_SLEEP;

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: SLEEP()\n",
					__func__);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 0, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "WATCHDOG") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_WATCHDOG;
		if (argv[2][0] == 'd' || argv[2][0] == 'D') {
			cmd[1] = 0x00;
		} else if (argv[2][0] == 'e' || argv[2][0] == 'E') {
			cmd[1] = 0x01;
		} else {
			usage(execname, "Invalid arg");
		}

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: WATCHDOG(0x%02X)\n",
					__func__, cmd[1]);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "CLOCK") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_CLOCK;
		cmd[1] = getbytearg(execname, argv[2]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: CLOCK(0x%02X)\n",
					__func__, cmd[1]);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "DIRECTION") == 0)
	{
		if (argc < 4)
			usage(execname, "Missing arg");
		if (argc > 4)
			usage(execname, "Too many args");

		cmd[0] = CMD_DIRECTION;
		cmd[1] = getportarg(execname, argv[2]);
		cmd[2] = getbytearg(execname, argv[3]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: DIRECTION(0x%02X,0x%02X)\n",
					__func__, cmd[1], cmd[2]);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 2, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "OUTPUT") == 0)
	{
		if (argc < 4)
			usage(execname, "Missing arg");
		if (argc > 4)
			usage(execname, "Too many args");

		cmd[0] = CMD_OUTPUT;
		cmd[1] = getportarg(execname, argv[2]);
		cmd[2] = getbytearg(execname, argv[3]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: OUTPUT(0x%02X,0x%02X)\n",
					__func__, cmd[1], cmd[2]);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 2, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "INPUT") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_INPUT;
		cmd[1] = getportarg(execname, argv[2]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: INPUT(0x%02X)\n",
					__func__, cmd[1]);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, &res, 1);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%02X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "ANALOG") == 0) {
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_ANALOG;
		if (argv[2][0] == 'd' || argv[2][0] == 'D') {
			cmd[1] = 0xFF;
		} else {
			cmd[1] = getbytearg(execname, argv[2]);
		}

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: ANALOG(0x%02X)\n",
					__func__, cmd[1]);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "SAMPLE") == 0)
	{
		if (argc > 2)
			usage(execname, "Too many args");

		cmd[0] = CMD_SAMPLE;

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: SAMPLE()\n",
					__func__);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, FWSAMPLE, cmd, 0, &res, 2);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%04X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "EEREAD") == 0) {
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_EEREAD;
		cmd[1] = getbytearg(execname, argv[2]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: EEREAD(0x%02X)\n",
					__func__, cmd[1]);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, &res, 1);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%02X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "EEWRITE") == 0)
	{
		if (argc < 4)
			usage(execname, "Missing arg");
		if (argc > 4)
			usage(execname, "Too many args");

		cmd[0] = CMD_EEWRITE;
		cmd[1] = getbytearg(execname, argv[2]);
		cmd[2] = getbytearg(execname, argv[3]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: EEWRITE(0x%02X,0x%02X)\n",
					__func__, cmd[1], cmd[2]);

			err = icspio_command(p.fwsleep, FWEEPROM, cmd, 2, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "READ") == 0) {
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_READ;
		uint16_t addr = getshortarg(execname, argv[2]);
		cmd[1] = addr >> 8; /* High */
		cmd[2] = addr;      /* Low  */

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: READ(0x%02X,0x%02X)\n",
					__func__, cmd[1], cmd[2]);

			uint32_t res;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 2, &res, 2);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%04X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "VREF") == 0) {
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_VREF;
		if (argv[2][0] == 'd' || argv[2][0] == 'D') {
			cmd[1] = 0xFF;
		} else {
			cmd[1] = getbytearg(execname, argv[2]);
		}

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: VREF(0x%02X)\n",
					__func__, cmd[1]);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "8") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_ARG8;
		cmd[1] = getbytearg(execname, argv[2]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: 8(0x%02X)\n",
					__func__, cmd[1]);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, &res, 1);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%02X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "16") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_ARG16;
		uint16_t arg = getshortarg(execname, argv[2]);
		cmd[1] = arg >> 8; /* High */
		cmd[2] = arg;      /* Low  */

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: 16(0x%02X,0x%02X)\n",
					__func__, cmd[1], cmd[2]);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 2, &res, 1);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%02X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "24") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_ARG24;
		uint32_t arg = get24arg(execname, argv[2]);
		cmd[1] = arg >> 16; /* High */
		cmd[2] = arg >> 8;
		cmd[3] = arg;       /* Low  */

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: 24(0x%02X,0x%02X,0x%02X)\n",
					__func__, cmd[1], cmd[2], cmd[3]);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 3, &res, 1);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%02X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "32") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_ARG24;
		uint32_t arg = getintarg(execname, argv[2]);
		cmd[1] = arg >> 24; /* High */
		cmd[2] = arg >> 16;
		cmd[3] = arg >> 8;
		cmd[4] = arg;       /* Low  */

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: 32(0x%02X,0x%02X,0x%02X,0x%02X)\n",
					__func__, cmd[1], cmd[2], cmd[3], cmd[4]);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 4, &res, 1);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%02X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "TEST") == 0)
	{
		if (argc < 3)
			usage(execname, "Missing arg");
		if (argc > 3)
			usage(execname, "Too many args");

		cmd[0] = CMD_TEST;
		cmd[1] = getbytearg(execname, argv[2]);

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: TEST(0x%02X)\n",
					__func__, cmd[1]);

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 1, NULL, 0);
			if (err == ERRNONE)
				goto done;

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else if (strcasecmp(argv[1], "ERROR") == 0)
	{
		if (argc > 2)
			usage(execname, "Too many args");

		cmd[0] = CMD_ERROR;

		while (retry-- && !io_stop) {
			if (p.debug >= 1)
				fprintf(stderr, "%s: ERROR()\n",
					__func__);

			uint32_t res = 0;

			err = icspio_command(p.fwsleep, p.fwsleep << 1, cmd, 0, &res, 1);
			if (err == ERRNONE) {
				fprintf(stdout, "0x%02X\n", res);
				goto done;
			}

			if (p.debug >= 1)
				fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, icspio_err(err), err);

			io_usleep(RESYNC * p.fwsleep);
		}
		fprintf(stdout, (io_stop) ? "ABORT\n" : "ERROR\n");
	}

	else {
		usage(execname, "Unknown command");
	}

done:	io_exit(EX_OK);
	return 0;
}
