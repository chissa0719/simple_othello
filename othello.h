/*******************************************************************

	オセロ プログラム　　　　　Othello.c

	松江工業高等専門学校　情報工学科　准教授　橋本　剛
	プログラミング　授業用

*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>//memcpy

#define SIDE 8 // 一辺の長さ 
#define ASIDE (SIDE + 2) // 局面用配列の一辺 緩衝地帯分２を足す Side for array, add 2
#define BOARDSIZE (ASIDE * ASIDE) // ボードの大きさ　Size of board
#define UP (-ASIDE)
#define DOWN ASIDE
#define RIGHT 1
#define LEFT (-1)
// Piece number
#define B 1 // BLACK 1
#define W 2 // WHITE 2
#define N 3 // 番兵 Sentinel (out of board) 3
#define BLACK_TURN 0 
#define WHITE_TURN 1

//座標からX座標またはY座標を計算するマクロ
#define X(pos) (pos % ASIDE)
#define Y(pos) (pos / ASIDE)
#define TURNCOLOR( turn ) (turn + 1)
#define OPPONENT(turn) !turn

#define TRUE 1
#define FALSE 0

#define MOVENUM 32
#define PASSMOVE 99 //パス手には99を入れる 

///////////////////////////AI用に追加　ここから
#define MAXDEPTH 5 //探索を行う最大深さ
int enemy_depth; //深さ(選択)
#define INFINITY 1000 //十分大きい数を無限大として扱う
#define SEARCH_LIMIT_DEPTH 128 //探索深さの上限
///////////////////////////AI用に追加　ここまで

//表示に使う文字定数
const char* piece[3] = { "  ", "●", "○" };
const char* abc[8] = { "a","b","c","d","e","f","g","h" };
int turn; // 手番
int ply; //手数 
int log_result = 0; //対AIでの勝敗を記録(0:無記録,1:自分の勝ち,2:AIの勝ち)

int AI_level = 10; //AIのレベル表示(初期値は適当)
int input_level = 0; //AIレベルが設定されたか
int input_turn = 0; //先手後手などが設定されたか
int input_init = 0; //忖度モードが設定されたか
int your_turn = 10; //先手後手
int sontaku_mode = 10;
int replay_count = 0; //リプレイ盤面の何番目に保存するか
int count = 0;
int tmp_count = 0;

typedef char Move;//手だとわかりやすくするため型を宣言（しなくてもよい）
Move nextmove;//次の手 
unsigned char stonenum[2];//石の数を記憶する変数

///////////////////////////AI用に追加　ここから
struct Position //過去の局面などを記憶するための構造体
{
	unsigned char board[BOARDSIZE];
	unsigned char stonenum[2];
};
struct Position history[SEARCH_LIMIT_DEPTH];
//場所による評価を与える配列
//角が良いことを教えることが一番の目的
//本当はパターンによる評価が出来ればよいが、
//この程度でもそれっぽくすることは可能
int evalboard[BOARDSIZE] =
{
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  99, -15,  20,   4,   4,  20, -15,  99,   0,
   0, -15, -30,  -3,  -3,  -3,  -3, -30, -15,   0,
   0,  20,  -3,   5,   1,   1,   5,  -3,  20,   0,
   0,   4,  -3,   1,   1,   1,   1,  -3,   4,   0,
   0,   4,  -3,   1,   1,   1,   1,  -3,   4,   0,
   0,  20,  -3,   5,   1,   1,   5,  -3,  20,   0,
   0, -15, -30,  -3,  -3,  -3,  -3, -30, -15,   0,
   0,  99, -15,  20,   4,   4,  20, -15,  99,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};
///////////////////////////AI用に追加　ここまで

//2次元の座標を一次元に変換
int getposition(int x, int y) { return y * ASIDE + x; };
//一番大事な変数、ここでは一次元で表現
unsigned char board[BOARDSIZE] = //intでも良いが、データ格納のことを考えてcharにしている
{
	N, N, N, N, N, N, N, N, N, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, B, W, 0, 0, 0, N,
	N, 0, 0, 0, W, B, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, N, N, N, N, N, N, N, N, N
};

unsigned char init_board[BOARDSIZE] = //初期化用board
{
	N, N, N, N, N, N, N, N, N, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, B, W, 0, 0, 0, N,
	N, 0, 0, 0, W, B, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, N, N, N, N, N, N, N, N, N
};

//リプレイ用
unsigned char replay_board[BOARDSIZE][61] = //リプレイ用board
{
	N, N, N, N, N, N, N, N, N, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, B, W, 0, 0, 0, N,
	N, 0, 0, 0, W, B, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, N, N, N, N, N, N, N, N, N
};

// 表示関数 display function
void output()
{//とにかく表示する関数を作りましょう
	int x, y;
	printf("   a  b  c  d  e  f  g  h \n");
	for (y = 1; y <= SIDE; y++)
	{
		printf("%d |", y);
		for (x = 1; x <= SIDE; x++)
			printf("%s|", piece[board[getposition(x, y)]]);//ちょっと簡潔に書くためのヒント
		printf("\n");
	}

	//printf("ヒント : %f%d");

	// デバッグ用表示
/*	printf( "   a  b  c  d  e  f  g  h \n" );
	for ( y = 1; y <= SIDE; y++ )
	{
		printf( "%d |", y );
		for ( x = 1; x <= SIDE; x++ )
			printf( "%d|", getposition( x, y ));
		printf( "\n" );
	}
*/
}

