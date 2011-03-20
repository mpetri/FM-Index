FM-Index - Compressed full-text Index
=====================================

A simple C++ based FM-Index [1] implementation using RRR [4] wavelet trees [5]
which allows to build a full-text index over a given text `T` of size `n`
supporting the following operations:

  * `count(P,m)`   : count the number of occurences of pattern `P`  of size `m` in `T`.
  * `locate(P,m)`  : locate the text positions of all occurences of `P` of size `m` in `T`.
  * `extract(A,B)` : extract `T[A,B]` from the index.
  * `recover()`    : recover `T` from the index.
  
The constructed index uses `nH_k + o(n log sigma)` bits of space [3] which is roughly the size of
the **compressed** representation of `T` and can perform the above operations without the need
to store `T`. An empirical evaluation of the index is sown in the Benchmark section below.

Drawbacks of the FM-Index is long construction time and high memory requirements during construction.
  
Usage
-----

### Compiling the Index

	make

### Building an Index

	./fmbuild alice29.txt alice29.txt.fm
	
Builds and writes the FM-Index `alice29.txt.fm`.

### Running count() queries

	./fmcount -i alice29.txt.fm alice.qry

The queries are stored in a new line seperated file:

	the
	house
	keep
	Alice
	and
	
The index returns the **number of occurrences** for each query:

	./fmcount -i alice29.txt.fm alice.qry
	the : 2101
	house : 20
	keep : 11
	Alice : 395
	and : 880
	
### Running locate() queries

	./fmlocate -i alice29.txt.fm alice.qry

	
The index returns a sorted list of the **locations of all occurences** for each query:

	./fmlocate -i alice29.txt.fm alice.qry
	Read 3 queries
	keep (11) : 46385 51125 69491 74680 81562 83046 104830 105180 133621 149966 151623
	poison (3) : 8151 8619 8731
	tomorrow (1) : 63637

### Running extract() queries
	
	./fmextract -i alice29.txt.fm alice.extract
	
The queries are stored in a new line seperated file:

	118 147
	1213 1245
	24 55

The index returns the **extracted text snippet** for each query:

	./fmextract -i alice29.txt.fm alice.extract
	118 - 147 : 'THE MILLENNIUM FULCRUM EDITION'
	1213 - 1245 : 'TOOK A WATCH OUT OF ITS WAISTCOAT'
	24 - 55 : 'ALICE'S ADVENTURES IN WONDERLAND'
	
### Recover the original text from the index

	./fmrecover -i alice29.txt.fm 
	
The index outputs the original text to `stdout`.

### Verbose output

The `-v` command line parameter enables verbose messages:

	./fmbuild -v alice29.txt
	building index.
	- remapping alphabet.
	- creating cumulative counts C[].
	- performing bwt.
	- sample SA locations.
	- creating bwt output.
	- create RRR wavelet tree over bwt.
	build FM-Index done. (0.101 sec)
	space usage:
	- remap_reverse: 75 bytes (0.07%)
	- C: 1028 bytes (0.90%)
	- Suffixes: 9508 bytes (8.31%)
	- Positions: 9512 bytes (8.31%)
	- Sampled: 7948 bytes (6.95%)
	- T_bwt: 86088 bytes (75.23%)
	input Size n = 152090 bytes
	index Size = 114431 bytes (0.75 n)
	writing FM Index to file 'alice29.txt.fm'
	
	
Using the FM-Index to provide full-text search on a given text `T`
------------------------------------------------------------------

A small code example illustrating the use of the FM class:

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include "FM.h"

	int
	main(int argc,char** argv) {
		uint8_t* T = /* read text */
		uint32_t n = /* sizeof T */
		FM* = new FM(T,n);
		if(FM) {
			/* count the occurences of 'house' in T */
			uint32_t cnt = FM->count("house",strlen("house"));
			
			/* get all locations offsets of 'house' in T */
			uint32_t matches; /* number of matches */
			uint32_t* locs; /* list of location offsets */
			locs = FM->locate("house",strlen("house"),&matches);
			
			/* extract text snippet from T */
			uint8_t* snippet = FM->extract(5,251);
			
			/* recover T from index */
			uint32_t nnew; /* size of Tnew */
			uint8_t* Tnew = FM->reconstructText(&nnew);
			
			delete FM;
		}
	}
	
Compile with `g++ -o test main.cpp FM.cpp util.c libcds.a libdivsufsort.a`

Benchmarks
----------

Experiments are similar to the ones described in [1]. All experiments were run on a `Intel(R) Core(TM)2 Duo CPU E8500 @ 3.16GHz` using 4GB of RAM. 
The default samplerate `s=64` was used in all experiments.

### Test data

