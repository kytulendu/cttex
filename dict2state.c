/* Thai word-separator by dictionary */
/* By Vuthichai A.                   */
/* Win32 stack overflow fixed by NuuNeoi, bpasu  */
/* neon@nuuneoi.com, bpasu@intania85.org         */
#define                 DICTFILE               "tdict.txt"
#define                 MAXWORD                 70000
#define                 MAXWORDLENGTH           500
#define                 MAXLINELENGTH           500
#define                 MAXSTATE                500000

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int levtable[] = {
	0, 2, 0, 0, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 2, 3, 3, 3, 3, 3, 2, 3, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 };

int map[MAXSTATE][96];
int state[MAXSTATE];
int maxstate, rnumword;
int mincol, maxcol;

void readfile( unsigned char * );
void fixline( unsigned char * );
void dooneline( unsigned char *, unsigned char * );
int findword( unsigned char * );
void add2map( unsigned char *, int );
void initmap( );
void prmap( );
void check_headchar( );

unsigned char *wordptr[MAXWORD];
int numword;                    /* Number of words in memory */

main( ) {
	FILE *fp, *fopen( );
	unsigned char inputfile[80];
	unsigned char str[MAXLINELENGTH], out[MAXLINELENGTH];
	int i, count, nn;

	numword = nn = 0;

	/* Sort while reading */
	readfile( DICTFILE );

	initmap( );
	for ( i = 0; i<numword; i++ ) {
		/* Remove duplicate words */
		if ( i<numword - 1 ) {
			if ( strcmp( wordptr[i] + 1, wordptr[i + 1] + 1 ) != 0 ) {
				add2map( wordptr[i] + 1, wordptr[i][0] );
				nn++;
			}
		} else {
			add2map( wordptr[i] + 1, wordptr[i][0] );
			nn++;
		}
	}

	count = 0;
	for ( i = 0; i<maxstate; i++ )
		count += state[i];
	printf( "Word Count: %d %d\n", nn, count );
	printf( "Min Max Code: %d %d\n", mincol, maxcol );

	/* check_headchar(); */

	rnumword = nn;

	printf( "maxstate = %d\n", maxstate );
	printf( "rnumword = %d\n", rnumword );

	prmap( );
	return 0;
}

/* See if any characters appear only at beginning of
words */

void check_headchar( ) {
	int i, ch, sum;
	for ( ch = 0; ch<96; ch++ ) {
		sum = 0;
		for ( i = 1; i<maxstate; i++ ) {
			if ( map[i][ch] )
				sum++;
		}
		/* sum=0 -> no intra-word occurence */
		if ( sum == 0 && map[0][ch]>0 )
			printf( "%d %c sum %d %d\n", ch + 160, ( 160 + ch ), sum, map[0][ch] );
	}
}

void readfile( unsigned char *fname ) {
	FILE *fp, *fopen( );
	unsigned char str[MAXWORDLENGTH];
	unsigned char *ostr;
	int l, i;

	fp = fopen( fname, "r" );

	while ( !feof( fp ) ) {
		fgets( str, MAXWORDLENGTH - 1, fp );
		if ( !feof( fp ) ) {
			fixline( str );
			wordptr[numword] = ( unsigned char * ) malloc( ( l = strlen( str ) ) + 2 );
			if ( wordptr[numword] == NULL )
				printf( "Memory Error\n" );
			strcpy( wordptr[numword] + 1, str );
			wordptr[numword][l] = 0;            /* Remove new line */
			wordptr[numword][0] = l - 1;

			if ( numword > 0 )
				if ( strcmp( wordptr[numword] + 1, wordptr[numword - 1] + 1 )<0 ) {
					ostr = wordptr[numword];
					i = numword;
					while ( i && ( strcmp( ostr + 1, wordptr[i - 1] + 1 )<0 ) ) {
						wordptr[i] = wordptr[i - 1];
						i--;
					}
					wordptr[i] = ostr;
				}
			numword++;
			if ( numword % 2000 == 0 )
				printf( "%d\n", numword );
		}
	}
	fclose( fp );
	printf( "Reading dictionary done %d.\n", numword );
}


void fixline( line )
unsigned char *line;
{
	unsigned char top, up, middle, low;
	unsigned char out[MAXLINELENGTH];
	int i, j, c;

	i = j = 0;
	strcpy( out, line );
	top = up = middle = low = 0;
	while ( c = out[i++] ) {
		switch ( ( c>0xD0 ) ? levtable[c - 0xD0] : 0 ) {
		case 0:
			if ( middle ) {
				line[j++] = middle;
				if ( low ) line[j++] = low;
				if ( up )  line[j++] = up;
				if ( top ) line[j++] = top;
			}
			top = up = middle = low = 0;
			middle = c; break;
		case 1:
			low = c; break;
		case 2:
			up = c; break;
		case 3:
			top = c; break;
		}
	}
	if ( middle ) {
		line[j++] = middle;
		if ( low ) line[j++] = low;
		if ( up )  line[j++] = up;
		if ( top ) line[j++] = top;
	}
	line[j] = 0;
}

