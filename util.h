/* 
 * File:   util.h
 * Author: matt
 *
 * Created on March 7, 2011, 12:53 PM
 */

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

