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

#undef DEBUG

#include "pickle.h"

/******************************************************************************
 *
 * Session
 *
 *****************************************************************************/

extern struct pickle p;

/******************************************************************************
 *
 * Back-end
 *
 *****************************************************************************/

static int fd = -1;		/* File descriptor */

static uint8_t latch;		/* Shadow output */

/*******************************************************************************
 *
 * I/O operations
 *
 ******************************************************************************/

struct io_ops pcf8574_ops = {
	.type		= IOPCF8574,
	.run		= 1,
	.open		= pcf8574_open,
	.release	= pcf8574_release,
	.close		= pcf8574_close,
	.error		= pcf8574_error,
	.usleep		= NULL,
	.set_pgm	= pcf8574_set_pgm,
	.set_vpp	= pcf8574_set_vpp,
	.set_pgd	= pcf8574_set_pgd,
	.set_pgc	= pcf8574_set_pgc,
	.get_pgd	= pcf8574_get_pgd,
	.configure	= NULL,
	.shift_in	= NULL,
	.shift_out	= NULL,
};

uint8_t
pcf8574_backend(void)
{
	p.io = &pcf8574_ops;

	return p.io->type;
}

int
pcf8574_open(void)
{
	fd = open(p.iface, O_RDWR);
	if (fd < 0) {
		fd = -1;
		return -1; /* Error */
	}

	if (ioctl(fd, I2C_SLAVE, p.addr) < 0) {
		close(fd);
		fd = -1;
		return -1; /* Error */
	}

	/* Init. I/O */
	if (p.pgdi != p.pgdo) {
		/* Always input */
		latch = 1 << p.pgdi;
	} else {
		/* Input needs PULLUP on direction change */
		latch = 0;
	}

	pcf8574_set(latch);

	return fd;
}

void
pcf8574_release(void)
{
	DPRINT("%s()\n", __func__);

	if (p.bitrules & PGD_RELEASE)
		latch |= (1 << p.pgdo);
	if (p.bitrules & PGC_RELEASE)
		latch |= (1 << p.pgc);
	if (p.bitrules & PGM_RELEASE && p.pgm != GPIO_DISABLED)
		latch |= (1 << p.pgm);
	if (p.bitrules & VPP_RELEASE)
		latch |= (1 << p.vpp);

	pcf8574_set(latch);
}

void
pcf8574_close(void)
{
	close(fd);
	fd = -1;
}

char *
pcf8574_error(void)
{
	return "Can't open PCF8574 I2C I/O";
}

void
pcf8574_set_pgm(uint8_t pgm)
{
	DPRINT("%s(0x%02X)\n", __func__, pgm);

	if (p.pgm != GPIO_DISABLED) {
		if (pgm)
			latch |= (1 << p.pgm);
		else
			latch &= ~(1 << p.pgm);

		pcf8574_set(latch);
	}
}

void
pcf8574_set_vpp(uint8_t vpp)
{
	DPRINT("%s(0x%02X)\n", __func__, vpp);

	if (vpp)
		latch |= (1 << p.vpp);
	else
		latch &= ~(1 << p.vpp);

	pcf8574_set(latch);
}

void
pcf8574_set_pgd(uint8_t pgd)
{
	DPRINT("%s(0x%02X)\n", __func__, pgd);

	if (pgd)
		latch |= (1 << p.pgdo);
	else
		latch &= ~(1 << p.pgdo);

	pcf8574_set(latch);
}

void
pcf8574_set_pgc(uint8_t pgc)
{
	DPRINT("%s(0x%02X)\n", __func__, pgc);

	if (pgc)
		latch |= (1 << p.pgc);
	else
		latch &= ~(1 << p.pgc);

	pcf8574_set(latch);
}

uint8_t
pcf8574_get_pgd(void)
{
	DPRINT("%s()\n", __func__);

	uint8_t pgd = 0;

	pcf8574_get(&pgd);

	if (pgd & (1 << p.pgdi))
		return HIGH;

	return LOW;
}

void
pcf8574_set(uint8_t val)
{
	DPRINT("%s(0x%02X)\n", __func__, val);

	uint8_t buf[1];

	buf[0] = val;
	int rc = write(fd, buf, 1);
	if (rc != 1) {
		printf("%s: warning: write error [%s]\n", __func__, strerror(errno));
	}
}

void
pcf8574_get(uint8_t *val)
{
	DPRINT("%s()\n", __func__);

	uint8_t buf[1] = {0};

	int rc = read(fd, buf, 1);
	if (rc != 1) {
		printf("%s: warning: read error [%s]\n", __func__, strerror(errno));
	}
	*val = buf[0];
}
