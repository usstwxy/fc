#include "fc_def.h"
#include <memory.h>
#include <stdio.h>

#define TP_DEAD	0
#define TP_D3	1
#define TP_D4	2
#define TP_3	3
#define TP_33	4
#define	TP_43	5
#define TP_44	6
#define TP_4	7
#define TP_5	8

#define SGN(x)	(((x)>0) - ((x)<0))



// public
int board_evaluate(BOARD *board);


int extend_type(char A[17][17], char id, int n, int lx, int ly, int rx, int ry)
{
	int dx=SGN(rx-lx), dy=SGN(ry-ly);
	int tmp1=TP_DEAD, tmp2=TP_DEAD;
	//先扩展左边看看
	if (A[ly][lx] == ' ' && A[ly-dy][lx-dx] == id) {
		int x = lx - dx;
		int y = ly - dy;
		int ln = 0;
		while (A[y][x] == id) {
			x-=dx; y-=dy; ln++;
		}
		int l=A[y][x]==' ', r=A[ry][rx]==' ';
		if (n+ln >= 4) tmp1 = TP_D4;
		if (n+ln == 3) 
			tmp1 = (l&&r) ? TP_3 : TP_D3;
	}
	//再扩展右边！
	if (A[ry][rx] == ' ' && A[ry+dy][rx+dx] == id) {
		int x = rx + dx;
		int y = ry + dy;
		int rn = 0;
		while (A[y][x] == id) {
			x+=dx; y+=dy; rn++;
		}
		int l=A[ly][lx]==' ', r=A[y][x]==' ';
		if (n+rn >= 4) tmp2 = TP_D4;
		if (n+rn == 3) 
			tmp2 = (l&&r) ? TP_3 : TP_D3;
	}
	//得到结果
	if (tmp1==TP_D4 && tmp2==TP_D4) return TP_44;
	if (tmp1==TP_D4 || tmp2==TP_D4) return TP_D4;
	if (tmp1==TP_3 || tmp2==TP_3) return TP_3;
	if (tmp1==TP_D3 || tmp2==TP_D3) return TP_D3;
	return TP_DEAD;
}

int get_type(char A[17][17], char id, int x, int y, int dx, int dy)
{
	int lx=x-dx, ly=y-dy,
		rx=x+dx, ry=y+dy,
		n = 1;
	
	// 获取本段信息
	while (A[ly][lx] == id) {
		lx-=dx; ly-=dy; n++;
	}
	while (A[ry][rx] == id) {
		rx+=dx; ry+=dy; n++;
	}
	int l=(A[ly][lx]==' '),
		r=(A[ry][rx]==' ');
	
	// 检查各种情况
	if (n >= 5) return TP_5;
	if (!l && !r) return TP_DEAD;
	if (n == 4) {
		if ((l && A[ly-dy][lx-dx]==id) &&
			(r && A[ry+dy][rx+dx]==id)) return TP_44;
		return (l&&r) ? TP_4 : TP_D4;
	}
	if (n == 3) {
		int flag = 0;
		flag += (l && A[ly-dy][lx-dx]==id);
		flag += (r && A[ry+dy][rx+dx]==id);
		if (flag == 2) return TP_44;
		if (flag == 1) return TP_D4;
		if (l && r) {
			int ll = A[ly-dy][lx-dx];
			int rr = A[ry+dy][rx+dx];
			if (ll == ' ' || ll == id) return TP_3;
			if (rr == ' ' || rr == id) return TP_3;
			return TP_D3;
		}else return TP_D3;
	}
	if (n == 1 || n == 2)
		return extend_type(A, id, n, lx, ly, rx, ry);
}

int analysis(BOARD *board, char id, int x, int y)
{
	int dx[4] = {1, 0, 1,  1};
	int dy[4] = {0, 1, 1, -1};
	//char A[17][17];
	int tp[12]={0}, i;
	
	//memcpy(A, board->data, sizeof(A));
	
	for (i=0; i<4; i++)
		tp[get_type(board->data, id, x, y, dx[i], dy[i])]++;
		
	if (tp[TP_5] > 0) return TP_5;
	if (tp[TP_44]> 0) return TP_44;
	if (tp[TP_4] > 0) return TP_4;
	
	if (tp[TP_D4] > 1) return TP_44;
	if (tp[TP_D4] > 0 && tp[TP_3] > 0) return TP_43;
	if (tp[TP_3] > 1) return TP_33;
	
	if (tp[TP_D4] > 0) return TP_D4;
	if (tp[TP_3] > 0) return TP_3;
	if (tp[TP_D3] > 0) return TP_D3;
	
	return TP_DEAD;
}


