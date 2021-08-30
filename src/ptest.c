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

#include "ptest.h"

/*
 * ptest session
 */
struct pickle p = {0};

extern int io_stop;

static void
usage(char *execname, char *msg)
{
	printf("USAGE: ptest TEST ARG\n");
	printf("Hardware tests.\n\n");

	if (msg)
		printf("Error: %s.\n\n", msg);

	printf("FILES:\n"
		" %s\n"
		"\t\tConfiguration file.\n\n", p.dotfile);

	printf("ENVIRONMENT:\n"
		" PICKLE\n"
		"\t\tConfiguration file.\n\n");

	printf("EXAMPLES:\n"
		" ptest VPP|PGC|PGD|PGM 5\n"
		"\t\tVPP, PGC, PGD or PGM LOW->HIGH->LOW output test with 5 seconds mark time.\n"
		" ptest INPUT 10\n"
		"\t\tPGD input test with 10 iterations of 1 second per step.\n"
		" ptest DSUB9 10\n"
		"\t\tD-SUB-9 test with 10 seconds per step.\n"
		" ptest ICSP 10\n"
		"\t\tICSP test with 10 seconds per step.\n"
		" ptest CLOCK 0\n"
		"\t\tPGC/RTS(7) clock test with no mark time.\n"
		" ptest CLOCK 10\n"
		"\t\tPGC/RTS(7) clock test with 10us mark time.\n"
		" ptest 16F627 100\n"
		"\t\tPIC16F627 debug test with 100us clock mark time.\n"
#ifdef PIO
		" ptest ICSPIO 100\n"
		"\t\tICSPIO test with 100us clock mark time.\n"
#endif
#ifdef RPI
		" ptest RPI 10\n"
		"\t\tRPI GPIO test with 10 seconds mark time.\n"
#endif
#ifdef ALLWINNER
		" ptest ALLWINNER 10\n"
		"\t\tBPI/OPI GPIO test with 10 seconds mark time.\n"
#endif
		"\n");

	printf("VERSION:\n %s\n", VERSION);

	if (msg)
		io_exit(EX_USAGE);
	io_exit(EX_OK);
}

/******************************************************************************
 * Hardware test routines for `ptest'
 *****************************************************************************/

/*
 * Test VPP, PGC or PGD output
 */
static void
test_pin(int pin, int t)
{
	printf("\nTEST VPP|PGD|PGC|PGM OUTPUT\n\n");

	switch (pin) {
	case PIN_VPP:
		printf("VPP LOW  (ICSP 1) (D-SUB-9 TX 3) [3 seconds]\n");
		io_set_vpp(LOW);
		break;
	case PIN_PGC:
		printf("PGC LOW  (ICSP 5) (D-SUB-9 RTS 7) [3 seconds]\n");
		io_set_pgc(LOW);
		break;
	case PIN_PGD:
		printf("PGD LOW  (ICSP 4) (D-SUB-9 DTR 3) [3 seconds]\n");
		io_set_pgd(LOW);
		break;
	case PIN_PGM:
		printf("PGM LOW  [3 seconds]\n");
		io_set_pgm(LOW);
		break;
	}
	if (!io_stop)
		sleep(3);

	switch (pin) {
	case PIN_VPP:
		printf("VPP HIGH (ICSP 1) (D-SUB-9 TX 3) [%d seconds]\n", t);
		io_set_vpp(HIGH);
		break;
	case PIN_PGC:
		printf("PGC HIGH (ICSP 5) (D-SUB-9 RTS 7) [%d seconds]\n", t);
		io_set_pgc(HIGH);
		break;
	case PIN_PGD:
		printf("PGD HIGH (ICSP 4) (D-SUB-9 DTR 3) [%d seconds]\n", t);
		io_set_pgd(HIGH);
		break;
	case PIN_PGM:
		printf("PGM HIGH [%d seconds]\n", t);
		io_set_pgm(HIGH);
		break;
	}
	if (!io_stop)
		sleep(t);

	switch (pin) {
	case PIN_VPP:
		printf("VPP LOW  (ICSP 1) (D-SUB-9 TX 3) [3 seconds]\n");
		io_set_vpp(LOW);
		break;
	case PIN_PGC:
		printf("PGC LOW  (ICSP 5) (D-SUB-9 RTS 7) [3 seconds]\n");
		io_set_pgc(LOW);
		break;
	case PIN_PGD:
		printf("PGD LOW  (ICSP 4) (D-SUB-9 DTR 3) [3 seconds]\n");
		io_set_pgd(LOW);
		break;
	case PIN_PGM:
		printf("PGM LOW  [3 seconds]\n");
		io_set_pgm(LOW);
		break;
	}
	if (!io_stop)
		sleep(3);
	
	printf("\nTEST DONE\n\n");
}

