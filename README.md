FM-Index - Compressed full-text Index
=====================================

A simple c++ FM-Index [1] implementation using RRR [4] wavelet trees [5]
which allows to build a full-text index over a given text `T` supporting the
following operations:

  * `count(P)`     : count the number of occurences of `P` in `T`.
  * `locate(P)`    : locate the text positions of all occurences of `P` in `T`.
  * `display(A,B)` : extract `T[A,B]` 
  
Usage
-----

### Compiling the Index

	make

### Command Parameters

	./fm-index
	USAGE: ./fm-index -i <index> -q <queries> -k <depth> -s <samplerate> -r -l -d <input>
		-i index file
		-d input file
		-q query file
		-s samplerate [default = 64]
		-l locate occurrences
		-r recover original text

### Examples

#### Building an Index

	./fm-index -d alice29.txt
	
Builds and writes the FM-Index `alice29.txt.fm`.

#### Running count() queries

	./fm-index -i alice29.txt.fm -q alice.qry

The queries are stored in a new line seperated file:

	the
	house
	keep
	Alice
	and
	
The index returns the following results:

	./fm-index -i alice29.txt.fm -q alices.qry
	Read 5 queries
	the : 2101
	house : 20
	keep : 11
	Alice : 395
	and : 880
	Finished processing queries: 0.000 sec
	
#### Running locate() queries
	
Testing
-------

TODO
 
Benchmarks
----------

### Construction


### Count


### Locate


### Display


Libraries
---------

The following libraries are needed to use the index. Sourcecode for both
libaries is included.

 * [libcds](http://libcds.recoded.cl/) -- succinct low level data structures
 * [libdivsufsort](http://code.google.com/p/libdivsufsort/) -- fast suffix sorting

Licence
--------

GPL v3

References
-----------

 1. Paolo Ferragina and Giovanni Manzini. Indexing compressed text. Journal of the ACM, 52(4):552-581, 2005.
 2. Francisco Claude and Gonzalo Navarro. Practical Rank/Select Queries over Arbitrary Sequences. Proc. SPIRE'08 176-187, 2008.
 3. Veli M"akinen and Gonzalo Navarro. Implicit Compression Boosting with Applications to Self-Indexing. Proc. SPIRE'07 214-226.
 4. R. Raman, V. Raman, and S. Srinivasa Rao. Succinct indexable dictionaries with applications to encoding k-ary trees and multisets. In SODA'02, 233-242.
 5. R. Grossi, A. Gupta, and J. Vitter. High-order entropy-compressed text indexes. In SODA'03, 841-850.
 
 Author
 ------
 
 Matthias Petri -- Matthias.Petri@gmail.com