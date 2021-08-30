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

#ifndef _MCP2221_H
#define _MCP2221_H

#include <linux/hiddev.h>
#include <linux/input.h>

#define HIDDEV "/dev/usb/hiddev%d"	/* 0 .. 15 */
#define HIDMAX 15

#define VENDOR  (0x04D8)
#define PRODUCT (0x00DD)

#define LENGTH (64)

#define CMD_STATUS_SET_PARAMETERS         (0x10)
#define CMD_READ_FLASH_DATA               (0xB0)
#define CMD_WRITE_FLASH_DATA              (0xB1)
#define CMD_SEND_FLASH_ACCESS_PASSWORD    (0xB2)
#define CMD_I2C_WRITE_DATA                (0x90)
#define CMD_I2C_WRITE_DATA_REPEATED_START (0x92)
#define CMD_I2C_WRITE_DATA_NO_STOP        (0x94)
#define CMD_I2C_READ_DATA                 (0x91)
#define CMD_I2C_READ_DATA_REPEATED_START  (0x93)
#define CMD_I2C_READ_DATA_GET_I2C_DATA    (0x40)
#define CMD_SET_GPIO_OUTPUT_VALUES        (0x50)
#define CMD_GET_GPIO_VALUES               (0x51)
#define CMD_SET_SRAM_SETTINGS             (0x60)
#define CMD_GET_SRAM_SETTINGS             (0x61)
#define CMD_RESET_CHIP                    (0x70)

uint8_t mcp2221_backend(void);
int mcp2221_open(void);
void mcp2221_close(void);
char *mcp2221_error(void);
void mcp2221_set_pgm(uint8_t);
void mcp2221_set_vpp(uint8_t);
void mcp2221_set_pgd(uint8_t);
void mcp2221_set_pgc(uint8_t);
uint8_t mcp2221_get_pgd(void);
int mcp2221_open_hid(void);
int mcp2221_cmd_set_sram_settings(void);
int mcp2221_cmd_set_gpio_output_values(void);
int mcp2221_cmd_get_gpio_values(void);

#endif /* _MCP2221_H */
