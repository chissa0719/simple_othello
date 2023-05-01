/*******************************************************************

	�I�Z�� �v���O�����@�@�@�@�@Othello.c

	���]�H�ƍ������w�Z�@���H�w�ȁ@�y�����@���{�@��
	�v���O���~���O�@���Ɨp

*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>//memcpy

#define SIDE 8 // ��ӂ̒��� 
#define ASIDE (SIDE + 2) // �ǖʗp�z��̈�� �ɏՒn�ѕ��Q�𑫂� Side for array, add 2
#define BOARDSIZE (ASIDE * ASIDE) // �{�[�h�̑傫���@Size of board
#define UP (-ASIDE)
#define DOWN ASIDE
#define RIGHT 1
#define LEFT (-1)
// Piece number
#define B 1 // BLACK 1
#define W 2 // WHITE 2
#define N 3 // �ԕ� Sentinel (out of board) 3
#define BLACK_TURN 0 
#define WHITE_TURN 1

//���W����X���W�܂���Y���W���v�Z����}�N��
#define X(pos) (pos % ASIDE)
#define Y(pos) (pos / ASIDE)
#define TURNCOLOR( turn ) (turn + 1)
#define OPPONENT(turn) !turn

#define TRUE 1
#define FALSE 0

#define MOVENUM 32
#define PASSMOVE 99 //�p�X��ɂ�99������ 

///////////////////////////AI�p�ɒǉ��@��������
#define MAXDEPTH 5 //�T�����s���ő�[��
int enemy_depth; //�[��(�I��)
#define INFINITY 1000 //�\���傫�����𖳌���Ƃ��Ĉ���
#define SEARCH_LIMIT_DEPTH 128 //�T���[���̏��
///////////////////////////AI�p�ɒǉ��@�����܂�

//�\���Ɏg�������萔
const char* piece[3] = { "  ", "��", "��" };
const char* abc[8] = { "a","b","c","d","e","f","g","h" };
int turn; // ���
int ply; //�萔 
int log_result = 0; //��AI�ł̏��s���L�^(0:���L�^,1:�����̏���,2:AI�̏���)

int AI_level = 10; //AI�̃��x���\��(�����l�͓K��)
int input_level = 0; //AI���x�����ݒ肳�ꂽ��
int input_turn = 0; //�����Ȃǂ��ݒ肳�ꂽ��
int input_init = 0; //�u�x���[�h���ݒ肳�ꂽ��
int your_turn = 10; //�����
int sontaku_mode = 10;
int replay_count = 0; //���v���C�Ֆʂ̉��Ԗڂɕۑ����邩
int count = 0;
int tmp_count = 0;

typedef char Move;//�肾�Ƃ킩��₷�����邽�ߌ^��錾�i���Ȃ��Ă��悢�j
Move nextmove;//���̎� 
unsigned char stonenum[2];//�΂̐����L������ϐ�

///////////////////////////AI�p�ɒǉ��@��������
struct Position //�ߋ��̋ǖʂȂǂ��L�����邽�߂̍\����
{
	unsigned char board[BOARDSIZE];
	unsigned char stonenum[2];
};
struct Position history[SEARCH_LIMIT_DEPTH];
//�ꏊ�ɂ��]����^����z��
//�p���ǂ����Ƃ������邱�Ƃ���Ԃ̖ړI
//�{���̓p�^�[���ɂ��]�����o����΂悢���A
//���̒��x�ł�������ۂ����邱�Ƃ͉\
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
///////////////////////////AI�p�ɒǉ��@�����܂�

//2�����̍��W���ꎟ���ɕϊ�
int getposition(int x, int y) { return y * ASIDE + x; };
//��ԑ厖�ȕϐ��A�����ł͈ꎟ���ŕ\��
unsigned char board[BOARDSIZE] = //int�ł��ǂ����A�f�[�^�i�[�̂��Ƃ��l����char�ɂ��Ă���
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

unsigned char init_board[BOARDSIZE] = //�������pboard
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

//���v���C�p
unsigned char replay_board[BOARDSIZE][61] = //���v���C�pboard
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

// �\���֐� display function
void output()
{//�Ƃɂ����\������֐������܂��傤
	int x, y;
	printf("   a  b  c  d  e  f  g  h \n");
	for (y = 1; y <= SIDE; y++)
	{
		printf("%d |", y);
		for (x = 1; x <= SIDE; x++)
			printf("%s|", piece[board[getposition(x, y)]]);//������ƊȌ��ɏ������߂̃q���g
		printf("\n");
	}

	//printf("�q���g : %f%d");

	// �f�o�b�O�p�\��
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

// ���@�肩�ǂ������肷��֐� 
// ���@��Ȃ�TRUE�A�������FALSE��Ԃ�
int isLegalMove(Move pos)
{
	int dirx, diry, dir;
	int pos1;
	// �����̐F�A����̐F�͉����ϐ��ɓ���Ă���
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));

	if (board[pos] != 0)//�󂫃}�X�łȂ���
		return FALSE;

	// pos�̎���8�����𒲂ב���΂����邩���ׂ�
	// 8�������[�v�̏�����1
	for (dirx = -1; dirx <= 1; dirx++)
	{
		for (diry = -ASIDE; diry <= ASIDE; diry += ASIDE)
		{
			dir = dirx + diry;
			if (dir == 0)
				continue;
			pos1 = pos + dir;//pos�ׂ̗̃}�X
			//�ȉ��� board[pos1] �̒��g���`�F�b�N���Ȃ���A
			//pos1�� +=dir���Ă���
			//���߂Ȃ� continue
			if (board[pos1] != opponentcolor)//����̐΂����邩
				continue;
			do // ����̐΂�����Ԃ͎��𒲂ׂ�
			{
				pos1 += dir;
			} while (board[pos1] == opponentcolor);
			// �Ђ�����Ԃ����߂ɂ͍Ō�Ɏ����̐΂��Ȃ��Ƃ����Ȃ�
			if (board[pos1] != color)
				continue;
			//�Ō�܂ŗ����琬��
			return TRUE;
		}
	}
	return FALSE;
}

// ���@�肩�ǂ������肷��֐� 
// ���@��Ȃ�TRUE�A�������FALSE��Ԃ�
int replay_isLegalMove(Move pos)
{
	extern int count;
	int dirx, diry, dir;
	int pos1;
	// �����̐F�A����̐F�͉����ϐ��ɓ���Ă���
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));

	if (replay_board[pos][count] != 0) {
		//�󂫃}�X�łȂ���
		return FALSE;
	}

	// pos�̎���8�����𒲂ב���΂����邩���ׂ�
	// 8�������[�v�̏�����1
	for (dirx = -1; dirx <= 1; dirx++)
	{
		for (diry = -ASIDE; diry <= ASIDE; diry += ASIDE)
		{
			dir = dirx + diry;
			if (dir == 0) {
				continue;
			}
			pos1 = pos + dir;//pos�ׂ̗̃}�X
			//�ȉ��� board[pos1] �̒��g���`�F�b�N���Ȃ���A
			//pos1�� +=dir���Ă���
			//���߂Ȃ� continue
			if (replay_board[pos1][count] != opponentcolor) {
				//����̐΂����邩
				continue;
			}
			do // ����̐΂�����Ԃ͎��𒲂ׂ�
			{
				pos1 += dir;
			} while (replay_board[pos1][count] == opponentcolor);
			// �Ђ�����Ԃ����߂ɂ͍Ō�Ɏ����̐΂��Ȃ��Ƃ����Ȃ�
			if (replay_board[pos1][count] != color) {
				continue;
			}
			//�Ō�܂ŗ����琬��
			return TRUE;
		}
	}
	return FALSE;
}

//��̐��� generation of moves
//����������̐���Ԃ��Ă��� 
int generateMoves(Move moves[])
{
	int num = 0;//�������鍇�@��̐�
	int pos;
	// ���ォ�珇�ɐ΂�u���A���@�肩���ׂ�
	for (pos = 11; pos <= 88; pos++)
	{
		if (isLegalMove(pos))
			moves[num++] = pos;//num�Ԗڂ̔z���
	}
	return num;
}

//��̐��� generation of moves
//����������̐���Ԃ��Ă��� 
int replay_generateMoves(Move moves[])
{
	int num = 0;//�������鍇�@��̐�
	int pos;
	// ���ォ�珇�ɐ΂�u���A���@�肩���ׂ�
	for (pos = 11; pos <= 88; pos++)
	{
		if (replay_isLegalMove(pos))
			moves[num++] = pos;//num�Ԗڂ̔z���
	}
	return num;
}


// �Q�[���I���ǖʂȂ�TRUE��Ԃ� Return TRUE in case of GAME OVER
// ���������肪�O�̎��Ă΂��
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

///////////////////////////AI�p�ɒǉ��@��������

// �Q�[���I���ǖʂ̕]���l��Ԃ� ���� �� �������� 0 ���� �|��
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

// �]���֐��̌v�Z�@Calculation of evaluation function
int getEvaluationValue()
{
	int pos, value, c;
	Move moves[MOVENUM];
	// ���@�萔�̍���]���֐��Ƃ���(���R�x)
	value = generateMoves(moves);
	turn = OPPONENT(turn);
	value -= generateMoves(moves);
	turn = OPPONENT(turn);
	value *= 30;//���R�x�P��30�_�Ƃ��Ă����i�K���j
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
///////////////////////////AI�p�ɒǉ��@�����܂�

// �]���֐��̌v�Z�@Calculation of evaluation function
int replay_getEvaluationValue()
{
	int pos, value, c;
	Move moves[MOVENUM];
	// ���@�萔�̍���]���֐��Ƃ���(���R�x)
	value = replay_generateMoves(moves);
	turn = OPPONENT(turn);
	value -= generateMoves(moves);
	turn = OPPONENT(turn);
	value *= 30;//���R�x�P��30�_�Ƃ��Ă����i�K���j
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

// ���ۂɎ��i�߂�
void makeMove(Move pos, int depth)
{
	int pos1, dir, dirx, diry, count = 0;
	int isLegal = TRUE;
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));

	///////////////////////////AI�p�ɒǉ��@��������
	// �ǖʂ�z��history�ɕۑ�
	memcpy(history[depth].board, board, sizeof(board));// �z��̒��g���R�s�[����̂�memcpy()���g���ƊȒP
	memcpy(history[depth].stonenum, stonenum, sizeof(stonenum));
	///////////////////////////AI�p�ɒǉ��@�����܂�

	if (pos == PASSMOVE)//�p�X��̏ꍇ
	{
		turn = OPPONENT(turn);
		return;
	}
	board[pos] = color;

	// pos�̎���8�����𒲂ב���΂��Ђ�����Ԃ�
	for (dirx = -1; dirx <= 1; dirx++)
	{
		for (diry = -ASIDE; diry <= ASIDE; diry += ASIDE)
		{
			dir = dirx + diry;
			if (dir == 0)
				continue;
			pos1 = pos + dir;//pos�ׂ̗̃}�X
			//�ȉ��� board[pos1] �̒��g���`�F�b�N���Ȃ���A
			//pos1�� +=dir���Ă���
			//���߂Ȃ� continue
			if (board[pos1] != opponentcolor)//����̐΂����邩
				continue;
			do // ����̐΂�����Ԃ͎��𒲂ׂ�
			{
				pos1 += dir;
			} while (board[pos1] == opponentcolor);
			// �Ђ�����Ԃ����߂ɂ͍Ō�Ɏ����̐΂��Ȃ��Ƃ����Ȃ�
			if (board[pos1] != color)
				continue;

			//�Ō�܂ŗ����琬��!���ۂɂЂ�����Ԃ�
			pos1 = pos + dir;
			do // ����̐΂�����ԂЂ�����Ԃ��@
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
///////////////////////////AI�p�ɒǉ��@��������

// ���߂�
void unmakeMove(int depth)
{
	// �z��̒��g���R�s�[����̂�memcpy()���g���ƊȒP
	// �t�̎���makeMove()�ł��Ȃ��Ƃ����Ȃ�
	// �ǖʂ�z��history���畜��
	memcpy(board, history[depth].board, sizeof(board));
	memcpy(stonenum, history[depth].stonenum, sizeof(stonenum));
	turn = OPPONENT(turn);
}

// �T�����čł��]���̍������I��
int search(int depth)
{
	int i;
	int movenum;//��̐�
	Move moves[MOVENUM];//�������z�� an array of moves
	int value;
	int bestvalue = -INFINITY - 1;//�܂��ŏ��l������
	extern int enemy_depth;

	//�ċA
	if (depth >= enemy_depth) {
		return getEvaluationValue();
	}

	//��𐶐�
	movenum = generateMoves(moves);
	if (movenum == 0)
	{
		if (isTerminalNode(TRUE))// Game Over
			return getTerminalValue();
		else // �p�X
			moves[movenum++] = PASSMOVE;
	}
	for (i = 0; i < movenum; i++)
	{
		makeMove(moves[i], depth);//���i��
		value = -search(depth + 1);
		//output();//for Debug
		//printf("i = %d, value = %d, move = %d\n", i, value, moves[i]);//for Debug
		unmakeMove(depth);//���߂�
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

// �T�����čł��]���̒Ⴂ���I��(�u�x)
int min_search(int depth)
{
	int i;
	int movenum;//��̐�
	Move moves[MOVENUM];//�������z�� an array of moves
	int value;
	int minvalue = INFINITY + 1;//�܂��ő�l������
	extern int enemy_depth;

	//�ċA
	if (depth >= enemy_depth) {
		return getEvaluationValue();
	}

	//��𐶐�
	movenum = generateMoves(moves);
	if (movenum == 0)
	{
		if (isTerminalNode(TRUE))// Game Over
			return getTerminalValue();
		else // �p�X
			moves[movenum++] = PASSMOVE;
	}
	for (i = 0; i < movenum; i++)
	{
		makeMove(moves[i], depth);//���i��
		value = min_search(depth + 1);
		//output();//for Debug
		//printf("i = %d, value = %d, move = %d\n", i, value, moves[i]);//for Debug
		unmakeMove(depth);//���߂�
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

//COM�̎�𐶐�����֐�
void comPlayer()
{
	extern int sontaku_mode;
	int value;
	//printf("Com Thinking...\n");
	if (sontaku_mode == 0) { //�u�x�I�t
		value = search(0);
	}
	else if (sontaku_mode == 1) { //�u�x�I��
		value = min_search(0);
	}
	//printf("value = %d\n", value);
}

///////////////////////////AI�p�ɒǉ��@�����܂�

//�l�Ԃ̓��͂��Ǘ�����֐�
Move manPlayer()
{
	//���͂������A���@�肩�`�F�b�N
	char line[256];
	int x, y, num, move;
	Move moves[MOVENUM];
	num = generateMoves(moves);

	// ���@�肪�����ꍇ
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



// �����_���Ɏ��Ԃ��v���C��
//�����_���Ɏ��Ԃ�
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

//�O���[�o���ϐ��Ȃǂ�������
void init()
{
	turn = BLACK_TURN;
	ply = 0;
	stonenum[BLACK_TURN] = 2;
	stonenum[WHITE_TURN] = 2;
	//�Ֆʏ�����
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
