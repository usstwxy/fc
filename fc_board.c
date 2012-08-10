#include "fc_def.h"
#include "fc_tools.h"
#include <malloc.h>
#include <memory.h>

void board_init(BOARD *b)
{
	b->num = 0;
	b->turn = Black;
	memset(b->data, Empty, sizeof(b->data));
	int i;
	for (i=0; i<=16; i++){
		b->data[i][0]  = b->data[0][i]  = Wall;
		b->data[i][16] = b->data[16][i] = Wall;
	}
	memset(b->posn, 0, sizeof(b->posn));
}


BOARD* board_new()
{
	BOARD *b = malloc(sizeof(BOARD));
	board_init(b);
	return b;
}


int board_put(BOARD* b, MOVE m)
{
	if (b->data[m.y][m.x] != Empty) return 0;
	
	// calc msk
	int t = KTURN(b->turn);
	b->msk32 ^= key32[t][m.y][m.x];
	b->msk64 ^= key64[t][m.y][m.x];
	
	// make move
	b->data[m.y][m.x] = b->turn;
	b->turn = NEXT(b->turn);
	b->num++;
	
	// change posn
	int i, j;
	for (i=m.y-2; i<=m.y+2; i++)
		for (j=m.x-2; j<=m.x+2; j++)
			if (i>0 && j>0 && i<16 && j<16) {
				b->posn[i][j]++;
			}

	return 1;
}


int board_unput(BOARD* b, MOVE m)
{
	if (b->data[m.y][m.x] == Empty) return 0;
	
	// calc msk
	int t = KTURN(b->turn);
	b->msk32 ^= key32[t][m.y][m.x];
	b->msk64 ^= key64[t][m.y][m.x];
	
	// unmake move
	b->data[m.y][m.x] = Empty;
	b->turn = NEXT(b->turn);
	b->num--;
	
	// change posn
	int i, j;
	for (i=m.y-2; i<=m.y+2; i++)
		for (j=m.x-2; j<=m.x+2; j++)
			if (i>0 && j>0 && i<16 && j<16) {
				b->posn[i][j]--;
			}
			
	return 1;
}
