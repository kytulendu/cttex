/* Thai word-separator by dictionary */
/* By Vuthichai A.                   */

#define DICTFILE "tdict.txt"
#define MAXWORD 12000
#define MAXWORDLENGTH 30
#define MAXLINELENGTH 400

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void readfile( unsigned char * );
void dooneline( unsigned char *, unsigned char * );
int findword( unsigned char * );

unsigned char *wordptr[MAXWORD];
int numword;                    /* Number of words in memory */

main( ) {
	FILE *fp, *fopen( );
	unsigned char inputfile[80];
	unsigned char str[MAXLINELENGTH], out[MAXLINELENGTH];
	int i;

	numword = 0;
	readfile( DICTFILE );

	fp = fopen( "tdict.new", "w" );
	for ( i = 0; i<numword; i++ ) {
		/* Remove duplicate words */
		if ( i<numword - 1 ) {
			if ( strcmp( wordptr[i] + 1, wordptr[i + 1] + 1 ) != 0 )
				fprintf( fp, "%s\n", wordptr[i] + 1 );
		} else {
			fprintf( fp, "%s\n", wordptr[i] + 1 );
		}

	}
	fclose( fp );
	return 0;
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
			if ( numword % 100 == 0 )
				printf( "%d", numword );
		}
	}
	fclose( fp );
	printf( "Reading dictionary done.\n" );
}