Test data was taken from the [The Pizza&Chili Site](http://pizzachili.di.unipi.it/) and TREC. 

<table>
  <tr>
    <th>File</th><th align=left>Description</th><th>Alphabet size</th><th>Entropy (bps)</th>
  </tr>
  <tr>
    <td>wsj</td><td align=left >English Text taken from the TREC wsj collection</td><td>90</td><td>4.60</td>
  </tr>
  <tr>
	<td>src</td><td align=left>Concatenated source code (.c,.h,.C,.java) of linux-2.6.11.6 and gcc-4.0.0</td><td>230</td><td>5.47</td>
  </tr>
  <tr>
	<td>proteins</td><td align=left>Sequence of newline-separated protein sequences</td><td>25</td><td>4.20</td>
  </tr>
  <tr>
	<td>dna</td><td align=left>Gene DNA sequences</td><td>4</td><td>1.97</td>
  </tr>
  <tr>
	<td>xml</td><td align=left>XML that provides bibliographic info on compsci pubs(dblp)</td><td>96</td><td>5.26</td>
  </tr>
</table>

### Construction

Peak memory usage was measured using the `valgrind --tool=massif` tool. Running time was measured using the `gettimeofday()` system call.

<table>
  <tr>
    <th>File</th><th>Size [MB]</th><th>Time [sec]</th><th>Memory [MB]</th>
  </tr>
  <tr>
    <td>wsj</td><td>3</td><td>2.157</td><td>20.02</td>
  </tr>
  <tr>
	<td>wsj</td><td>6</td><td>4.487</td><td>39.73</td>
  </tr>
  <tr>
	<td>wsj</td><td>12</td><td>9.293</td><td>79.15</td>
  </tr>
  <tr>
	<td>wsj</td><td>25</td><td>19.028</td><td>158.0</td>
  </tr>
  <tr>
	<td>wsj</td><td>50</td><td>38.699</td><td>315.7</td>
  </tr>
  <tr>
	<td>wsj</td><td>100</td><td>78.287</td><td>631.2</td>
  </tr>
  <tr>
	<td>wsj</td><td>200</td><td>158.341</td><td>1233</td>
  </tr>
  <tr>
	<td>src</td><td>50</td><td>39.909</td><td>315.7</td>
  </tr>
  <tr>
	<td>src</td><td>100</td><td>80.489</td><td>631.2</td>
  </tr>
  <tr>
	<td>src</td><td>200</td><td>163.072</td><td>1233</td>
  </tr>
  <tr>
	<td>proteins</td><td>50</td><td>35.374</td><td>315.7</td>
  </tr>
  <tr>
	<td>proteins</td><td>100</td><td>72.824</td><td>631.2</td>
  </tr>
  <tr>
	<td>proteins</td><td>200</td><td>146.173</td><td>1233</td>
  </tr>
  <tr>
	<td>dna</td><td>50</td><td>25.941</td><td>315.7</td>
  </tr>
  <tr>
	<td>dna</td><td>100</td><td>53.287</td><td>631.2</td>
  </tr>
  <tr>
	<td>dna</td><td>200</td><td>109.449</td><td>1233</td>
  </tr>  
  <tr>
	<td>xml</td><td>50</td><td>36.601</td><td>315.7</td>
  </tr>
  <tr>
	<td>xml</td><td>100</td><td>74.104</td><td>631.2</td>
  </tr>
  <tr>
	<td>xml</td><td>200</td><td>150.050</td><td>1233</td>
  </tr>  
</table>

Construction time increases linearly `O(n)` with size `n` of `T`. Memory requirement is roughly `6n` independent of the file type.

<table>
  <tr>
    <th>File</th><th>Size [MB]</th><th>Index Size [MB]</th><th>Index Size [relative to file size]</th>
  </tr>
  <tr>
    <td>wsj</td><td>25</td><td>15</td><td>0.6</td>
  </tr>
  <tr>
	<td>wsj</td><td>50</td><td>29</td><td>0.58</td>
  </tr>
  <tr>
	<td>wsj</td><td>100</td><td>57</td><td>0.57</td>
  </tr>
  <tr>
	<td>wsj</td><td>200</td><td>112</td><td>0.56</td>
  </tr>
  <tr>
	<td>src</td><td>50</td><td>30</td><td>0.77</td>
  </tr>
  <tr>
	<td>src</td><td>100</td><td>59</td><td>0.77</td>
  </tr>
  <tr>
	<td>src</td><td>200</td><td>117</td><td>0.75</td>
  </tr>
  <tr>
	<td>proteins</td><td>50</td><td>36</td><td>0.72</td>
  </tr>
  <tr>
	<td>proteins</td><td>100</td><td>71</td><td>0.71</td>
  </tr>
  <tr>
	<td>proteins</td><td>200</td><td>137</td><td>0.685</td>
  </tr>
  <tr>
	<td>dna</td><td>50</td><td>24</td><td>0.48</td>
  </tr>
  <tr>
	<td>dna</td><td>100</td><td>48</td><td>0.48</td>
  </tr>
  <tr>
	<td>dna</td><td>200</td><td>95</td><td>0.475</td>
  </tr>
  <tr>
	<td>xml</td><td>50</td><td>25</td><td>0.50</td>
  </tr>
  <tr>
	<td>xml</td><td>100</td><td>50</td><td>0.50</td>
  </tr>
  <tr>
	<td>xml</td><td>200</td><td>99</td><td>0.495</td>	
  </tr>  
</table>

Index size depends on the compressability of `T`. For each specific file type, as `n` increases, 
the size of the auxillary data becomes less significant which leads to better overall compression ratio.

### Count

50000 random patterns for each length `m=5 to 50` were extracted from each 200MB file. 

![FM-Index count() - 200MB - 50000 Queries](https://github.com/mpetri/FM-Index/raw/master/benchmark/FM_count_200MB_Q50000.png)

The graph shows the time in seconds it took the FM-Index to answer all 50000 queries on different file types. Note that the query
time grows linearly `O(m)` with the pattern length. Files with smaller alphabet show faster query times overall.

Also note that the query time (`m=20`) does **not** depend on the size `n` of the text `T`:

<table>
  <tr>
    <th>File</th><th>Size [MB]</th><th>Query Time [sec]</th>
  </tr>
  <tr>
    <td>wsj</td><td>3</td><td>13.37</td>
  </tr>
  <tr>
    <td>wsj</td><td>6</td><td>11.96</td>
  </tr>
  <tr>
    <td>wsj</td><td>12</td><td>12.74</td>
  </tr>
  <tr>
    <td>wsj</td><td>25</td><td>12.74</td>
  </tr>
  <tr>
    <td>wsj</td><td>50</td><td>12.07</td>
  </tr>
  <tr>
    <td>wsj</td><td>100</td><td>14.59</td>
  </tr>
  <tr>
    <td>wsj</td><td>200</td><td>14.34</td>
  </tr>
</table>

Overall, searching for 50000 patterns in `T` using a FM-Index is fast but requieres considerable amount of upfront work (time+space).

### Locate

`k` queries pf length `5` are randomly selected from `T` so they roughly amount to a certain number of total occurences over all queries.

![FM-Index locate() - 200MB - Query length 5](https://github.com/mpetri/FM-Index/raw/master/benchmark/FM_locate_200MB_QL5.png)

Note the running time increases linearly with the number of occurrences. Similar to `count()`, files with small alphabet size perform 
better most likely due to decreased height of the wavelet tree. Comparing the running times of both `count()` and `locate()` we observe
that `locate()` is significantly slower than `count()` as patterns (especially short ones), on large text collections, tend to have many occurrences.

The samplerate `s` (default = 64) determines how often the underlying suffix array is sampled to enable faster `locate()` calls. For different samplerates
the index achieves different time and space trade-offs during the `locate()` operation:

![FM-Index locate() - 50MB - Query length 6 - 50000 occurrences - variable samplerate](https://github.com/mpetri/FM-Index/raw/master/benchmark/FM_locate_samplerate_50MB_QL6.png)

The graph above shows the time required to retrieve the locations of 50000 occurrences compared to the size of the FM-Index for different suffix samplerates.
Smaller samplerate decrease the `locate()` running time but the space used by the index increases. For `s=8` the index is 1.5 times the sizes of the original text. 
For `s=512`, that is only every 512th position in the underlying suffix array is sampled, the index is less than 50% of the original text. The above graph shows that the
best time-space trade-off is achieved for samplerates between 64 and 128.

### Extract

Extracting snippets of `T` from the index shows the same properties as the `locate()` operation. Like `locate()`, the running time of `extract()` grows linearly with the size of the
size of the snippet. The running time is also influenced by the samplerate `s` which provides the same time and space trade-offs described above. 

Libraries
---------

The following libraries are needed to use the index. Sourcecode for both
libaries is included.

 * [libcds](http://libcds.recoded.cl/) -- succinct low level data structures
 * [libdivsufsort](http://code.google.com/p/libdivsufsort/) -- fast suffix sorting (requires cmake to build)

Licence
--------

GPL v3 (see LICENCE file)

References
-----------

 1. Paolo Ferragina and Giovanni Manzini. Indexing compressed text. 
    Journal of the ACM, 52(4):552-581, 2005.
 2. Francisco Claude and Gonzalo Navarro. Practical Rank/Select Queries over Arbitrary Sequences. 
    SPIRE'08 176-187, 2008.
 3. Veli M"akinen and Gonzalo Navarro. Implicit Compression Boosting with Applications to Self-Indexing. 
    SPIRE'07 214-226.
 4. R. Raman, V. Raman, and S. Srinivasa Rao. Succinct indexable dictionaries with applications to encoding k-ary trees and multisets. 
    SODA'02, 233-242.
 5. R. Grossi, A. Gupta, and J. Vitter. High-order entropy-compressed text indexes. 
    SODA'03, 841-850.
 6. [The Pizza&Chili Site](http://pizzachili.di.unipi.it/).

Author
------

Matthias Petri <Matthias.Petri@gmail.com> see [github](http://github/com/mpetri/FM-Index).