// 合法手かどうか判定する関数 
// 合法手ならTRUE、違ったらFALSEを返す
int isLegalMove(Move pos)
{
	int dirx, diry, dir;
	int pos1;
	// 自分の色、相手の色は何か変数に入れておく
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));

	if (board[pos] != 0)//空きマスでないか
		return FALSE;

	// posの周り8方向を調べ相手石が取れるか調べる
	// 8方向ループの書き方1
	for (dirx = -1; dirx <= 1; dirx++)
	{
		for (diry = -ASIDE; diry <= ASIDE; diry += ASIDE)
		{
			dir = dirx + diry;
			if (dir == 0)
				continue;
			pos1 = pos + dir;//posの隣のマス
			//以下は board[pos1] の中身をチェックしながら、
			//pos1を +=dirしていく
			//だめなら continue
			if (board[pos1] != opponentcolor)//相手の石があるか
				continue;
			do // 相手の石がある間は次を調べる
			{
				pos1 += dir;
			} while (board[pos1] == opponentcolor);
			// ひっくり返すためには最後に自分の石がないといけない
			if (board[pos1] != color)
				continue;
			//最後まで来たら成功
			return TRUE;
		}
	}
	return FALSE;
}

// 合法手かどうか判定する関数 
// 合法手ならTRUE、違ったらFALSEを返す
int replay_isLegalMove(Move pos)
{
	extern int count;
	int dirx, diry, dir;
	int pos1;
	// 自分の色、相手の色は何か変数に入れておく
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));

	if (replay_board[pos][count] != 0) {
		//空きマスでないか
		return FALSE;
	}

	// posの周り8方向を調べ相手石が取れるか調べる
	// 8方向ループの書き方1
	for (dirx = -1; dirx <= 1; dirx++)
	{
		for (diry = -ASIDE; diry <= ASIDE; diry += ASIDE)
		{
			dir = dirx + diry;
			if (dir == 0) {
				continue;
			}
			pos1 = pos + dir;//posの隣のマス
			//以下は board[pos1] の中身をチェックしながら、
			//pos1を +=dirしていく
			//だめなら continue
			if (replay_board[pos1][count] != opponentcolor) {
				//相手の石があるか
				continue;
			}
			do // 相手の石がある間は次を調べる
			{
				pos1 += dir;
			} while (replay_board[pos1][count] == opponentcolor);
			// ひっくり返すためには最後に自分の石がないといけない
			if (replay_board[pos1][count] != color) {
				continue;
			}
			//最後まで来たら成功
			return TRUE;
		}
	}
	return FALSE;
}

//手の生成 generation of moves
//生成した手の数を返している 
int generateMoves(Move moves[])
{
	int num = 0;//生成する合法手の数
	int pos;
	// 左上から順に石を置き、合法手か調べる
	for (pos = 11; pos <= 88; pos++)
	{
		if (isLegalMove(pos))
			moves[num++] = pos;//num番目の配列に
	}
	return num;
}

//手の生成 generation of moves
//生成した手の数を返している 
int replay_generateMoves(Move moves[])
{
	int num = 0;//生成する合法手の数
	int pos;
	// 左上から順に石を置き、合法手か調べる
	for (pos = 11; pos <= 88; pos++)
	{
		if (replay_isLegalMove(pos))
			moves[num++] = pos;//num番目の配列に
	}
	return num;
}


