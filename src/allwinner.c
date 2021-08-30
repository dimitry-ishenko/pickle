/*
 * Copyright (C) 2015-2020 Darron Broad
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

#include "pickle.h"

/******************************************************************************
 *
 * Session
 *
 *****************************************************************************/

extern struct pickle p;

/******************************************************************************
 *
 * Back-end
 *
 *****************************************************************************/

static int gpio_mem = -1;	/* Memory handle */

static void *gpio_map = NULL;	/* Memory mapped I/O pointer */

/* I/O Pins 0..NPINS-1 */
static uint8_t gpio_pins[AW_NPINS], gpio_dirs[AW_NPINS];
static uint32_t gpio_cfg[AW_NPINS], gpio_dat[AW_NPINS], gpio_pull[AW_NPINS];

static aw_device_t aw = {0};	/* Type of Pi.  */

/*******************************************************************************
 *
 * I/O operations
 *
 ******************************************************************************/

struct io_ops allwinner_ops = {
	.type		= IOALLWINNER,
	.run		= 1,
	.open		= allwinner_open,
	.release	= allwinner_release,
	.close		= allwinner_close,
	.error		= allwinner_error,
	.usleep		= allwinner_usleep,
	.set_pgm	= allwinner_set_pgm,
	.set_vpp	= allwinner_set_vpp,
	.set_pgd	= allwinner_set_pgd,
	.set_pgc	= allwinner_set_pgc,
	.get_pgd	= allwinner_get_pgd,
	.configure	= NULL,
	.shift_in 	= NULL,
	.shift_out	= NULL,
};

uint8_t
allwinner_backend(void)
{
	p.io = &allwinner_ops;

	return p.io->type;
}

int
allwinner_open(void)
{
	/* Setup device */
	if ((strcasecmp(p.device, "bpi") == 0) || (strcasecmp(p.device, "opi") == 0)) {
		aw.npins = AW_A20_NPINS;
		aw.tx = BOPI_TX;
		aw.rx = BOPI_RX;
		aw.sel = AW_PX_SELECT4;
	}
	else if (strcasecmp(p.device, "opi0") == 0) {
		aw.npins = AW_H3_NPINS;
		aw.tx = OPI0_TX;
		aw.rx = OPI0_RX;
		aw.sel = AW_PX_SELECT2;
	}
	else if (mystrcasestr(p.device, "opi") == p.device) {
		aw.npins = AW_H3_NPINS;
		aw.tx = OPIX_TX;
		aw.rx = OPIX_RX;
		aw.sel = AW_PX_SELECT3;
	}
	else
		return -1;

	/* Open /dev/mem */
	gpio_mem = open("/dev/mem", O_RDWR | O_SYNC);
	if (gpio_mem < 0) {
		printf("%s: warning: open failed [%s]\n", __func__, strerror(errno));
		gpio_mem = -1;
		return -1;
	}

	/* Memory map GPIO */
	gpio_map = mmap(NULL, AW_MAP_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, gpio_mem, AW_BASE_ADDR);
	if (gpio_map == MAP_FAILED) {
		printf("%s: warning: mmap failed [%s]\n", __func__, strerror(errno));
		close(gpio_mem);
		gpio_mem = -1;
		return -1;
	}

	/* Reset used pin flags */
	memset(&gpio_pins, 0, aw.npins);

	/* Initialise CFG / DAT / PULL offset tables */
	for (uint16_t i = 0; i < aw.npins; ++i) {
		uint16_t ofs;

		ofs = AW_BASE_OFS1 + (i >> 5) * 36 + ((i & 0x18) >> 1);
		gpio_cfg[i] = (ofs >> 2);

		ofs = AW_BASE_OFS1 + (i >> 5) * 36 + 16;
		gpio_dat[i] = (ofs >> 2);

		ofs = AW_BASE_OFS1 + (i >> 5) * 36 + ((i & 0x10) >> 2) + 28;
		gpio_pull[i] = (ofs >> 2);
	}

	return 0;
}

void
allwinner_release(void)
{
	if (p.bitrules & PGD_RELEASE)
		allwinner_release_pin(p.pgdo);
	if (p.bitrules & PGC_RELEASE)
		allwinner_release_pin(p.pgc);
	if (p.bitrules & PGM_RELEASE && p.pgm != GPIO_DISABLED)
		allwinner_release_pin(p.pgm);
	if (p.bitrules & VPP_RELEASE)
		allwinner_release_pin(p.vpp);
}

void
allwinner_close(void)
{
	if (gpio_map) {
		if (munmap(gpio_map, AW_MAP_LEN)) {
			printf("%s: warning: munmap failed\n", __func__);
		}
		gpio_map = NULL;
	}
	if (gpio_mem >= 0) {
		if (close(gpio_mem)) {
			printf("%s: warning: close failed\n", __func__);
		}
		gpio_mem = -1;
	}
}

char *
allwinner_error(void)
{
	return "Can't open AllWinner I/O";
}

void
allwinner_usleep(int n)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + gpio_dat[aw.rx];
	uint32_t val;

	while (n--)
		val = *reg;

	(void)val;
}

