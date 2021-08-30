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

#undef DEBUG

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

/* I/O Pins 0..31 on headers P1 & P5 */
static uint8_t gpio_pins[GPIO_RPI_NPINS], gpio_dirs[GPIO_RPI_NPINS], gpio_alt[GPIO_RPI_NPINS];

/*******************************************************************************
 *
 * I/O operations
 *
 ******************************************************************************/

struct io_ops raspi_ops = {
	.type		= IORPI,
	.run		= 1,
	.open		= raspi_open,
	.release	= raspi_release,
	.close		= raspi_close,
	.error		= raspi_error,
	.usleep		= raspi_usleep,
	.set_pgm	= raspi_set_pgm,
	.set_vpp	= raspi_set_vpp,
	.set_pgd	= raspi_set_pgd,
	.set_pgc	= raspi_set_pgc,
	.get_pgd	= raspi_get_pgd,
	.configure	= NULL,
	.shift_in	= NULL,
	.shift_out	= NULL,
};

uint8_t
raspi_backend(void)
{
	p.io = &raspi_ops;

	return p.io->type;
}

int
raspi_open(void)
{
	off_t gpio_base_addr;
	uint8_t type = p.device[3];

	/* Determine GPIO base address */
	if (type == '\0' || type == '0' || type == '1') {
		gpio_base_addr = BCM2835_PERI_BASE_ADDR + GPIO_BASE_ADDR_OFFSET;
	}
	else if (type == '2' || type == '3') {
		gpio_base_addr = BCM2836_PERI_BASE_ADDR + GPIO_BASE_ADDR_OFFSET;
	}
	else if (type == '4') {
		gpio_base_addr = BCM2711_PERI_BASE_ADDR + GPIO_BASE_ADDR_OFFSET;
	}
	else {
		return -1; /* Unknown */
	}

	/* Open /dev/mem */
	gpio_mem = open("/dev/mem", O_RDWR | O_SYNC);
	if (gpio_mem < 0) {
		printf("%s: warning: open failed [%s]\n", __func__, strerror(errno));
		gpio_mem = -1;
		return -1;
	}

	/* Memory map GPIO */
	gpio_map = mmap(NULL, GPIO_MAP_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, gpio_mem, gpio_base_addr);
	if (gpio_map == MAP_FAILED) {
		printf("%s: warning: mmap failed [%s]\n", __func__, strerror(errno));
		close(gpio_mem);
		gpio_mem = -1;
		return -1;
	}

	/* Reset used pin flags */
	memset(&gpio_pins, 0, GPIO_RPI_NPINS);

	return 0;
}

void
raspi_release(void)
{
	if (p.bitrules & PGD_RELEASE)
		raspi_release_pin(p.pgdo);
	if (p.bitrules & PGC_RELEASE)
		raspi_release_pin(p.pgc);
	if (p.bitrules & PGM_RELEASE && p.pgm != GPIO_DISABLED)
		raspi_release_pin(p.pgm);
	if (p.bitrules & VPP_RELEASE)
		raspi_release_pin(p.vpp);
}

void
raspi_close(void)
{
	if (gpio_map) {
		if (munmap(gpio_map, GPIO_MAP_LEN)) {
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
raspi_error(void)
{
	return "Can't open RPI I/O";
}

void
raspi_usleep(int n)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + GPLEV0;
	uint32_t val;

	while (n--)
		val = *reg;

	(void)val;
}

void
raspi_set_pgm(uint8_t pgm)
{
	if (p.pgm != GPIO_DISABLED)
		raspi_set(p.pgm, pgm);
}

void
raspi_set_vpp(uint8_t vpp)
{
	raspi_set(p.vpp, vpp);
}

void
raspi_set_pgd(uint8_t pgd)
{
	raspi_set(p.pgdo, pgd);
}

void
raspi_set_pgc(uint8_t pgc)
{
	raspi_set(p.pgc, pgc);
}

uint8_t
raspi_get_pgd(void)
{
	uint8_t pgd;

	raspi_get(p.pgdi, &pgd);

	return pgd;
}

static inline void
raspi_read(uint8_t gpio_reg, uint32_t *val)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + gpio_reg;

	*val = *reg;
}

static inline void
raspi_write(uint8_t gpio_reg, uint32_t val)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + gpio_reg;

	*reg = val;
}

static inline void
raspi_pud(uint8_t pin, uint8_t pud)
{
	raspi_write(GPPUD, pud);
	usleep(10); /* ? */
	raspi_write(GPPUDCLK0, (1 << pin));
	usleep(10); /* ? */
	raspi_write(GPPUD, 0);
	raspi_write(GPPUDCLK0, 0);
}

