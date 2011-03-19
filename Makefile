
CCP=g++
#CFLAGS=-g -W -Wall -O0 -DDEBUG
CFLAGS=-W -Wall -O3

INCCDS=./libcds/includes/
INCDIVSUF=./libdivsufsort/include/

all: ./libcds/lib/libcds.a libdivsufsort/lib/libdivsufsort.a fmbuild fmcount fmlocate fmrecover fmextract

./libcds/lib/libcds.a: 
	$(MAKE) -C libcds
	
libdivsufsort/lib/libdivsufsort.a: 
	cd libdivsufsort;cmake .;$(MAKE);cd ..
	
libfmindex.a:  util.o  FM.o 
	ar rcs libfmindex.a util.o FM.o
	
fmbuild: fmbuild.cpp ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a libfmindex.a
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) $(CFLAGS) -o fmbuild fmbuild.cpp libfmindex.a ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a 

fmcount: fmcount.cpp ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a libfmindex.a
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) $(CFLAGS) -o fmcount fmcount.cpp libfmindex.a ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a 
	
fmlocate: fmlocate.cpp ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a libfmindex.a
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) $(CFLAGS) -o fmlocate fmlocate.cpp libfmindex.a ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a

fmextract: fmextract.cpp ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a libfmindex.a
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) $(CFLAGS) -o fmextract fmextract.cpp libfmindex.a ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a

fmrecover: fmrecover.cpp ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a libfmindex.a
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) $(CFLAGS) -o fmrecover fmrecover.cpp libfmindex.a ./libdivsufsort/lib/libdivsufsort.a ./libcds/lib/libcds.a
	
# pattern rule for all objects files
%.o: %.c *.h
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) -c $(CFLAGS) $< -o $@

%.o: %.cpp *.h
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) -c $(CFLAGS) $< -o $@

clean:
	rm -f fmbuild fmcount fmlocate fmextract fmrecover libfmindex.a *~ *.o ;
	
cleanall:
	rm -f fmbuild fmcount fmlocate fmextract fmrecover libfmindex.a *~ *.o ; cd libdivsufsort; make -f Makefile clean ; cd .. ; cd libcds ; make -f Makefile clean ; cd ..
