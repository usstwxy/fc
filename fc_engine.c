#include "fc_engine.h"
#include "fc_tools.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// private ?
int board_score(BOARD *board);
int board_genmove(BOARD *board, MOVE *mlist);
MOVE alphabeta(BOARD *board, int alpha, int beta, int dep, MOVE mlist[10][256]);

// public	  
MOVE fc_search(BOARD *board);





extern int callcnt;
int board_score(BOARD *board)
{
	callcnt++;
	return board_evaluate(board);	
}	


int board_genmove(BOARD *board, MOVE *mlist)
{
	int i, j, cnt=0;

	for (i=1; i<=15; i++)
		for (j=1; j<=15; j++)
			if (board->data[i][j] == Empty && board->posn[i][j] > 0) {
				mlist[cnt].x = j;
				mlist[cnt].y = i;
				cnt++;
			}
	

	//排下序
	for (i=0; i<cnt; i++) 
		mlist[i].value = pos_score(board, mlist[i].x, mlist[i].y);
		
	for (i=0; i<cnt; i++)
		for (j=i+1; j<cnt; j++)
			if (mlist[i].value < mlist[j].value) {
				MOVE t = mlist[i];
				mlist[i] = mlist[j];
				mlist[j] = t;
			}

	return cnt;
}

	
int hashnumber;
MOVE alphabeta(BOARD *board, int alpha, int beta, int dep, MOVE mlist[10][256])
{
	int val, is_exact=0;
	// look hash table
	val = HT_look(board, alpha, beta, dep);
	if (val != UNKNOWN && dep != MAX_DEPTH) {
		hashnumber++;
		MOVE r = {0, 0, val};
		return r;
	}
	
	// check board
	val = board_score(board);
	if (dep == 0 || ABS(val) == WIN) { // return the evalute score
		HT_push(board, dep, TP_EXACT, val);
		MOVE r = {0, 0, val};
		return r;
	}

	MOVE bestmove = {0, 0, alpha}, move;
	
	int i, num = board_genmove(board, mlist[dep]);	
	for (i=0; i<num; i++) {
		board_put(board, mlist[dep][i]);
		move = alphabeta(board, -beta, -alpha, dep-1, mlist);
		board_unput(board, mlist[dep][i]);
		
		move.value = -move.value;
		move.x = mlist[dep][i].x;
		move.y = mlist[dep][i].y;
		
		// alpha beta cutting
		if (move.value > alpha) {
			bestmove = move;
			alpha = move.value;
			is_exact = 1;
		}
		if (move.value >= beta) {
			HT_push(board, dep, TP_LOWER, move.value);
			return move;
		}
	}
	
	HT_push(board, dep, (is_exact? TP_EXACT:TP_UPPER), alpha);
	
	return bestmove;
}


MOVE fc_search(BOARD* board)
{
	int now = board_score(board);
	if (ABS(now) == WIN) {//find move in particular state
		return find_move(board, now==WIN);
	}	

	hashnumber = 0;
	MOVE mlist[10][256], move;
	move = alphabeta(board, -WIN-1, WIN+1, MAX_DEPTH, mlist);
	printf("hash number : %d  ", hashnumber);
	
	return move;
}