int board_evaluate(BOARD *board)
{
	int Type[2][12]={0}, *F=Type[0], *S=Type[1];
	int i, j, v1=0, v2=0;
	char first=board->turn, second=NEXT(first);
	
	for (i=1; i<=15; i++)
		for (j=1; j<=15; j++) {
			if (board->data[j][i] == Empty) continue;
			if (board->data[j][i] == first) {
				F[analysis(board, first, i, j)]++;
			}else {
				S[analysis(board, second, i, j)]++;
			}
		}
	
	//-------- 判断各种情形 --------
	//已经5连
	if (F[TP_5] > 0) return WIN;
	if (S[TP_5] > 0) return -WIN;
	
	//自己有4，这步5连
	if (F[TP_4] + F[TP_44] + F[TP_43] + F[TP_D4] > 0) return WIN;
	
	//对面有杀招
	if (S[TP_4] + S[TP_44] + S[TP_43] > 0) return -WIN;
	
	//自己活三，对面没有眠四，做成活四赢
	if (F[TP_33]+F[TP_3] > 0 && S[TP_D4] == 0) return WIN;
	
	//对方三三，自己没有三
	if (S[TP_33] > 0 && F[TP_D4]+F[TP_3]+F[TP_D3] == 0) return -WIN;
	
//--------------------------------------------	
/*
**
**	应该考虑先后手加权，调换次序
**
*/	
//--------------------------------------------

	int flag = S[TP_D4] > 0;

	// 剩余棋型，计算价值
	if (!flag) {//先后手不变
		if (F[TP_33] > 0) return WIN;//赢了（上面判断过的逻辑）
		if (F[TP_3] > 0)  return WIN;
	
		if (S[TP_33] > 0) return -WIN;
		if (S[TP_3] > 0)  v2 += 100;//鼓励进攻
	}else {//先后手交换
		if (F[TP_33] > 0) v1 += 2000;//这步必须堵四，但还是很可能赢
		if (F[TP_3] > 0)  v1 += 100;//这步堵四，下步对方先手，变成进攻活三
		
		if (S[TP_33] > 0) return -WIN;//有四又有3，赢了（在上面判断过的逻辑）
		if (S[TP_3] > 0)  return -WIN;
	}
	
	//分析下一步出现的情况
	memset(Type, 0, sizeof(Type));
	for (i=1; i<=15; i++)
		for (j=1; j<=15; j++) 
			if (board->data[j][i] == Empty && board->posn[j][i] > 0){
				F[analysis(board, first, i, j)]++;
				S[analysis(board, second, i, j)]++;
			}
			
	//先后手权值
	double k1=1.2, k2=1.0;	
	
	if (!flag) {//保持先手
		if (F[TP_44]>0 || F[TP_43]>0) return WIN;//这步做成44，赢了
		if (F[TP_33] > 0) {//这步做成33
			if (S[TP_D4]+S[TP_4]+S[TP_44]+S[TP_43] == 0) {//对面下步做不成4，赢了
				return WIN;
			}else {
				if (S[TP_4]+S[TP_44]+S[TP_43] == 0) {//不用堵对面的杀招，做33很可能赢
					v1 += 2000;
				}else {//否则要堵杀招，下一步对面变先手……
					k1 = 0.9;
				}
			}
		}
	}else {//这步堵四，下步对面先手
		k1 = 0.9;
		if (S[TP_44] + S[TP_43] + S[TP_33] > 0) {//对面很可能赢……
			v2 += 500 + 500*(S[TP_33]==0);//若是33则只加500分
		}
		if (F[TP_44] + F[TP_43] + F[TP_33] > 0) {//把先手再反过来……
			k1 = 1.1;
		}
	}
	
	
	// 累计剩余棋型分数
	v1 += k1*F[TP_D4]*5;
	v1 += k1*F[TP_3]*5;
	v1 += k1*F[TP_D3]*1;
	
	v2 += k2*S[TP_D4]*5;
	v2 += k2*S[TP_3]*5;
	v2 += k2*S[TP_D3]*1;
	
	
	return v1-v2;
}



MOVE find_move(BOARD *board, int flag)
{
	char id = flag ? board->turn : NEXT(board->turn);
	int i, j, tmp=0;
	MOVE move = {0, 0, (flag? WIN:-WIN)};
	
	for (i=1; i<=15; i++)
		for (j=1; j<=15; j++)
			if (board->data[j][i] == Empty && board->posn[j][i] > 0) {
				int v = analysis(board, id, i, j);
				if (v > tmp) {
					tmp = v;
					move.x = i;
					move.y = j;
				}
			}
	
	return move;
}

int pos_score(BOARD *board, int x, int y)
{
	int v1 = analysis(board, board->turn, x, y);
	int v2 = analysis(board, NEXT(board->turn), x, y);
	int score = board->posn[y][x];
	
	if (v1 > TP_33) score += 9000 + v1*100;
	if (v2 > TP_33) score += 8000 + v2*100;

	if (v1 == TP_33) score += 2000;
	if (v2 == TP_33) score += 1000;
	
	if (v1 == TP_3 || v1 == TP_D4) score += v1*10;
	if (v2 == TP_3 || v2 == TP_D4) score += 2;
	if (v1 == TP_D3) score += 2;
	
	return score;
}


