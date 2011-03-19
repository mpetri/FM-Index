FM-Index - Compressed full-text Index
=====================================

A simple c++ FM-Index [1] implementation using RRR [4] wavelet trees [5]
which allows to build a full-text index over a given text `T` supporting the
following operations:

  * `count(P)`     : count the number of occurences of `P` in `T`.
  * `locate(P)`    : locate the text positions of all occurences of `P` in `T`.
  * `display(A,B)` : extract `T[A,B]`.
  * `recover()`    : recover `T` from the index.
  
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
	
The index returns the **number of occurrences** for each query:

	./fm-index -i alice29.txt.fm -q alices.qry
	Read 5 queries
	the : 2101
	house : 20
	keep : 11
	Alice : 395
	and : 880
	Finished processing queries: 0.000 sec
	
#### Running locate() queries

	./fm-index -i alice29.txt.fm -q alice.qry -l

	
The index returns a sorted list of the **locations of all occurences** for each query:

	./fm-index -i alice29.txt.fm -q alices.qry -l
	Read 3 queries
	keep (11) : 46385 51125 69491 74680 81562 83046 104830 105180 133621 149966 151623
	poison (3) : 8151 8619 8731
	tomorrow (1) : 63637
	Finished processing queries: 0.008 sec
	
#### Recover the original text from the index

	./fm-index -i alice29.txt.fm -r
	
The index outputs the original text to `stdout`.

#### Running display() queries
	
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
