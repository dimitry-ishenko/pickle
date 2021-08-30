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

#ifndef _MCP23XXX_H
#define _MCP23XXX_H

#if 0
/*
 * MCP23X17 IOCON.BANK = 0 (DEFAULT)
 */
#define MCP23X17_IODIRA   (0x00)
#define MCP23X17_IODIRB   (0x01)
#define MCP23X17_IPOLA    (0x02)
#define MCP23X17_IPOLB    (0x03)
#define MCP23X17_GPINTENA (0x04)
#define MCP23X17_GPINTENB (0x05)
#define MCP23X17_DEFVALA  (0x06)
#define MCP23X17_DEFVALB  (0x07)
#define MCP23X17_INTCONA  (0x08)
#define MCP23X17_INTCONB  (0x09)
#define MCP23X17_IOCON1   (0x0A)
#define MCP23X17_IOCON2   (0x0B)
#define MCP23X17_GPPUA    (0x0C)
#define MCP23X17_GPPUB    (0x0D)
#define MCP23X17_INTFA    (0x0E)
#define MCP23X17_INTFB    (0x0F)
#define MCP23X17_INTCAPA  (0x10)
#define MCP23X17_INTCAPB  (0x11)
#define MCP23X17_GPIOA    (0x12)
#define MCP23X17_GPIOB    (0x13)
#define MCP23X17_OLATA    (0x14)
#define MCP23X17_OLATB    (0x15)
#else
/*
 * MCP23X08
 * MCP23X17 IOCON.BANK = 1
 *
 * Name    Addr  B7     B6     B5     B4     B3     B2     B1     B0     Reset
 * IODIR   00    IO7    IO6    IO5    IO4    IO3    IO2    IO1    IO0    1111 1111
 * IPOL    01    IP7    IP6    IP5    IP4    IP3    IP2    IP1    IP0    0000 0000
 * GPINTEN 02    GPINT7 GPINT6 GPINT5 GPINT4 GPINT3 GPINT2 GPINT1 GPINT0 0000 0000
 * DEFVAL  03    DEF7   DEF6   DEF5   DEF4   DEF3   DEF2   DEF1   DEF0   0000 0000
 * INTCON  04    IOC7   IOC6   IOC5   IOC4   IOC3   IOC2   IOC1   IOC0   0000 0000
 * IOCON   05    BANK   MIRROR SREAD  DISSLW HAEN   ODR    INTPOL -      0000 0000
 * GPPU    06    PU7    PU6    PU5    PU4    PU3    PU2    PU1    PU0    0000 0000
 * INTF    07    INT7   INT6   INT5   INT4   INT3   INT2   INT1   INTO   0000 0000
 * INTCAP  08    ICP7   ICP6   ICP5   ICP4   ICP3   ICP2   ICP1   ICP0   0000 0000
 * GPIO    09    GP7    GP6    GP5    GP4    GP3    GP2    GP1    GP0    0000 0000
 * OLAT    0A    OL7    OL6    OL5    OL4    OL3    OL2    OL1    OL0    0000 0000
 */
#define MCP23XXX_IODIRA   (0x00)
#define MCP23XXX_IPOLA    (0x01)
#define MCP23XXX_GPINTENA (0x02)
#define MCP23XXX_DEFVALA  (0x03)
#define MCP23XXX_INTCONA  (0x04)
#define MCP23XXX_IOCON1   (0x05)
#define MCP23XXX_GPPUA    (0x06)
#define MCP23XXX_INTFA    (0x07)
#define MCP23XXX_INTCAPA  (0x08)
#define MCP23XXX_GPIOA    (0x09)
#define MCP23XXX_OLATA    (0x0A)
#endif
#if 0
#define MCP23X17_IODIRB   (0x10)
#define MCP23X17_IPOLB    (0x11)
#define MCP23X17_GPINTENB (0x12)
#define MCP23X17_DEFVALB  (0x13)
#define MCP23X17_INTCONB  (0x14)
#define MCP23X17_IOCON2   (0x15)
#define MCP23X17_GPPUB    (0x16)
#define MCP23X17_INTFB    (0x17)
#define MCP23X17_INTCAPB  (0x18)
#define MCP23X17_GPIOB    (0x19)
#define MCP23X17_OLATB    (0x1A)
#endif

/* I/O port A */
#define MCP23XXX_IODIR (MCP23XXX_IODIRA)
#define MCP23XXX_OUT   (MCP23XXX_OLATA)
#define MCP23XXX_IN    (MCP23XXX_GPIOA)

/* I/O control */
#define MCP23X17_IOCON        (0x0B)	/* MCP23X17 IOCON2 (DEFAULT) */
#define MCP23X17_IOCON_BANK   (0x80)
#if 0
#define MCP23X17_IOCON_MIRROR (0x40)
#define MCP23XXX_IOCON_SEQOP  (0x20)
#define MCP23XXX_IOCON_DISSLW (0x10)
#define MCP23XXX_IOCON_ODR    (0x04)
#define MCP23XXX_IOCON_INTPOL (0x02)
#endif

#endif /* !_MCP23XXX_H */
