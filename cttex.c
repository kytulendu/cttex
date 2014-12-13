/* Thai word-separator by dictionary */
/* By Vuthichai A.                   */
/* --------------------------------- */

/* $Header: /home/hodaka/vuthi/tex/thai/hui/RCS/cttex.c,v 1.4 1994/12/14 10:50:18 vuthi Exp vuthi $
*/

/* Name of dictionary file */
#define DICTFILE "tdict.txt"

/* Maximum number of words in the dictionary */
#define MAXWORD 15000

/* Maximum length of one word in the dict */
#define MAXWORDLENGTH 40

/* Maximum length of input line */
#define MAXLINELENGTH 1000

/* Maximum number of WORDS in one line */
#define MW 40

/* Maximum number of words to LOOKBACK */
#define BACKDEPTH 3

/* Characters to be skipped */
#define SKIPWORD(x) \
		(((x)<128) || (((x)<=0xF9)&&((x)>=0xF0)))

/* HIGH Chars */
#define HIGHWORD(x) \
		(((x)>=128))

/* Check level of a character */
#define NOTMIDDLE(x) \
		((x)<0xD0?0:(levtable[(x)-0xD0]!=0))

#define	CUTCODE	254

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void readfile( unsigned char * );
void dooneline( unsigned char *, unsigned char * );
void savestatus( int*, int*, int*, int*, int*, int*, unsigned char *, int );
int mystrncmp( unsigned char *, unsigned char *, int );
int findword( unsigned char *, int * );
int countmatch( unsigned char *in, unsigned char *out );
void adj( unsigned char * );

