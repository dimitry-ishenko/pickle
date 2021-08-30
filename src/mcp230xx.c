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

struct io_ops mcp230xx_ops = {
	.type		= IOMCP230XX,
	.run		= 1,
	.open		= mcp230xx_open,
	.release	= NULL,
	.close		= mcp230xx_close,
	.error		= mcp230xx_error,
	.usleep		= NULL,
	.set_pgm	= mcp230xx_set_pgm,
	.set_vpp	= mcp230xx_set_vpp,
	.set_pgd	= mcp230xx_set_pgd,
	.set_pgc	= mcp230xx_set_pgc,
	.get_pgd	= mcp230xx_get_pgd,
	.configure	= NULL,
	.shift_in	= NULL,
	.shift_out	= NULL,
};

uint8_t
mcp230xx_backend(void)
{
	p.io = &mcp230xx_ops;

	return p.io->type;
}

int
mcp230xx_open(void)
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

	/* Initialise
 	 *
 	 * If IOCON.BANK == 0
 	 *	MCP23X08 N/A
	 *	MCP23X17 IOCON.BANK = 1
	 *
	 * If IOCON.BANK == 1
 	 *	MCP23X08 N/A
	 *	MCP23X17 N/A
	 */
	mcp230xx_set(MCP23X17_IOCON, MCP23X17_IOCON_BANK);

	/* All O/P except PGDI I/P */
	mcp230xx_set(MCP23XXX_IODIR, 1 << p.pgdi);

	/* All O/P low */
	latch = 0;
	mcp230xx_set(MCP23XXX_OUT, latch);

	return fd;
}

void
mcp230xx_close(void)
{
	close(fd);
	fd = -1;
}

char *
mcp230xx_error(void)
{
	return "Can't open MCP230XX I2C I/O";
}

void
mcp230xx_set_pgm(uint8_t pgm)
{
	if (p.pgm != GPIO_DISABLED) {
		if (pgm)
			latch |= (1 << p.pgm);
		else
			latch &= ~(1 << p.pgm);

		mcp230xx_set(MCP23XXX_OUT, latch);
	}
}

void
mcp230xx_set_vpp(uint8_t vpp)
{
	if (vpp)
		latch |= (1 << p.vpp);
	else
		latch &= ~(1 << p.vpp);

	mcp230xx_set(MCP23XXX_OUT, latch);
}

void
mcp230xx_set_pgd(uint8_t pgd)
{
	if (pgd)
		latch |= (1 << p.pgdo);
	else
		latch &= ~(1 << p.pgdo);

	mcp230xx_set(MCP23XXX_OUT, latch);
}

void
mcp230xx_set_pgc(uint8_t pgc)
{
	if (pgc)
		latch |= (1 << p.pgc);
	else
		latch &= ~(1 << p.pgc);

	mcp230xx_set(MCP23XXX_OUT, latch);
}

uint8_t
mcp230xx_get_pgd(void)
{
	uint8_t pgd = 0;

	mcp230xx_get(MCP23XXX_IN, &pgd);

	if (pgd & (1 << p.pgdi))
		return HIGH;

	return LOW;
}

void
mcp230xx_set(uint8_t reg, uint8_t val)
{
	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = val;
	int rc = write(fd, buf, 2);
	if (rc != 2) {
		printf("%s: warning: write error [%s]\n", __func__, strerror(errno));
	}
}

void
mcp230xx_get(uint8_t reg, uint8_t *val)
{
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
