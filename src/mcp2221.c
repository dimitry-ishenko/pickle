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

static int fd = -1;		/* File descriptor */

/* Shadow I/O */
static uint8_t latch = 0xF, port = 0xF, tris;

/*******************************************************************************
 *
 * I/O operations
 *
 ******************************************************************************/

struct io_ops mcp2221_ops = {
	.type		= IOMCP2221,
	.run		= 1,
	.open		= mcp2221_open,
	.release	= NULL,
	.close		= mcp2221_close,
	.error		= mcp2221_error,
	.usleep		= NULL,
	.set_pgm	= mcp2221_set_pgm,
	.set_vpp	= mcp2221_set_vpp,
	.set_pgd	= mcp2221_set_pgd,
	.set_pgc	= mcp2221_set_pgc,
	.get_pgd	= mcp2221_get_pgd,
	.configure	= NULL,
	.shift_in 	= NULL,
	.shift_out	= NULL,
};

uint8_t
mcp2221_backend(void)
{
	p.io = &mcp2221_ops;

	return p.io->type;
}

int
mcp2221_open(void)
{
	int rc;

	rc = mcp2221_open_hid();
	if (rc < 0) {
		return -1;
	}

	/* Configure I/O */
	tris = (1 << p.pgdi);
	rc = mcp2221_cmd_set_sram_settings();
	if (rc < 0) {
		return -1;
	}

	return 0;
}

void
mcp2221_close(void)
{
	/* Always release I/O */
	tris = 0xF;
	mcp2221_cmd_set_sram_settings();

	close(fd);
	fd = -1;
}

char *
mcp2221_error(void)
{
	return "Can't open MCP2221 I/O";
}

void
mcp2221_set_pgm(uint8_t pgm)
{
	if (p.pgm != GPIO_DISABLED) {
		if (pgm)
			latch |= (1 << p.pgm);
		else
			latch &= ~(1 << p.pgm);

		mcp2221_cmd_set_gpio_output_values();
	}
}

void
mcp2221_set_vpp(uint8_t vpp)
{
	if (vpp)
		latch |= (1 << p.vpp);
	else
		latch &= ~(1 << p.vpp);

	mcp2221_cmd_set_gpio_output_values();
}

void
mcp2221_set_pgd(uint8_t pgd)
{
	if (pgd)
		latch |= (1 << p.pgdo);
	else
		latch &= ~(1 << p.pgdo);

	mcp2221_cmd_set_gpio_output_values();
}

void
mcp2221_set_pgc(uint8_t pgc)
{
	if (pgc)
		latch |= (1 << p.pgc);
	else
		latch &= ~(1 << p.pgc);

	mcp2221_cmd_set_gpio_output_values();
}

uint8_t
mcp2221_get_pgd(void)
{
	mcp2221_cmd_get_gpio_values();

	if (port & (1 << p.pgdi))
		return HIGH;

	return LOW;
}

