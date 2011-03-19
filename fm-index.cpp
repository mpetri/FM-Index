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

typedef enum {
    UNKNOWN,
    BUILD,
    LOAD,
    COUNT,
    LOCATE,
    RECOVER
} idxmode_t;

static void
print_usage(const char *program)
{
    fprintf(stderr, "USAGE: %s -i <index> -q <queries> -k <depth> -s <samplerate>-l -d <input>\n", program);
    fprintf(stderr, "  -i index file\n");
    fprintf(stderr, "  -d input file\n");
    fprintf(stderr, "  -q query file\n");
    fprintf(stderr, "  -s samplerate [default = %d]\n",DEFAULT_SAMPLERATE);
    fprintf(stderr, "  -l locate occurrences\n");
    fprintf(stderr, "  -r recover original text\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "EXAMPLE: %s -i bible.fm -q bible.qry -c\n",program);
    fprintf(stderr, "EXAMPLE: %s -k 3 -d bible.txt\n",program);
    fprintf(stderr, "\n");
    return;
}

/*
 * 
 */
int main(int argc, char** argv) {
    int32_t opt,samplerate;
    char* idxname;char* qryname;char* inname;
    idxmode_t mode;
    idxmode_t qrymode;
    FILE* f;
    FM* FMIdx;
    uint8_t* T;
    uint32_t n,nqrys,maxqry,i,cnt,start,stop,matches,j;
    uint8_t** queries;
    uint32_t* result;
    char buf[4096];
    
    /* parse command line parameter */
    opt = -1;
    if (argc <= 1) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    
    mode = UNKNOWN;
    qrymode = COUNT;
    idxname = qryname = inname = NULL;
    samplerate = DEFAULT_SAMPLERATE;
    while ((opt = getopt(argc, argv, "i:q:k:d:crls:")) != -1) {
        switch (opt) {
            case 'i':
                idxname = optarg;
                break;
            case 'q':
                qryname = optarg;
                break;
            case 'd':
                inname = optarg;
                mode = BUILD;
                break;
            case 'l':
                qrymode = LOCATE;
                break;
            case 'r':
                qrymode = RECOVER;
                break;
            case 's':
                samplerate = atoi(optarg);
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    /* read index */
    if(mode != BUILD) {
        FMIdx = FM::load(idxname);
        debug("Index Size = %lu bytes (%.2f n)",FMIdx->getSize(),
                        FMIdx->getSizeN());
        if(!FMIdx) {
            perror("error loading index from file");
            exit(EXIT_FAILURE);
        }
    } else {
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
        
        debug("Index Size = %lu bytes (%.2f n)",FMIdx->getSize(),
                        FMIdx->getSizeN());
        
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
    }
    
    if(qryname != NULL) {
        /* read queries */
        f = safe_fopen(qryname,"r");
        maxqry = REALLOC_INCREMENT;
        queries = (uint8_t**) safe_malloc(REALLOC_INCREMENT * sizeof(uint8_t*));
        nqrys = 0;
        while( fscanf(f,"%s\n",buf) == 1 ) {
            queries[nqrys] = (uint8_t*) safe_strdup(buf);
            if(nqrys == maxqry-1) {
                queries = (uint8_t**) safe_realloc(queries,
                                                (maxqry*2)*sizeof(uint8_t*));
                maxqry *= 2;
            }
            nqrys++;
        }
        fclose(f);
        info("Read %d queries",nqrys);
        
        start = gettime();
        for(i=0;i<nqrys;i++) {
            if(qrymode == COUNT) {
                cnt = FMIdx->count(queries[i],strlen((char*)queries[i]));
                fprintf(stdout,"%s : %d\n",queries[i],cnt);
            }
            if(qrymode == LOCATE) {
                result = FMIdx->locate(queries[i],strlen((char*)queries[i]),&matches);
                fprintf(stdout,"%s (%d) : ",queries[i],matches);
                for(j=0;j<matches-1;j++) fprintf(stdout,"%d ",result[j]);
                fprintf(stdout,"%d\n",result[matches-1]);
                free(result);
            }
        }
        stop = gettime();
        info("Finished processing queries: %.3f sec",((float)(stop-start))/1000000);
    } else {
        if(qrymode == RECOVER) {
            T = FMIdx->reconstructText(&n);
            fprintf(stdout,"%s",T);
            free(T);
        }
    }
    
    return (EXIT_SUCCESS);
}

