#include "fc_def.h"
#include "fc_tools.h"
#include <memory.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>

WORD64 rand64(void)
{
	return rand() ^ ((WORD64)rand() << 15) ^ ((WORD64)rand() << 30) ^
					((WORD64)rand() << 45) ^ ((WORD64)rand() << 60) ;
}
WORD32 rand32(void)
{
	return rand() ^ ((WORD32)rand() << 15) ^ ((WORD32)rand() << 30) ;
}



WORD32 key32[2][16][16];
WORD64 key64[2][16][16];

struct item
{
	unsigned char	type, dep;
	short 			val;
	WORD64	 		msk;
};

struct map
{
	int *head, *next;
	struct item *data;
	int size, top;
};


struct map* map_new(int sz)
{
	struct map *m = malloc(sizeof(struct map));
	m->top	= 0;
	m->size = sz;
	m->data = malloc(sz*sizeof(struct item));
	m->head = malloc(sz*sizeof(int));
	m->next = malloc(sz*sizeof(int));
	memset(m->head, -1, sz*sizeof(int));
	return m;
}
void map_delete(struct map *m)
{
	free(m->head);
	free(m->next);
	free(m->data);
	free(m);
}

void map_clear(struct map *m)
{
	m->top = 0;
	memset(m->head, -1, m->size*sizeof(int));
}

void map_push(struct map *m, WORD32 msk32, struct item it)
{
	if (m->top >= m->size) return ;
	int pos = msk32 % m->size;
	int i;
	for (i=m->head[pos]; i!=-1; i=m->next[i])
		if (m->data[i].msk == it.msk) {// update
			m->data[i] = it;
			return ;
		}
		
	int next = m->head[pos];
	m->head[pos] 	= m->top;
	m->data[m->top] = it;
	m->next[m->top] = next;
	m->top++;
}

struct item* map_find(struct map *m, WORD32 msk32, WORD64 msk64)
{
	int pos = msk32 % m->size;
	int i;
	for (i=m->head[pos]; i!=-1; i=m->next[i])
		if (m->data[i].msk == msk64) {
			return &m->data[i];
		}
	return 0;
}




struct map *table[2];

void HT_init()
{
	table[0] = map_new(99997);
	table[1] = map_new(99997);
	srand(time(0));
	int i, j, k;
	for (k=0; k<2; k++)
		for (i=1; i<=15; i++)
			for (j=1; j<=15; j++) {
				key32[k][i][j] = rand32();
				key64[k][i][j] = rand64();
			}
}

int HT_look(BOARD *board, int alpha, int beta, int dep)
{
	struct map* m = table[KTURN(board->turn)];
	struct item *it = map_find(m, board->msk32, board->msk64);
	if (it == 0) return UNKNOWN;
	if (it->dep < dep) return UNKNOWN;
	switch (it->type)
	{
	case TP_EXACT: return it->val;
	case TP_LOWER: return it->val>=beta ? it->val : UNKNOWN;
	case TP_UPPER: return it->val<=alpha ? it->val: UNKNOWN;
	}
	return UNKNOWN;
}

void HT_push(BOARD *board, int dep, int type, int val)
{
	struct item it;
	it.dep 	= dep;
	it.type = type;
	it.val	= val;
	it.msk	= board->msk64;
	
	struct map* m = table[KTURN(board->turn)];
	map_push(m, board->msk32, it);
}

int HT_item_num()
{
	return table[0]->top + table[1]->top;
}

