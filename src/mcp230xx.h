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

#ifndef _MCP230XX_H
#define _MCP230XX_H

#include <linux/i2c-dev.h>

#include "mcp23xxx.h"

uint8_t mcp230xx_backend(void);
int mcp230xx_open(void);
void mcp230xx_close(void);
char *mcp230xx_error(void);
void mcp230xx_set_pgm(uint8_t);
void mcp230xx_set_vpp(uint8_t);
void mcp230xx_set_pgd(uint8_t);
void mcp230xx_set_pgc(uint8_t);
uint8_t mcp230xx_get_pgd(void);
void mcp230xx_set(uint8_t, uint8_t);
void mcp230xx_get(uint8_t, uint8_t *);

#endif /* !_MCP230XX_H */
