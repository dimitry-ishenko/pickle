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

#ifndef _SERIAL_BB_H
#define _SERIAL_BB_H

uint8_t serial_bb_backend(void);
int serial_bb_open(void);
void serial_bb_close(void);
char *serial_bb_error(void);
void serial_bb_usleep(int);
void serial_bb_set_vpp(uint8_t);
void serial_bb_set_pgd(uint8_t);
void serial_bb_set_pgc(uint8_t);
uint8_t serial_bb_get_pgd(void);

#endif