/*
 * Test PGD input
 */
static void
test_in(int t)
{
	printf("\nTEST PGD INPUT\n\n");
	
	while (t-- && !io_stop) {
		printf("PGD IN: %d\n", io_get_pgd());
		sleep(1);
	}

	printf("\nTEST DONE\n\n");
}

/*
 * Test D-SUB-9
 */
static void
test_dsub9(int t)
{
	printf("\nTEST D-SUB-9\n\n");

	p.bitrules = 0; /* Disable BITRULES */

	printf("Tx  SET  (+VE) (D-SUB-9 3) [%d seconds] ", t);
	fflush(stdout);
	io_set_vpp(HIGH);
	if (!io_stop)
		sleep(t);
	printf("CTS IN: %d\n", io_get_pgd());

	printf("Tx  CLR  (-VE) (D-SUB-9 3) [%d seconds] ", t);
	fflush(stdout);
	io_set_vpp(LOW);
	if (!io_stop)
		sleep(t);
	printf("CTS IN: %d\n\n", io_get_pgd());

	printf("DTR SET  (+VE) (D-SUB-9 4) [%d seconds] ", t);
	fflush(stdout);
	io_set_pgd(HIGH);
	if (!io_stop)
		sleep(t);
	printf("CTS IN: %d\n", io_get_pgd());

	printf("DTR CLR  (-VE) (D-SUB-9 4) [%d seconds] ", t);
	fflush(stdout);
	io_set_pgd(LOW);
	if (!io_stop)
		sleep(t);
	printf("CTS IN: %d\n\n", io_get_pgd());
	
	printf("RTS SET  (+VE) (D-SUB-9 7) [%d seconds] ", t);
	fflush(stdout);
	io_set_pgc(HIGH);
	if (!io_stop)
		sleep(t);
	printf("CTS IN: %d\n", io_get_pgd());

	printf("RTS CLR  (-VE) (D-SUB-9 7) [%d seconds] ", t);
	fflush(stdout);
	io_set_pgc(LOW);
	if (!io_stop)
		sleep(t);
	printf("CTS IN: %d\n\n", io_get_pgd());
	
	printf("TEST DONE\n\n");
}

/*
 * Test ICSP
 */
static void
test_icsp(int t)
{
	printf("\nTEST ICSP CONNECTOR\n\n");

	printf("VPP LOW  (0V)  (ICSP 1) [%d seconds] ", t);
	fflush(stdout);
	io_set_vpp(LOW);
	if (!io_stop)
		sleep(t);
	printf("PGD IN: %d\n", io_get_pgd());

	printf("VPP HIGH (12V) (ICSP 1) [%d seconds] ", t);
	fflush(stdout);
	io_set_vpp(HIGH);
	if (!io_stop)
		sleep(t);
	printf("PGD IN: %d\n\n", io_get_pgd());

	printf("PGD LOW  (0V)  (ICSP 4) [%d seconds] ", t);
	fflush(stdout);
	io_set_pgd(LOW);
	if (!io_stop)
		sleep(t);
	printf("PGD IN: %d\n", io_get_pgd());

	printf("PGD HIGH (5V)  (ICSP 4) [%d seconds] ", t);
	fflush(stdout);
	io_set_pgd(HIGH);
	if (!io_stop)
		sleep(t);
	printf("PGD IN: %d\n\n", io_get_pgd());

	printf("PGC LOW  (0V)  (ICSP 5) [%d seconds] ", t);
	fflush(stdout);
	io_set_pgc(LOW);
	if (!io_stop)
		sleep(t);
	printf("PGD IN: %d\n", io_get_pgd());

	printf("PGC HIGH (5V)  (ICSP 5) [%d seconds] ", t);
	fflush(stdout);
	io_set_pgc(HIGH);
	if (!io_stop)
		sleep(t);
	printf("PGD IN: %d\n\n", io_get_pgd());

	printf("TEST DONE\n\n");
}

