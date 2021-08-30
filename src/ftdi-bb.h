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

#ifndef _FTDI_BB_H
#define _FTDI_BB_H

#include <ftdi.h>

#define FTDI_BB_STEP    (4)
#if 0
#define FTDI_BB_RISING  (2)
#else
#define FTDI_BB_RISING  (3)
#endif
#define FTDI_BB_FALLING (3)

#define FTDI_BB_MAX_BITS_TRANSFER (64)
#define FTDI_BB_BUFFER_SIZE (FTDI_BB_MAX_BITS_TRANSFER * FTDI_BB_STEP)

struct ftdi_bb_io {
	uint8_t dir;
	uint8_t pin;
	uint8_t bit;
} __attribute__((packed));

struct ftdi_bb_shift {
	uint8_t dir;
	uint8_t nbits;
	uint64_t bits;
} __attribute__((packed));

uint8_t ftdi_bb_backend(void);
int ftdi_bb_open(void);
void ftdi_bb_close(void);
char *ftdi_bb_error(void);
void ftdi_bb_set_pgm(uint8_t);
void ftdi_bb_set_vpp(uint8_t);
void ftdi_bb_set_pgd(uint8_t);
void ftdi_bb_set_pgc(uint8_t);
uint8_t ftdi_bb_get_pgd(void);
uint32_t ftdi_bb_shift_in(uint8_t);
void ftdi_bb_shift_out(uint32_t, uint8_t);
int ftdi_bb_bitmode(uint8_t);
int ftdi_bb_io(struct ftdi_bb_io *);
int ftdi_bb_shift(struct ftdi_bb_shift *);

#endif /* _FTDI_BB_H */
