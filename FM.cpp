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


FM::FM(uint8_t* T,uint32_t N,uint32_t samplerate) {
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
    uint32_t Freq_old[size_uchar];
    
    for(i=0;i<size_uchar;i++) Freq_old[i]=0;
    for(i=0;i<n;i++) if(Freq_old[T[i]]++==0) size++;
    
    this->sigma=size;
    
    // remap alphabet
    if (Freq_old[0]>1) {i=1;sigma++;} //test if some character of T is zero, we already know that text[n-1]='\0'
    else i=0;

    remap_reverse = (uint8_t*) malloc(size*sizeof(uint8_t));
    for(j=0;j<size_uchar;j++) {
      if(Freq_old[j]!=0) {
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
    
    /* remap if 0 in text */
    debug("remapping alphabet.");
    X = remap0(T,n);
    free(T);
    
    /* create cumulative counts */
    debug("creating cumulative counts C[].");
    for (i=0;i<size_uchar+1;i++) C[i]=0;
    for (i=0;i<n;++i) C[X[i]]++;
    prev=C[0];C[0]=0;
    for (i=1;i<size_uchar+1;i++) {
      tmp = C[i];
      C[i]=C[i-1]+prev;
      prev = tmp;
    }
    
    /* perform k-BWT */
    debug("performing bwt.");
    SA = (int32_t*) safe_malloc( n * sizeof(int32_t)  );
    if( divsufsort(X,SA,n) != 0 ) {
        fatal("error divsufsort");
    }
    
    /* sample SA for locate() */
    debug("sample SA locations.");
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
    this->sampled = new BitSequenceRRR(B,20);
    
    debug("creating bwt output.");
    X_bwt = (uint8_t*) safe_malloc( n * sizeof(uint8_t)  );
    for(i=0;i<n;i++) {
        if(SA[i]==0) { 
            X_bwt[i] = X[n-1];
            this->I = i;
        } else X_bwt[i] = X[SA[i]-1];
    }
    free(SA);
    
    debug("create RRR wavelet tree over bwt.");
    MapperNone * map = new MapperNone();
    /*wt_coder_huff* wc = new wt_coder_huff(X_bwt,n,map);*/
    BitSequenceBuilder * bsb = new BitSequenceBuilderRRR(20);
    T_bwt = new WaveletTreeNoptrs((uint32_t*)X_bwt,n,sizeof(uint8_t)*8,bsb,map,true);
    
    stop = gettime();
    elapsed = (float)(stop-start)/1000000;
    
    /* build aux data */
    debug("build FM done. (%.3f sec)",elapsed);
    
    uint32_t bytes;
    debug("Space usage:");
    bytes = sigma * sizeof(uint8_t);
    debug("remap_reverse: %d bytes (%.2f)",bytes,(float)bytes/getSize()*100);
    bytes = sizeof(this->C);
    debug("C: %d bytes (%.2f)",bytes,(float)bytes/getSize()*100);
    bytes = ((n/samplerate)+1) * sizeof(uint32_t);
    debug("Suffixes: %d bytes (%.2f)",bytes,(float)bytes/getSize()*100);
    bytes = sampled->getSize();
    debug("Sampled: %d bytes (%.2f)",bytes,(float)bytes/getSize()*100);
    bytes = T_bwt->getSize();
    debug("T_bwt: %d bytes (%.2f)",bytes,(float)bytes/getSize()*100);
}


int32_t
FM::save(char* filename) {
    std::ofstream f;
    f.open(filename,std::ios::out | std::ios::binary); 
    
    if(f.is_open()) {
        f.write(reinterpret_cast<char*>(&samplerate),sizeof(uint32_t));
        f.write(reinterpret_cast<char*>(&sigma),sizeof(uint32_t));
        f.write(reinterpret_cast<char*>(&I),sizeof(uint32_t));
        f.write(reinterpret_cast<char*>(&n),sizeof(uint32_t));
        f.write(reinterpret_cast<char*>(C),sizeof(uint32_t)*(size_uchar+1));
        f.write(reinterpret_cast<char*>(remap),sizeof(uint8_t)*size_uchar);
        f.write(reinterpret_cast<char*>(remap_reverse),sizeof(uint8_t)*sigma);
        f.write(reinterpret_cast<char*>(suffixes),sizeof(uint32_t)*((n/samplerate)+1));
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
        debug("Loading FM Index from file '%s'",filename);
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
        newIdx->T_bwt = WaveletTreeNoptrs::load(f);
        newIdx->sampled = BitSequenceRRR::load(f);
        f.close();
        debug("samplerate = %d",newIdx->samplerate);
        debug("sigma = %d",newIdx->sigma);
        debug("I = %d",newIdx->I);
        debug("n = %d",newIdx->n);
    } else {
        delete newIdx;
        return NULL;
    }
        
    return newIdx;
}

uint32_t
FM::count(uint8_t* pattern,uint32_t m) {
    uint8_t c = remap[pattern[m-1]]; 
    uint32_t i=m-1;
    uint32_t j = 1;
    
    uint32_t sp = C[c];
    uint32_t ep = C[c+1]-1;
    while (sp<=ep && i>=1) {
      c = remap[pattern[--i]];
            
      sp = C[c] + T_bwt->rank(c, sp-1);
      ep = C[c] + T_bwt->rank(c, ep)-1;
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
    
    /* count */
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
        /* sort */
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
FM::display(uint32_t start,uint32_t stop)
{
    uint8_t* T;
    
    return T;
}

uint8_t*
FM::reconstructText(uint32_t* size)
{
    uint8_t* T;
    uint8_t c;
    uint32_t j,i;
    
    T = (uint8_t*) safe_malloc( n * sizeof(uint8_t)  );
    
    j = I;
    
    for(i=0;i<n;i++) {
        c = T_bwt->access(j);
        T[n-i-1] = remap_reverse[c];
        j = C[c]+T_bwt->rank(c,j)-1;; // LF-mapping
    }
    
    *size = n; /* we added 0 during construction */
    
    return T;
}
