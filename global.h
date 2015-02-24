/* Maximum length of input line */
#define                 MAXLINELENGTH           20000

/* Characters to be skipped */
#define                 SKIPWORD(x)             \
                        (((x)<128) || (((x)<=0xF9)&&((x)>=0xF0)))

/* HIGH Chars */
#define                 HIGHWORD(x)             \
                        ((x)>=128)

/* Check level of a character */
#define                 NOTMIDDLE(x)            \
                        ((x)<0xD0?0:(levtable[(x)-0xD0]!=0))

#define                 GETLENGTH(x)            \
                        ((x)<-100?-(x)-100:((x)<0?-(x):(x)))

/* Never change this value. If you do, make sure it's below 255. */
#define			CUTCODE			254

/* Set this one will reduce output size with new TeX */
#define                 HIGHBIT                 1 

/* LISTSTACKDEPTH must be over 50,000 otherwise bench0.txt
will not complete */
#define                 LISTSTACKDEPTH          20000
#define                 CUTLISTSIZE             500
#define                 DIFLISTSIZE             500

#define                 MAXWORDLENGTH           500

