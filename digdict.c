/*
$Log: digdict.c,v $
Revision 1.3  2004/02/24 08:17:12  vuthi
Improve the speed of findbegin() by checking first, using trie,
if the prefix is valid before performing sequential search.


*/

#include "findword.h"
#include "global.h"
#include <stdio.h>

extern int numword;
extern int mincol;
extern int maxcol;
extern int maxstate;
extern int map[388952];
extern unsigned char state[105835];
extern unsigned char state_min[105835];
extern unsigned char state_max[105835];
extern int state_offset[105835];

#define	DICTFILE "tdict.txt"
#define	MAXWORD 70000

char allwords[MAXWORD][MAXWORDLENGTH];

char lstr[100][MAXWORDLENGTH];
char rstr[100][MAXWORDLENGTH];

int lnum, rnum;
int next;
int allow = 1;

int wordcount;
int maxdepth;

void readfile( unsigned char *fname ) {
	FILE *fp, *fopen( );
	unsigned char str[MAXWORDLENGTH];
	int k;

	fp = fopen( fname, "r" );

	k = 0;
	while ( !feof( fp ) ) {
		fgets( str, MAXWORDLENGTH - 1, fp );
		if ( !feof( fp ) ) {
			fixline( str );
			str[strlen( str ) - 1] = 0;
			strcpy( allwords[k], str );
			k++;
		}
	}
	fclose( fp );
	wordcount = k;

}

/* See if there are word(s) starting with word */

int findbegin( unsigned char *word, int n ) {
	int i;
	int from = -1;

	/* Make sure "word" is prefix of some words in dict */
	int curstate, c, ns;
	curstate = i = 0;
	while ( c = word[i] ) {
		if ( c >= state_min[curstate] && c <= state_max[curstate] ) {
			if ( ( ns = map[state_offset[curstate] + c - state_min[curstate]] )>0 ) {
				curstate = ns;
			} else
				break;
		} else
			break;
		i++;
	}
	if ( i<n )
		return -1;

	for ( i = 0; i<wordcount; i++ ) {
		c = memcmp( word, allwords[i], n );
		// printf("F %s == %s %d => %d\n", word, allwords[i], n, c);
		if ( c == 0 ) {
			return i;
		}
		if ( c<0 ) break;
	}
	return from;
}

void pushword( char *word ) {
	if ( next == 0 ) {
		strcpy( lstr[lnum], word );
		lnum++;
		next = 1;
	} else {
		strcpy( rstr[rnum], word );
		rnum++;
		next = 0;
	}
}

void popword( ) {
	if ( next == 0 ) {
		rnum--;
		next = 1;
	} else {
		lnum--;
		next = 0;
	}
}

void printwords( ) {
	int i;
	for ( i = 0; i<lnum; i++ )
		printf( "%s ", lstr[i] );
	printf( " = " );
	for ( i = 0; i<rnum; i++ )
		printf( "%s ", rstr[i] );
	printf( "\n" );
}


int dopair( unsigned char *longer, int shortlen, int depth ) {
	unsigned char *dif = longer + shortlen;
	int ret = 0;
	int n, k;

	if ( NOTMIDDLE( dif[0] ) )
		return 0;

	// printf("Try %s %d @%d\n", longer, shortlen, depth);
	if ( depth>allow )
		return 0;

	n = strlen( dif );
	k = findbegin( dif, n );

	// printf("Find %s (%d) %d\n", dif, n, k);

	if ( k >= 0 ) {
		if ( strcmp( allwords[k], dif ) == 0 ) {
			// printf("> %s\n", dif);
			pushword( dif );
			printwords( );
			popword( );
			if ( depth>maxdepth )
				maxdepth = depth;

			ret++;
			k++;
		}
		while ( memcmp( dif, allwords[k], n ) == 0 ) {
			pushword( allwords[k] );
			if ( dopair( allwords[k], n, depth + 1 )>0 ) {
				// printf("<- (%s,%d)\n", allwords[k], n);
				ret++;
			}
			popword( );
			k++;
		}
	}
	return ret;
}

void process( ) {
	int i, k;
	unsigned char str[MAXWORDLENGTH];
	int matchlist[MAXLINELENGTH];

	for ( i = wordcount - 1; i >= 0; i-- ) {
		strcpy( str, allwords[i] );
		k = findword( allwords[i], matchlist );
		k -= 2;
		while ( k >= 0 ) {
			str[matchlist[k]] = 0;
			// printf("%s -> %s\n", allwords[i], str);
			maxdepth = 0;
			lnum = 0; rnum = 0; next = 0;
			pushword( str );
			pushword( allwords[i] );
			if ( dopair( allwords[i], matchlist[k], 0 )>0 ) {
				// printf("<- (%s,%d) %d\n", allwords[i], matchlist[k], maxdepth);
			}
			k--;
		}
		if ( i % 100 == 0 )
			printf( "Count down %d\n", i );
	}
}

main( int argc, char *argv[] ) {
	if ( argc>1 ) {
		sscanf( argv[1], "%d", &allow );
	}
	printf( "DigDict @Depth=%d\n", allow );

	readfile( DICTFILE );
	printf( "%d words read\n\n", wordcount );
	process( );
}
