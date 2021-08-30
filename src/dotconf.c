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

/*
 * Session
 */
extern struct pickle p;

/*
 * Dot path
 */
int
getdotpath(void)
{
	struct stat st;
	int rc;

	if (stat(p.dotfile, &st) < 0) {
		if (errno == ENOENT)
			return -1;
		printf("%s: fatal error: stat failed [%s]\n", __func__, p.dotfile);
		io_exit(EX_SOFTWARE); /* Panic */
	}
	if (S_ISREG(st.st_mode)) {
		if (getuid() && getuid() != st.st_uid) {
			printf("%s: fatal error: invalid owner [%s]\n", __func__, p.dotfile);
			io_exit(EX_SOFTWARE); /* Panic */
		}
		return access(p.dotfile, R_OK);
	}

	if (S_ISDIR(st.st_mode)) {
		char *dotdup = (char *)strdup(p.dotfile);

		if (dotdup == NULL) {
			printf("%s: fatal error: strdup failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		}

		rc = snprintf(p.dotfile, STRLEN, "%s/%s", dotdup, DOTCONFIGNAME);
		if (rc < 0) {
			printf("%s: fatal error: snprintf failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		} else if (rc >= STRLEN) {
			printf("%s: fatal error: snprintf overrun\n", __func__);
			io_exit(EX_SOFTWARE); /* Panic */
		}

		free(dotdup);

		if (stat(p.dotfile, &st) < 0) {
			if (errno == ENOENT)
				return -1;
			printf("%s: fatal error: stat failed [%s]\n", __func__, p.dotfile);
			io_exit(EX_SOFTWARE); /* Panic */
		}
		if (S_ISREG(st.st_mode)) {
			if (getuid() && getuid() != st.st_uid) {
				printf("%s: fatal error: invalid owner [%s]\n", __func__, p.dotfile);
				io_exit(EX_SOFTWARE); /* Panic */
			}
			return access(p.dotfile, R_OK);
		}
	}

	return -1;
}

/*
 * Dot file
 */
void
getdotfile(void)
{
	int rc;

	char *filename = getenv("PICKLE");
	if (filename != NULL) {
		rc = snprintf(p.dotfile, STRLEN, "%s", filename);
		if (rc < 0) {
			printf("%s: fatal error: snprintf failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		} else if (rc >= STRLEN) {
			printf("%s: fatal error: snprintf overrun\n", __func__);
			io_exit(EX_SOFTWARE); /* Panic */
		}

		if (getdotpath() == 0)
			return;

		printf("%s: warning: invalid PICKLE environment variable\n", __func__);
	}

	char dir[STRLEN];
	if (getcwd(dir, STRLEN) == NULL) {
		printf("%s: fatal error: getcwd failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}

	rc = snprintf(p.dotfile, STRLEN, "%s/%s", dir, DOTFILENAME);
	if (rc < 0) {
		printf("%s: fatal error: snprintf failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	} else if (rc >= STRLEN) {
		printf("%s: fatal error: snprintf overrun\n", __func__);
		io_exit(EX_SOFTWARE); /* Panic */
	}

	if (getdotpath() == 0)
		return;

	char *homedir = getenv("HOME");
	if (homedir != NULL) {
		rc = snprintf(p.dotfile, STRLEN, "%s/%s", homedir, DOTFILENAME);
		if (rc < 0) {
			printf("%s: fatal error: snprintf failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		} else if (rc >= STRLEN) {
			printf("%s: fatal error: snprintf overrun\n", __func__);
			io_exit(EX_SOFTWARE); /* Panic */
		}

		if (getdotpath() == 0)
			return;
	}

	char *username = getenv("USER");
	if (username != NULL) {
		rc = snprintf(p.dotfile, STRLEN, "/home/%s/%s", username, DOTFILENAME);
		if (rc < 0) {
			printf("%s: fatal error: snprintf failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		} else if (rc >= STRLEN) {
			printf("%s: fatal error: snprintf overrun\n", __func__);
			io_exit(EX_SOFTWARE); /* Panic */
		}

		if (getdotpath() == 0)
			return;
	}

	/* No dot file found */
	p.dotfile[0] = '\0';
}

/*
 * Dot variable
 *
 * return line with whitespace and comments removed
 */
char *
getvar(char *s, int slen)
{
	int i = 0, j = 0;
	char *t;

	if (!slen) {
		printf("%s: fatal error: invalid length\n", __func__);
		io_exit(EX_SOFTWARE); /* Panic */
	}

	t = malloc(slen);
	if (t == NULL) {
		printf("%s: fatal error: malloc failed\n", __func__);
		io_exit(EX_OSERR); /* Panic */
	}

	s[slen - 1] = '\0';

	while (s[i] && s[i] != '#') {

		if (s[i] > ' ')
			t[j++] = s[i];
		++i;
	}

	t[j] = '\0';

	strcpy(s, t);
	free(t);

	return s;
}

/*
 * Configuration
 */
void
getconf(void)
{
	FILE *f1;
	char line[STRLEN];
	int rc;

	/* Config defaults */
	io_config();

	/* Determine dot file name */
	getdotfile();

	/* Load dot file when available */
	if (p.dotfile[0] && (f1 = fopen(p.dotfile, "rb")) != NULL) {
		while (fgets(line, STRLEN, f1) != NULL) {

			/* Get variable in line */
			getvar(line, STRLEN);
			
			/* Ignore empty line */
			if (!*line)
				continue;

			if (mystrcasestr(line, "DEVICE=") == line) {
				strncpy(p.device, &line[7], STRLEN);
			}
			else if (mystrcasestr(line, "BAUDRATE=") == line) {
				p.baudrate = strtoul(&line[9], NULL, 0);
			}
			else if (mystrcasestr(line, "SLEEP=") == line) {
				p.sleep_low = strtoul(&line[6], NULL, 0);
				p.sleep_high = p.sleep_low;
			}
			else if (mystrcasestr(line, "SLEEP_LOW=") == line) {
				p.sleep_low = strtoul(&line[10], NULL, 0);
			}
			else if (mystrcasestr(line, "SLEEP_HIGH=") == line) {
				p.sleep_high = strtoul(&line[11], NULL, 0);
			}
			else if (mystrcasestr(line, "SLEEP_ALGO=") == line) {
				p.sleep_algo = strtoul(&line[11], NULL, 0);
			}
			else if (mystrcasestr(line, "BITRULES=") == line) {
				p.bitrules = strtoul(&line[9], NULL, 0);
			}
			else if (mystrcasestr(line, "BUSY=") == line) {
				p.busy = strtoul(&line[5], NULL, 0);
			}
			else if (mystrcasestr(line, "VPP=") == line) {
				p.vpp = strtoul(&line[4], NULL, 0);
			}
			else if (mystrcasestr(line, "PGM=") == line) {
				p.pgm = strtoul(&line[4], NULL, 0);
			}
			else if (mystrcasestr(line, "PGC=") == line) {
				p.pgc = strtoul(&line[4], NULL, 0);
			}
			else if (mystrcasestr(line, "PGD=") == line) {
				p.pgdi = strtoul(&line[4], NULL, 0);
				p.pgdo = p.pgdi;
			}
			else if (mystrcasestr(line, "PGDO=") == line) {
				p.pgdo = strtoul(&line[5], NULL, 0);
			}
			else if (mystrcasestr(line, "PGDI=") == line) {
				p.pgdi = strtoul(&line[5], NULL, 0);
			}
			else if (mystrcasestr(line, "SERIAL=") == line) {
				strncpy(p.serial, &line[7], STRLEN);
			}
			else if (mystrcasestr(line, "ADDR=") == line) {
				p.addr = strtoul(&line[5], NULL, 0);
			}
			else if (mystrcasestr(line, "IFACE=") == line) {
				strncpy(p.iface, &line[6], STRLEN);
			}
			else if (mystrcasestr(line, "FWSLEEP=") == line) {
				p.fwsleep = strtoul(&line[8], NULL, 0);
			}
			else if (mystrcasestr(line, "DEBUG=") == line) {
				p.debug = strtoul(&line[6], NULL, 0);
			}
			else if (mystrcasestr(line, "ETC=") == line) {
				strncpy(p.etc, &line[4], STRLEN);
			}
			else if (mystrcasestr(line, "CONFIG=") == line) {
				p.config = strtoul(&line[7], NULL, 0);
			}
			else if (mystrcasestr(line, "ERROR=") == line) {
				p.error = strtoul(&line[6], NULL, 0);
			}
			else {
				printf("%s: warning: invalid config option [%s]\n", __func__, line);
			}
		}
		fclose(f1);
	} else {	/* We are using the defaults */
		rc = snprintf(p.dotfile, STRLEN, "Using defaults (override in %s)", DOTFILENAME);
		if (rc < 0) {
			printf("%s: fatal error: snprintf failed\n", __func__);
			io_exit(EX_OSERR); /* Panic */
		} else if (rc >= STRLEN) {
			printf("%s: fatal error: snprintf overrun\n", __func__);
			io_exit(EX_SOFTWARE); /* Panic */
		}
	}
}