// ゲーム終了局面ならTRUEを返す Return TRUE in case of GAME OVER
// 生成した手が０の時呼ばれる
int isTerminalNode(int isPass)
{
	int num;
	Move moves[MOVENUM];
	if (!isPass)
	{
		num = generateMoves(moves);
		if (num > 0)
			return FALSE;

	}
	turn = OPPONENT(turn);
	num = generateMoves(moves);
	turn = OPPONENT(turn);
	if (num == 0)
		return TRUE;
	return FALSE;
}

///////////////////////////AI用に追加　ここから

// ゲーム終了局面の評価値を返す 勝ち ∞ 引き分け 0 負け －∞
int getTerminalValue()
{
	int diff = stonenum[turn] - stonenum[OPPONENT(turn)];
	if (diff > 0) // Win
		return INFINITY;
	else if (diff == 0) // DRAW
		return 0;
	else // if ( diff < 0 ) // LOSE
		return -INFINITY;
}

// 評価関数の計算　Calculation of evaluation function
int getEvaluationValue()
{
	int pos, value, c;
	Move moves[MOVENUM];
	// 合法手数の差を評価関数とする(自由度)
	value = generateMoves(moves);
	turn = OPPONENT(turn);
	value -= generateMoves(moves);
	turn = OPPONENT(turn);
	value *= 30;//自由度１を30点としておく（適当）
	for (pos = 11; pos <= 88; pos++)
	{
		c = board[pos];
		if (c == 0)
			continue;
		else if (c == TURNCOLOR(turn))
			value += evalboard[pos];
		else
			value -= evalboard[pos];
	}
	return value;
}
///////////////////////////AI用に追加　ここまで

// 評価関数の計算　Calculation of evaluation function
int replay_getEvaluationValue()
{
	int pos, value, c;
	Move moves[MOVENUM];
	// 合法手数の差を評価関数とする(自由度)
	value = replay_generateMoves(moves);
	turn = OPPONENT(turn);
	value -= generateMoves(moves);
	turn = OPPONENT(turn);
	value *= 30;//自由度１を30点としておく（適当）
	for (pos = 11; pos <= 88; pos++)
	{
		c = replay_board[pos][count];
		if (c == 0)
			continue;
		else if (c == TURNCOLOR(turn))
			value += evalboard[pos];
		else
			value -= evalboard[pos];
	}
	return value;
}

// 実際に手を進める
void makeMove(Move pos, int depth)
{
	int pos1, dir, dirx, diry, count = 0;
	int isLegal = TRUE;
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));

	///////////////////////////AI用に追加　ここから
	// 局面を配列historyに保存
	memcpy(history[depth].board, board, sizeof(board));// 配列の中身をコピーするのはmemcpy()を使うと簡単
	memcpy(history[depth].stonenum, stonenum, sizeof(stonenum));
	///////////////////////////AI用に追加　ここまで

	if (pos == PASSMOVE)//パス手の場合
	{
		turn = OPPONENT(turn);
		return;
	}
	board[pos] = color;

	// posの周り8方向を調べ相手石をひっくり返す
	for (dirx = -1; dirx <= 1; dirx++)
	{
		for (diry = -ASIDE; diry <= ASIDE; diry += ASIDE)
		{
			dir = dirx + diry;
			if (dir == 0)
				continue;
			pos1 = pos + dir;//posの隣のマス
			//以下は board[pos1] の中身をチェックしながら、
			//pos1を +=dirしていく
			//だめなら continue
			if (board[pos1] != opponentcolor)//相手の石があるか
				continue;
			do // 相手の石がある間は次を調べる
			{
				pos1 += dir;
			} while (board[pos1] == opponentcolor);
			// ひっくり返すためには最後に自分の石がないといけない
			if (board[pos1] != color)
				continue;

			//最後まで来たら成功!実際にひっくり返す
			pos1 = pos + dir;
			do // 相手の石がある間ひっくり返す　
			{
				board[pos1] = color;
				pos1 += dir;
				count++;
			} while (board[pos1] == opponentcolor);
		}
	}
	stonenum[turn] += count + 1;
	stonenum[OPPONENT(turn)] -= count;
	turn = OPPONENT(turn);
}
///////////////////////////AI用に追加　ここから

// 手を戻す
void unmakeMove(int depth)
{
	// 配列の中身をコピーするのはmemcpy()を使うと簡単
	// 逆の事をmakeMove()でしないといけない
	// 局面を配列historyから復元
	memcpy(board, history[depth].board, sizeof(board));
	memcpy(stonenum, history[depth].stonenum, sizeof(stonenum));
	turn = OPPONENT(turn);
}

