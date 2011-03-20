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

#include "FM.h"
#include "util.h"
#include "divsufsort.h"

#include <algorithm>

int FM::verbose = 0;

FM::FM(uint8_t* T,uint32_t N,uint32_t samplerate = DEFAULT_SAMPLERATE) {
    this->samplerate = samplerate;
    this->n = N;
    
    /* 0 terminate */
    if(T[N-1] != 0) {
        T = (uint8_t*) safe_realloc(T,(N+1) * sizeof(uint8_t));
        T[N] = 0;
        this->n++;
    } 
    
    build(T,n,samplerate);
}

FM::FM() {
    this->n = 0;
    this->sigma = 0;
    this->I = 0;
    this->remap_reverse = NULL;
    this->T_bwt = NULL;
    this->sampled = NULL;
    this->suffixes = NULL;
}

FM::~FM() {
    free(remap_reverse);
    free(suffixes);
	free(positions);
    delete T_bwt;
    delete sampled;
}

uint32_t
FM::getSize() {
    uint32_t bytes = 0;
    
    bytes += sizeof(this->n);
    bytes += sizeof(this->samplerate);
    bytes += sizeof(this->sigma);
    bytes += sizeof(this->I);
    bytes += sizeof(this->remap);
    bytes += sizeof(this->C);
    bytes += this->sigma * sizeof(uint8_t); /* remap_reverse */
    bytes += ((n/samplerate)+1) * sizeof(uint32_t); /* suffixes */
	bytes += ((n/samplerate)+2) * sizeof(uint32_t); /* positions */
    bytes += this->sampled->getSize();
    bytes += this->T_bwt->getSize();
    
    return bytes;
}

float
FM::getSizeN() {
    uint32_t bytes = getSize();
    return (float)(bytes)/(float)(n);
}

uint8_t*
FM::remap0(uint8_t* T,uint32_t n) {
    uint8_t* X;
    uint32_t i,j,size=0;
    uint32_t freqs[size_uchar];
    
    for(i=0;i<size_uchar;i++) freqs[i]=0;
    for(i=0;i<n;i++) if(freqs[T[i]]++==0) size++;
    
    this->sigma=size;
    
    // remap alphabet
    if (freqs[0]>1) {i=1;sigma++;} //test if some character of T is zero, we already know that text[n-1]='\0'
    else i=0;

    remap_reverse = (uint8_t*) malloc(size*sizeof(uint8_t));
    for(j=0;j<size_uchar;j++) {
      if(freqs[j]!=0) {
        remap[j]=i;
        remap_reverse[i++]=j;
      }
    }
    // remap text
    X = (uint8_t*) malloc(n * sizeof(uint8_t));
    for(i=0;i<n-1;i++) // the last character must be zero
      X[i]=remap[T[i]];
    
    return X;
}

