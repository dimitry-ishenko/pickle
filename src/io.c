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

#include "pickle.h"

/*
 * SESSION
 */
extern struct pickle p;

/*
 * DETERMINE BACK-END
 *
 * Returns 0 if undetermind, else 1..N
 */
uint8_t
io_backend(void)
{
#ifdef ALLWINNER
	/*
	 * DEVICE=BPI
	 * DEVICE=OPI
	 * DEVICE=OPI0
	 */
	if ((strcasecmp(p.device, "BPI") == 0) || (mystrcasestr(p.device, "OPI") == p.device))
		return allwinner_backend();
#endif
#ifdef BITBANG
	/*
	 * DEVICE=GPIO-BB
	 * IFACE=/dev/gpio-bb
	 */
	if (strcasecmp(p.device, "GPIO-BB") == 0)
		return gpio_bb_backend();
#endif
#ifdef CP2104
	/*
	 * DEVICE=CP2104
	 * IFACE=/dev/ttyUSB0
	 */
	if (strcasecmp(p.device, "CP2104") == 0)
		return cp2104_bb_backend();
#endif
#ifdef FTDI
	/*
	 * DEVICE=FTDI
	 * IFACE=0
	 * SERIAL=ABCDEFGH
	 */
	if (strcasecmp(p.device, "FTDI") == 0)
		return ftdi_bb_backend();
#endif
#ifdef MCP2221
	/*
	 * DEVICE=MCP2221
	 */
	if (strcasecmp(p.device, "MCP2221") == 0)
		return mcp2221_backend();
#endif
#ifdef MCP23016
	/*
	 * DEVICE=MCP23016
	 * IFACE=/dev/i2c-1
	 */
	if (strcasecmp(p.device, "MCP23016") == 0)
		return mcp23016_backend();
#endif
#ifdef MCP230XX
	/*
	 * DEVICE=MCP230XX
	 * IFACE=/dev/i2c-1
	 */
	if (strcasecmp(p.device, "MCP230XX") == 0)
		return mcp230xx_backend();
#endif
#ifdef MCP23SXX
	/*
	 * DEVICE=MCP23SXX
	 * IFACE=/dev/spidev0.1
	 */
	if (strcasecmp(p.device, "MCP23SXX") == 0)
		return mcp23sxx_backend();
#endif
#ifdef PCF8574
	/*
	 * DEVICE=PCF8574
	 * IFACE=/dev/i2c-1
	 */
	if (strcasecmp(p.device, "PCF8574") == 0)
		return pcf8574_backend();
#endif
#ifdef RPI
	/*
	 * DEVICE=RPI
	 * DEVICE=RPI0
	 * DEVICE=RPI1
	 * DEVICE=RPI2
	 * DEVICE=RPI3
	 * DEVICE=RPI4
	 */
	if (mystrcasestr(p.device, "RPI") == p.device)
		return raspi_backend();
#endif
#ifdef SERIAL
	/*
	 * DEVICE=TTY
	 * IFACE=/dev/ttyS0
	 * IFACE=/dev/ttyUSB0
	 */
	if (strcasecmp(p.device, "TTY") == 0)
		return serial_bb_backend();
#endif
	/* Undetermined, unsupported, unknown */
	return 0;
}

/*
 * CTRL-C signal handler
 */
int io_stop = 0;

void
io_signal()
{
	io_stop = 1;
}

void
io_signal_on()
{
	io_stop = 0;
	signal(SIGINT, io_signal);
	signal(SIGPIPE, SIG_IGN);
}

void
io_signal_off()
{
	signal(SIGINT, SIG_DFL);
	signal(SIGPIPE, SIG_DFL);
}

/*
 * Config defaults
 */
void
io_config(void)
{
	bzero(&p, sizeof(struct pickle));
	p.f = stderr;					/* Message output stream    */
	strncpy(p.device, "TTY", STRLEN);		/* Default device           */
	strncpy(p.iface, "/dev/ttyS0", STRLEN);		/* Default interface        */
	p.bitrules = PGD_IN_PULLUP | PGD_OUT_FLIP |	/* Default BITRULES for the */
	PGC_OUT_FLIP | VPP_OUT_FLIP | PGD_IN_FLIP;	/*           Velleman K8048 */
	p.sleep_high = 1;				/* Mark time                */
	p.sleep_low = 1;				/* Space time               */
	p.fwsleep = 30;					/* ICSPIO mark/space time   */
	p.baudrate = 115200;				/* STK500v2/SPI baud rate   */
	p.addr = 0x20;					/* I2C/SPI iface address    */
	p.vpp  = 11;					/* TX/!MCLR/VPP             */
	p.pgc  = 10;					/* RTS/PGC CLOCK            */
	p.pgdo = 9;					/* DTR/PGD DATA_OUT         */
	p.pgdi = 9;					/* CTS/PGD DATA_IN          */
	p.pgm  = GPIO_DISABLED;				/* PGM                      */
	p.config = CONFIGVER + CONFIGAND;		/* VERIFY + DUMP            */
}

/*
 * Open
 */
