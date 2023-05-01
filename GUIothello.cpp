/*******************************************************************

	〇シンプル〇●リバーシ●
	
	松江工業高等専門学校　情報工学科　J1919 髙橋千賢


[1]音量ボタン画像(ON/OFF)
https://www.gettyimages.co.jp/%E3%82%A4%E3%83%A9%E3%82%B9%E3%83%88/mute-button


*******************************************************************/

/*追加項目
(1)タイトル画面
(2)①ソロプレイ②マルチプレイを用意
(3)音楽を追加
(4)リプレイ機能を追加
(5)その他、UIなど全体的な修正
*/

#include "DxLib.h"
#include "othello.h"
#include<time.h>
#include<math.h>

void ShowBoard();

//通常盤面
int d = 86;//マスの間隔
int r = d / 2;//石の半径
int sx = 160, sy = 70;//盤左上の座標
int ex = sx + d * SIDE;//盤右下のx座標
int ey = sy + d * SIDE;//盤右下のy座標
int rx = ex + 10;//画面右　結果など表示するための場所ｘ座標

//リプレイ盤面
int r_d = 66;//マスの間隔
int r_r = r_d / 2;//石の半径
int r_sx = 50, r_sy = 150;//盤左上の座標
int r_ex = r_sx + r_d * SIDE;//盤右下のx座標
int r_ey = r_sy + r_d * SIDE;//盤右下のy座標
int r_rx = r_ex + 10;//画面右　結果など表示するための場所ｘ座標

int STRCOLOR = GetColor( 0, 0, 200 );//文字色
int STRFONTSIZE = 32;//文字フォントサイズ
int mx, my; //マウス座標
int opt_put_flag = 1; //置ける場所を表示するかどうかのフラグ
int play_mode = 0; //「ぼっち」か「みんなで」かAutoか

bool music_flag = true; //音を鳴らすかどうか true;鳴らす,false:鳴らさない
bool main_on = false; //タイトル画面で音再生をいじったか
bool step_out = false; //GIOothello内で「タイトルへ戻る」が選択されたか

//色取得
static int red = GetColor(255, 0, 0);
static int yellow = GetColor(255, 255, 0);
static int white = GetColor(255, 255, 255);
static int porple = GetColor(204, 0, 204);
static int porple2 = GetColor(102, 0, 153);
static int crimson = GetColor(220, 20, 60);
static int deepskyblue = GetColor(0, 191, 255);
static int green = GetColor(153, 255, 0);
static int blue = GetColor(0, 0, 255);
static int black = GetColor(0, 0, 0);
static int aqua = GetColor(0, 255, 255);
static int thingreen = GetColor(127, 255, 0);
static int s = GetColor(255, 60, 122);
static int s1 = GetColor(255, 222, 173);
static int orange = GetColor(255,153, 0);
static int thin_yellow = GetColor(255, 255, 204);

//void ShowBoard();

//マウスの座標からオセロの座標に変換する関数
int GetPosfromMouse(int MouseX, int MouseY)
{
	//緑盤の範囲内なら
	if (MouseX >= sx && MouseX <= ex && MouseY >= sy && MouseY <= ey) {

		MouseX = (MouseX - sx) / d + 1;
		MouseY = (MouseY - sy) / d + 1;

	}

	return (MouseX + (MouseY * 10));
}