int
mcp2221_open_hid(void)
{
	char str[STRLEN];
	struct stat st;
	int rc, i, f;
	struct hiddev_devinfo devinfo;

	for (i = 0; i <= HIDMAX; ++i) {
		rc = snprintf(str, STRLEN, HIDDEV, i);
		if (rc < 0) {
			printf("%s: fatal error: snprintf failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		} else if (rc >= STRLEN) {
			printf("%s: fatal error: snprintf overrun\n", __func__);
			io_exit(EX_SOFTWARE); /* Panic */
		}
		rc = stat(str, &st);
		if (rc < 0)
			continue;
		fd = open(str, O_RDONLY | O_NONBLOCK);
		if (fd < 0) {
			continue;
		}
		rc = ioctl(fd, HIDIOCGDEVINFO, &devinfo);
		if (rc < 0) {
			close(fd);
			continue;
		}
		if (devinfo.vendor != VENDOR || devinfo.product != PRODUCT) {
			close(fd);
			continue;
		}
		f = HIDDEV_FLAG_UREF | HIDDEV_FLAG_REPORT;
		rc = ioctl(fd, HIDIOCSFLAG, &f);
		if (rc < 0) {
			close(fd);
			continue;
		}

		return fd;
	}

	return -1;
}

int
mcp2221_out(__s32 values[], int length)
{
	int rc;
	struct hiddev_report_info info_out;
	struct hiddev_usage_ref_multi multi_out = {.uref = {0}};

	multi_out.uref.report_type = HID_REPORT_TYPE_OUTPUT;
	multi_out.uref.report_id   = HID_REPORT_ID_FIRST;
	multi_out.num_values       = length;
	memcpy(multi_out.values, values, length * sizeof(__s32));
	rc = ioctl(fd, HIDIOCSUSAGES, &multi_out);
	if (rc < 0)
		return -10;

	info_out.report_type = HID_REPORT_TYPE_OUTPUT;
	info_out.report_id   = HID_REPORT_ID_FIRST;
	info_out.num_fields  = 1;
	rc = ioctl(fd, HIDIOCSREPORT, &info_out);
	if (rc < 0)
		return -11;

	return 0;
}

int
mcp2221_poll(void)
{
	int rc;
	struct pollfd pfd;
	struct hiddev_usage_ref ref;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	do {
		rc = poll(&pfd, 1, 1000);
		if (rc < 0)
			return -12; /* POLL ERROR */
		if (rc == 0)
			return -13; /* TIMED OUT */
		if (rc != 1)
			return -14; /* UNEXPECTED ERROR */
		if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
			return -15; /* I/O ERROR */

		rc = read(fd, &ref, sizeof(struct hiddev_usage_ref));
		if (rc != sizeof(struct hiddev_usage_ref))
			return -16; /* READ ERROR */
	}
	while (ref.field_index != HID_FIELD_INDEX_NONE);

	return 0;
}

int
mcp2221_in(__s32 values[], int length)
{
	int rc;
	struct hiddev_report_info info_in;
	struct hiddev_usage_ref_multi multi_in = {.uref = {0}};

	info_in.report_type = HID_REPORT_TYPE_INPUT;
	info_in.report_id   = HID_REPORT_ID_FIRST;
	info_in.num_fields  = 1;
	rc = ioctl(fd, HIDIOCGREPORT, &info_in);
	if (rc < 0)
		return -17;

	multi_in.uref.report_type = HID_REPORT_TYPE_INPUT;
	multi_in.uref.report_id   = HID_REPORT_ID_FIRST;
	multi_in.num_values       = length;
	rc = ioctl(fd, HIDIOCGUSAGES, &multi_in);
	if (rc < 0)
		return -18;

	memcpy(values, multi_in.values, length * sizeof(__s32));

	return 0;
}

int
mcp2221_cmd(int cmd, __s32 values[], int length)
{
	int rc;

	values[0] = cmd;
	rc = mcp2221_out(values, length);
	if (rc < 0)
		return rc;

	rc = mcp2221_poll();
	if (rc < 0)
		return rc;

	rc = mcp2221_in(values, length);
	if (rc < 0)
		return rc;

	if (values[0] != cmd)
		return -19;

	if (values[1] != /* SUCCESS */ 0)
		return -20;

	return 0;
}

int
mcp2221_cmd_set_sram_settings(void)
{
	__s32 values[LENGTH] = {0};
	int rc;

	values[7] = 0xFF; /* Alter GPIO configuration */

	uint8_t mask = 1;

	for (int i = 0; i < 4; ++i) {
		int j = i + 8;

		if (tris & mask) {	/* INPUT MODE */
			values[j] = 0x08;
		} else {		/* OUTPUT MODE */
			if (latch & mask)
				values[j] = 0x10;
			else
				values[j] = 0x00;
		}

		mask <<= 1;
	}

	rc = mcp2221_cmd(CMD_SET_SRAM_SETTINGS, values, LENGTH);
	if (rc < 0)
		return rc;

	return 0;
}

int
mcp2221_cmd_set_gpio_output_values(void)
{
	__s32 values[LENGTH] = {0};
	int rc;

	uint8_t mask = 1;

	for (int i = 0; i < 4; ++i) {
		int j = i * 4 + 2;

		/* Alter GP output */
		values[j + 0] = 0xFF;
		values[j + 1] = (latch & mask) != 0;

		/* Alter GP pin direction */
		values[j + 2] = 0xFF;
		values[j + 3] = (tris & mask) != 0;

		mask <<= 1;
	}

	rc = mcp2221_cmd(CMD_SET_GPIO_OUTPUT_VALUES, values, LENGTH);
	if (rc < 0)
		return rc;

	return 0;
}

int
mcp2221_cmd_get_gpio_values(void)
{
	__s32 values[LENGTH] = {0};
	int rc;

	rc = mcp2221_cmd(CMD_GET_GPIO_VALUES, values, LENGTH);
	if (rc < 0)
		return rc;

	uint8_t mask = 1;

	port = latch;

	for (int i = 0; i < 4; ++i) {
		int j = i * 2 + 2;

		if (values[j] == 0xEE)		/* NOT A GPIO */
			goto next;
		if (values[j + 1] == 0xEF)	/* NOT A GPIO */
			goto next;
		if (values[j + 1] == 0)		/* OUTPUT */
			goto next;
		if (values[j]) {
			port |= mask;		/* HIGH */
		} else {
			port &= ~mask;		/* LOW */
		}
next:
		mask <<= 1;
	}

	return 0;
}
