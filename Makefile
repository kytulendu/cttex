PACKAGE = cttex.c Makefile dictsort.c dict2c.pl tdict.txt README

all: cttex dictsort

cttex: cttex.c tdict.h
	gcc -O2 -o cttex cttex.c

dictsort: dictsort.c
	gcc -O2 -o dictsort dictsort.c

tdict.h: tdict.txt
	dict2c.pl tdict.txt > tdict.h

pack: $(PACKAGE)
	tar zcvf cttex.tar.gz $(PACKAGE)

clean:
	rm cttex dictsort tdict.h *~

