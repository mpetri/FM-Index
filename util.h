/*  FM-Index - Text Index
 *  Copyright (C) 2011  Matthias Petri
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef UTIL_H
#define	UTIL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>

#define ALPHABET_SIZE     256
#define REALLOC_INCREMENT 256
#define MAX_QRY_LEN       4096
#define GETOPT_FINISHED -1

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(x) (((x) < 0) ? -(x) : + (x) )
#endif

void *safe_malloc (size_t size);
void *safe_realloc (void *old_mem, size_t new_size);
char *safe_strdup (const char *str);
char *safe_strcat (char *str1, const char *str2);
void fatal (const char *format, ...);
void debug (const char *format, ...);
void info (const char *format, ...);
FILE* safe_fopen (const char *filename,const char* mode);
int safe_filesize(FILE* f);
void safe_fclose (FILE* f);
uint8_t* safe_readfile(const char* filename,uint32_t* size);

uint32_t gettime();

#ifdef	__cplusplus
}
#endif

#endif	/* UTIL_H */

