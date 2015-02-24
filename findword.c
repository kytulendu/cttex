#include "global.h"
#include "map.h"
#include "findword.h"

/* Table Look-Up for level of a character */
/* only those in the range D0-FF */
int levtable[] = {
	0, 2, 0, 0, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 2, 3, 3, 3, 3, 3, 2, 3, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0
};

/********************************************************/
/* Find list of words which match  head of given string */
/********************************************************/

int findword( unsigned char *str, int *matchlist ) {
	int curstate, i, c, j, ns;

	curstate = i = j = 0;
	while ( c = str[i] ) {
		if ( c >= state_min[curstate] && c <= state_max[curstate] ) {
			/*
			printf("%d: %d %d %d\n",curstate,state_offset[curstate],c,state_min[curstate]);
			fflush(stdout);
			*/
			if ( ( ns = map[state_offset[curstate] + c - state_min[curstate]] )>0 ) {
				curstate = ns;
				if ( state[curstate] )
					matchlist[j++] = i + 1;
			} else
				break;
		} else
			break;
		i++;
	}
	ns = j;
	j = 0;

	/* Remove words which are not followed by a middle alphabet.
	This can reduce the number of recursive calls in dooneline2sub()
	by half. */
	for ( i = 0; i<ns; i++ )
		if ( !NOTMIDDLE( str[matchlist[i]] ) )
			matchlist[j++] = matchlist[i];

	/* printf("At %s (%d)\n", str, j); */
	return j;
}

/************************************************************/
/* Fix alphabet/vowel order, remove redundant vowels/toners */
/************************************************************/

void fixline( line )
unsigned char *line;
{
	unsigned char top, up, middle, low;
	unsigned char *out;
	int i, j, c;

	i = j = 0;

	out = line; /* Overwrite itself */
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

