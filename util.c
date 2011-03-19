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

#include "util.h"


/*  safe_malloc ()
 *
 *  Call calloc () and abort if the specified amount of
 *  memory cannot be allocated.
 */

void *
safe_malloc (size_t size)
{
  void *mem_block = NULL;

  if ((mem_block = calloc (1,size)) == NULL) {
    fprintf (stderr, "ERROR: safe_malloc(%zu) cannot allocate memory.", size);
    exit (EXIT_FAILURE);
  }
  return (mem_block);
}

/*  safe_realloc ()
 *
 *  Call realloc () and abort if the specified amount of
 *  memory cannot be allocated.
 */

void *
safe_realloc (void *old_mem, size_t new_size)
{
  if ((old_mem = realloc (old_mem, new_size)) == NULL) {
    fprintf (stderr, "ERROR: safe_realloc() cannot allocate"
             "%zu blocks of memory.\n", new_size);
    exit (EXIT_FAILURE);
  }
  return (old_mem);
}

/*
 * safe_strdup ()
 *
 * Safe version of strdup avoid buffer overflow, etc.
 *
 */

char *
safe_strdup (const char *str)
{
  char *copy = NULL;

  if (str == NULL) {
    fprintf (stderr, "ERROR safe_strdup(): str == NULL");
    exit (EXIT_FAILURE);
  }

  copy = (char*) safe_malloc ((strlen (str) + 1) * sizeof (char));

  (void) strcpy (copy, str);

  return (copy);
}

char *
safe_strcat (char *str1, const char *str2)
{
  char *rv = NULL;
  size_t len = 0;

  if (str1 == NULL || str2 == NULL) {
    fprintf (stderr, "ERROR safe_strcat_new(): str == NULL");
    exit (EXIT_FAILURE);
  }
  len = strlen (str1) + strlen (str2) + 1;
  rv = (char*) safe_malloc (len * sizeof (char));
  (void) strcpy (rv, str1);
  rv = strcat (rv, str2);
  return (rv);
}


FILE*
safe_fopen (const char *filename,const char* mode)
{
  FILE* f;
  f = fopen(filename,mode);
  if (f == NULL) {
    fprintf (stderr, "ERROR: safe_fopen(%s,%s)\n", filename,mode);
    exit (EXIT_FAILURE);
  }
  return (f);
}

void
safe_fclose (FILE* f)
{
    if (fclose(f) != 0) {
        perror("Error: file close():");
        exit (EXIT_FAILURE);
    }
}

int
safe_filesize(FILE* f)
{
    int size;
    int cur = ftell(f);
    if( cur == -1 ) {
        perror("Error: file ftell():");
        exit (EXIT_FAILURE);
    } else {
        fseek(f,0,SEEK_END);
        size = ftell(f);
        if( size == -1 ) {
            perror("Error: file ftell():");
            exit (EXIT_FAILURE);
        }
        fseek(f,cur,SEEK_SET);
    }
    return size;
}

uint8_t* safe_readfile(const char* filename,uint32_t* size)
{
    FILE* f;
    uint32_t n;
    uint8_t* T;

    f = safe_fopen(filename,"r");
    n = safe_filesize(f);

    T = (uint8_t*) safe_malloc( (n)* sizeof(uint8_t));

    if( fread(T,1,n,f) != n) {
        perror("Error: file fread():");
        exit (EXIT_FAILURE);
    }

    *size = n;
    return T;
}

void
fatal (const char *format, ...)
{
    va_list vargs;

    va_start (vargs, format);
    vfprintf (stderr, format, vargs);
    fprintf (stderr, "\n");
    exit (EXIT_FAILURE);
}

#ifdef DEBUG
void
debug (const char *format, ...)
{

    va_list vargs;

    va_start (vargs, format);
    vfprintf (stderr, format, vargs);
    fprintf (stderr, "\n");
}
#else
void
debug (const char *, ...) {};
#endif

void
info (const char *format, ...)
{
    va_list vargs;
    va_start (vargs, format);
    vfprintf (stdout, format, vargs);
    fprintf (stdout, "\n");
}

uint32_t
gettime()
{
    struct timeval tp;
    gettimeofday(&tp,NULL);
    return (tp.tv_sec*1000000) + tp.tv_usec;
}