/*
 * Determine GPFSEL register offset for GPIO
 *
 *  GPIO00 .. GPIO09 GPFSEL0
 *  GPIO10 .. GPIO19 GPFSEL1
 *  GPIO20 .. GPIO29 GPFSEL2
 *  GPIO30 .. GPIO39 GPFSEL3
 *  GPIO40 .. GPIO49 GPFSEL4
 *  GPIO50 .. GPIO59 GPFSEL5
 *
 * BCM2835-ARM-Peripherals Page 92
 */
static inline uint32_t
raspi_gpfsel(uint8_t pin)
{
	return (pin / 10) /* + GPFSEL0 */;
}

/*
 * Determine GPFSEL bit shift for GPIO
 *
 *  GPIO0 0	GPIO5 15
 *  GPIO1 3	GPIO6 18
 *  GPIO2 6	GPIO7 21
 *  GPIO3 9	GPIO8 24
 *  GPIO4 12	GPIO9 27
 *
 * BCM2835-ARM-Peripherals Page 92
 */
static inline uint32_t
raspi_lshift(uint8_t pin)
{
	return (pin % 10) * 3;
}

/*
 * Function select = Input
 */
static inline void
raspi_select_input(uint8_t pin)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + raspi_gpfsel(pin);

	uint32_t val = ~(7 << raspi_lshift(pin));
	*reg &= val; /* 000 = Input */

	gpio_dirs[pin] = 1;
}

/*
 * Function select = Output
 *
 * Call after pin set to input.
 */
static inline void
raspi_select_output(uint8_t pin)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + raspi_gpfsel(pin);

	uint32_t val = 1 << raspi_lshift(pin);
	*reg |= val; /* 001 = Output */

	gpio_dirs[pin] = 0;
}

/*
 * Function select = Input (no change), Output or ALT0 .. ALT5
 *
 * Call after pin set to input.
 */
static inline void
raspi_select_alt(uint8_t pin)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + raspi_gpfsel(pin);

	uint32_t val = gpio_alt[pin] << raspi_lshift(pin);

	DPRINT("%s() PIN=%2d ALT=%d\n", __func__, pin, gpio_alt[pin]);

	*reg |= val; /* Input, Output or ALT0 .. ALT5 */
}

/*
 * Prepare pin for use.
 *
 *  1. Save previous configuration.
 *  2. Enable pull-up.
 *  3. Set to input.
 */
static inline void
raspi_select_pin(uint8_t pin)
{
	GPIO_ADDR reg = (GPIO_ADDR)(gpio_map) + raspi_gpfsel(pin);

	uint32_t val = 7 << raspi_lshift(pin);
	val = (*reg & val) >> raspi_lshift(pin);

	gpio_alt[pin] = val;		/* Save Input, Output or ALT0 .. ALT5 */

	DPRINT("%s() PIN=%2d ALT=%d\n", __func__, pin, gpio_alt[pin]);

	raspi_select_input(pin);	/* Input */

	raspi_pud(pin, GPPUD_UP);	/* Pull-up on */

	gpio_pins[pin] = 1;		/* Pin now in use */
}

/*
 * Read pin.
 */
int
raspi_get(uint16_t pin, uint8_t *level)
{
	if (pin >= GPIO_RPI_NPINS)
		return -1;

	if (gpio_pins[pin] == 0) {
		raspi_select_pin(pin);
	}
	else if (gpio_dirs[pin] == 0) {
		raspi_select_input(pin);
	}

	uint32_t val;

	raspi_read(GPLEV0, &val);

	*level = (val & (1 << pin)) ? (HIGH) : (LOW);

	return 0;
}

/*
 * Write pin.
 */
int
raspi_set(uint16_t pin, uint8_t level)
{
	if (pin >= GPIO_RPI_NPINS)
		return -1;

	if (gpio_pins[pin] == 0) {
		raspi_select_pin(pin);
		raspi_select_output(pin);
	}
	else if (gpio_dirs[pin] == 1) {
		raspi_select_output(pin);
	}

	raspi_write((level) ? (GPSET0) : (GPCLR0), (1 << pin));

	return 0;
}

/*
 * Release and reset pin.
 */
int
raspi_release_pin(uint16_t pin)
{
	if (pin >= GPIO_RPI_NPINS || gpio_pins[pin] == 0)
		return -1;

	raspi_pud(pin, GPPUD_OFF);	/* Pull-up off */

	raspi_select_input(pin);	/* Input */

	if ((p.bitrules & ALT_RELEASE) != 0) {
		/* Restore Input (no change), Output or ALT0 .. ALT5 */
		raspi_select_alt(pin);
	}

	gpio_pins[pin] = 0;		/* Pin no longer in use */

	return 0;
}
