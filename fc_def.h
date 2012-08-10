#ifndef FC_DEF_H
#define FC_DEF_H

#include <stdint.h>
#include <stdio.h>

#ifndef MIN
#	define MIN(a,b) ((a)<(b)? (a):(b))
#endif

#ifndef MAX
#	define MAX(a,b)	((a)>(b)? (a):(b))
#endif

#ifndef ABS
#	define ABS(x) 	((x)>0 ? (x):-(x))
#endif


typedef unsigned int 		WORD32;
typedef unsigned long long 	WORD64;


#define WIN		9999
#define UNKNOWN	33333
typedef struct tagMOVE
{
	unsigned char x, y;
	short value;
}MOVE;


#define Empty 	' '
#define Black	'B'
#define White	'W'
#define Wall	'#'
#define NEXT(x)	((x)=='B' ? 'W':'B')
typedef struct tagBOARD
{
	unsigned char num;
	char data[17][17], turn;
	unsigned char posn[17][17];
	WORD32 msk32;
	WORD64 msk64;
}BOARD;


extern BOARD* 	board_new();
extern void 	board_init(BOARD*);
extern int 		board_put(BOARD*, MOVE);
extern int 		board_unput(BOARD*, MOVE);


#endif