/*
 * Test CLOCK
 */
static void
test_toggle(int t)
{
	printf("\nTEST CLOCK PGC/RTS(7) [CTRL-C TO STOP]\n\n");

	while (!io_stop) {
		io_set_pgc(1);
		io_usleep(t);
		io_set_pgc(0);
		io_usleep(t);
	}

	printf("\nTEST DONE\n\n");
}

/*
 * Test PIC16F627 on Velleman K8048 (PIC16F628A is compatible)
 *
 * Program:
 *	p14 program pickles/examples/pic16f627/debug.hex
 *
 * Note: This is the PICMicro which came with the Velleman K8048 kit.
 */
static void
test_debug(int t)
{
	int i, j = 0;
	uint8_t c, line[STRLEN];

	printf("\nTEST 16F627 debug.asm [CTRL-C TO STOP]\n\n");

	/* Initialise for data input */
	io_data_input();

	while (!io_stop) {
		/* Handshake */
		io_set_pgc(LOW);			/* PGC LOW */

		while (io_get_pgd() != HIGH && !io_stop)/* WAIT FOR PGD HIGH */
			io_usleep(1);
		
		io_set_pgc(HIGH);			/* PGC HIGH */
		
		while (io_get_pgd() != LOW && !io_stop)/* WAIT FOR PGD LOW */
			io_usleep(1);

		/* Byte input */
		c = 0;
		for (i = 0; i < 8; i++) {
			io_set_pgc(LOW);		/* PGC LOW */
			io_usleep(t);

			c = c >> 1;			/* CLOCK BIT IN */
			if (io_get_pgd())
				c |= 0x80;

			io_set_pgc(HIGH);		/* PGC HIGH */
			io_usleep(t);
		}

		printf("%s: read byte: %02X %c\n", __func__, c, PIC_CHAR(c));

		if (c == 0x03) {			/* ETX */
			if (j > 1 && line[0] == 0x02) {	/* STX */
				line[j] = '\0';
				printf("%s: read line: %s", __func__,
					&line[1]);
			}
			j = 0;
		}
		else if (j < STRMAX)
			line[j++] = c;
	}

	printf("\nTEST DONE\n\n");
}

#ifdef PIO
/*
 * Test ICSP I/O LEDs and switches
 */
static void
test_icspio(int t)
{
	int err;
	uint8_t cmd[2];
	uint32_t ld = 0;

	printf("\nTEST ICSPIO [CTRL-C TO STOP]\n\n");

	/* VPP LOW */
	io_set_vpp(LOW);
	io_usleep(10000); /* 10ms */

	/* VPP HIGH */
	io_set_vpp(HIGH);
	io_usleep(10000); /* 10ms */

	while (!io_stop) {
		cmd[0] = CMD_LED;
		cmd[1] = ld++;
		err = icspio_command(t, t << 1, cmd, 1, NULL, 0);
		if (err != ERRNONE) {
			fprintf(stderr, "%s: error: %s [0x%02X]\n",
				__func__, icspio_err(err), err);
			break;
		}
		err = icspio_switch(t);
		if (err != ERRNONE) {
			fprintf(stderr, "%s: error: %s [0x%02X]\n",
				__func__, icspio_err(err), err);
			break;
		}
		err = icspio_lasterror(t);
		if (err != ERRNONE) {
			fprintf(stderr, "%s: error: %s [0x%02X]\n",
				__func__, icspio_err(err), err);
			break;
		}
	}

	printf("\nTEST DONE\n\n");
}
#endif /* PIO */

#ifdef RPI
/*
 * Test RPi GPIO pins
 */
