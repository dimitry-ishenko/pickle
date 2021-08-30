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

struct io_ops mcp23016_ops = {
	.type		= IOMCP23016,
	.run		= 1,
	.open		= mcp23016_open,
	.release	= NULL,
	.close		= mcp23016_close,
	.error		= mcp23016_error,
	.usleep		= NULL,
	.set_pgm	= mcp23016_set_pgm,
	.set_vpp	= mcp23016_set_vpp,
	.set_pgd	= mcp23016_set_pgd,
	.set_pgc	= mcp23016_set_pgc,
	.get_pgd	= mcp23016_get_pgd,
	.configure	= NULL,
	.shift_in	= NULL,
	.shift_out	= NULL,
};

uint8_t
mcp23016_backend(void)
{
	p.io = &mcp23016_ops;

	return p.io->type;
}

int
mcp23016_open(void)
{
	DPRINT("%s()\n", __func__);

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

	/* All O/P except PGDI I/P */
	mcp23016_set(MCP23016_IODIR, 1 << p.pgdi);

	/* All O/P low */
	latch = 0;
	mcp23016_set(MCP23016_OUT, latch);

	return fd;
}

void
mcp23016_close(void)
{
	DPRINT("%s()\n", __func__);

	close(fd);
	fd = -1;
}

char *
mcp23016_error(void)
{
	return "Can't open MCP23016 I2C I/O";
}

void
mcp23016_set_pgm(uint8_t pgm)
{
	DPRINT("%s(0x%02X)\n", __func__, pgm);

	if (p.pgm != GPIO_DISABLED) {
		if (pgm)
			latch |= (1 << p.pgm);
		else
			latch &= ~(1 << p.pgm);

		mcp23016_set(MCP23016_OUT, latch);
	}
}

void
mcp23016_set_vpp(uint8_t vpp)
{
	DPRINT("%s(0x%02X)\n", __func__, vpp);

	if (vpp)
		latch |= (1 << p.vpp);
	else
		latch &= ~(1 << p.vpp);

	mcp23016_set(MCP23016_OUT, latch);
}

void
mcp23016_set_pgd(uint8_t pgd)
{
	DPRINT("%s(0x%02X)\n", __func__, pgd);

	if (pgd)
		latch |= (1 << p.pgdo);
	else
		latch &= ~(1 << p.pgdo);

	mcp23016_set(MCP23016_OUT, latch);
}

void
mcp23016_set_pgc(uint8_t pgc)
{
	DPRINT("%s(0x%02X)\n", __func__, pgc);

	if (pgc)
		latch |= (1 << p.pgc);
	else
		latch &= ~(1 << p.pgc);

	mcp23016_set(MCP23016_OUT, latch);
}

uint8_t
mcp23016_get_pgd(void)
{
	DPRINT("%s()\n", __func__);

	uint8_t pgd = 0;

	mcp23016_get(MCP23016_IN, &pgd);

	if (pgd & (1 << p.pgdi))
		return HIGH;

	return LOW;
}

void
mcp23016_set(uint8_t reg, uint8_t val)
{
	DPRINT("%s(0x%02X,0x%02X)\n", __func__, reg, val);

	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = val;
	int rc = write(fd, buf, 2);
	if (rc != 2) {
		printf("%s: warning: write error [%s]\n", __func__, strerror(errno));
	}
}

void
mcp23016_get(uint8_t reg, uint8_t *val)
{
	DPRINT("%s(0x%02X,)\n", __func__, reg);

	uint8_t buf[1];

	buf[0] = reg;
	int rc = write(fd, buf, 1);
	if (rc != 1) {
		printf("%s: warning: write error [%s]\n", __func__, strerror(errno));
	}
	rc = read(fd, buf, 1);
	if (rc != 1) {
		printf("%s: warning: read error [%s]\n", __func__, strerror(errno));
	}
	*val = buf[0];
}