void
allwinner_set_pgm(uint8_t pgm)
{
	if (p.pgm != GPIO_DISABLED)
		allwinner_set(p.pgm, pgm);
}

void
allwinner_set_vpp(uint8_t vpp)
{
	allwinner_set(p.vpp, vpp);
}

void
allwinner_set_pgd(uint8_t pgd)
{
	allwinner_set(p.pgdo, pgd);
}

void
allwinner_set_pgc(uint8_t pgc)
{
	allwinner_set(p.pgc, pgc);
}

uint8_t
allwinner_get_pgd(void)
{
	uint8_t pgd;

	allwinner_get(p.pgdi, &pgd);

	return pgd;
}

static inline void
allwinner_read(uint32_t gpio_reg, uint32_t *val)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + gpio_reg;

	*val = *reg;
}

static inline void
allwinner_write(uint32_t gpio_reg, uint32_t val)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + gpio_reg;

	*reg = val;
}

/*
 * Determine PULL bit shift for GPIO
 */
static inline uint32_t
allwinner_pshift(uint16_t pin)
{
	return (pin & 0x0F) << 1;
}

static inline void
allwinner_pud(uint16_t pin, uint8_t pud)
{
	uint32_t val;

	allwinner_read(gpio_pull[pin], &val);

	val &= ~(3UL << allwinner_pshift(pin));
	val |= (uint32_t)pud << allwinner_pshift(pin);

	allwinner_write(gpio_pull[pin], val);
}

/*
 * Determine CFG SEL bit shift for GPIO
 */
static inline uint32_t
allwinner_lshift(uint16_t pin)
{
	return (pin & 0x07) << 2;
}

/*
 * Function select = Input
 */
static inline void
allwinner_select_input(uint16_t pin)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + gpio_cfg[pin];

	uint32_t val = ~(0x7 << allwinner_lshift(pin));
	*reg &= val; /* X000 = Input */

	gpio_dirs[pin] = 1;
}

/*
 * Function select = Output
 *
 * Call after pin set to input.
 */
static inline void
allwinner_select_output(uint16_t pin)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + gpio_cfg[pin];
	
	uint32_t val = 0x1 << allwinner_lshift(pin);
	*reg |= val; /* X001 = Output */

	gpio_dirs[pin] = 0;
}

/*
 * Function select = Input (no change), Output or MULTIPLEX FUNCTION SELECT
 *
 * Call after pin set to input.
 */
static inline void
allwinner_select_alt(uint16_t pin, uint8_t alt)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + gpio_cfg[pin];

	uint32_t val = alt << allwinner_lshift(pin);

	*reg |= val; /* MULTIPLEX FUNCTION SELECT */
}

/*
 * Determine DAT bit for GPIO
 */
static inline uint32_t
allwinner_bit(uint16_t pin)
{
	return (1UL << (pin & 31));
}

/*
 * Prepare pin for use.
 *
 *  1. Enable pull-up.
 *  2. Set to input.
 */
static inline void
allwinner_select_pin(uint8_t pin)
{
	allwinner_select_input(pin);		/* Input */

	allwinner_pud(pin, AW_PX_PULL_UP);	/* Pull-up on */

	gpio_pins[pin] = 1;			/* Pin now in use */
}

int
allwinner_get(uint16_t pin, uint8_t *level)
{
	if (pin >= aw.npins)
		return -1;

	if (gpio_pins[pin] == 0) {
		allwinner_select_pin(pin);
	}
	else if (gpio_dirs[pin] == 0) {
		allwinner_select_input(pin);
	}

	uint32_t val;

	allwinner_read(gpio_dat[pin], &val);

	*level = (val & allwinner_bit(pin)) ? (HIGH) : (LOW);

	return 0;
}

int
allwinner_set(uint16_t pin, uint8_t level)
{
	if (pin >= aw.npins)
		return -1;

	if (gpio_pins[pin] == 0) {
		allwinner_select_pin(pin);
		allwinner_select_output(pin);
	}
	else if (gpio_dirs[pin] == 1) {
		allwinner_select_output(pin);
	}

	uint32_t val;

	allwinner_read(gpio_dat[pin], &val);

	if (!level)
		val &= ~allwinner_bit(pin);
	else
		val |= allwinner_bit(pin);

	allwinner_write(gpio_dat[pin], val);

	return 0;
}

/*
 * Release and reset pin.
 */
int
allwinner_release_pin(uint16_t pin)
{
	if (pin >= aw.npins || gpio_pins[pin] == 0)
		return -1;

	allwinner_pud(pin, AW_PX_PULL_DIS); 	/* Pull-up off */

	allwinner_select_input(pin);		/* Input */

	if ((p.bitrules & ALT_RELEASE) != 0) {
		if (pin == aw.tx || pin == aw.rx) {
			/* Restore MULTIPLEX FUNCTION SELECT */
			allwinner_select_alt(pin, aw.sel);
		}
	}

	gpio_pins[pin] = 0;			/* Pin no longer in use */

	return 0;
}
