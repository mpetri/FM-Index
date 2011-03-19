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

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "FM.h"

static void
print_usage(const char *program)
{
    fprintf(stderr, "USAGE: %s -v -s <input> <index>\n", program);
	fprintf(stderr, "  input : file to be indexed\n");
	fprintf(stderr, "  index : index file [default = input.fm]\n");
	fprintf(stderr, "  -v verbose output\n");
    fprintf(stderr, "  -s samplerate [default = %d]\n",DEFAULT_SAMPLERATE);
    fprintf(stderr, "\n");
    fprintf(stderr, "EXAMPLE: %s alice29.txt alice29.fm\n",program);
    fprintf(stderr, "\n");
    return;
}

/*
 * 
 */
int main(int argc, char** argv) {
    int32_t opt,samplerate;
    char* idxname;char* inname;
    FILE* f;
    FM* FMIdx;
    uint8_t* T;
    uint32_t n;
    char buf[4096];
    
    /* parse command line parameter */
    if (argc <= 1) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    
	opt = -1;
    idxname = inname = NULL;
    samplerate = DEFAULT_SAMPLERATE;
    while ((opt = getopt(argc, argv, "vhs:")) != -1) {
        switch (opt) {
            case 's':
                samplerate = atoi(optarg);
                break;
			case 'v':
				FM::verbose = 1;
				break;
            case 'h':
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
	/* read filenames */
	if(optind < argc) { 
		inname = argv[optind];
		optind++;
	}
	if(optind < argc) { 
		idxname = argv[optind];
		optind++;
	}
	
	/* load input file */
	debug("Reading input file '%s'",inname);
	f = safe_fopen(inname,"r");
	n = safe_filesize(f);
	T = (uint8_t*) safe_malloc( (n+1)*sizeof(uint8_t) );
	if( fread(T,1,n,f) != n ) {
		perror("error reading input file");
		exit(EXIT_FAILURE);
	}
	fclose(f);
        
	/* build index */
	FMIdx = new FM(T,n,samplerate);
	
	if(!FMIdx) {
		perror("error building index");
		exit(EXIT_FAILURE);
	}
          
	/* write index */
	if(idxname==NULL) {
		sprintf(buf,"%s.fm",inname);
		idxname = buf;
	}
	if( FMIdx->save(idxname) != 0 ) {
		perror("error writing index");
		exit(EXIT_FAILURE);
	}
    
	/* clean up */
	delete FMIdx;
	/* T already deleted in FMIdx */
    
    return (EXIT_SUCCESS);
}

