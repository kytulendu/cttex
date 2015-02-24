MYOBJ = cttex.o map.o cache.o findword.o
# DEBUG = -g
DEBUG = -O2
CC = gcc
# CC = icc

all: cttex dict2state digdict

cttex: $(MYOBJ)
	$(CC) $(DEBUG) -o cttex $(MYOBJ)

cttex.o: cttex.c global.h cache.h map.c findword.h
	$(CC) $(DEBUG) -c cttex.c

cache.o: cache.c cache.h global.h
	$(CC) $(DEBUG) -c cache.c

map.o: map.c
	$(CC) $(DEBUG) -c map.c

findword.o: findword.c findword.h map.c
	$(CC) $(DEBUG) -c findword.c

dictsort: dictsort.c
	$(CC) $(DEBUG) -o dictsort dictsort.c

dict2state: dict2state.c
	$(CC) $(DEBUG) -o dict2state dict2state.c

digdict: digdict.c findword.o map.o
	$(CC) $(DEBUG) -o digdict digdict.c findword.o map.o

map.c: tdict.txt dict2state
	./dict2state

tdict.txt: tdict.org tdict.hui huiri.txt fixlist.txt
	cat tdict.org tdict.hui huiri.txt fixlist.txt | LANG=C  sort | uniq > tdict.txt

clean:
	rm cttex dictsort dict2state tdict.h map.c map.h digdict *~ *.o \
	tdict.sorted tdict.txt

# Change Log
# $Log: Makefile,v $
# Revision 1.6  2004/03/03 09:15:04  vuthi
# Fix dependency on map.c
#
# Revision 1.5  2004/03/03 09:10:14  vuthi
# Fix make clean to remove digdict
#
# Revision 1.4  2004/03/03 05:10:43  vuthi
# Add option to use ICC instead of GCC
#

