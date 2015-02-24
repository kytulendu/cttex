#define IS_CACHE
#include "cache.h"

void clear_history( ) {
	HistoryPointer = 0;
}

void add_history( unsigned char *in, int errcount ) {
	return; /* Because we are not using get_history */

	if ( HistoryPointer <LISTSTACKDEPTH ) {
		HistoryErrorCount[HistoryPointer] = errcount;
		HistoryList[HistoryPointer] = in;
		HistoryPointer++;
	} else {
		fprintf( stderr, "History Dept Exceeded %d\n", HistoryPointer );
		exit( 1 );
	}
}

int get_history( unsigned char *in ) {
	int i;
	for ( i = 0; i<HistoryPointer; i++ ) {
		if ( in == HistoryList[i] )
			return HistoryErrorCount[i];

	}
	return -1;
}
