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

#ifndef _PIC16N_H
#define _PIC16N_H

/******************************************************************************
 * 16-bit CONFIGURATION
 *****************************************************************************/

#define PIC16N_WORD_MASK (0x0000FFFF)
#define PIC16N_DATA_MASK (0x000000FF)
#define PIC16N_USERID_MAX  (256)	/* 256 BYTES / 128 WORDS */
#define PIC16N_CONFIG_MAX  (12)		/* 12  BYTES / 6   WORDS */
#define PIC16N_DEVINFO_MAX (128)	/* WORDS */
#define PIC16N_DEVCONF_MAX (5)		/* WORDS */

struct pic16n_config {
	uint8_t userid[PIC16N_USERID_MAX];	/* 200000 .. 2000FF */
	uint8_t config[PIC16N_CONFIG_MAX];	/* 300000 .. 30000B */
	uint16_t devinfo[PIC16N_DEVINFO_MAX];	/* 3F0000 .. 3F003F */
	uint16_t devconf[PIC16N_DEVCONF_MAX];	/* 3FFF00 .. 3FFF09 */
	uint16_t revisionid;			/* 3FFFFC           */
	uint16_t deviceid;			/* 3FFFFE           */
};

struct pic16n_dsmap {
	char devicename[STRLEN];	/*                                    */
	uint16_t deviceid;		/*                                    */
	uint32_t datasheet;		/* programming specification          */
	uint32_t flash;			/* program flash size in words        */
	uint32_t config;		/* configuration size in bytes (even) */
	uint32_t eeprom;		/* nvm eeprom size in bytes           */
	uint32_t latches;		/* latch size                         */
#if 0
	uint32_t erase	;		/* erase size                         */
#endif
	uint32_t devinfo;		/* devinfo size in words              */
	uint32_t devconf;		/* devconf size in words              */
	uint32_t idsize;		/* user id size in bytes (even)       */
	uint8_t *masks;			/* config masks                       */
};

/******************************************************************************
 * MEMORY
 *****************************************************************************/

#define PIC16N_WORD(X) ((X) * 512) /* KB to words */

#define PIC16N_USERID_ADDR	(0x200000) /* User ID words */
#define PIC16N_CONFIG_ADDR	(0x300000) /* Configuration words or bytes (Q43) */

#define PIC16N_EEPROM_ADDR	(0x310000) /* EEPROM physical address */
#define PIC16N_EEPROM_ADDR_Q43	(0x380000) /* Q43 EEPROM physical address */

#define PIC16N_DEVINFO_ADDR	(0x3F0000) /* Device information address */
#define PIC16N_DEVINFO_ADDR_Q43	(0x2C0000) /* Q43 device information address */

#define PIC16N_DEVCONF_ADDR	(0x3FFF00) /* Device configuration address */
#define PIC16N_DEVCONF_ADDR_Q43	(0x3C0000) /* Q43 device configuration address */

#define PIC16N_REVID_ADDR	(0x3FFFFC) /* Revision ID word */
#define PIC16N_DEVID_ADDR	(0x3FFFFE) /* Device ID word */

#define PIC16N_ERASE        (0x3FFF00)
#define PIC16N_LATCHES      (0x3FFF02)
#define PIC16N_ROWS         (0x3FFF04)
#define PIC16N_EEPROM       (0x3FFF06)
#define PIC16N_PINS         (0x3FFF08)

/* K series */
#define PIC16N_TPINT_CODE   (2800)		/*  2.8 ms */
#define PIC16N_TPINT_CONFIG (5600)		/*  5.6 ms */
#define PIC16N_TERAB        (25200)		/* 25.2 ms */
#define PIC16N_TERAR        (2800)		/*  2.8 ms */

/* Q series */
#define PIC16N_TERAS_Q10    (11000)		/* 11.0 ms PAGE ERASE    */
#define PIC16N_TPINT_Q10    (65)		/*   65 us FLASH/CONFIG  */
#define PIC16N_TERAB_Q10    (75000)		/* 75.0 ms BULK ERASE    */
#define PIC16N_TPDFM_Q10    (11000)		/* 11.0 ms EEPROM        */

#define PIC16N_TERAS_Q43    (11000)		/* 11.0 ms PAGE ERASE    */
#define PIC16N_TPINT_Q43    (75)		/*   75 us FLASH         */
#define PIC16N_TERAB_Q43    (11000)		/* 11.0 ms BULK ERASE    */
#define PIC16N_TPDFM_Q43    (11000)		/* 11.0 ms EEPROM/CONFIG */

#define PIC16N_TERAS_Q      (PIC16N_TPINT_Q43)
#define PIC16N_TPINT_Q      (PIC16N_TPINT_Q43)
#define PIC16N_TPDFM_Q      (PIC16N_TPINT_Q43)

#define PIC16N_MAJOR_SHIFT  (6)
#define PIC16N_REV_MASK     (0x003F)