// 探索して最も評価の高い手を選ぶ
int search(int depth)
{
	int i;
	int movenum;//手の数
	Move moves[MOVENUM];//手を入れる配列 an array of moves
	int value;
	int bestvalue = -INFINITY - 1;//まず最小値を入れる
	extern int enemy_depth;

	//再帰
	if (depth >= enemy_depth) {
		return getEvaluationValue();
	}

	//手を生成
	movenum = generateMoves(moves);
	if (movenum == 0)
	{
		if (isTerminalNode(TRUE))// Game Over
			return getTerminalValue();
		else // パス
			moves[movenum++] = PASSMOVE;
	}
	for (i = 0; i < movenum; i++)
	{
		makeMove(moves[i], depth);//一手進め
		value = -search(depth + 1);
		//output();//for Debug
		//printf("i = %d, value = %d, move = %d\n", i, value, moves[i]);//for Debug
		unmakeMove(depth);//一手戻る
		if (value > bestvalue)
		{
			bestvalue = value;
			if (depth == 0) {
				nextmove = moves[i];
			}
		}
	}
	return bestvalue;
}

// 探索して最も評価の低い手を選ぶ(忖度)
int min_search(int depth)
{
	int i;
	int movenum;//手の数
	Move moves[MOVENUM];//手を入れる配列 an array of moves
	int value;
	int minvalue = INFINITY + 1;//まず最大値を入れる
	extern int enemy_depth;

	//再帰
	if (depth >= enemy_depth) {
		return getEvaluationValue();
	}

	//手を生成
	movenum = generateMoves(moves);
	if (movenum == 0)
	{
		if (isTerminalNode(TRUE))// Game Over
			return getTerminalValue();
		else // パス
			moves[movenum++] = PASSMOVE;
	}
	for (i = 0; i < movenum; i++)
	{
		makeMove(moves[i], depth);//一手進め
		value = min_search(depth + 1);
		//output();//for Debug
		//printf("i = %d, value = %d, move = %d\n", i, value, moves[i]);//for Debug
		unmakeMove(depth);//一手戻る
		if (value < minvalue)
		{
			minvalue = value;
			if (depth == 0) {
				nextmove = moves[i];
			}
		}
	}
	return minvalue;
}

//COMの手を生成する関数
void comPlayer()
{
	extern int sontaku_mode;
	int value;
	//printf("Com Thinking...\n");
	if (sontaku_mode == 0) { //忖度オフ
		value = search(0);
	}
	else if (sontaku_mode == 1) { //忖度オン
		value = min_search(0);
	}
	//printf("value = %d\n", value);
}

///////////////////////////AI用に追加　ここまで

//人間の入力を管理する関数
Move manPlayer()
{
	//入力をさせ、合法手かチェック
	char line[256];
	int x, y, num, move;
	Move moves[MOVENUM];
	num = generateMoves(moves);

	// 合法手が無い場合
	if (num == 0)
	{
		//printf("PASS!\n");
		//printf("Press Enter!\n");
		//fgets(line, 256, stdin);
		return PASSMOVE;
	}
	do
	{
		// input X coordinate
		do
		{
			printf("x(a-h):");
			fgets(line, 256, stdin);
		} while (line[0] < 'a' || line[0] > 'h');
		x = line[0] - 'a' + 1;
		// input Y coordinate
		do
		{
			printf("y(1-8):");
			fgets(line, 256, stdin);
		} while (line[0] < '1' || line[0] > '8');
		y = line[0] - '0';
		// Check legal move
		move = getposition(x, y);
		if (isLegalMove(move))
		{
			break;
		}
		printf("Illegal Move!\n");
	} while (1);
	return (Move)move;
}



// ランダムに手を返すプレイヤ
//ランダムに手を返す
Move randplayer()
{
	int num;
	Move moves[MOVENUM];
	num = generateMoves(moves);
	printf("RandPlayer\n");
	if (num == 0)
		return PASSMOVE;
	return moves[rand() % num];
}

//グローバル変数などを初期化
void init()
{
	turn = BLACK_TURN;
	ply = 0;
	stonenum[BLACK_TURN] = 2;
	stonenum[WHITE_TURN] = 2;
	//盤面初期化
	for (int i = 0; i < 100; i++) {
		board[i] = init_board[i];
	}
	AI_level = 10;
	input_level = 0;
	input_turn = 0;
	input_init = 0;
	your_turn = 10;
	sontaku_mode = 10;
	replay_count = 0;
	count = 0;
	tmp_count = 0;
	log_result = 0;
}
#pragma once