int
io_open(void)
{
	if (p.iot != IONONE)
		return -1;

	if (p.io && p.io->open) {
		if (p.io->open() < 0)
			return -1;

		io_signal_on();

		p.iot = p.io->type;

		return 0;
	}

	return -1;
}

/*
 * Close
 */
void
io_close(int run)
{
	if (p.iot == IONONE)
		return;

	if (p.io != NULL) {
		if (p.io->release) {
			p.io->release();
		}

		if (p.io->run) {
			if ((p.bitrules & VPP_RELEASE) == 0) {
				io_usleep(10);
				if (run)
					io_set_vpp(HIGH);
				else
					io_set_vpp(LOW);
			}
		}

		if (p.io->close) {
			p.io->close();
		}

		p.io = NULL;
	}

	p.iot = IONONE;

	io_signal_off();
}

void
io_exit(int err)
{
	fflush(stdout);

	if (p.iot != IONONE)
		io_close(p.bitrules & VPP_RUN);

	exit(err);
}

/*
 * Error
 *
 * Eg. Can't open Serial I/O.
 */
char *
io_error(void)
{
	static char msg[STRLEN];
	int rc;

	if (p.io && p.io->error)
		rc = snprintf(msg, STRLEN, "%s [DEVICE=%s] [IFACE=%s]",
			p.io->error(), p.device, p.iface);
 	else
		rc = snprintf(msg, STRLEN, "Can't open unsupported I/O [DEVICE=%s] [IFACE=%s].\n\n"
			"       Run `pickle` to list supported backend I/O", p.device, p.iface);
	if (rc < 0) {
		printf("%s: fatal error: snprintf failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	} else if (rc >= STRLEN) {
		printf("%s: fatal error: snprintf overrun\n", __func__);
		io_exit(EX_SOFTWARE); /* Panic */
	}

	return msg;
}

char *
io_fault(int errorcode)
{
	static char *msg;

	if (errorcode == 0)
		msg = "device not detected";
	else
		msg = "device not compatible";

	return msg;
}

/*
 * Usleep
 */
void
io_usleep(uint32_t n)
{
	struct timeval tv1, tv2;
	int diff;

	/* No sleep */
	if (n == 0)
		return;

	switch (p.sleep_algo) {
	case IOSLEEP_DEFAULT:
		/* Input */
		if (n < 10 && p.io && p.io->usleep) {
			p.io->usleep(n);
			return;
		}
		/* Busy */
		if (n < 100) {
			gettimeofday(&tv1, NULL);
			do {
				gettimeofday(&tv2, NULL);
				diff = tv2.tv_usec - tv1.tv_usec;
				if (diff < 0)
					diff += 1000000;
			}
			while (diff < n);
			return;
		}
		/* Usleep() */
		usleep(n);
		break;

	case IOSLEEP_INPUT:
		/* Input */
		if (p.io && p.io->usleep) {
			p.io->usleep(n);
			return;
		}
		/* Follow through */

	case IOSLEEP_USLEEP:
		/* Usleep() */
		usleep(n);
		break;

	case IOSLEEP_BUSY:
		/* Busy */
		gettimeofday(&tv1, NULL);
		do {
			gettimeofday(&tv2, NULL);
			diff = tv2.tv_usec - tv1.tv_usec;
			if (diff < 0)
				diff += 1000000;
		}
		while (diff < n);
		break;
	}
}

/*
 * Set PGM (LVP PROGRAM OUT)
 */
void
io_set_pgm(uint8_t pgm)
{
	if (p.io && p.io->set_pgm) {

		if (p.bitrules & PGM_OUT_FLIP)
			pgm = HIGH - pgm;

		p.io->set_pgm(pgm);
	}
}

/*
 * Set VPP (PROGRAM OUT)
 */
void
io_set_vpp(uint8_t vpp)
{
	if (p.io && p.io->set_vpp) {

		if (p.bitrules & VPP_OUT_FLIP)
			vpp = HIGH - vpp;

		p.io->set_vpp(vpp);
	}
}

/*
 * Set PGD (DATA OUT)
 */
void
io_set_pgd(uint8_t pgd)
{
	if (p.io && p.io->set_pgd) {

		if (p.bitrules & PGD_OUT_FLIP)
			pgd = HIGH - pgd;

		p.io->set_pgd(pgd);
	}
}

/*
 * Set PGC (CLOCK OUT)
 */
void
io_set_pgc(uint8_t pgc)
{
	if (p.io && p.io->set_pgc) {

		if (p.bitrules & PGC_OUT_FLIP)
			pgc = HIGH - pgc;

		p.io->set_pgc(pgc);
	}
}

/*
 * Get PGD (DATA IN)
 */
uint8_t
io_get_pgd(void)
{
	uint8_t pgd = 0;

	if (p.io && p.io->get_pgd) {

		pgd = p.io->get_pgd();

		if (p.bitrules & PGD_IN_FLIP)
			pgd = HIGH - pgd;
	}

	return pgd;
}

/*
 * Configure
 *
 * Clock falling is to satisfy LVP mode PIC24F devices which output data
 * on the falling edge of the clock and not on the rising edge.
 *
 * msb_first is to satisfy the next generation 8-bit chip programming.
 */
void
io_configure(uint8_t clock_falling, uint8_t msb_first)
{
	/* Device configuration */
	p.clock_falling = clock_falling;
	p.msb_first = msb_first;

	/* Back-end configuration */
	if (p.io && p.io->configure)
		p.io->configure();
}

/*
 * Clock bit in or out
 */
static inline void
io_clock_bit(uint32_t ldly, uint32_t hdly)
{
	io_set_pgc(HIGH);	/* Clock high */
	io_usleep(hdly);

	io_set_pgc(LOW);	/* Clock low */
	io_usleep(ldly);
}

/*
 * Data Input
 *
 * Initialise data input from chip (PGD=IN, SDATA=OUTPUT)
 */
void
io_data_input(void)
{
	if (p.bitrules & PGD_IN_PULLUP) {
		io_set_pgd(HIGH);
	}
	else if (p.bitrules & PGD_IN_PULLDOWN) {
		io_set_pgd(LOW);
	}
}

/*
 * Return first bit mask
 */
static inline uint32_t
io_first_mask(uint8_t nbits)
{
	return (p.msb_first) ? (1U << (nbits - 1)) : (1 << 0);
}

/*
 * Return next bit mask
 */
static inline uint32_t
io_next_mask(uint32_t mask)
{
	return (p.msb_first) ? (mask >> 1) : (mask << 1);
}

/*
 * Clock in bit(s)
 */
uint32_t
io_clock_in_bits(uint32_t ldly, uint32_t hdly, uint8_t nbits)
{
	uint32_t bits = 0, mask = io_first_mask(nbits);

	/* Initialise for data input */
	io_data_input();

	/* Clock in bits 0..N */
	while (nbits--) {
		if (!p.clock_falling)
			io_clock_bit(ldly, hdly);

		if (io_get_pgd())
			bits |= mask;

		if (p.clock_falling)
			io_clock_bit(ldly, hdly);

		mask = io_next_mask(mask);
	}
	return bits;
}

/*
 * Clock out bit(s)
 */
void
io_clock_out_bits(uint32_t ldly, uint32_t hdly, uint32_t bits, uint8_t nbits)
{
	uint32_t mask = io_first_mask(nbits);

	/* Clock out bits 0..N */
	while (nbits--) {
		io_set_pgd((bits & mask) != 0);
		io_clock_bit(ldly, hdly);

		mask = io_next_mask(mask);
	}
}

#ifdef P32
/*
 * Clock 1 bit in and 2 bits out (2-wire 4-phase)
 */
uint8_t
io_clock_bit_4phase(uint8_t tms, uint8_t tdi)
{
	uint8_t tdo;

	/*********
	 * PHASE 1
	 */
	io_set_pgd(tdi);	/* TDI OUTPUT */
	io_clock_bit(p.sleep_low, p.sleep_high);

	/*********
	 * PHASE 2
	 */
	io_set_pgd(tms);	/* TMS OUTPUT */
	io_clock_bit(p.sleep_low, p.sleep_high);

	/*********
	 * PHASE 3
	 */
	io_data_input();
	io_get_pgd();		/* DISCARD INPUT */
	io_clock_bit(p.sleep_low, p.sleep_high);

	/*********
	 * PHASE 4
	 */
	tdo = io_get_pgd();	/* TDO INPUT (0 or 1) */
	io_clock_bit(p.sleep_low, p.sleep_high);

	return tdo;
}

/*
 * Clock multiple bits in and multiple bits out (2-wire 4-phase)
 */
uint32_t
io_clock_bits_4phase(uint8_t nbits, uint32_t tms, uint32_t tdi)
{
	uint32_t tdo = 0;

	if (p.busy)
		io_program_feedback('+');

	for (uint32_t i = 0; i < nbits; ++i)
		tdo |= io_clock_bit_4phase((tms >> i) & 1, (tdi >> i) & 1) << i;

	return tdo;
}
#endif /* P32 */

/*
 * Program In
 *
 * Shift in 1..N bits for program/verify mode
 */
uint32_t
io_program_in(uint8_t nbits)
{
	if (p.busy)
		io_program_feedback('i');

	if (p.io && p.io->shift_in)
		return p.io->shift_in(nbits);

	return io_clock_in_bits(p.sleep_low, p.sleep_high, nbits);
}

/*
 * Program Out
 *
 * Shift out 1..N bits for program/verify mode
 */
void
io_program_out(uint32_t bits, uint8_t nbits)
{
	if (p.busy)
		io_program_feedback('o');

	if (p.io && p.io->shift_out)
		p.io->shift_out(bits, nbits);
	else
		io_clock_out_bits(p.sleep_low, p.sleep_high, bits, nbits);
}

/*
 * Busy feedback for program/verify mode
 */
void
io_program_feedback(char c)
{
	static int c_count = 0, a_count = 0;
	static const char arrow[] = "|/-\\";
	static char c_last = 0;

	if ((c_last != c) || (c_count++ > p.busy)) {
		fprintf(stderr, "%c[%c]\r", arrow[a_count++ % strlen(arrow)], c);
		c_last = c;
		c_count = 0;
	}
}