void
FM::build(uint8_t* T,uint32_t n,uint32_t samplerate) {
    uint8_t* X;
    uint8_t* X_bwt;
    int32_t* SA;
    uint32_t i,prev,tmp,start,stop;
    float elapsed;
    
    start = gettime();
	
	info("building index.");
    
    /* remap if 0 in text */
    info("- remapping alphabet.");
    X = remap0(T,n);
    free(T);
    
    /* create cumulative counts */
    info("- creating cumulative counts C[].");
    for (i=0;i<size_uchar+1;i++) C[i]=0;
    for (i=0;i<n;++i) C[X[i]]++;
    prev=C[0];C[0]=0;
    for (i=1;i<size_uchar+1;i++) {
      tmp = C[i];
      C[i]=C[i-1]+prev;
      prev = tmp;
    }
    
    /* perform k-BWT */
    info("- performing bwt.");
    SA = (int32_t*) safe_malloc( n * sizeof(int32_t)  );
    if( divsufsort(X,SA,n) != 0 ) {
        fatal("error divsufsort");
    }
    
    /* sample SA for locate() */
    info("- sample SA locations.");
    suffixes = (uint32_t*) safe_malloc( ((n/samplerate)+1) * sizeof(uint32_t));
    BitString B(n);
    tmp = 0;
    for(i=0;i<n;i++) {
        if( SA[i] % samplerate == 0) {
            suffixes[tmp] = SA[i];
            B.setBit(i,true);
            tmp++;
        } else B.setBit(i,false);
    }
    /* enable rank on context vector */
    this->sampled = new BitSequenceRRR(B,RRR_SAMPLERATE);
	
	/* sample SA for display() */
	positions = (uint32_t*) safe_malloc( ((n/samplerate)+2) * sizeof(uint32_t));
    for (i=0;i<this->n;i++) {
        if (SA[i] % samplerate == 0) this->positions[SA[i]/samplerate] = i;
	}
    positions[(this->n-1)/samplerate+1] = positions[0];
	
    info("- creating bwt output.");
    X_bwt = (uint8_t*) safe_malloc( n * sizeof(uint8_t)  );
    for(i=0;i<n;i++) {
        if(SA[i]==0) { 
            X_bwt[i] = X[n-1];
            this->I = i;
        } else X_bwt[i] = X[SA[i]-1];
    }
    free(SA);
    
    info("- create RRR wavelet tree over bwt.");
    MapperNone * map = new MapperNone();
    BitSequenceBuilder * bsb = new BitSequenceBuilderRRR(RRR_SAMPLERATE);
    T_bwt = new WaveletTreeNoptrs((uint32_t*)X_bwt,n,sizeof(uint8_t)*8,bsb,map,true);
    
    stop = gettime();
    elapsed = (float)(stop-start)/1000000;
    
    /* build aux data */
    info("build FM-Index done. (%.3f sec)",elapsed);
    
    uint32_t bytes;
    info("space usage:");
    bytes = sigma * sizeof(uint8_t);
    info("- remap_reverse: %d bytes (%.2f\%)",bytes,(float)bytes/getSize()*100);
    bytes = sizeof(this->C);
    info("- C: %d bytes (%.2f\%)",bytes,(float)bytes/getSize()*100);
    bytes = ((n/samplerate)+1) * sizeof(uint32_t);
    info("- Suffixes: %d bytes (%.2f\%)",bytes,(float)bytes/getSize()*100);
    bytes = ((n/samplerate)+2) * sizeof(uint32_t);
    info("- Positions: %d bytes (%.2f\%)",bytes,(float)bytes/getSize()*100);
    bytes = sampled->getSize();
    info("- Sampled: %d bytes (%.2f\%)",bytes,(float)bytes/getSize()*100);
    bytes = T_bwt->getSize();
    info("- T_bwt: %d bytes (%.2f\%)",bytes,(float)bytes/getSize()*100);
	info("input Size n = %lu bytes\n",this->n);
	info("index Size = %lu bytes (%.2f n)",getSize(),getSizeN());
}


int32_t
FM::save(char* filename) {
    std::ofstream f;
    f.open(filename,std::ios::out | std::ios::binary); 
    
	info("writing FM Index to file '%s'",filename);
    if(f.is_open()) {
        f.write(reinterpret_cast<char*>(&samplerate),sizeof(uint32_t));
        f.write(reinterpret_cast<char*>(&sigma),sizeof(uint32_t));
        f.write(reinterpret_cast<char*>(&I),sizeof(uint32_t));
        f.write(reinterpret_cast<char*>(&n),sizeof(uint32_t));
        f.write(reinterpret_cast<char*>(C),sizeof(uint32_t)*(size_uchar+1));
        f.write(reinterpret_cast<char*>(remap),sizeof(uint8_t)*size_uchar);
        f.write(reinterpret_cast<char*>(remap_reverse),sizeof(uint8_t)*sigma);
        f.write(reinterpret_cast<char*>(suffixes),sizeof(uint32_t)*((n/samplerate)+1));
		f.write(reinterpret_cast<char*>(positions),sizeof(uint32_t)*((n/samplerate)+2));
        T_bwt->save(f);
        sampled->save(f);
        f.close();
    } else return 1;
    
    return 0;
}

FM*
FM::load(char* filename) {
    FM* newIdx = new FM();
    std::ifstream f;
    f.open(filename,std::ios::in | std::ios::binary); 
    
    if(f.is_open()) {
        info("loading FM Index from file '%s'",filename);
        f.read(reinterpret_cast<char*>(&newIdx->samplerate),sizeof(uint32_t));
        f.read(reinterpret_cast<char*>(&newIdx->sigma),sizeof(uint32_t));
        f.read(reinterpret_cast<char*>(&newIdx->I),sizeof(uint32_t));
        f.read(reinterpret_cast<char*>(&newIdx->n),sizeof(uint32_t));
        f.read(reinterpret_cast<char*>(newIdx->C),sizeof(uint32_t)*(size_uchar+1));
        f.read(reinterpret_cast<char*>(newIdx->remap),sizeof(uint8_t)*size_uchar);
        newIdx->remap_reverse = (uint8_t*) safe_malloc(sizeof(uint8_t)*(newIdx->sigma));
        f.read(reinterpret_cast<char*>(newIdx->remap_reverse),sizeof(uint8_t)*newIdx->sigma);
        newIdx->suffixes = (uint32_t*) safe_malloc(sizeof(uint32_t)*((newIdx->n/newIdx->samplerate)+1));
        f.read(reinterpret_cast<char*>(newIdx->suffixes),sizeof(uint32_t)*((newIdx->n/newIdx->samplerate)+1));
        newIdx->positions = (uint32_t*) safe_malloc(sizeof(uint32_t)*((newIdx->n/newIdx->samplerate)+2));
        f.read(reinterpret_cast<char*>(newIdx->positions),sizeof(uint32_t)*((newIdx->n/newIdx->samplerate)+2));
        newIdx->T_bwt = WaveletTreeNoptrs::load(f);
        newIdx->sampled = BitSequenceRRR::load(f);
        f.close();
        info("samplerate = %d",newIdx->samplerate);
        info("sigma = %d",newIdx->sigma);
        info("I = %d",newIdx->I);
        info("n = %d",newIdx->n);
    } else {
        delete newIdx;
        return NULL;
    }
        
    return newIdx;
}

