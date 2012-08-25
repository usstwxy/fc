#include "fc_def.h"
#include "fc_engine.h"
#include "fc_tools.h"
#include <stdio.h>
#include <time.h>

void board_show(BOARD* board)
{
	int i, j;
	printf("\n\n\t    a b c d e f g h i j k l m n o\n");
	for (i=1; i<=15; i++) {
		printf("\t %c  ", (char)('a'+i-1));
		for (j=1; j<=15; j++) {
			if (board->data[i][j] == Black)
				printf("X");
			else if (board->data[i][j] == White)
				printf("O");
			else
				printf(".");
			printf(" ");
		}
		printf("\n");
	}
	printf("\n\n");
}


extern int board_evaluate(BOARD *board);

#define isalpha(c) ((c)>='a' && (c)<='z')
int callcnt;



int main()
{
	HT_init();
	BOARD *board = board_new();
	MOVE move;
	
	
	board_show(board);
	while (1) {
		printf(">>");
		char x, y;
		do {x=getchar();}while(!isalpha(x));
		do {y=getchar();}while(!isalpha(y));
		
		move.x = x-'a'+1;
		move.y = y-'a'+1;
		printf("%d %d\n", move.x, move.y);
		if (!board_put(board, move)) {
			printf("wrong position\n");
			continue;
		}
		board_show(board);
		
		//int score = board_evaluate(board);
		//printf("%c : %d\n", (board->turn=='B'? 'X' : 'O'), score);
		//continue;
		
		
		int t = clock();
		callcnt = 0;
		move = fc_search(board);
		t = clock() - t;
		printf("value : %d, time:%lf\n", move.value, ((double)t/CLOCKS_PER_SEC));
		printf("board_score call times: %d\n", callcnt);
		printf("%c%c\n", (char)(move.x+'a'-1), (char)(move.y+'a'-1));
		board_put(board, move);
		board_show(board);	
	}
	
	return 0;
}
