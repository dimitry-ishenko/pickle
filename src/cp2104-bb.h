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

#ifndef _CP2104_H
#define _CP2104_H

/* CP210x_VCP_Linux_3.13 IOCTL */
#define IOCTL_GPIOGET 0x8000
#define IOCTL_GPIOSET 0x8001

/* CP210x_VCP_Linux_3.13 GPIO */
#define GPIO_0_ON   0x0101
#define GPIO_0_OFF  0x0001
#define GPIO_1_ON   0x0202
#define GPIO_1_OFF  0x0002
#define GPIO_2_ON   0x0404
#define GPIO_2_OFF  0x0004
#define GPIO_3_ON   0x0808
#define GPIO_3_OFF  0x0008
#define GPIO_0_GET  0x0001
#define GPIO_1_GET  0x0002
#define GPIO_2_GET  0x0004
#define GPIO_3_GET  0x0008

uint8_t cp2104_bb_backend(void);
int cp2104_bb_open(void);
void cp2104_bb_close(void);
char *cp2104_bb_error(void);
void cp2104_bb_usleep(int);
void cp2104_bb_set_pgm(uint8_t);
void cp2104_bb_set_vpp(uint8_t);
void cp2104_bb_set_pgd(uint8_t);
void cp2104_bb_set_pgc(uint8_t);
uint8_t cp2104_bb_get_pgd(void);
void cp2104_bb_set(uint8_t, uint8_t);
uint8_t cp2104_bb_get(uint8_t);

#endif
