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

#include "pickle.h"

/******************************************************************************
 *
 * Session
 *
 *****************************************************************************/

extern struct pickle p;

/******************************************************************************
 *
 * UTILITY FUNCTIONS
 *
 *****************************************************************************/

/*
 * return the value of a hex nibble
 */
static uint8_t
inhx32_hex2nibble(const char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0;
}

/*
 * return the value of a hex byte
 */
static uint8_t
inhx32_hex2byte(const char *s)
{
	if (s[0] && s[1])
		return inhx32_hex2nibble(s[0]) << 4 | inhx32_hex2nibble(s[1]);

	return 0;
}

/*
 * return string with CRLF removed from end
 */
static char *
inhx32_trim(char *s, int slen)
{
	int l;

	if (!slen) {
		printf("%s: fatal error: invalid length\n", __func__);
		io_exit(EX_SOFTWARE); /* Panic */
	}

	s[slen - 1] = '\0';

	l = strlen(s) - 1;

	while (l >= 0 && (s[l]=='\r' || s[l]=='\n'))
		s[l--]= '\0';

	return s;
}

/******************************************************************************
 *
 * Intel hex 32 parser
 *
 *  Two algorithms exist for parsing input. Quick sort or tree sort.
 *
 *  Tree sort is the original algorithm and quick sort a new addition that
 *  was added 2014-10-19 to support old BSD without tree sort.
 *
 *  Selection of which algorithm to use is done in make file config
 *
 *****************************************************************************/

/*
 * Get line
 *
 * Reads input line and updates the byte count and address fields.
 *
 * Reads input line and updates the record type, byte count and address fields.
 *
 * Return codes:
 *   1 OKAY
 *   0 EOF
 *  -1 Invalid line prefix
 *  -2 Invalid line length
 *  -3 Invalid checksum
 *  -4 Invalid byte length
 *  -5 Invalid data record length (0)
 *  -6 Invalid data alignment for this architecture
 *  -7 Invalid extended address record
 *  -8 Invalid record type for this architecture
 */
static int
inhx32_fgets(char *line, FILE *fp,
	uint8_t *bb, uint16_t *aaaa, uint8_t *tt, uint16_t *extended_addr)
{
	size_t len;
	uint8_t cc;
	uint16_t i;

	*bb = 0;
	*aaaa = 0;
	*tt = ~TT_DATA; /* Not a data record (255) */

	/* Get line */
	if (fgets(line, STRLEN, fp) == NULL)
		return 0; /* EOF */

	/* Validate line prefix */
	if (line[0] != ':')
		return -1; /* Invalid line prefix */

	/* Remove CRLF */
	inhx32_trim(line, STRLEN);

	/* Validate line length */
	len = strlen(line);
	if ((len & 1) == 0 || len < 11)
		return -2; /* Invalid line length */

	/* Validate checksum */
	cc = 0;
	for (i = 1; line[i]; i += 2)
		cc += inhx32_hex2byte(&line[i]);
	if (cc != 0)
		return -3; /* Invalid checksum */

	/* Determine number of bytes in this line */
	*bb = inhx32_hex2byte(&line[BB]);

	/* Validate byte length */
	if (len != (2 * *bb + 11))
		return -4; /* Invalid byte length */

	/* Determine address for this line */
	*aaaa = (inhx32_hex2byte(&line[AAAA]) << 8) |
		inhx32_hex2byte(&line[AAAA + 2]);

	/* Determine record type */
	*tt = inhx32_hex2byte(&line[TT]);

	/* Process a data record */
	if (*tt == TT_DATA) {
		/* Care about *bb but don't care about *aaaa */

		/* Validate line length */
		if (*bb == 0)
			return -5; /* Invalid data record length */

		/* Validate alignment */
		if (p.pic && (*bb % p.pic->align))
			return -6; /* Invalid data record alignment for this architecture */

		/* Valid data record */
		return 1;
	}

	/* Process an extended address record */
	if (*tt == TT_EXTENDED_LINEAR_ADDRESS) {
		/* Care about *bb and *aaaa */

		/* Validate extended address */
		if (*aaaa != 0 || *bb != 2)
			return -7; /* Invalid address record */

		/* Determine extended address */
		*extended_addr = (inhx32_hex2byte(&line[HHHH]) << 8) |
				inhx32_hex2byte(&line[HHHH + 2]);

		/* Valid address record */
		return 1;
	}

	/* Process an EOF record */
	if (*tt == TT_EOF) {
		/* Don't care about *bb or *aaaa */

		/* Valid EOF record */
		return 1;
	}


	return -8; /* Invalid record type for this architecture */
}