// GUI用のmanPlayer関数
void manPlayerGUI()
{
	//入力をさせ、合法手かチェック
	char line[256];
	int x, y, num, move;
	Move moves[MOVENUM];
	num = generateMoves(moves);
	int MouseX = 0;
	int MouseY = 0;
	static int bgm = LoadSoundMem("musics/bgm.m4a");
	extern bool music_flag;
	//static int music_on = LoadGraph("images/test9.png"); // 音ON画像
	static int music_on = LoadGraph("images/volume_on_origin.jpg"); // 音ON画像
	//static int music_off = LoadGraph("images/test8.png"); // 音OFF画像
	static int music_off = LoadGraph("images/volume_off_origin.jpg"); // 音ON画像
	int control = 0; //この関数内で操作されたか
	extern bool main_on;
	int flag = 0;
	extern bool step_out;
	extern int AI_level;
	extern int your_turn;
	extern int sontaku_mode;
	bool help_flag = false;
	extern int play_mode;

	// 合法手が無い場合
	if (num == 0)
	{
		//printf("PASS!\n");
		//printf("Press Enter!\n");
		//DrawString(rx, ey / 2 - 20, "PASS!\nPress Enter", STRCOLOR);
		//WaitKey();
		//fgets(line, 256, stdin);
		nextmove = PASSMOVE;
		return;
	}
	
	if (opt_put_flag == 1) {
		for (y = 1; y <= SIDE; y++) {
			for (x = 1; x <= SIDE; x++) {
				//置ける場所のマスの色を変える
				for (int i = 0; i < num; i++) { //いまの場所が合法手に存在しているかチェック
					if (x + (10 * y) == moves[i]) { //存在していれば表示
						DrawBox(160 + (86 * (x - 1)), 70 + (86 * (y - 1)), 246 + (86 * (x - 1)), 156 + (86 * (y - 1)), yellow, TRUE);
					}
					DrawBox(160 + (86 * (x - 1)), 70 + (86 * (y - 1)), 246 + (86 * (x - 1)), 156 + (86 * (y - 1)), GetColor(0, 0, 0), FALSE);
				}
			}
		}
	}
	

	//DrawFormatString(200, 200, red, "");
	
	
	while(1) {
		// マウスの座標取得
		GetMousePoint(&MouseX, &MouseY);
		//ShowBoard();
		//ClearDrawScreen();
		//SetDrawScreen(DX_SCREEN_BACK);

		//マウスの座標表示
		//DrawFormatString(0, 0, GetColor(0, 0, 0), "mx : %4d, my : %4d", MouseX, MouseY);

		//Auto Mode
		extern int play_mode;
		if (CheckHitKey(KEY_INPUT_SPACE) == TRUE && play_mode == 0) {
			play_mode = 2;
			break;
		}

		//音停止
		if (control == 0 && music_flag == true && MouseX >= 900 && MouseY <= 150 && MouseX <= 950 && MouseY >= 100 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			StopSoundMem(bgm);
			music_flag = false;
			WaitTimer(500);
			control = 1;
			//flag = 0;
		}

		//音再生
		if (control == 0 && music_flag == false && MouseX >= 900 && MouseY <= 150 && MouseX <= 950 && MouseY >= 100 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			//flag = 1;
			// ループ位置を音の先頭から0.1秒後にセットする(謎の無音の間対策)
			SetLoopPosSoundMem(50, bgm);
			//音再生
			if (control == 1) {
				PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
			}
			music_flag = true;
			WaitTimer(500);
			control = 1;
		}

		if (music_flag == true) {
			DrawExtendGraph(900, 100, 950, 150, music_on, false);
		}
		else if (music_flag == false) {
			DrawExtendGraph(900, 100, 950, 150, music_off, false);
		}

		if (control == 1) {
			SetFontSize(15);
			DrawFormatString(870, 160, black, "手を置いた後に");
			DrawFormatString(870, 175, black, " 実行されます");
			DrawFormatString(852, 80, black, "現在は操作できません");
			SetFontSize(32);
		}

		//相手レベル表示(「みんなで」なら先手後手表示)
		DrawBox(870, 220, 1000, 548, white, TRUE); //白枠
		DrawBox(870, 220, 1000, 548, blue, FALSE); //青枠
		SetFontSize(20);
		DrawFormatString(890, 240, black,      "AIレベル");
		if (AI_level == 0) {
			DrawFormatString(890, 280, green,  "ちょろい");
		}
		else if (AI_level == 1) {
			DrawFormatString(890, 280, blue,   "へいぼん");
		}
		else if (AI_level == 2) {
			DrawFormatString(890, 280, orange, "きびしい");
		}
		else if (AI_level == 3) {
			DrawFormatString(908, 280, red,    "がち");
		}
		else if (play_mode == 1) {
			DrawFormatString(908, 280, black,    "----");
		}
		DrawFormatString(900, 330, black, "あなた");
		if (your_turn == 0) {
			DrawFormatString(910, 370, red, "先手");
			//DrawFormatString(900, 400, black, "　　●");
		}
		else if (your_turn == 1) {
			DrawFormatString(910, 370, blue, "後手");
			//DrawFormatString(900, 400, black, "　　〇");
		}
		else if (play_mode == 1) {
			DrawFormatString(910, 370, black, "----");
		}
		DrawFormatString(882, 420, black, "忖度モード");
		if (sontaku_mode == 0) {
			DrawFormatString(910, 460, blue, "オフ");
		}
		else if (sontaku_mode == 1) {
			DrawFormatString(910, 460, red, "オン");
		}
		else if (play_mode == 1) {
			DrawFormatString(910, 460, black, "----");
		}

		//そのすぐ下にヘルプ
		DrawFormatString(892, 510, black, "(ヘルプ)");
		if (MouseX >= 890 && MouseX <= 975 && MouseY <= 532 && MouseY >= 508) {
			DrawBox(200, 200, 824, 568, white, TRUE); //白枠
			DrawBox(200, 200, 824, 568, blue, FALSE); //青枠
			SetFontSize(15);
			DrawFormatString(485, 210, black, "ヘルプ");

			DrawFormatString(468, 240, black, "<AIレベル>");
			DrawFormatString(290, 260, black, "①やさしい → AIが1手先まで読みます。弱いです。");
			DrawFormatString(290, 280, black, "②へいぼん → AIが3手先まで読みます。まだ勝てます");
			DrawFormatString(290, 300, black, "③きびしい → AIが5手先まで読みます。正直厳しいです。");
			DrawFormatString(290, 320, black, "④おに 　　→ AIが7手先まで読みます。");
			DrawFormatString(292, 335, black, "             相手が打つまでかなり時間がかかります。");

			DrawFormatString(480, 380, black, "<手番>");
			DrawFormatString(330, 400, black, "①先手　    →　自分が先手●になります。");
			DrawFormatString(330, 415, black, "②後手    　→　自分が後手〇になります。");
			DrawFormatString(330, 430, black, "③ランダム　→　先手後手をランダムで決定します。");

			DrawFormatString(460, 475, black, "<忖度モード>");
			DrawFormatString(310, 495, black, "AIが「わざと負けるように」打ちます。");
			DrawFormatString(310, 510, black, "どうしても勝ちたいとき、スッキリしたい人向けです。");
			help_flag = true;
		}
		else {
			if (help_flag == true) {
				ShowBoard();
				manPlayerGUI();
			}
			help_flag = false;
		}

		//対局終了
		SetFontSize(25);
		DrawBox(870, 640, 1000, 685, white, TRUE); //白枠
		DrawBox(870, 640, 1000, 685, blue, FALSE); //青枠
		DrawFormatString(880, 650, black, "対局終了");
		if (MouseX >= 870 && MouseY <= 685 && MouseX <= 1000 && MouseY >= 640 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			//flag = 1; //trueへ
			while (1) {
				GetMousePoint(&MouseX, &MouseY);
				SetDrawScreen(DX_SCREEN_BACK);
				ShowBoard();
				//manPlayerGUI();
				//ClearDrawScreen();
				//表示
				DrawBox(330, 200, 724, 538, white, TRUE); //白枠
				DrawBox(330, 200, 724, 538, blue, FALSE); //青枠
				SetFontSize(20);
				DrawFormatString(470, 230, black, "<注意!!>");
				DrawFormatString(400, 280, black, "タイトル画面へ戻ります。");
				DrawFormatString(365, 340, red, "※現在の対局はリセットされます");
				DrawFormatString(410, 400, black, "本当によろしいですか？");
				DrawFormatString(440, 460, crimson, "はい");
				DrawFormatString(560, 460, blue, "いいえ");
				//「はい」でタイトルへ
				if (MouseX >= 430 && MouseY <= 490 && MouseX <= 490 && MouseY >= 450 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
					step_out = true;
					//ScreenFlip();
					ClearDrawScreen();
					break;
				}
				//「いいえ」でflag=0
				if (MouseX >= 550 && MouseY <= 490 && MouseX <= 630 && MouseY >= 450 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
					//flag = 0;
					//WaitTimer(200);
					SetDrawScreen(DX_SCREEN_FRONT);
					ClearDrawScreen();
					ShowBoard();
					manPlayerGUI();
					//ScreenFlip();
					break;
				}
				ScreenFlip();
			}

		}

		if (step_out == true) {
			break;
		}

		SetFontSize(32);

		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
		{
			move = GetPosfromMouse(MouseX, MouseY);
			if (isLegalMove(move))
			{
				break;
				//continue;
			}
		}
		//ScreenFlip();
		//WaitTimer(10);
		//ClearDrawScreen();
	}	
	if (play_mode == 2 || step_out == true) {
		nextmove = randplayer();
	}
	else {
		nextmove = move;
	}
}

