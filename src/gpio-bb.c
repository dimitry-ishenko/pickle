/*
 * Copyright (C) 2013-2020 Darron Broad
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

struct io_ops gpio_bb_ops = {
	.type		= IOBITBANG,
	.run		= 1,
	.open		= gpio_bb_open,
	.release	= gpio_bb_release,
	.close		= gpio_bb_close,
	.error		= gpio_bb_error,
	.usleep		= NULL,
	.set_pgm	= gpio_bb_set_pgm,
	.set_vpp	= gpio_bb_set_vpp,
	.set_pgd	= gpio_bb_set_pgd,
	.set_pgc	= gpio_bb_set_pgc,
	.get_pgd	= gpio_bb_get_pgd,
	.configure	= gpio_bb_configure,
	.shift_in	= gpio_bb_shift_in,
	.shift_out	= gpio_bb_shift_out,
};

uint8_t
gpio_bb_backend(void)
{
	p.io = &gpio_bb_ops;

	return p.io->type;
}

int
gpio_bb_open(void)
{
	fd = open(p.iface, O_RDWR);
	if (fd < 0) {
		fd = -1;
		return -1;
	}

	return 0;
}

void
gpio_bb_release(void)
{
	if (p.bitrules & PGD_RELEASE) {
		gpio_bb_release_pin(p.pgdo);
	}
	if (p.bitrules & PGC_RELEASE) {
		gpio_bb_release_pin(p.pgc);
	}
	if (p.bitrules & PGM_RELEASE && p.pgm != GPIO_DISABLED) {
		gpio_bb_release_pin(p.pgm);
	}
	if (p.bitrules & VPP_RELEASE) {
		gpio_bb_release_pin(p.vpp);
	}
}

void
gpio_bb_close(void)
{
	close(fd);
	fd = -1;
}

char *
gpio_bb_error(void)
{
	return "Can't open GPIO BIT-BANG I/O";
}

void
gpio_bb_set_pgm(uint8_t pgm)
{
	if (p.pgm != GPIO_DISABLED) {
		struct gpio_bb_io io = {.dir = 0, .pin = p.pgm, .bit = pgm};

		ioctl(fd, GPIO_BB_IO, &io);
	}
}

void
gpio_bb_set_vpp(uint8_t vpp)
{
	struct gpio_bb_io io = {.dir = 0, .pin = p.vpp, .bit = vpp};

	ioctl(fd, GPIO_BB_IO, &io);
}

void
gpio_bb_set_pgd(uint8_t pgd)
{
	struct gpio_bb_io io = {.dir = 0, .pin = p.pgdo, .bit = pgd};

	ioctl(fd, GPIO_BB_IO, &io);
}

void
gpio_bb_set_pgc(uint8_t pgc)
{
	struct gpio_bb_io io = {.dir = 0, .pin = p.pgc, .bit = pgc};

	ioctl(fd, GPIO_BB_IO, &io);
}

uint8_t
gpio_bb_get_pgd(void)
{
	struct gpio_bb_io io = {.dir = 1, .pin = p.pgdi, .bit = 0};

	ioctl(fd, GPIO_BB_IO, &io);

	return io.bit;
}

void
gpio_bb_configure(void)
{
	struct gpio_bb_config config;

	config.clock_pin = p.pgc;
	config.clock_falling = p.clock_falling;
	config.msb_first = p.msb_first;
	config.data_pin_input = p.pgdi;
	config.data_pin_output = p.pgdo;
	config.clock_delay_low = p.sleep_low;
	config.clock_delay_high = p.sleep_high;
	config.lock = (p.bitrules & BB_LOCK) ? 1 : 0;

	ioctl(fd, GPIO_BB_CONFIGURE, &config);
}

uint32_t
gpio_bb_shift_in(uint8_t nbits)
{
	struct gpio_bb_shift shift = {1, nbits, 0};

	ioctl(fd, GPIO_BB_SHIFT, &shift);

	return (uint32_t)(shift.bits);
}

void
gpio_bb_shift_out(uint32_t bits, uint8_t nbits)
{
	struct gpio_bb_shift shift = {0, nbits, (uint64_t)(bits)};

	ioctl(fd, GPIO_BB_SHIFT, &shift);
}

/*
 * Reselect pin as input
 */
void
gpio_bb_release_pin(uint16_t pin)
{
	struct gpio_bb_io io = {.dir = 1, .pin = pin, .bit = 0};

	ioctl(fd, GPIO_BB_IO, &io);
}
