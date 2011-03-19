FM-Index - Compressed full-text Index
=====================================

A simple c++ FM-Index [1] implementation using RRR [4] wavelet trees [5]
which allows to build a full-text index over a given text `T` supporting the
following operations:

  * `count(P)`     : count the number of occurences of `P` in `T`.
  * `locate(P)`    : locate the text positions of all occurences of `P` in `T`.
  * `extract(A,B)` : extract `T[A,B]`.
  * `recover()`    : recover `T` from the index.
  
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

	
Benchmarks
----------

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
	<td>proteins</td><td align=left>Sequence of newline-separated protein sequences</td><td>25</td><td>79.15</td>
  </tr>
  <tr>
	<td>dna</td><td align=left>Gene DNA sequences</td><td>4</td><td>1.97</td>
  </tr>
  <tr>
	<td>xml</td><td align=left>XML that provides bibliographic information on major computer science journals and proceedings (dblp)</td><td>96</td><td>5.26</td>
  </tr>
</table>

### Construction

<table>
  <tr>
    <th>File</th><th>Size [MB]</th><th>Time [sec]</th><th>Memory [MB]</th>
  </tr>
  <tr>
    <th>wsj</th><th>3</th><th>2.157</th><th>20.02</th>
  </tr>
  <tr>
	<th>wsj</th><th>6</th><th>4.487</th><th>39.73</th>
  </tr>
  <tr>
	<th>wsj</th><th>12</th><th>9.293</th><th>79.15</th>
  </tr>
  <tr>
	<th>wsj</th><th>25</th><th>19.028</th><th>158.0</th>
  </tr>
  <tr>
	<th>wsj</th><th>50</th><th>38.699</th><th>315.7</th>
  </tr>
  <tr>
	<th>wsj</th><th>100</th><th>78.287</th><th>631.2</th>
  </tr>
  <tr>
	<th>wsj</th><th>200</th><th>158.341</th><th>1233</th>
  </tr>
</table>


### Count


### Locate


### Extract


Libraries
---------

The following libraries are needed to use the index. Sourcecode for both
libaries is included.

 * [libcds](http://libcds.recoded.cl/) -- succinct low level data structures
 * [libdivsufsort](http://code.google.com/p/libdivsufsort/) -- fast suffix sorting

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

Matthias Petri <Matthias.Petri@gmail.com>