// 画面描画関数
void ShowBoard()
{
	int color;
	int x, y, stone;
	int ry = 40, rdy = STRFONTSIZE + 5;
	//static int GrHandle = LoadGraph( "back.bmp" );//背景画像登録 640x480
	//DrawGraph( 0 , 0 , GrHandle , FALSE );//背景を描く
	//DrawExtendGraph(0, 0, 1024, 768, GrHandle, FALSE);
	DrawBox(0, 0, 1024, 768, s1, TRUE); //背景色
	SetFontSize( STRFONTSIZE ) ;
	DrawBox( sx, sy, ex, ey, GetColor(0, 255, 0), TRUE);//緑の盤を描画
	extern bool music_flag;
	static int music_on = LoadGraph("images/volume_on_origin.jpg"); // 音ON画像
	static int music_off = LoadGraph("images/volume_off_origin.jpg"); // 音ON画像
	extern bool main_on;
	extern bool step_out;
	extern int AI_level;
	extern int your_turn;
	extern int sontaku_mode;

	for ( y = 1; y <= SIDE; y++ )
	{
		for ( x = 1; x <= SIDE; x++ )
		{
			//ここから枠の描画
			//ここでは小さな四角形をたくさん書いているが、別のループで長い線を引いてももちろん良い
			DrawBox(160 + (86 * (x - 1)), 70 + (86 * (y - 1)), 246 + (86 * (x - 1)), 156 + (86 * (y - 1)), GetColor(0, 0, 0), FALSE);
			//ここから石の描画
			stone = board[x + (10 * y)];//石の色を調べる
			if ( stone == 0 )
				continue;
			if ( stone == B )
				color = GetColor( 0, 0, 0 );//黒
			else
				color = GetColor( 255, 255, 255 );//白
			DrawCircle((sx + (d / 2)) + (d * (x - 1)), (sy + (d / 2)) + (d * (y - 1)), 42, color, TRUE);//円で石を描画
		}
	}
	SetFontSize(50);
	DrawFormatString( 280, 10, GetColor( 0, 0, 0 ),"●: %2d", stonenum[BLACK_TURN] );
	//ry += rdy;
	DrawFormatString( 580, 10, GetColor(  255, 255, 255 ),"○: %2d", stonenum[WHITE_TURN] );
	//ry += rdy * 2;
	//SetFontSize(20);
	//DrawFormatString(800, 710, red, "ply : %d", ply);

	extern int play_mode;
	if (play_mode == 2) {
		SetFontSize(20);
		DrawFormatString(10, 748, black, "Auto Mode");
	}

	SetFontSize(25);
	DrawBox(870, 640, 1000, 685, white, TRUE); //白枠
	DrawBox(870, 640, 1000, 685, blue, FALSE); //青枠
	DrawFormatString(880, 650, black, "対局終了");

	//相手レベル表示(「みんなで」なら先手後手表示)
	DrawBox(870, 220, 1000, 548, white, TRUE); //白枠
	DrawBox(870, 220, 1000, 548, blue, FALSE); //青枠
	SetFontSize(20);
	DrawFormatString(890, 240, black, "AIレベル");
	if (AI_level == 0) {
		DrawFormatString(890, 280, green, "ちょろい");
	}
	else if (AI_level == 1) {
		DrawFormatString(890, 280, blue, "へいぼん");
	}
	else if (AI_level == 2) {
		DrawFormatString(890, 280, orange, "きびしい");
	}
	else if (AI_level == 3) {
		DrawFormatString(908, 280, red, "がち");
	}
	DrawFormatString(900, 330, black, "あなた");
	if (your_turn == 0) {
		DrawFormatString(910, 370, red, "先手");
		//DrawFormatString(900, 400, black, "　　●");
	}
	else if (your_turn == 1) {
		DrawFormatString(910, 370, blue, "後手");
		//DrawFormatString(900, 400, black, "　　〇");
	}
	DrawFormatString(882, 420, black, "忖度モード");
	if (sontaku_mode == 0) {
		DrawFormatString(910, 460, blue, "オフ");
	}
	else if (sontaku_mode == 1) {
		DrawFormatString(910, 460, red, "オン");
	}

	//そのすぐ下にヘルプ
	DrawFormatString(892, 510, black, "(ヘルプ)");


	SetFontSize(32);

	if (music_flag == true) {
		DrawExtendGraph(900, 100, 950, 150, music_on, false);
	}
	else if (music_flag == false) {
		DrawExtendGraph(900, 100, 950, 150, music_off, false);
	}

	SetFontSize(25);
}

