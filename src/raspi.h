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

#ifndef _RASPI_H
#define _RASPI_H

/* Access from ARM Running Linux */
#define BCM2835_PERI_BASE_ADDR (0x20000000)
#define BCM2836_PERI_BASE_ADDR (0x3F000000)
#define BCM2837_PERI_BASE_ADDR (0x3F000000)
#define BCM2711_PERI_BASE_ADDR (0xFE000000)
#define GPIO_BASE_ADDR_OFFSET (0x200000)
#define GPIO_MAP_LEN (0x1000)

/* Only GPIO header pins supported */
#define GPIO_RPI_NPINS (32)

/* GPIO registers (32-bit) */
#define GPFSEL0 (0x00)
#define GPFSEL1 (0x01)
#define GPFSEL2 (0x02)
#define GPFSEL3 (0x03)
#define GPFSEL4 (0x04)
#define GPFSEL5 (0x05)
/* RESERVED 0x06 */
#define GPSET0 (0x07)
#define GPSET1 (0x08)
/* RESERVED 0x09 */
#define GPCLR0 (0x0A)
#define GPCLR1 (0x0B)
/* RESERVED 0x0C */
#define GPLEV0 (0x0D)
#define GPLEV1 (0x0E)
/* RESERVED 0x0F */
#define GPEDS0 (0x10)
#define GPEDS1 (0x11)
/* RESERVED 0x12 */
#define GPREN0 (0x13)
#define GPREN1 (0x14)
/* RESERVED 0x15 */
#define GPFEN0 (0x16)
#define GPFEN1 (0x17)
/* RESERVED 0x18 */
#define GPHEN0 (0x19)
#define GPHEN1 (0x1A)
/* RESERVED 0x1B */
#define GPLEN0 (0x1C)
#define GPLEN1 (0x1D)
/* RESERVED 0x1E */
#define GPAREN0 (0x1F)
#define GPAREN1 (0x20)
/* RESERVED 0x21 */
#define GPAFEN0 (0x22)
#define GPAFEN1 (0x23)
/* RESERVED 0x24 */
#define GPPUD (0x25)
#define GPPUDCLK0 (0x26)
#define GPPUDCLK1 (0x27)
/* RESERVED 0x28 */
/* RESERVED 0x29 */
/* RESERVED 0x2A */
/* RESERVED 0x2B */
/* TEST 0x2C */

#define GPPUD_OFF (0)
#define GPPUD_DOWN (1)
#define GPPUD_UP (2)
#define GPPUD_RESERVED (3)
#define GPPUD_DELAY (150)

/*
 *  FSEL - Function Select
 *
 *  000 = GPIO Pin is an input
 *  001 = GPIO Pin is an output
 *  100 = GPIO Pin takes alternate function 0
 *  101 = GPIO Pin takes alternate function 1
 *  110 = GPIO Pin takes alternate function 2
 *  111 = GPIO Pin takes alternate function 3
 *  011 = GPIO Pin takes alternate function 4
 *  010 = GPIO Pin takes alternate function 5
 */
#define GPIO_ALT0 (4) /* 100 */
#define GPIO_ALT1 (5) /* 101 */
#define GPIO_ALT2 (6) /* 110 */
#define GPIO_ALT3 (7) /* 111 */
#define GPIO_ALT4 (3) /* 011 */
#define GPIO_ALT5 (2) /* 010 */

uint8_t raspi_backend(void);
int raspi_open(void);
void raspi_release(void);
void raspi_close(void);
char *raspi_error(void);
void raspi_usleep(int);
void raspi_set_pgm(uint8_t);
void raspi_set_vpp(uint8_t);
void raspi_set_pgd(uint8_t);
void raspi_set_pgc(uint8_t);
uint8_t raspi_get_pgd(void);
int raspi_get(uint16_t, uint8_t *);
int raspi_set(uint16_t, uint8_t);
int raspi_release_pin(uint16_t);

#endif /* !_RASPI_H */
