PACKAGE = cttex.c Makefile dictsort.c dict2c.pl tdict.txt README

all: cttex dictsort

cttex: cttex.c tdict.h
	gcc -o cttex cttex.c

dictsort: dictsort.c
	gcc -O2 -o dictsort dictsort.c

tdict.h: tdict.sorted
	dict2c.pl tdict.sorted > tdict.h

tdict.sorted: tdict.txt dictsort
	dictsort

tdict.txt: tdict.org tdict.hui
	cat tdict.org tdict.hui > tdict.txt

pack: $(PACKAGE)
	tar zcvf cttex.tar.gz $(PACKAGE)

clean:
	rm cttex dictsort tdict.h *~