/*
 * Display input line warning or error message
 *
 * Return true if escalated to an error
 */
static int
inhx32_error(int err, int val)
{
	static char warning[] = "warning", error[] = "error";
	char *msg;
	int rc = 0;

	if (err >= 0)
		return rc;
	
	if (p.error & BADINPUT) {
		rc = 1;		/* Error escalation */
		msg = error;
	} else {
		msg = warning;
	}
	if (p.f) switch (err) {
	case -1:fprintf(p.f, "%s: %s: invalid line prefix\n", __func__, msg);
		break;
	case -2:fprintf(p.f, "%s: %s: invalid line length\n", __func__, msg);
		break;
	case -3:fprintf(p.f, "%s: %s: invalid checksum\n", __func__, msg);
		break;
	case -4:fprintf(p.f, "%s: %s: invalid byte length\n", __func__, msg);
		break;
	case -5:fprintf(p.f, "%s: %s: invalid data record length\n", __func__, msg);
		break;
	case -6:fprintf(p.f, "%s: %s: invalid data record alignment\n", __func__, msg);
		break;
	case -7:fprintf(p.f, "%s: %s: invalid address record\n", __func__, msg);
		break;
	case -8:fprintf(p.f, "%s: %s: invalid record type [0x%02X]\n", __func__, msg, val);
		break;
	}

	return rc;
}

/*
 * Get data
 *
 *  Returns number of bytes or 0 for EOF or -1 for error escalation
 */
