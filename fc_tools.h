#ifndef FC_TOOLS_H
#define FC_TOOLS_H

#include "fc_def.h"


// hash table
#define TP_EXACT	1
#define TP_LOWER	2
#define TP_UPPER	3

#define KTURN(x)	((x)=='B' ? 0 : 1)

extern WORD32 key32[2][16][16];
extern WORD64 key64[2][16][16];

extern void HT_push(BOARD *board, int dep, int type, int val);
extern int  HT_look(BOARD *board, int alpha, int beta, int dep);
extern void HT_init();
extern int  HT_item_num();

// board evaluate
extern int board_evaluate(BOARD *board);
extern int pos_score(BOARD *board, int x, int y);
extern MOVE find_move(BOARD* board, int flag);

#endif