/* Table Look-Up for level of a character */
/*
int levtable[]={
0,2,0,0,2,2,2,2,1,1,1,2,0,0,0,0,
0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
*/
int levtable[] = {
	0, 2, 0, 0, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 2, 3, 3, 3, 3, 3, 3, 3, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Pointer to each dictionary words */
unsigned char *wordptr[MAXWORD];
/* Number of words read into memory */
int numword;
int cutcode;

int main( argc, argv )
int argc;
char *argv[];
{
	FILE *fp, *fopen( );
	unsigned char str[MAXLINELENGTH], out[MAXLINELENGTH];
	int i, j, thaimode, c, cr;

	numword = 0;
	cutcode = CUTCODE;

	fprintf( stderr, "C-TTeX $Revision: 1.4 $\n" );
	if ( argc>1 ) {
		sscanf( argv[1], "%d", &cutcode );
	}

	readfile( DICTFILE );

	i = 0;
	fp = stdin;
	thaimode = cr = 0;
	while ( !feof( fp ) ) {
		fgets( str, MAXLINELENGTH - 1, fp );
		if ( !feof( fp ) ) {
			dooneline( str, out );
			if ( argc>1 ) {
				printf( "%s", out );
			} else {
				adj( out );			/* Choose appropriate WANNAYUK */
				j = 0;
				while ( c = out[j] ) {
					if ( !HIGHWORD( c ) && ( j == 0 ) && cr && thaimode ) {
						putchar( '}' );
						putchar( ' ' );
						cr = thaimode = 0;
					}
					if ( thaimode ) {
						if ( c == '\n' ) {
							/* Add a % before newline in Thai Mode */
							putchar( '%' );
							putchar( '\n' );
							cr = 1;	/* We got a CR in Thai mode */
						} else if ( !HIGHWORD( c ) ) {
							putchar( '}' );
							putchar( c );
							thaimode = 0;
						} else
							printf( "\\c%03d", c );
					} else {
						if ( !HIGHWORD( c ) )
							putchar( c );
						else {
							printf( "{\\thai\\c%03d", c );
							thaimode = 1;
						}
					}
					j++;
				}
			}
			i++;
			if ( i % 100 == 0 )
				fprintf( stderr, "%d\n", i );
		}
	}
	fprintf( stderr, "Done\n" );

	return 0;
}

/* Read dictionary file */
void readfile( unsigned char *fname ) {
	FILE *fp, *fopen( );
	unsigned char str[MAXWORDLENGTH];
	unsigned char ostr[MAXWORDLENGTH];
	int l;

	fp = fopen( fname, "r" );
	ostr[0] = 0;
	while ( !feof( fp ) ) {
		fgets( str, MAXWORDLENGTH - 1, fp );
		if ( !feof( fp ) ) {
			if ( strcmp( ostr, str ) >= 0 )
				fprintf( stderr, "Dictionary order error %s %s", ostr, str );
			wordptr[numword] = ( unsigned char * ) malloc( ( l = strlen( str ) ) + 2 );
			strcpy( wordptr[numword] + 1, str );
			strcpy( ostr, str );
			wordptr[numword][l] = 0;            /* Remove new line */
			wordptr[numword][0] = l - 1;
			numword++;
		}
	}
	fclose( fp );
	fprintf( stderr, "Reading dictionary done. %d\n", numword );
}

void dooneline( unsigned char *in, unsigned char *out ) {
	int i, j, k, l, old_i;
	int wlist[MW], poslist[MW], jlist[MW], windex,
		pos, fence, backmode;
	int prev_error = 0;

	i = old_i = j = 0;
	windex = 0;
	fence = 0;
	backmode = 0;
	prev_error = 0;

	while ( in[i] ) {

		/* old_i is the value of i before looking back mode */
		/* i>old_i means looking back was successful, cancel it */
		if ( i>old_i )
			backmode = 0;

		if ( SKIPWORD( in[i] ) ) {                 /* Chars to be skipped ? */
			if ( prev_error ) {
				/* out[j++] = '!';
				*/
				prev_error = 0;
			}
			backmode = fence = windex = 0;          /* Begin new word list */
			while ( SKIPWORD( in[i] ) && in[i] ) {   /* Skip English char */
				out[j++] = in[i++];
			}
		}
		if ( in[i] )                             /* Still not EOL ? */
			do {
				if ( k = findword( in + i, &pos ) ) {         /* Found in dict */
					if ( prev_error ) {
						/* out[j++] = '!';
						*/
						prev_error = 0;
					}
					wlist[windex] = i;
					poslist[windex] = pos;
					jlist[windex] = j;
					windex++;

					/* For speed, limit the number of words to LOOK back */
					/* by creating a fence */
					/* Fence may only INCREASE */
					if ( windex - BACKDEPTH > fence ) {
						fence = windex - BACKDEPTH;
					}

					for ( l = 0; l<k; l++ )                /* Copy word */
						out[j++] = in[i++];

					/* Mai Ya Mok & Pai Yan Noi */
					/*	I was using this code before adding these two characters
					into the dict. Now they are in the dict and I no longer need
					these two lines.
					*/
					while ( ( in[i] == 0xE6 ) || ( in[i] == 0xCF ) )
						out[j++] = in[i++];

					out[j++] = cutcode;                   /* Insert word sep symbol */
				} else {                            /* Not in Dict */

					/* Shortening the prev wordS may help */
					/* Try to Look Back */
					while ( ( windex > fence ) && !k ) {
						/* Save status before looking back */
						if ( !backmode ) {
							backmode = 1;
							savestatus( &windex, wlist, poslist, jlist, &i, &j, out, 1 );
							old_i = i;
						}
						pos = poslist[windex - 1] - 1;        /* Skip back one word */
						while ( ( pos >= 0 ) &&
							( l = countmatch( wordptr[pos] + 1, in + wlist[windex - 1] ) ) ) {
							if ( ( l == wordptr[pos][0] ) &&
								!NOTMIDDLE( in[wlist[windex - 1] + l] ) ) {
								k = 1; break;
							}
							pos--;
						}
						/* A shorter version of prev word found */
						if ( k ) {
							out[j = jlist[windex - 1] + l] = cutcode;
							poslist[windex - 1] = pos;
							j++;
							i = wlist[windex - 1] + l;
						} else {
							if ( backmode && ( windex == fence + 1 ) ) {
								/* Search-Back method can't help, restore prev status */
								savestatus( &windex, wlist, poslist, jlist, &i, &j, out, 0 );
								break;
							}
							windex--;
						}
					}
					/* Sure that word is not in dictionary */
					if ( k == 0 ) {
						prev_error = 1;               /* Begin unknown word area */
						out[j++] = in[i++];             /* Copy it */
						backmode = fence = windex = 0;  /* Clear Word List */
					}
				}
			} while ( ( k == 0 ) && ( !SKIPWORD( in[i] ) ) );
	}
	out[j] = 0;
}

/* Sequential verion */
/*
int findword(unsigned char *in)
{
	int i;

	for(i=numword-1;i>=0;i--) {
		if(mystrncmp(in,wordptr[i]+1,wordptr[i][0])==0) {
			printf("Found : %s %d\n", wordptr[i]+1,wordptr[i][0]);
			return wordptr[i][0];
		}
	}
	return 0;
}
*/

/* Calling : Index to a string
Return  : Length of recognized word, and position of that word in
dictionary
Binary search method
*/
int findword( unsigned char *in, int *pos ) {
	int up, low, mid, a, l;

	up = numword - 1;              /* Upper bound */
	low = 0;                      /* Lower bound */
	*pos = -1;                    /* If word not found */

	/* Found word at the boundaries ? */
	if ( mystrncmp( in, wordptr[up] + 1, wordptr[up][0] ) == 0 ) {
		*pos = up;
		return wordptr[up][0];
	}
	if ( mystrncmp( in, wordptr[low] + 1, wordptr[low][0] ) == 0 ) {
		mid = low;
	} else {                        /* Begin Binary search */
		do {
			mid = ( up + low ) / 2;
			a = mystrncmp( in, wordptr[mid] + 1, wordptr[mid][0] );
			/*
			printf("%d %d %d %s\n",low, mid, up,  wordptr[mid]+1);
			*/
			if ( a != 0 ) {
				if ( a>0 )
					low = mid;
				else
					up = mid;
			}
		} while ( ( a != 0 ) && ( up - low>1 ) );
		if ( a != 0 ) {                            /* Word not found */
			mid--;
			if ( !countmatch( wordptr[mid] + 1, in ) )  /* Can we find the shorter word ? */
				return 0;                         /* No, */

			while ( mid && ( l = countmatch( wordptr[mid] + 1, in ) ) ) {
				if ( ( l == wordptr[mid][0] ) && !NOTMIDDLE( in[l] ) ) {
					*pos = mid;
					return l;
				}
				mid--;
			}
			if ( a ) return 0;
		}
	}

	up = mid;
	if ( up < numword )
		do {          /* Find the longest match */
			up++;
			a = mystrncmp( in, wordptr[up] + 1, wordptr[up][0] );
			if ( a == 0 ) mid = up;
		} while ( ( a >= 0 ) && ( up<numword - 1 ) );
		/*
		printf("Found : %s %d\n", wordptr[mid]+1,wordptr[mid][0]);
		*/
		*pos = mid;
		return wordptr[mid][0];
}

int countmatch( unsigned char *in, unsigned char *out ) {
	int i;

	i = 0;
	while ( in[i] == out[i] )
		i++;
	return i;
}

void savestatus( int* windex, int* wlist, int* poslist, int* jlist,
	int* oi, int* j, unsigned char * out, int mode ) {
	static int lwindex, lwlist[MW], lposlist[MW], ljlist[MW], li, lj;
	int i;
	static unsigned char lout[MAXLINELENGTH];

	/*
	printf("Save call %d\n",mode);
	*/
	if ( mode ) {            /* Save */
		lwindex = *windex;
		for ( i = 0; i<lwindex; i++ ) {
			lwlist[i] = wlist[i];
			lposlist[i] = poslist[i];
			ljlist[i] = jlist[i];
		}
		for ( i = 0; i<*j; i++ )
			lout[i] = out[i];
		li = *oi;
		lj = *j;
	} else {
		*windex = lwindex;
		for ( i = 0; i<lwindex; i++ ) {
			wlist[i] = lwlist[i];
			poslist[i] = lposlist[i];
			jlist[i] = ljlist[i];
		}
		for ( i = 0; i<lj; i++ )
			out[i] = lout[i];
		*oi = li;
		*j = lj;
	}
}

/* Thai version of strncmp :
b must be the word from dictionary
*/
int mystrncmp( a, b, l )
unsigned char *a, *b;
int l;
{
	int i;

	i = strncmp( a, b, l );
	if ( i )
		return i;
	else {
		return( NOTMIDDLE( a[l] ) );
	}
}

#define PORPAR 187
#define FORFAR 189
#define FORFUN 191
#define MAITAI 231
#define MAIHAN 209
#define SARAAMP 211
#define YORYING 173
#define isyol(x) ((x)==PORPAR||(x)==FORFAR||(x)==FORFUN)

void adj( line )
unsigned char *line;
{
	unsigned char top[MAXLINELENGTH];
	unsigned char up[MAXLINELENGTH];
	unsigned char middle[MAXLINELENGTH];
	unsigned char low[MAXLINELENGTH];

	int i, k, c;

	/* Split string into 4 levels */

	/* Clear Buffer */
	for ( i = 0; i<MAXLINELENGTH; i++ )
		top[i] = up[i] = middle[i] = low[i] = 0;

	i = 0; k = -1;
	while ( c = line[i++] ) {
		switch ( ( c>0xD0 ) ? levtable[c - 0xD0] : 0 ) {
		case 0: /*Middle*/
			k++;
			middle[k] = c; break;
		case 1: /*Low*/
			low[k] = c; break;
		case 2: /*Up*/
			up[k] = c; break;
		case 3: /*Top*/
			top[k] = c; break;
		}
	}

	/* Beauty Part */
	/* Check through each condition */
	for ( i = 0; middle[i] != '\n'; i++ ) {
		if ( isyol( middle[i] ) && middle[i + 1] != SARAAMP ) {
			if ( up[i] != 0 ) {
				if ( up[i] != MAIHAN&&up[i] != MAITAI )
					up[i] = up[i] - 64; /*SARA for PORPAR*/
				if ( up[i] == MAITAI )
					up[i] = up[i] - 84; /*MAITAIKOOL for PORPAR*/
				if ( up[i] == MAIHAN )
					up[i] = up[i] - 63; /*MAIHANAREKARD for PORPAR*/
				if ( top[i] != 0 )
					top[i] = top[i] - 80; /*MAIEK for PORPAR and SARA*/
			} else {
				if ( top[i] != 0 )
					top[i] = top[i] - 101; /*MAIEK for PORPAR*/
			}
		} else {
			if ( top[i] != 0 && up[i] == 0 && middle[i + 1] != SARAAMP )
				top[i] = top[i] - 96; /*MAIEK for BORBAIMAI*/
		}
		if ( middle[i] == YORYING&&low[i] != 0 )
			middle[i] = 144; /*YORYING for SARAUOO*/
		if ( middle[i + 1] == SARAAMP&&top[i] != 0 )
			top[i] = top[i] - 80; /*MAIEK for SARAAMP*/
	}

	/* Pack Back To Line */

	i = 0; k = 0;
	while ( middle[i] ) {
		line[k++] = middle[i];
		if ( low[i] ) line[k++] = low[i];
		if ( up[i] )  line[k++] = up[i];
		if ( top[i] ) line[k++] = top[i];
		i++;
	}
}


/*
* $Log: cttex.c,v $
* Revision 1.4  1994/12/14  10:50:18  vuthi
* Command Line Option, use "%" to terminal Thai lines
*
* Revision 1.3  1994/12/14  10:12:22  vuthi
* Add Header Revision and Log
*
*/
