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
 * UTILITY FUNCTIONS
 *****************************************************************************/

/*
 * strcasestr implementation for cygwin
 */
const char *
mystrcasestr(const char *haystack, const char *needle)
{
	int i, j;
	int nlen = strlen(needle);
	int hlen = strlen(haystack) - nlen;

	for (i = 0; i <= hlen; i++) {
		for (j = 0; j < nlen; j++) {
			if (tolower((int)haystack[i + j]) != tolower((int)needle[j]))
				break;
		}
		if (j == nlen) {
			return &haystack[i];
		}
	}
	return NULL;
}
