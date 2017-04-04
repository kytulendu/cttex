#include <stdio.h>
#include <stdlib.h>

#include "global.h"

#ifdef IS_CACHE
#define MYEXTERN 
#else
#define MYEXTERN extern
#endif

MYEXTERN unsigned char *HistoryList[LISTSTACKDEPTH];
MYEXTERN int HistoryErrorCount[LISTSTACKDEPTH];
MYEXTERN int HistoryPointer;

void clear_history( );
void add_history( unsigned char *in, int errcount );
int get_history( unsigned char *in );