// 画面描画関数
void Replay_Board(int a)
{

	//DrawFormatString(500, 100, red, "in the Replay_Board");

	int color;
	int x, y, stone;
	int ry = 40, rdy = STRFONTSIZE + 5;
	//static int GrHandle = LoadGraph("back.bmp");//背景画像登録 640x480
	DrawBox(0, 0, 1024, 768, s1, TRUE); //背景色
	int num, move;
	Move moves[MOVENUM];
	int black_count = 0; //関数内で黒の個数を独自カウント
	int white_count = 0; //関数内で白の個数を独自カウント
	extern int count;

	//DrawGraph( 0 , 0 , GrHandle , FALSE );//背景を描く
	//DrawExtendGraph(0, 0, 1024, 768, GrHandle, FALSE);
	DrawBox(0, 0, 1024, 768, s1, TRUE); //背景色
	//DrawBox(0, 0, 1024, 768, thin_yellow, TRUE); //背景色
	SetFontSize(STRFONTSIZE);
	DrawBox(r_sx, r_sy, r_ex, r_ey, GetColor(0, 255, 0), TRUE);//緑の盤を描画

	//num番目(num手目)のリプレイ画像を表示
	for (y = 1; y <= SIDE; y++)
	{
		for (x = 1; x <= SIDE; x++)
		{
			//通常のボードより少し小さめに描く
			DrawBox(50 + (66 * (x - 1)), 150 + (66 * (y - 1)), 116 + (66 * (x - 1)), 216 + (66 * (y - 1)), GetColor(0, 0, 0), FALSE);
			//ここから石の描画
			stone = replay_board[x + (10 * y)][a];//石の色を調べる
			if (stone == 0) {
				continue;
			}
			if (stone == B) {
				color = GetColor(0, 0, 0);//黒
				black_count++;
			}
			else {
				color = GetColor(255, 255, 255);//白
				white_count++;
			}
			DrawCircle((r_sx + (r_d / 2)) + (r_d * (x - 1)), (r_sy + (r_d / 2)) + (r_d * (y - 1)), 32, color, TRUE);//円で石を描画
		}
	}

	if (play_mode == 0) {
		//最善手の赤枠
		num = replay_generateMoves(moves);

		for (y = 1; y <= SIDE; y++) {
			for (x = 1; x <= SIDE; x++) {
				//置ける場所のマスの色を変える
				for (int i = 0; i < num; i++) { //いまの場所が合法手に存在しているかチェック
					if (x + (10 * y) == moves[i]) { //存在していれば表示
						DrawBox(50 + (66 * (x - 1)), 150 + (66 * (y - 1)), 116 + (66 * (x - 1)), 216 + (66 * (y - 1)), yellow, TRUE);
					}
					DrawBox(50 + (66 * (x - 1)), 150 + (66 * (y - 1)), 116 + (66 * (x - 1)), 216 + (66 * (y - 1)), GetColor(0, 0, 0), FALSE);
				}
			}
		}
	}
	
	count++;
	SetFontSize(45);
	DrawFormatString(120, 100, GetColor(0, 0, 0), "●: %2d", black_count);
	//ry += rdy;
	DrawFormatString(380, 100, GetColor(255, 255, 255), "○: %2d", white_count);
	//ry += rdy * 2;
	//DrawFormatString( rx, 10, STRCOLOR,"%d 手", ply );
	SetFontSize(25);
}


//numで割った余りをランダムで返す
int random_return(int num) {

	int debug_cnt = 0;
	srand((unsigned int)time(NULL)); // 現在時刻の情報で初期化
	int sum = rand() % num;
	return sum;

}

//与えられた番号によって、コメントを表示
void print_comment(int num) {

	SetFontSize(30);

	DrawString(300, ey / 2 - 60, "～製作者からのアドバイス～", black);

	if (num == 0) {
		DrawString(210, ey / 2 - 10, "      まじでやってる？？本気？？", red);
	}
	else if (num == 1) {
		DrawString(205, ey / 2 - 10, "その負けっぷり、お話にならないんだけど", red);

	}
	else {
		DrawString(205, ey / 2 - 10, "     もう脳が退化してんじゃない？", red);
	}

}

//与えられた番号によって、アドバイスを表示
void print_advice(int num) {
	
	SetFontSize(30);

	if (num == 0) {
		DrawString(150, ey / 2 + 25, "     もっと時間をかけて慎重にやってみよう！", black);
	}
	else if (num == 1) {
		DrawString(150, ey / 2 + 25, "欲張ってたくさんひっくり返さないように気を付けよう！", black);
	}
	else {
		DrawString(150, ey / 2 + 25, "     端っこに置くときはもう少し考えてみよう！", black);
	}

}

void ai_comment() {

	extern int log_result;

	SetFontSize(30);
	//int ran = random_return(3);
	//aiの僅差勝利なら
	if (log_result == 2) {
		if (abs(stonenum[BLACK_TURN] - stonenum[WHITE_TURN]) <= 10) {
			DrawFormatString(630, 330, black, "・なかなか強かったよ");
		}
		else if (abs(stonenum[BLACK_TURN] - stonenum[WHITE_TURN]) <= 25) {
			DrawFormatString(630, 330, black, "・弱くて余裕だったよ");
		}
		else {
			SetFontSize(20);
			DrawFormatString(650, 330, black, "・もう少しオセロを勉強しようね");
		}
	}
	else if (log_result == 1 || log_result == 0) {
		DrawFormatString(650, 330, black, "・勝利おめでとう！");
	}
	SetFontSize(30);
	//DrawFormatString(630, 330, black, "・今の手は良くないね");

}