static void
test_rpi(int seconds)
{
	uint8_t output_level = 0, input_level;

	printf("\nTEST RPI GPIO [CTRL-C TO STOP]\n");

	while (!io_stop) {
		printf("\n");

		raspi_set_vpp(output_level);
		printf("GPIO %-3d (VPP) (TX)  = %d\n", p.vpp, output_level);

		if (p.pgm != GPIO_DISABLED) {
			raspi_set_pgm(output_level);
			printf("GPIO %-3d (PGM)       = %d\n", p.pgm, output_level);
		}

		raspi_set_pgc(output_level);
		printf("GPIO %-3d (PGC) (RTS) = %d\n", p.pgc, output_level);

		raspi_set_pgd(output_level);
		printf("GPIO %-3d (PGD) (DTR) = %d\n", p.pgdo,output_level);

		if (p.pgdi != p.pgdo) {
			input_level = raspi_get_pgd();
			printf("GPIO %02d (PGD) (CTS) = %d\n", p.pgdi, input_level);
		}

		fflush(stdout);
		
		sleep(seconds);
		output_level = 1 - output_level;
	}
}
#endif /* RPI */

#ifdef ALLWINNER
/*
 * Test AllWinner GPIO pins
 */
static void
test_allwinner(int seconds)
{
	uint8_t output_level = 0, input_level;

	printf("\nTEST ALLWINNER GPIO [CTRL-C TO STOP]\n");

	while (!io_stop) {
		printf("\n");

		allwinner_set_vpp(output_level);
		printf("GPIO %-3d (VPP) = %d\n", p.vpp, output_level);

		if (p.pgm != GPIO_DISABLED) {
		        allwinner_set_pgm(output_level);
		        printf("GPIO %-3d (PGM) = %d\n", p.pgm, output_level);
		}

		allwinner_set_pgc(output_level);
		printf("GPIO %-3d (PGC) = %d\n", p.pgc, output_level);

		allwinner_set_pgd(output_level);
		printf("GPIO %-3d (PGD) = %d\n", p.pgdo,output_level);

		if (p.pgdi != p.pgdo) {
		        input_level = allwinner_get_pgd();
		        printf("GPIO %02d (PGD) = %d\n", p.pgdi, input_level);
		}

		fflush(stdout);

		sleep(seconds);
		output_level = 1 - output_level;
	}
}
#endif /* ALLWINNER */

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
	if (argc < 3)
		usage(execname, "Missing args");
	if (argc > 3)
		usage(execname, "Too many args");

	/* Get args */
	int32_t testarg = strtol(argv[2], NULL, 0);
	if (testarg < 0)
		usage(execname, "Invalid arg");

	/*
	 * Perform operation
	 */

	if (strcasecmp(argv[1], "VPP") == 0) {
		test_pin(PIN_VPP, testarg);
	}
	else if (strcasecmp(argv[1], "PGC") == 0) {
		test_pin(PIN_PGC, testarg);
	}
	else if (strcasecmp(argv[1], "PGD") == 0) {
		test_pin(PIN_PGD, testarg);
	}
	else if (strcasecmp(argv[1], "PGM") == 0) {
		test_pin(PIN_PGM, testarg);
	}
	else if (strcasecmp(argv[1], "INPUT") == 0) {
		test_in(testarg);
	}
	else if (strcasecmp(argv[1], "DSUB9") == 0) {
		test_dsub9(testarg);
	}
	else if (strcasecmp(argv[1], "ICSP") == 0) {
		test_icsp(testarg);
	}
	else if (strcasecmp(argv[1], "CLOCK") == 0) {
		test_toggle(testarg);
	}
	else if (strcasecmp(argv[1], "16F627") == 0) {
		test_debug(testarg);
	}
#ifdef PIO
	else if (strcasecmp(argv[1], "ICSPIO") == 0) {
		test_icspio(testarg);
	}
#endif
#ifdef RPI
	else if (strcasecmp(argv[1], "RPI") == 0) {
		test_rpi(testarg);
	}
#endif
#ifdef ALLWINNER
	else if (strcasecmp(argv[1], "ALLWINNER") == 0) {
		test_allwinner(testarg);
	}
#endif
	else {
		usage(execname, "Invalid test");
	}
	io_exit(EX_OK);
}
