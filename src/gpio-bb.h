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

#ifndef _GPIO_BB_H
#define _GPIO_BB_H

#define GPIO_BB_VERSION_MAJ (1)
#define GPIO_BB_VERSION_MIN (3)

struct gpio_bb_io {
	uint16_t pin;
	uint8_t dir;
	uint8_t bit;
} __attribute__((packed));

struct gpio_bb_config {
	uint16_t clock_pin;
	uint16_t data_pin_input;
	uint16_t data_pin_output;
	uint8_t clock_falling;
	uint8_t msb_first;
	uint8_t clock_delay_low;
	uint8_t clock_delay_high;
	uint8_t lock;
} __attribute__((packed));

struct gpio_bb_shift {
	uint8_t dir;
	uint8_t nbits;
	uint64_t bits;
} __attribute__((packed));

#define GPIO_BB_MAX (512)

#define GPIO_BB_MAJOR (180)
#define GPIO_BB_IO		_IOWR(GPIO_BB_MAJOR, 205, struct gpio_bb_io *)
#define GPIO_BB_CONFIGURE	_IOW(GPIO_BB_MAJOR,  206, struct gpio_bb_config *)
#define GPIO_BB_SHIFT		_IOWR(GPIO_BB_MAJOR, 207, struct gpio_bb_shift *)

uint8_t gpio_bb_backend(void);
int gpio_bb_open(void);
void gpio_bb_release(void);
void gpio_bb_close(void);
char *gpio_bb_error(void);
void gpio_bb_set_pgm(uint8_t);
void gpio_bb_set_vpp(uint8_t);
void gpio_bb_set_pgd(uint8_t);
void gpio_bb_set_pgc(uint8_t);
uint8_t gpio_bb_get_pgd(void);
void gpio_bb_configure(void);
uint32_t gpio_bb_shift_in(uint8_t);
void gpio_bb_shift_out(uint32_t, uint8_t);
void gpio_bb_release_pin(uint16_t);

#endif /* _GPIO_BB_H */
