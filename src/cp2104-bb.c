/*
 * Copyright (C) 2017-2020 Darron Broad
 * Copyright (C) 2015 Gerhard Bertelsmann
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

/*******************************************************************************
 *
 * I/O operations
 *
 ******************************************************************************/

struct io_ops cp2104_bb_ops = {
	.type		= IOCP2104,
	.run		= 1,
	.open		= cp2104_bb_open,
	.release	= NULL,
	.close		= cp2104_bb_close,
	.error		= cp2104_bb_error,
	.usleep		= NULL,
	.set_pgm	= cp2104_bb_set_pgm,
	.set_vpp	= cp2104_bb_set_vpp,
	.set_pgd	= cp2104_bb_set_pgd,
	.set_pgc	= cp2104_bb_set_pgc,
	.get_pgd	= cp2104_bb_get_pgd,
	.configure	= NULL,
	.shift_in	= NULL,
	.shift_out	= NULL,
};

uint8_t
cp2104_bb_backend(void)
{
	p.io = &cp2104_bb_ops;

	return p.io->type;
}

int
cp2104_bb_open(void)
{
	fd = open(p.iface, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		fd = -1;
		return -1;
	}

	/* Open-drain input mode */
	cp2104_bb_set(p.pgdi, HIGH);

	return 0;
}

void
cp2104_bb_close(void)
{
	close(fd);
	fd = -1;
}

char *
cp2104_bb_error(void)
{
	return "Can't open CP2104 I/O";
}

void
cp2104_bb_set_pgm(uint8_t pgm)
{
	if (p.pgm != GPIO_DISABLED)
		cp2104_bb_set(p.pgm, pgm);
}

void
cp2104_bb_set_vpp(uint8_t vpp)
{
	cp2104_bb_set(p.vpp, vpp);
}

void
cp2104_bb_set_pgd(uint8_t pgd)
{
	cp2104_bb_set(p.pgdo, pgd);
}

void
cp2104_bb_set_pgc(uint8_t pgc)
{
	cp2104_bb_set(p.pgc, pgc);
}

uint8_t
cp2104_bb_get_pgd(void)
{
	return cp2104_bb_get(p.pgdi);
}

void
cp2104_bb_set(uint8_t pin, uint8_t val)
{
	uint32_t gpio;

	pin &= 3;

	gpio = 1 << pin;
	if (val)
		gpio |= 1 << (8 + pin);

	ioctl(fd, IOCTL_GPIOSET, &gpio);
}

uint8_t
cp2104_bb_get(uint8_t pin)
{
	uint32_t gpio;

	ioctl(fd, IOCTL_GPIOGET, &gpio);

	pin &= 3;

	if (gpio & (1 << pin))
		return HIGH;

	return LOW;
}