#define PIC16N_VERIFY_BYTE    (0)
#define PIC16N_VERIFY_HIGHLOW (1)
#define PIC16N_VERIFY_WORD    (2)

/******************************************************************************
 * PICMicro devices (8-bit data devices using new programming protocol)
 *****************************************************************************/

#define DS40001772B (40001772)
#define PIC18F47K40  (0x6900)
#define PIC18F46K40  (0x6920)
#define PIC18F45K40  (0x6940)
#define PIC18F27K40  (0x6960)
#define PIC18F26K40  (0x6980)
#define PIC18F25K40  (0x69A0)
#define PIC18F24K40  (0x69C0)
#define PIC18LF47K40 (0x69E0)
#define PIC18LF46K40 (0x6A00)
#define PIC18LF45K40 (0x6A20)
#define PIC18LF27K40 (0x6A40)
#define PIC18LF26K40 (0x6A60)
#define PIC18LF25K40 (0x6A80)
#define PIC18LF24K40 (0x6AA0)

#define DS40001836A (40001836)
#define PIC18F25K42  (0x6C80)
#define PIC18F24K42  (0x6CA0)
#define PIC18LF25K42 (0x6DC0)
#define PIC18LF24K42 (0x6DE0)

#define DS40001886B (40001886)
#define PIC18F26K42  (0x6C60)
#define PIC18F27K42  (0x6C40)
#define PIC18F45K42  (0x6C20)
#define PIC18F46K42  (0x6C00)
#define PIC18F47K42  (0x6BE0)
#define PIC18F55K42  (0x6BC0)
#define PIC18F56K42  (0x6BA0)
#define PIC18F57K42  (0x6B80)
#define PIC18LF26K42 (0x6DA0)
#define PIC18LF27K42 (0x6D80)
#define PIC18LF45K42 (0x6D60)
#define PIC18LF46K42 (0x6D40)
#define PIC18LF47K42 (0x6D20)
#define PIC18LF55K42 (0x6D00)
#define PIC18LF56K42 (0x6CE0)
#define PIC18LF57K42 (0x6CC0)

#define DS40001927A (40001927)
#define PIC18F25K83  (0x6EE0)
#define PIC18F26K83  (0x6EC0)
#define PIC18LF25K83 (0x6F20)
#define PIC18LF26K83 (0x6F00)

#define DS40001874F (40001874)
#define PIC18F46Q10 (0x7120)
#define PIC18F45Q10 (0x7140)
#define PIC18F26Q10 (0x7180)
#define PIC18F25Q10 (0x71A0)
#define PIC18F24Q10 (0x71C0)
#define PIC18F27Q10 (0x7100)
#define PIC18F47Q10 (0x70E0)

#define DS40002079D (40002079)
#define PIC18F25Q43 (0x73C0)
#define PIC18F26Q43 (0x7420)
#define PIC18F27Q43 (0x7480)
#define PIC18F45Q43 (0x73E0)
#define PIC18F46Q43 (0x7440)
#define PIC18F47Q43 (0x74A0)
#define PIC18F55Q43 (0x7400)
#define PIC18F56Q43 (0x7460)
#define PIC18F57Q43 (0x74C0)

/******************************************************************************/

uint32_t pic16n_arch(void);
void pic16n_selector(void);
void pic16n_bootloader(void);
void pic16n_program_verify(void);
void pic16n_standby(void);
void pic16n_bulk_erase(void);
void pic16n_row_erase(uint32_t, uint32_t);
int pic16n_read_config_memory(void);
void pic16n_write_panel(uint32_t, uint32_t, uint32_t *, uint32_t);
uint32_t pic16n_get_program_count(void);
uint32_t pic16n_get_program_size(uint32_t *, uint32_t);
uint32_t pic16n_get_data_size(uint32_t *);
uint32_t pic16n_read_program_memory_block(uint32_t *, uint32_t, uint32_t);
uint32_t pic16n_read_data_memory_block(uint16_t *, uint32_t, uint16_t);
uint32_t pic16n_write_config(void);
uint16_t pic16n_getregion(uint32_t);
uint32_t pic16n_program_data(uint32_t, pic_data *);
void pic16n_program_begin(void);
void pic16n_program_end(int);
uint32_t pic16n_verify_data(uint32_t, pic_data *, uint32_t *);
void pic16n_view_data(pic_data *);
void pic16n_dumpdeviceid(void);
void pic16n_dumpconfig(uint32_t, uint32_t);
void pic16n_dumphexcode(uint32_t, uint32_t, uint32_t *);
void pic16n_dumpinhxcode(uint32_t, uint32_t, uint32_t *);
void pic16n_dumphexdata(uint32_t, uint32_t, uint16_t *);
void pic16n_dumpinhxdata(uint32_t, uint32_t, uint16_t *);
void pic16n_dumpdevice(void);

#endif /* !_PIC16N_H */
