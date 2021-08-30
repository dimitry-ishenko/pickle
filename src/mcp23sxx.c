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

struct io_ops mcp23sxx_ops = {
	.type           = IOMCP23SXX,
	.run            = 1,
	.open           = mcp23sxx_open,
	.release	= NULL,
	.close          = mcp23sxx_close,
	.error          = mcp23sxx_error,
	.usleep         = NULL,
	.set_pgm        = mcp23sxx_set_pgm,
	.set_vpp        = mcp23sxx_set_vpp,
	.set_pgd        = mcp23sxx_set_pgd,
	.set_pgc        = mcp23sxx_set_pgc,
	.get_pgd        = mcp23sxx_get_pgd,
	.configure      = NULL,
	.shift_in       = NULL,
	.shift_out      = NULL,
};

uint8_t
mcp23sxx_backend(void)
{
	p.io = &mcp23sxx_ops;

	return p.io->type;
}

int
mcp23sxx_open(void)
{
	int arg;

	fd = open(p.iface, O_RDWR);
	if (fd < 0) {
		fd = -1;
		return -1;
	}

	arg = 0;
	if (ioctl(fd, SPI_IOC_WR_MODE, &arg) < 0) {
		close(fd);
		fd = -1;
		return -1;
	}

	arg = 8;
	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &arg) < 0) {
		close(fd);
		fd = -1;
		return -1;
	}

	arg = p.baudrate;
	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &arg) < 0) {
		close(fd);
		fd = -1;
		return -1;
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
	mcp23sxx_set(MCP23X17_IOCON, MCP23X17_IOCON_BANK);

	/* All O/P except PGDI I/P */
	mcp23sxx_set(MCP23XXX_IODIR, 1 << p.pgdi);

	/* All O/P low */
	latch = 0;
	mcp23sxx_set(MCP23XXX_OUT, latch);

	return fd;
}

void
mcp23sxx_close(void)
{
	close(fd);
	fd = -1;
}

char *
mcp23sxx_error(void)
{
	return "Can't open MCP23SXX SPI I/O";
}

void
mcp23sxx_set_pgm(uint8_t pgm)
{
	if (p.pgm != GPIO_DISABLED) {
		if (pgm)
			latch |= (1 << p.pgm);
		else
			latch &= ~(1 << p.pgm);

		mcp23sxx_set(MCP23XXX_OUT, latch);
	}
}

void
mcp23sxx_set_vpp(uint8_t vpp)
{
	if (vpp)
		latch |= (1 << p.vpp);
	else
		latch &= ~(1 << p.vpp);

	mcp23sxx_set(MCP23XXX_OUT, latch);
}

void
mcp23sxx_set_pgd(uint8_t pgd)
{
	if (pgd)
		latch |= (1 << p.pgdo);
	else
		latch &= ~(1 << p.pgdo);

	mcp23sxx_set(MCP23XXX_OUT, latch);
}

void
mcp23sxx_set_pgc(uint8_t pgc)
{
	if (pgc)
		latch |= (1 << p.pgc);
	else
		latch &= ~(1 << p.pgc);

	mcp23sxx_set(MCP23XXX_OUT, latch);
}

uint8_t
mcp23sxx_get_pgd(void)
{
	uint8_t pgd = 0;

	mcp23sxx_get(MCP23XXX_IN, &pgd);

	if (pgd & (1 << p.pgdi))
		return HIGH;

	return LOW;
}

static int
mcp23sxx_io(int f, char *s, int slen)
{
	struct spi_ioc_transfer spi;

	bzero((void *)(&spi), sizeof(struct spi_ioc_transfer));

	spi.tx_buf = (unsigned long)(s);
	spi.rx_buf = (unsigned long)(s);
	spi.len = slen;
	spi.bits_per_word = 8;
	spi.speed_hz = p.baudrate;

	return ioctl(f, SPI_IOC_MESSAGE(1), &spi);
}

void
mcp23sxx_set(uint8_t reg, uint8_t val)
{
	char buf[3];
	buf[0] = (p.addr << 1);
	buf[1] = reg;
	buf[2] = val;

	int rc = mcp23sxx_io(fd, buf, 3);
	if (rc == -1) {
		printf("%s: warning: I/O error [%s]\n", __func__, strerror(errno));
	}
}

void
mcp23sxx_get(uint8_t reg, uint8_t *val)
{
	char buf[3];
	buf[0] = (p.addr << 1) | 1;
	buf[1] = reg;
	buf[2] = 0;

	int rc = mcp23sxx_io(fd, buf, 3);
	if (rc == -1) {
		printf("%s: warning: I/O error [%s]\n", __func__, strerror(errno));
	}

	*val = buf[2];
}