uint32_t
FM::count(uint8_t* pattern,uint32_t m) {
    uint8_t c = remap[pattern[m-1]]; /* map pattern to our alphabet */
    uint32_t i=m-1;
    uint32_t j = 1;
    
    uint32_t sp = C[c]; /* starting range in M from p[m-1] */
    uint32_t ep = C[c+1]-1;
	/* while there are possible occs and pattern not done */
    while (sp<=ep && i>=1) { 
      c = remap[pattern[--i]]; /* map pattern to our alphabet */
      sp = C[c] + T_bwt->rank(c, sp-1); /* LF Mapping */
      ep = C[c] + T_bwt->rank(c, ep)-1; /* LF Mapping */
      j++;
    }
    
    if (sp<=ep) {
      return ep-sp+1;
    } else {
      return 0;
    }
}

uint32_t*
FM::locate(uint8_t* pattern,uint32_t m,uint32_t* matches) {
    uint32_t* locations;
    uint8_t c =  remap[pattern[m-1]];
    uint32_t i=m-1;
    
    /* count occs */
    uint32_t sp = C[c];
    uint32_t ep = C[c+1]-1;
    while (sp<=ep && i>=1) {
      c =  remap[pattern[--i]];
      sp = C[c] + T_bwt->rank(c, sp-1);
      ep = C[c] + T_bwt->rank(c, ep)-1;
    }
    
    if (sp<=ep) {
        /* determine positions */
        *matches = ep-sp+1;
        uint32_t locate=0;
        locations= (uint32_t*) safe_malloc((*matches)*sizeof(uint32_t));
        i=sp;
        int32_t j,dist,rank;
        while (i<=ep) {
            j=i,dist=0;
            while (!sampled->access(j)) {
                c = T_bwt->access(j);
                rank = T_bwt->rank(c,j)-1;
                j = C[c]+rank; // LF-mapping
                ++dist;
            }
            locations[locate]=suffixes[sampled->rank1(j)-1]+dist;
            locate++;
            ++i;
        }
        /* locations are in SA order */
        std::sort(locations,locations+(*matches));
        return locations;
    } else {
      /* no matches */
      *matches = 0;
      return NULL;
    }
    
    return locations;
}

uint8_t*
FM::extract(uint32_t start,uint32_t stop)
{
    uint8_t* T;
	uint32_t m,j,skip,todo,dist;
	uint8_t c;
	
	/* last text pos is n-2 */
	if(stop > (this->n-1) ) stop = n-2; 
    if(start > stop) {
		return NULL;
	}
	
	m = stop-start+1; /* snippet len */
	T = (uint8_t*) safe_malloc( (m+1) * sizeof(uint8_t)  );
	
	/* determine start pos of backwards search */
	j = positions[(stop/samplerate)+1];
	
	/* determine distance from start pos to the text snippet we want */
	if ((stop/samplerate+1) == ((n-1)/samplerate+1)) 
	   skip = n-2 - stop;
	else 
	   skip = (samplerate-stop)%samplerate-1;
	   
	/* start the backwards search */
	todo = m;
	dist = 0;
	while(todo>0) {
		c = T_bwt->access(j);
		j = C[c] + T_bwt->rank(c,j)-1;
		
		/* check if we are at the snippet */
		if(dist>=skip) {
			c = remap_reverse[c];
			T[todo-1] = c;
			todo--;
		}
		dist++;
	}
	
	/* terminate */
	T[m] = 0;
	
    return T;
}

uint8_t*
FM::reconstructText(uint32_t* size)
{
    uint8_t* T;
    uint8_t c;
    uint32_t j,i;
    
    T = (uint8_t*) safe_malloc( n * sizeof(uint8_t)  );
    
    j = I; /* I is sa[I] = 0 -> last sym in T */
    for(i=0;i<n;i++) {
        c = T_bwt->access(j); /* L[j] = c */
        T[n-i-1] = remap_reverse[c]; /* undo sym mapping */
        j = C[c]+T_bwt->rank(c,j)-1; /* LF-mapping: j = LF[j] */
    }
    
	if(T[n-1] == 0) *size = n-1;
    else *size = n;
    
    return T;
}

