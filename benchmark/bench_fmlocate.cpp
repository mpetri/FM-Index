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
    fprintf(stderr, "USAGE: %s -i <index> -l <qrylen> -n <occnumber> -t <text>\n", program);
	fprintf(stderr, "  -l qrylen : length of the queries in the experiment\n");
	fprintf(stderr, "  -n occnumber : total number of occurences in the experiment\n");
	fprintf(stderr, "  -i index : index file\n");
	fprintf(stderr, "  -t text file\n");
	fprintf(stderr, "  -v verbose output\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "EXAMPLE: %s -i alice29.fm 5 5000\n",program);
    fprintf(stderr, "\n");
    return;
}

/*
 * 
 */
int main(int argc, char** argv) {
    int32_t opt,noccs,qrylen,i;
    char* idxname;char* textname;
    uint8_t* T;
    uint32_t n;
    FILE* f;
    FM* FMIdx;
	uint8_t** queries;
	char buf[4096];
	uint32_t start,stop,cnt;
    
    /* parse command line parameter */
    if (argc <= 3) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    
	opt = -1;
    idxname = textname = NULL;
    while ((opt = getopt(argc, argv, "vhi:l:t:n:")) != -1) {
        switch (opt) {
            case 'l':
                qrylen = atoi(optarg);
                break;
            case 'n':
                noccs = atoi(optarg);
                break;
            case 't':
                textname = optarg;
                break;
			case 'i':
				idxname = optarg;
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
	
		
	/* load index */
	FMIdx = FM::load(idxname);
	if(!FMIdx) {
		perror("error loading index from file");
		exit(EXIT_FAILURE);
	}
	
	/* generate queries */
    f = safe_fopen(textname,"r");
    n = safe_filesize(f);
    T = (uint8_t*) safe_malloc(n*sizeof(uint8_t));
    if( fread(T,1,n,f) != n ) {
        perror("error reading text");
        exit(EXIT_FAILURE);
    }
	fclose(f);

	FM::info("generating queries");
	srand( gettime() );
	queries = (uint8_t**) safe_malloc( 256 * sizeof(uint8_t*));
	int curq = 256;
	int j = 0;
	i = 0;
	while( i < noccs ) {
        start = (rand()*rand()) % (n-qrylen-1);
        memcpy(buf,(uint8_t*)(T+start),qrylen);
        cnt = FMIdx->count((uint8_t*)buf,qrylen);
        if(i+cnt > (noccs+500) ) continue;
        else {
            i+=cnt;
            queries[j] = (uint8_t*) safe_strdup(buf);
            j++;
            if(j>curq-1) {
                queries = (uint8_t**) safe_realloc(queries, (curq*2) * sizeof(uint8_t*));
                curq *= 2;
            }
        }
    }
    FM::info("generated %d queries with a total of %d occs\n",j,i);
    start = gettime();
	for(i=0;i<j;i++) {
	    uint32_t matches;
		uint32_t* locs = FMIdx->locate(queries[i],strlen((char*)queries[i]),&matches);
		free(locs);
	}
	stop = gettime();
	FM::info("finished processing %d queries: %f sec",j,((float)(stop-start))/1000000);
	
	/* clean up */
	for(i=0;i<j;i++) free(queries[i]);
	free(queries);
	free(T);
	delete FMIdx;
    
    return (EXIT_SUCCESS);
}