// WinMain関数
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow ) {

	// 画面サイズの設定
	SetGraphMode(1024, 768, 32);

	int manturn = BLACK_TURN;
	extern int mx, my;
	int debug_cnt = 0;
	int cnt = 0;
	int tmp1 = 0;
	int cnt_title = 0;
	extern int tmp_count;
	extern bool music_flag;
	extern bool main_on;
	extern bool step_out;
	
	// タイトルを 変更
	SetMainWindowText( "〇シンプル〇●リバーシ●" ) ;
	// ウインドウモードに変更
	ChangeWindowMode( TRUE ) ;
	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 )		// ＤＸライブラリ初期化処理
		return -1 ;			// エラーが起きたら直ちに終了
	// マウスを表示状態にする
	SetMouseDispFlag( TRUE ) ;

	static int put_man = LoadSoundMem("musics/putMAN.wav");
	static int put_com = LoadSoundMem("musics/putCOM.wav");
	static int bgm = LoadSoundMem("musics/bgm.m4a");
	static int music_on = LoadGraph("images/volume_on_origin.jpg"); // 音ON画像
	static int music_off = LoadGraph("images/volume_off_origin.jpg"); // 音ON画像

	// ループ位置を音の先頭から0.1秒後にセットする(謎の無音の間対策)
	SetLoopPosSoundMem(50, bgm);

	//音再生
	PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);

	title: //後に戻ってくる場所

	if (step_out == true) {
		step_out = false;
	}

	if (music_flag == false) {
		// ループ位置を音の先頭から0.1秒後にセットする(謎の無音の間対策)
		SetLoopPosSoundMem(50, bgm);
		//音再生
		PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
		//フラグをONへ
		music_flag = true;
		//タイトル画面でいじったかどうかはOFFにする
		main_on = false;
	}

	init();
	//ShowBoard();
	SetFontSize(40);

	WaitTimer(150);

	//タイトル画面
	while(1)
	{
		// マウスの座標取得
		GetMousePoint(&mx, &my);

		// 描画先を裏画面にする (画面チカチカ防止策①)
		SetDrawScreen(DX_SCREEN_BACK);

		DrawBox(0, 0, 1024, 768, s1, TRUE);

		SetFontSize(110);
		
		DrawFormatString(110, 100, white, "●シンプル●");
		DrawFormatString(260, 220, black, "●リバーシ●");

		SetFontSize(40);
		DrawString(390, 430, "ぼっちで遊ぶ", red);
		DrawString(390, 515, " 仲間と遊ぶ", red);
		DrawString(410, 600, "   終了", red);

		//音停止
		if (music_flag == true && mx >= 920 && my <= 750 && mx <= 970 && my >= 700 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			WaitTimer(500);
			StopSoundMem(bgm);
			music_flag = false;
			main_on = true;
		}

		//音再生
		if (music_flag == false && mx >= 920 && my <= 750 && mx <= 970 && my >= 700 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			WaitTimer(500);
			// ループ位置を音の先頭から0.1秒後にセットする(謎の無音の間対策)
			SetLoopPosSoundMem(50, bgm);
			//音再生
			PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
			music_flag = true;
			main_on = false;
		}

		//音楽再生
		if (music_flag == true) {
			DrawExtendGraph(920, 700, 970, 750, music_on, false);
		}
		else if (music_flag == false) {
			DrawExtendGraph(920, 700, 970, 750, music_off, false);
		}

		//DrawFormatString(0, 0, yellow, "mx : %d, my : %d", mx, my);

		// メッセージループに代わる処理をする
		if (ProcessMessage() == -1) {
			break;		// エラーが起きたらループを抜ける
		}

		//ぼっち
		if (mx >= 390 && mx <= 640 && my <= 470 && my >= 430 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			play_mode = 0;
			break;
		}
		//みんなで
		if (mx >= 390 && mx <= 640 && my <= 555 && my >= 515 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			play_mode = 1;
			break;
		}
		//終了
		if (mx >= 470 && mx <= 550 && my <= 640 && my >= 600 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
			//音停止
			StopSoundMem(bgm);
			DxLib_End();				// ＤＸライブラリ使用の終了処理
			return 0;				// ソフトの終了 
		}
		cnt_title += 5;
		//WaitTimer(100);
		// 裏画面の内容を表画面に反映 (画面チカチカ防止策②)
		ScreenFlip();
		ClearDrawScreen();
	}

	ClearDrawScreen();
	DrawBox(0, 0, 1024, 768, s1, TRUE); //背景色
	DrawBox(100, 120, 924, 648, white, TRUE); //白枠
	DrawBox(100, 120, 924, 648, blue, FALSE); //青枠

	//対局前オプション等設定画面「ぼっち」のみ使用
	if (play_mode == 0) {
		while (1) {

			// 描画先を裏画面にする (画面チカチカ防止策①)
			SetDrawScreen(DX_SCREEN_BACK);

			DrawBox(0, 0, 1024, 768, s1, TRUE); //背景色
			DrawBox(100, 120, 924, 648, white, TRUE); //白枠
			DrawBox(100, 120, 924, 648, blue, FALSE); //青枠

			// マウスの座標取得
			GetMousePoint(&mx, &my);

			//マウスの座標表示
			//DrawFormatString(100, 100, yellow, "mx : %d, my : %d", mx, my);

			// メッセージループに代わる処理をする
			if (ProcessMessage() == -1) {
				break;		// エラーが起きたらループを抜ける
			}

			SetFontSize(20);
			DrawFormatString(830, 90, black, "(ヘルプ)");

			SetFontSize(35);
			DrawFormatString(370, 140, black, "オプション設定");

			SetFontSize(30);
			DrawFormatString(390, 220, black, "   AIレベル   ");
			DrawFormatString(200, 290, green, "ちょろい");
			DrawFormatString(370, 290, blue, "へいぼん");
			DrawFormatString(540, 290, orange, "きびしい");
			DrawFormatString(720, 290, red, "がち");

			//難易度の選択判定
			if (mx >= 200 && mx <= 320 && my <= 320 && my >= 290 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				AI_level = 0; //やさしい
				enemy_depth = 1; //AIが読む深さ
				input_level = 1;
			}
			if (mx >= 370 && mx <= 490 && my <= 320 && my >= 290 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				AI_level = 1; //ふつう
				enemy_depth = 3; //AIが読む深さ
				input_level = 1;
			}
			if (mx >= 540 && mx <= 660 && my <= 320 && my >= 290 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				AI_level = 2; //きびしい
				enemy_depth = 5; //AIが読む深さ
				input_level = 1;
			}
			if (mx >= 720 && mx <= 780 && my <= 320 && my >= 290 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				AI_level = 3; //おに
				enemy_depth = 7; //AIが読む深さ
				input_level = 1;
			}

			//選択の四角判定
			if (AI_level == 0) { //やさしい
				DrawBox(190, 280, 330, 330, black, FALSE);
			}
			if (AI_level == 1) { //へいぼん
				DrawBox(360, 280, 500, 330, black, FALSE);
			}
			if (AI_level == 2) { //きびしい
				DrawBox(530, 280, 670, 330, black, FALSE);
			}
			if (AI_level == 3) { //おに
				DrawBox(710, 280, 790, 330, black, FALSE);
			}

			//手番選択
			DrawFormatString(460, 360, black, "手番");
			DrawFormatString(290, 430, red, "先手");
			DrawFormatString(460, 430, blue, "後手");
			DrawFormatString(630, 430, orange, "ランダム");

			//手番の選択判定
			if (mx >= 290 && mx <= 350 && my <= 460 && my >= 430 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				your_turn = 0; //先手
				input_turn = 1;
			}
			if (mx >= 460 && mx <= 520 && my <= 460 && my >= 430 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				your_turn = 1; //後手
				input_turn = 1;
			}
			if (mx >= 630 && mx <= 750 && my <= 460 && my >= 430 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				your_turn = 2; //ランダム
				input_turn = 1;
			}

			//選択の四角判定
			if (your_turn == 0) { //先手
				DrawBox(280, 420, 360, 470, black, FALSE);
			}
			if (your_turn == 1) { //後手
				DrawBox(450, 420, 530, 470, black, FALSE);
			}
			if (your_turn == 2) { //ランダム
				DrawBox(620, 420, 760, 470, black, FALSE);
			}

			//忖度モード選択
			DrawFormatString(420, 500, black, "忖度モード");
			DrawFormatString(380, 570, red, "オン");
			DrawFormatString(550, 570, blue, "オフ");

			//忖度モードの選択判定
			if (mx >= 380 && mx <= 440 && my <= 600 && my >= 570 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				sontaku_mode = 1; //オン
				input_init = 1;
			}
			if (mx >= 550 && mx <= 610 && my <= 600 && my >= 570 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				sontaku_mode = 0; //オフ
				input_init = 1;
			}

			//選択の四角判定
			if (sontaku_mode == 1) { //オン
				DrawBox(370, 560, 450, 610, black, FALSE);
			}
			if (sontaku_mode == 0) { //オフ
				DrawBox(540, 560, 620, 610, black, FALSE);
			}

			//ヘルプの判定
			if (mx >= 830 && mx <= 910 && my <= 110 && my >= 90) {
				DrawBox(200, 200, 824, 568, white, TRUE); //白枠
				DrawBox(200, 200, 824, 568, blue, FALSE); //青枠
				SetFontSize(15);
				DrawFormatString(485, 210, black, "ヘルプ");

				DrawFormatString(468, 240, black, "<AIレベル>");
				DrawFormatString(290, 260, black, "①やさしい → AIが1手先まで読みます。弱いです。");
				DrawFormatString(290, 280, black, "②へいぼん → AIが3手先まで読みます。まだ勝てます");
				DrawFormatString(290, 300, black, "③きびしい → AIが5手先まで読みます。正直厳しいです。");
				DrawFormatString(290, 320, black, "④おに 　　→ AIが7手先まで読みます。");
				DrawFormatString(292, 335, black, "             相手が打つまでかなり時間がかかります。");

				DrawFormatString(480, 380, black, "<手番>");
				DrawFormatString(330, 400, black, "①先手　    →　自分が先手●になります。");
				DrawFormatString(330, 415, black, "②後手    　→　自分が後手〇になります。");
				DrawFormatString(330, 430, black, "③ランダム　→　先手後手をランダムで決定します。");

				DrawFormatString(460, 475, black, "<忖度モード>");
				DrawFormatString(310, 495, black, "AIが「わざと負けるように」打ちます。");
				DrawFormatString(310, 510, black, "どうしても勝ちたいとき、スッキリしたい人向けです。");
			}

			//タイトルへ戻る
			SetFontSize(15);
			DrawFormatString(110, 95, black, "タイトルへ戻る");
			if (mx >= 110 && mx <= 215 && my <= 110 && my >= 95 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
				//DrawBox(200, 200, 824, 568, white, TRUE); //白枠
				//DrawBox(200, 200, 824, 568, blue, FALSE); //青枠
				// 裏画面の内容を表画面に反映 (画面チカチカ防止策②)
				//ScreenFlip();
				//ClearDrawScreen();
				SetDrawScreen(DX_SCREEN_FRONT);
				goto title; //タイトル画面へ
			}

			//入力完了(すべて押されていれば表示)
			if (input_level == 1 && input_turn == 1 && input_init == 1) {
				SetFontSize(30);
				DrawString(440, 680, "はじめる", black);
				DrawBox(430, 670, 570, 720, black, FALSE);
				//クリック判定
				if (mx >= 430 && mx <= 570 && my <= 720 && my >= 670 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
					break;;
				}
			}

			// 裏画面の内容を表画面に反映 (画面チカチカ防止策②)
			ScreenFlip();

			//画面クリア
			ClearDrawScreen();
		}
	}

	DrawBox(0, 0, 1024, 768, s1, TRUE); //背景色
	
	// 描画先を表画面にする
	SetDrawScreen(DX_SCREEN_FRONT);

	ShowBoard();
	SetFontSize(32);

	//枠のスライド演出
	for (int i = 16; i >= 0; i--) {
		SetDrawScreen(DX_SCREEN_BACK);
		DrawBox(0, ey / 2 - 100, 1024 - (i * 64), ey / 2 + 100, white, TRUE);//白枠
		DrawBox(0, ey / 2 - 100, 1024 - (i * 64), ey / 2 + 100, blue, FALSE);//青枠
		SetDrawScreen(DX_SCREEN_FRONT);
		ScreenFlip();
		WaitTimer(50);
	}
	WaitTimer(200);
	
	//「ぼっち」用
	if (play_mode == 0) {

		DrawFormatString(300, ey / 2 - 70, black, "あなたは　　　　　です。");
		WaitTimer(500);
		
		if (your_turn == 2) {
			for (int i = 22; i >= 0; i--) {
				SetDrawScreen(DX_SCREEN_BACK);
				DrawBox(470, ey / 2 - 70, 580, ey / 2 - 38, white, TRUE); //塗りつぶし用
				if (i % 2 == 0) {
					DrawFormatString(470, ey / 2 - 70, black, "先手●");
				}
				else {
					DrawFormatString(470, ey / 2 - 70, black, "後手〇");
				}
				SetDrawScreen(DX_SCREEN_FRONT);
				ScreenFlip();
				WaitTimer(75);
			}
		}

		//「ランダム」が選択されていれば
		if (your_turn == 2) {
			your_turn = random_return(2); //先手後手を決定
		}

		//最終表示
		DrawBox(470, ey / 2 - 70, 580, ey / 2 - 38, white, TRUE); //塗りつぶし用
		if (your_turn == 0) {
			DrawFormatString(470, ey / 2 - 70, black, "先手●");
		}
		else if (your_turn == 1) {
			DrawFormatString(470, ey / 2 - 70, black, "後手〇");
		}
		WaitTimer(500);
		DrawFormatString(370, ey / 2 - 10, red, "ゲームスタート！");
		SetFontSize(20);
		DrawString(430, ey / 2 + 50, "Push ANY Key!", STRCOLOR);
		WaitKey();

		ShowBoard();
	}
	else if (play_mode == 1) { //「みんなで」用

		DrawFormatString(350, ey / 2 - 70, black, "１P 黒● : 先手");
		DrawFormatString(350, ey / 2 - 20, black, "２P 白〇 : 後手");
		WaitTimer(500);
		//DrawFormatString(370, ey / 2 - 10, red, "ゲームスタート！");
		SetFontSize(20);
		DrawString(430, ey / 2 + 40, "Push ANY Key!", STRCOLOR);
		WaitKey();

		ShowBoard();
	}
	
	//ClearDrawScreen();
	//ShowBoard();

	if (music_flag == false && main_on == true) {
		// ループ位置を音の先頭から0.1秒後にセットする(謎の無音の間対策)
		SetLoopPosSoundMem(50, bgm);
		//音再生
		PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
	}

	music_flag = true;
	main_on = false;

	// 対局メインループ
	while ( !CheckHitKey( KEY_INPUT_ESCAPE ) )//ESCでゲーム終了
	{
		//SetDrawScreen(DX_SCREEN_BACK);
		//DrawBox(0, 0, 1024, 768, s1, TRUE); //背景色
		//ShowBoard();
		int num, result;
		Move moves[MOVENUM];
		// メッセージループに代わる処理をする
		if (ProcessMessage() == -1) {
			break;		// エラーが起きたらループを抜ける
		}
		num = generateMoves( moves );
		if ( isTerminalNode(FALSE))//終局かチェック
		{
			SetFontSize(50) ;
			//石の数で勝ち負け判定し表示
			if (your_turn == 0) { //自分が先手
				result = stonenum[BLACK_TURN] - stonenum[WHITE_TURN];//こんな感じで
			}
			else { //自分が後手
				result = stonenum[WHITE_TURN] - stonenum[BLACK_TURN];//こんな感じで
			}
			// result により表示を変える
			if (result == 0) {
				//SetDrawScreen(DX_SCREEN_FRONT);
				DrawString(rx-10, ey / 2 - 20, "引き分け!!", STRCOLOR);
			}
			else {
				//DrawFormatString(rx, ey / 2 - 20, STRCOLOR, "%s WIN!!", (result > 0 ? "BLACK" : "WHITE"));
				//SetDrawScreen(DX_SCREEN_FRONT);
				DrawBox(0, ey / 2 - 155, 1024, ey / 2 + 145, white, TRUE);
				DrawBox(0, ey / 2 - 155, 1024, ey / 2 + 145, blue, FALSE);
				//ぼっち
				if (play_mode == 0) {
					DrawFormatString(385, ey / 2 - 130, STRCOLOR, "You %s", (result > 0 ? "WIN!" : "LOSE!"));
					//負けたなら煽り文句
					if (result <= 0) {
						print_comment(random_return(3));
						log_result = 2;
					}
					else { //勝ちなら褒める
						SetFontSize(30);
						DrawString(360, ey / 2 - 40, " 勝利おめでとう！", black);
						log_result = 1;
					}
					print_advice(random_return(3)); //(勝ちでも負けでも)アドバイス
				}
				else { //みんなで
					DrawFormatString(385, ey / 2 - 130, STRCOLOR, "%s WIN!", (result < 0 ? "BLACK" : "WHITE"));
					SetFontSize(30);
					DrawString(300, ey / 2 - 60, "～製作者からのアドバイス～", black);
					DrawString(300, ey / 2 - 10, "        < 愚者へ >        ", black);
					print_advice(random_return(3));
				}
				DrawString(410, ey / 2 + 95, "Push ANY Key!", STRCOLOR);
				WaitKey();
				while (1) {
					static int tmp_cnt = 0;
					// マウスの座標取得
					GetMousePoint(&mx, &my);
					//DrawFormatString(0, 0, yellow, "mx : %d, my : %d", mx, my);
					SetDrawScreen(DX_SCREEN_BACK);
					DrawBox(0, ey / 2 - 155, 1024, ey / 2 + 145, white, TRUE); //白枠
					DrawBox(0, ey / 2 - 155, 1024, ey / 2 + 145, blue, FALSE); //青枠
					DrawFormatString(440, 330, red, "リプレイ");
					DrawFormatString(390, 400, blue, "タイトルへ戻る");
					//DrawFormatString(500, 500, red, "tmp_cnt : %d", tmp_cnt);
					SetDrawScreen(DX_SCREEN_FRONT);
					//選択判定
					if (mx >= 440 && mx <= 560 && my <= 360 && my >= 330 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
						//リプレイ
						//60手目(最終手)だけ例外で専用保存
						for (int i = 0; i < 100; i++) {
							replay_board[i][60] = board[i];
							//DrawFormatString(300, 0 + (30 * i), red, "i : %d", i);
						}
						//DrawFormatString(500, 50, red, "forを抜けました。");
						int auto_flag = 0; //自動再生確認フラグ(0 : OFF, 1 : ON)
						//WaitTimer(100);
						//表示メインループ
						//DrawFormatString(500, 150, red, "表示メインに入ります。");
						if (play_mode == 1 && ply != 60) {
							ply++;
						}
						else if (play_mode == 1 && ply == 60) {
							ply = 60;
						}
						for (int i = 0; i <= ply; i++) {
							//DrawFormatString(500, 200, red, "表示メインに入っています。");

							SetDrawScreen(DX_SCREEN_BACK);

							//DrawFormatString(500, 250, red, "Replay_Board直前");
							Replay_Board(i);
							//DrawFormatString(500, 300, red, "Replay_Board直後");

							DrawBox(660, 120, 960, 200, white, TRUE); //白枠
							DrawBox(660, 120, 960, 200, blue, FALSE); //青枠
							SetFontSize(40);
							DrawFormatString(725, 140, red, "リプレイ");
							SetFontSize(30);

							DrawFormatString(680, 250, red, "AIからのコメント");
							//置いた場所は何点だったかによって、コメントを変える
							//DrawFormatString(630, 330, black, "・今の手は良くないね");
							ai_comment();
							if (play_mode == 0) {
								DrawBox(660, 405, 940, 475, white, TRUE); //
								DrawBox(660, 405, 940, 475, blue, FALSE); //
								DrawFormatString(680, 425, black, "置ける場所");
								DrawFormatString(760, 425, yellow, "　　　　■");
							}
							/*
							if (tmp_count >= 2) {
								DrawFormatString(630, 510, blue, "前の手へ");
							}
							*/
							DrawFormatString(730, 510, orange, "次の手へ");

							DrawBox(670, 580, 900, 630, black, FALSE); //
							//自動再生ON/OFF(オプション設定画面みたいにする)
							DrawFormatString(680, 590, black, "自動再生");
							if (auto_flag == 0) {
								DrawFormatString(845, 590, blue, "OFF");
							}
							else if (auto_flag == 1) {
								DrawFormatString(842, 590, red, "ON");
								SetFontSize(20);
								DrawFormatString(670, 640, red, "左クリック長押しで停止");
								SetFontSize(30);
							}

							//DrawFormatString(500, 600, red, "tmp_count : %d", tmp_count);

							SetDrawScreen(DX_SCREEN_FRONT);

							//手動ならば
							if (auto_flag == 0) {
								while (1) {

									if (tmp_count == 0) {
										//break;
									}

									GetMousePoint(&mx, &my);

									//次の手へ
									if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && mx >= 730 && mx <= 850 && my <= 540 && my >= 510) {
										WaitTimer(150);
										break;
									}
									//前の手へ(こいつが悪い)
									/*
									if (auto_flag == 0 && mx >= 830 && mx <= 950 && my <= 540 && my >= 510 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
										i -= 2;
										count -= 1;
										WaitTimer(150);
										break;
									}
									*/
									//自動再生
									if (mx >= 670 && mx <= 900 && my <= 630 && my >= 580 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
										auto_flag = 1;
										//i--; //手は進めない
										WaitTimer(200);
										break;
									}
									//debug
									if (mx <= 0 && my <= 0) {
										break;
									}
								}
							}
							else if (auto_flag == 1) {
								WaitTimer(500);
							}
							//OFF
							if (auto_flag == 1 && mx >= 670 && mx <= 860 && my <= 620 && my >= 580 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
								auto_flag = 0;
								WaitTimer(200);
								//i--; //手は進めない
								//break;
							}
							//WaitKey();
							//WaitTimer(100);
							tmp_count++;
						}
						DrawFormatString(660, 690, blue, "Press Any Key!!");
						WaitKey();
						goto title; //タイトル画面へ
					}
					if (mx >= 390 && mx <= 600 && my <= 430 && my >= 400 && (GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {
						//タイトルへ戻る
						goto title; //タイトル画面へ
					}
					tmp_cnt++;
				}
			}
		}
		if (play_mode == 0) { //「ぼっち」
			if (your_turn == 0) { //自分が先手のとき
				if (turn == manturn) {
					manPlayerGUI();
					if (step_out == true) {
						goto title;
					}
					//nextmove = randplayer(); //debug
					// ゲーム音再生
					if (music_flag == true) {
						PlaySoundMem(put_man, DX_PLAYTYPE_BACK);
					}
				}
				else {
					comPlayer();
					// ゲーム音再生
					if (music_flag == true) {
						PlaySoundMem(put_com, DX_PLAYTYPE_BACK);
					}
				}
			}
			else if (your_turn == 1) { //自分が後手
				if (turn == manturn) {
					comPlayer();
					if (music_flag == true) {
						PlaySoundMem(put_com, DX_PLAYTYPE_BACK);
					}
				}
				else {
					manPlayerGUI();
					if (step_out == true) {
						goto title;
					}
					//nextmove = randplayer(); //debug
					// ゲーム音再生
					if (music_flag == true) {
						PlaySoundMem(put_man, DX_PLAYTYPE_BACK);
					}
				}
			}
		}
		else if (play_mode == 1) { //「みんなで」
			if (turn == manturn) {
				manPlayerGUI();
				if (step_out == true) {
					goto title;
				}
				// ゲーム音再生
				if (music_flag == true) {
					PlaySoundMem(put_man, DX_PLAYTYPE_BACK);
				}
			}
			else {
				manPlayerGUI();
				if (step_out == true) {
					goto title;
				}
				// ゲーム音再生
				if (music_flag == true) {
					PlaySoundMem(put_com, DX_PLAYTYPE_BACK);
				}
			}
		}
		else if (play_mode == 2) { //発表用(Auto mode)
			if (turn == manturn) {
				nextmove = randplayer();
				// ゲーム音再生
				if (music_flag == true) {
					PlaySoundMem(put_man, DX_PLAYTYPE_BACK);
				}
				WaitTimer(700);
			}
			else {
				comPlayer();
				// ゲーム音再生
				if (music_flag == true) {
					PlaySoundMem(put_com, DX_PLAYTYPE_BACK);
				}
				WaitTimer(700);
			}
		}
		static int fflag = 0;
		if (music_flag == false) {
			StopSoundMem(bgm);
			fflag = 1;
		}
		else if (music_flag == true && fflag == 1) {
			// ループ位置を音の先頭から0.1秒後にセットする(謎の無音の間対策)
			SetLoopPosSoundMem(50, bgm);
			//音再生
			PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
			fflag = 0;
		}
		//リプレイ用に保存
		for (int i = 0; i < 100; i++) {
			replay_board[i][ply] = board[i];
			//board[i] = replay_board[i][replay_count]; //debug
		}
		makeMove( nextmove, 0 );
		//パスの処理
		if (nextmove == PASSMOVE) {
			DrawBox(0, ey / 2 - 60, 1024, ey / 2 + 60, white, TRUE);//白枠
			DrawBox(0, ey / 2 - 60, 1024, ey / 2 + 60, blue, FALSE);//青枠
			DrawString(460, ey / 2 - 30, "パス！", STRCOLOR);
			DrawString(410, ey / 2 + 10, "Push ANY Key!", STRCOLOR);
			if (play_mode == 0 || play_mode == 1) {
				WaitKey();
			}
			else if (play_mode == 2) {
				WaitTimer(1000);
			}
		}
		else {
			ply++;
		}
		//SetDrawScreen(DX_SCREEN_FRONT);
		ShowBoard();
		//DrawFormatString(300, 300, red, "replay_count : %d", replay_count);
		//WaitKey();
		//なぜか条件分岐しないといけない模様
		if (replay_count >= 60) {
			replay_count = 60;
		}
		else if(replay_count <= 60) {
			replay_count++;
		}
		WaitTimer(100);
		//ClearDrawScreen();
	}
	//音停止
	StopSoundMem(bgm);
	DxLib_End() ;				// ＤＸライブラリ使用の終了処理
	return 0 ;				// ソフトの終了 
}