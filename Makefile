
CCP=g++
#CFLAGS=-g -W -Wall -O0 -DDEBUG
CFLAGS=-W -Wall -O3

INCCDS=./libcds/includes/
INCDIVSUF=./libdivsufsort/include/

all: index

./libcds/lib/libcds.a: 
	$(MAKE) -C libcds
	
libdivsufsort/lib/libdivsufsort.a: 
	cd libdivsufsort;cmake .;$(MAKE);cd ..

index: fm-index.cpp FM.o util.o ./libcds/lib/libcds.a ./libdivsufsort/lib/libdivsufsort.a
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) $(CFLAGS) -o fm-index fm-index.cpp FM.o util.o ./libcds/lib/libcds.a ./libdivsufsort/lib/libdivsufsort.a

# pattern rule for all objects files
%.o: %.c *.h
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) -c $(CFLAGS) $< -o $@

%.o: %.cpp *.h
	$(CCP) -I $(INCCDS) -I $(INCDIVSUF) -c $(CFLAGS) $< -o $@

clean:
	rm -f *~ *.o ; cd libdivsufsort; make -f Makefile clean ; cd .. ; cd libcds ; make -f Makefile clean ; cd ..