void initmap( ) {
	int i, j;

	for ( i = 0; i<MAXSTATE; i++ ) {
		state[i] = 0;
		for ( j = 0; j<96; j++ )
			map[i][j] = 0;
	}
	maxstate = 1;
	mincol = 255;
	maxcol = 0;
}

void add2map( unsigned char *str, int len ) {
	int curstate, i, c, cc;

	curstate = i = 0;
	while ( c = str[i] ) {
		cc = c;
		if ( cc>maxcol ) maxcol = cc;
		if ( cc<mincol ) mincol = cc;
		c -= 160;
		if ( c<0 ) {
			fprintf( stderr, "Fatal Error\n" );
			exit( 1 );
		}
		if ( map[curstate][c]>0 ) {
			curstate = map[curstate][c];
			/* printf("%c to %d\n", cc, curstate); */
		} else {
			map[curstate][c] = maxstate;
			curstate = maxstate;
			if ( i == len - 1 ) {
				state[maxstate] = 1;
				/* printf("%s at state %d\n", str, maxstate); */
			}
			maxstate++;
			if ( maxstate >= MAXSTATE ) {
				fprintf( stderr, "Not Enough No. of States\n" );
				exit( 1 );
			}
		}
		i++;
	}

}

void prmap( ) {
	FILE *FP;
	int i, j, c;
	int* state_min = ( int* ) malloc( MAXSTATE*sizeof( int ) );
	int* state_max = ( int* ) malloc( MAXSTATE*sizeof( int ) );
	int* state_offset = ( int* ) malloc( MAXSTATE*sizeof( int ) );
	int offset, min, max;

	c = maxcol - mincol + 1;
	offset = 0;

	printf( "Writing Map File...\n" );

	FP = fopen( "map.c", "w" );
	fprintf( FP, "int map[] = {\n" );
	for ( i = 0; i<maxstate; i++ ) {
		min = max = 0;
		for ( j = mincol; j <= maxcol; j++ ) {
			if ( map[i][j - 160] ) {
				if ( !min )
					min = j;
				max = j;
			}
		}
		state_min[i] = min;
		state_max[i] = max;
		state_offset[i] = offset;
		for ( j = min; j <= max; j++ ) {
			fprintf( FP, "%d", map[i][j - 160] );
			offset++;
			if ( i<maxstate - 1 || j<max ) fprintf( FP, "," );
			if ( offset % 16 == 15 ) fprintf( FP, "\n" );
		}
	}
	fprintf( FP, "};\n" );

	fprintf( FP, "unsigned char state[%d] = {\n", maxstate );
	for ( i = 0; i<maxstate; i++ ) {
		fprintf( FP, "%d", state[i] );
		if ( i<maxstate - 1 ) fprintf( FP, "," );
		if ( i % 16 == 15 ) fprintf( FP, "\n" );
	}
	fprintf( FP, "};\n" );

	fprintf( FP, "unsigned char state_min[%d] = {\n", maxstate );
	for ( i = 0; i<maxstate; i++ ) {
		fprintf( FP, "%d", state_min[i] );
		if ( i<maxstate - 1 ) fprintf( FP, "," );
		if ( i % 16 == 15 ) fprintf( FP, "\n" );
	}
	fprintf( FP, "};\n" );

	fprintf( FP, "unsigned char state_max[%d] = {\n", maxstate );
	for ( i = 0; i<maxstate; i++ ) {
		fprintf( FP, "%d", state_max[i] );
		if ( i<maxstate - 1 ) fprintf( FP, "," );
		if ( i % 16 == 15 ) fprintf( FP, "\n" );
	}
	fprintf( FP, "};\n" );

	fprintf( FP, "int state_offset[%d] = {\n", maxstate );
	for ( i = 0; i<maxstate; i++ ) {
		fprintf( FP, "%d", state_offset[i] );
		if ( i<maxstate - 1 ) fprintf( FP, "," );
		if ( i % 16 == 15 ) fprintf( FP, "\n" );
	}
	fprintf( FP, "};\n" );
	fclose( FP );

	FP = fopen( "map.h", "w" );
	fprintf( FP, "int numword = %d;\n", rnumword );
	fprintf( FP, "int mincol = %d;\n", mincol );
	fprintf( FP, "int maxcol = %d;\n", maxcol );
	fprintf( FP, "int maxstate = %d;\n", maxstate );
	fprintf( FP, "extern int map[%d];\n", offset );
	fprintf( FP, "extern unsigned char state[%d];\n", maxstate );
	fprintf( FP, "extern unsigned char state_min[%d];\n", maxstate );
	fprintf( FP, "extern unsigned char state_max[%d];\n", maxstate );
	fprintf( FP, "extern int state_offset[%d];\n", maxstate );
	fclose( FP );

	free( state_min );
	free( state_max );
	free( state_offset );
}
