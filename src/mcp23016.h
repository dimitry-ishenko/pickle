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

#ifndef _MCP23016_H
#define _MCP23016_H

#include <linux/i2c-dev.h>

/* Registers */
#define MCP23016_GP0     (0x00)
#define MCP23016_GP1     (0x01)
#define MCP23016_OLAT0   (0x02)
#define MCP23016_OLAT1   (0x03)
#define MCP23016_IPOL0   (0x04)
#define MCP23016_IPOL1   (0x05)
#define MCP23016_IODIR0  (0x06)
#define MCP23016_IODIR1  (0x07)
#define MCP23016_INTCAP0 (0x08)
#define MCP23016_INTCAP1 (0x09)
#define MCP23016_IOCON0  (0x0A)
#define MCP23016_IOCON1  (0x0B)

/* I/O port 0 */
#define MCP23016_IODIR (MCP23016_IODIR0)
#define MCP23016_OUT   (MCP23016_OLAT0)
#define MCP23016_IN    (MCP23016_GP0)

uint8_t mcp23016_backend(void);
int mcp23016_open(void);
void mcp23016_close(void);
char *mcp23016_error(void);
void mcp23016_set_pgm(uint8_t);
void mcp23016_set_vpp(uint8_t);
void mcp23016_set_pgd(uint8_t);
void mcp23016_set_pgc(uint8_t);
uint8_t mcp23016_get_pgd(void);
void mcp23016_set(uint8_t, uint8_t);
void mcp23016_get(uint8_t, uint8_t *);

#endif /* !_MCP23016_H */
