/*
 * Copyright (C) 2017-2020 Darron Broad
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

#ifndef _PCF8574_H
#define _PCF8574_H

#include <linux/i2c-dev.h>

uint8_t pcf8574_backend(void);
int pcf8574_open(void);
void pcf8574_release(void);
void pcf8574_close(void);
char *pcf8574_error(void);
void pcf8574_set_pgm(uint8_t);
void pcf8574_set_vpp(uint8_t);
void pcf8574_set_pgd(uint8_t);
void pcf8574_set_pgc(uint8_t);
uint8_t pcf8574_get_pgd(void);
void pcf8574_set(uint8_t);
void pcf8574_get(uint8_t *);

#endif /* _PCF8574_H */