int
inhx32_get(FILE *fp, pic_data **data)
{
	char line[STRLEN];
	uint8_t bb, tt;
	uint16_t aaaa, ix, i;
	static uint16_t extended_addr = 0;
	int rc;

	do {
		/* Get line */
		rc = inhx32_fgets(line, fp, &bb, &aaaa, &tt, &extended_addr);
		if (rc == 0 || tt == TT_EOF)
			return 0;	/* EOF */
		if (inhx32_error(rc, tt))
			return -1;	/* Error */
	}
	while (tt != TT_DATA);		/* Skip non data records */

	/* Allocate data */
	*data = (pic_data *)calloc(1, sizeof(pic_data));
	if (*data == NULL) {
		printf("%s: fatal error: calloc failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}

	/* Store address and number of bytes */
	(*data)->address = (extended_addr << 16) | aaaa;
	(*data)->nbytes = bb;

	/* Extract and store bytes */
	ix = HHHH;
	for (i = 0; i < bb; ++i) {
		(*data)->bytes[i] = inhx32_hex2byte(&line[ix]);
		ix += 2;
	}

	/* Return number of bytes */
	return bb;
}

/******************************************************************************
 *
 * Qsort allocator
 *
 *****************************************************************************/
#ifdef QSORT
/*
 * Stage 1: Input & Rewind
 *
 *  Read and copy `stdin' to a temporary file or read input file.
 *
 *  During read, determine the total number of hex data bytes and
 *  data line count.
 *
 *  Rewind input.
 *
 * Stage 2: Input & Alloc
 *
 *  Allocate array of pointers to data records.
 *
 *  Re-read input populating data records array.
 *
 *  Quick sort data records array.
 */

/*
 * Stage 1: Input & Rewind
 *
 *  filename should either be `-' for `stdin' else an accessible
 *  disk file.
 *
 *  tempname contains the temporary file template on entry and any
 *  temporary file name on return.
 */
uint32_t
inhx32_qsort_input(FILE **fp, const char *filename, char *tempname, uint32_t *count)
{
	char line[STRLEN];
	uint8_t bb, tt;
	uint16_t dummy;

	FILE *ftemp = NULL;
	int rc;
	
	uint32_t nbytes = 0;
	*count = 0;

	/* Open file or stdin */
	if (strcmp(filename, "-") == 0) {
		*fp = stdin;
		rc = mkstemp(tempname);
		if (rc < 0) {
			tempname[0] = '\0';
			printf("%s: error: mkstemp failed [%s]\n",
				__func__, tempname);
			return 0; /* Empty */
		}
		ftemp = fdopen(rc, "rb+");
		if (ftemp == NULL) {
			close(rc);
			unlink(tempname);
			tempname[0] = '\0';
			printf("%s: error: fdopen failed [%s]\n",
				__func__, tempname);
			return 0; /* Empty */
		}
	} else {
		*fp = fopen(filename, "rb");
		if (*fp == NULL) {
			printf("%s: error: fopen failed [%s]\n",
				__func__, filename);
			return 0; /* Empty */
		}
		tempname[0] = '\0';
	}

	/* While not EOF, count data bytes */
	while (1) {
		/* Get line */
		rc = inhx32_fgets(line, *fp, &bb, &dummy, &tt, &dummy);
		if (rc == 0 || tt == TT_EOF)
			break;	/* EOF */
		if (tt == TT_DATA) {
			nbytes += bb;
			(*count)++;
		}
		if (ftemp)
			fprintf(ftemp, "%s\n", line);
	}

	/* Rewind input */
	if (ftemp)
		*fp = ftemp;
	if (fseek(*fp, 0L, SEEK_SET) < 0) {
		fclose(*fp);
		if (tempname[0]) {
			unlink(tempname);
			tempname[0] = '\0';
		}
		printf("%s: error: fseek failed [%s] [%s]\n",
			__func__, filename, tempname);
		return 0; /* Empty */
	}

	/* Information */
	if (nbytes == 0) {
		fclose(*fp);
		printf("%s: error: no valid input data\n", __func__);
		if (tempname[0]) {
			unlink(tempname);
			tempname[0] = '\0';
		}
	}

	/* Return number of bytes */
	return nbytes;
}

/******************************************************************************
 *
 * Qsort array allocator
 *
 *****************************************************************************/

/*
 * Compare entries
 */
int
inhx32_array_compare(const void *d1, const void *d2)
{
	pic_data *data1 = *(pic_data **)d1;
	pic_data *data2 = *(pic_data **)d2;

	if (data1->address < data2->address)
		return -1;
	else if (data1->address == data2->address)
		return 0;
	return 1;
}

/*
 * Allocate array
 */
uint32_t
inhx32_array_create(pic_data ***pdata, const char *filename, uint32_t *count)
{
	FILE *fp;
	char tempname[] = "/tmp/inhx32.XXXXXX";
	uint32_t nbytes, index = 0;
	pic_data *data;
	int rc;

	*count = 0;
	
	/*
	 * Stage 1.
	 */

	/* Determine number of bytes and line count */
	nbytes = inhx32_qsort_input(&fp, filename, tempname, count);
	if (nbytes == 0)
		return 0; /* Empty */

	/*
	 * Stage 2.
	 */

	/* Allocate array */
	*pdata = (pic_data **)calloc(*count, sizeof(pic_data *));
	if (*pdata == NULL) {
		printf("%s: fatal error: calloc failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}

	/* While not EOF or error, process data */
	while ((rc = inhx32_get(fp, &data)) > 0) {
		(*pdata)[index++] = data;
	}
	fclose(fp);

	/* Remove any temporary file created in stage 1 */
	if (tempname[0])
		unlink(tempname);

	/* Exit on error condition */
	if (rc < 0)
		io_exit(EX_BADINPUT);

	/* Quick sort array */
	qsort(*pdata, *count, sizeof(pic_data *), inhx32_array_compare);

	/* Return number of bytes */
	return nbytes;
}

/*
 * Free array
 */
void
inhx32_array_free(pic_data **pdata, uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i) {
		free(pdata[i]);
	}
	if (count) {
		free(pdata);
	}
}

/******************************************************************************
 *
 * Qsort memory allocator (PIC32 PE)
 *
 *****************************************************************************/
#ifdef P32
/*
 * Allocate memory
 */
uint32_t
inhx32_memory_create(uint8_t **pmem, const char *filename)
{
	FILE *fp;
	char tempname[] = "/tmp/inhx32.XXXXXX";
	uint32_t nbytes, count = 0, index = 0;
	pic_data *data;
	int rc;
	uint8_t i;

	/*
	 * Stage 1.
	 */

	/* Determine number of bytes and line count */
	nbytes = inhx32_qsort_input(&fp, filename, tempname, &count);
	if (nbytes == 0)
		return 0; /* Empty */

	/*
	 * Stage 2.
	 */

	/* Allocate memory */
	*pmem = (uint8_t *)calloc(nbytes, sizeof(uint8_t));
	if (*pmem == NULL) {
		printf("%s: fatal error: calloc failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}

	/* While not EOF or error, process data */
	while ((rc = inhx32_get(fp, &data)) > 0) {
		for (i = 0; i < data->nbytes; ++i)
			(*pmem)[index++] = data->bytes[i];
	}
	fclose(fp);

	/* Remove any temporary file created in stage 1 */
	if (tempname[0])
		unlink(tempname);

	/* Exit on error condition */
	if (rc < 0)
		io_exit(EX_BADINPUT);

	/* Return number of bytes */
	return nbytes;
}
#endif /* P32   */
#endif /* QSORT */
/******************************************************************************
 *
 * Tree allocator
 *
 *****************************************************************************/
#ifndef QSORT
/*
 * Stage 1: Input
 *
 *  Read input file creating binary tree.
 *
 *  During read, determine the total number of hex data bytes and
 *  data line count.
 *
 *  Close input.
 *
 * Stage 2: Alloc
 *
 *  Allocate array of pointers to data records.
 *
 *  Walk tree populating data records array.
 */

/*
 * Compare entries
 */
int
inhx32_tree_compare(const void *d1, const void *d2)
{
	pic_data *data1 = (pic_data *)d1;
	pic_data *data2 = (pic_data *)d2;

	if (data1->address < data2->address)
		return -1;
	else if (data1->address == data2->address)
		return 0;
	return 1;
}

/*
 * Stage 1: Input
 *
 *  filename should either be `-' for `stdin' else an accessible
 *  disk file.
 */
uint32_t
inhx32_tree_input(void **root, const char *filename, uint32_t *count)
{
	FILE *fp = stdin;
	uint32_t nbytes = 0;
	void *entry;
	pic_data *data;
	int rc;

	*count = 0;

	/* Open file or stdin */
	if (strcmp(filename, "-") && (fp = fopen(filename, "rb")) == NULL) {
		printf("%s: error: fopen failed [%s] [%s]\n",
			__func__, filename, strerror(errno));
		return 0; /* Empty */
	}

	/* While not EOF or error, process data */
	while ((rc = inhx32_get(fp, &data)) > 0) {
		/* Detect duplicate data in tree */
		entry = tfind((void *)data, root, inhx32_tree_compare);
		if (entry != NULL) {
			printf("%s: fatal error: duplicate address [%08X]\n",
				__func__, data->address);
			io_exit(EX_SOFTWARE); /* Panic */
		}

		/* Add new data to tree */
		entry = tsearch((void *)data, root, inhx32_tree_compare);
		if (entry == NULL) {
			printf("%s: fatal error: tsearch failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		}

		/* Update counters */
		(*count)++;
		nbytes += data->nbytes;
	}

	/* Close file or stdin */
	if (strcmp(filename, "-"))
		fclose(fp);

	/* Exit on error condition */
	if (rc < 0)
		io_exit(BADINPUT);

	/* Information */
	if (nbytes == 0)
		printf("%s: error: no valid input data\n", __func__);

	/* Return number of bytes */
	return nbytes;
}

/*
 * Match entries
 */
int
inhx32_tree_match(const void *d1, const void *d2)
{
	return 0;
}

/*
 * Free tree (entries and/or data)
 */
void
inhx32_tree_free(void **root, int flag)
{
	void *entry;
	pic_data *data;

	while ((entry = tfind(NULL, root, inhx32_tree_match))) {
		data = *(pic_data **)entry;
		tdelete(data, root, inhx32_tree_match);
		if (flag)
			free(data);
	}
	if (*root != NULL) {
		printf("%s: fatal error: tree free failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}
}

/******************************************************************************
 *
 * Tree array allocator
 *
 *****************************************************************************/

/*
 * `twalk' array
 */
static pic_data **inhx32_pdata;

/*
 * `twalk' index
 */
static uint32_t inhx32_index;

/*
 * Walk array
 */
void
inhx32_array_walk(const void *entry, VISIT order, int level)
{
	pic_data *data = *(pic_data **)entry;

	if (order == postorder || order == leaf) {
		inhx32_pdata[inhx32_index++] = data;
	}
}

/*
 * Allocate array
 */
uint32_t
inhx32_array_create(pic_data ***pdata, const char *filename, uint32_t *count)
{
	uint32_t nbytes;
	void *root = NULL;

	/*
	 * Stage 1.
	 */

	/* Create tree */
	nbytes = inhx32_tree_input(&root, filename, count);
	if (nbytes == 0)
		return 0; /* Empty */

	/*
	 * Stage 2.
	 */

	/* Allocate array */
	*pdata = (pic_data **)calloc(*count, sizeof(pic_data *));
	if (*pdata == NULL) {
		printf("%s: fatal error: calloc failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}

	/* Populate array */
	inhx32_pdata = *pdata;
	inhx32_index = 0;
	twalk(root, inhx32_array_walk);

	/* Free tree (entries, not data) */
	inhx32_tree_free(&root, 0);

	/* Return number of bytes */
	return nbytes;
}
		
/*
 * Free array
 */
void
inhx32_array_free(pic_data **pdata, uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i) {
		free(pdata[i]);
	}
	if (count) {
		free(pdata);
	}
}

/******************************************************************************
 *
 * Tree memory allocator (PIC32 PE)
 *
 *****************************************************************************/
#ifdef P32
/*
 * `twalk' memory
 */
static uint8_t *inhx32_pmem;

/*
 * Walk memory
 */
void
inhx32_memory_walk(const void *entry, VISIT order, int level)
{
	pic_data *data = *(pic_data **)entry;
	uint8_t i;

	if (order == postorder || order == leaf) {
		for (i = 0; i < data->nbytes; ++i)
			inhx32_pmem[inhx32_index++] = data->bytes[i];
	}
}

/*
 * Allocate memory
 */
uint32_t
inhx32_memory_create(uint8_t **pmem, const char *filename)
{
	uint32_t nbytes, count = 0;
	void *root = NULL;

	/*
	 * Stage 1.
	 */

	/* Create tree */
	nbytes = inhx32_tree_input(&root, filename, &count);
	if (nbytes == 0)
		return 0; /* Empty */

	/*
	 * Stage 2.
	 */

	/* Allocate memory */
	*pmem = (uint8_t *)calloc(nbytes, sizeof(uint8_t));
	if (*pmem == NULL) {
		printf("%s: fatal error: calloc failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}

	/* Populate memory */
	inhx32_pmem = *pmem;
	inhx32_index = 0;
	twalk(root, inhx32_memory_walk);

	/* Free tree (entries and data) */
	inhx32_tree_free(&root, 1);

	/* Return number of bytes */
	return nbytes;
}
#endif /* P32   */
#endif /* !QSORT */
