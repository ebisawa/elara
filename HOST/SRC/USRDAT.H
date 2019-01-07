
#if !defined(USRDAT_H)
#define USRDAT_H
#include "chat.h"

#define SS_WAIT				0
#define SS_SHUTDOWN			1

#define CS_WAIT				0
#define CS_LOGIN			1
#define CS_USING			2
#define CS_SHORT			3
#define CS_INIT				4
#define CS_OFF				5
#define CS_SHUTDOWN			6

#define ESC_COLOR			0x01
#define ESC_CURSOR			0x02

#define PRTCL_ASCII			1
#define PRTCL_XMODEM		2
#define PRTCL_YMODEM		3
#define PRTCL_YMODEM_G		4
#define PRTCL_YMODEM_BATCH	5

typedef struct terminal {			/* 8 bytes */
	char esc;								/* エスケープ使用制御   */
	char term_width;						/* 端末画面横幅         */
	char term_height;						/* 端末画面縦幅         */
	char up_protocol;						/* upload   プロトコル  */
	char down_protocol;						/* download プロトコル  */
	char more;								/* more                 */
	char editor_width;						/* エディタ横幅         */
	char editor_lm;							/* エディタ左マージン   */
	char prompt[PROMPT_MAX + 1];			/* プロンプト文字列     */
} terminal;

typedef struct userdat {			/* 128 bytes */
	char id[ID_MAX + 1];					/* ID                   */
	char password[PASSWORD_MAX + 1];		/* Password             */
	char handle[HANDLE_MAX + 1];			/* ハンドル             */
	unsigned pwerr;							/* パスワードエラー回数 */
	long usrflag;							/* ユーザーフラグ       */
	long last_login;						/* 最終ログイン日時     */
	long last_logout;						/* 最終ログアウト日時   */
	long last_msgread;						/* 最終メッセージ読み   */
	unsigned access;						/* アクセス数           */
	unsigned post;							/* メッセージポスト数   */
	unsigned upload;						/* アップロード回数     */
	unsigned download;						/* ダウンロード回数     */
	char expert;							/* エキスパートレベル   */
	terminal term;							/* 端末環境設定         */
	char shell[13];							/* シェル               */
	char autosign;							/* 自動シグネチャ付加   */
	char reserve[15];
} userdat;

typedef struct dhist {
	unsigned post;							/* メッセージポスト数  */
	unsigned read;							/* メッセージリード数  */
	unsigned mail;							/* メール送信数        */
	unsigned upload;						/* アップロード回数    */
	unsigned download;						/* ダウンロード回数    */
} dhist;

typedef struct uid_struct {
	int chstat;

	union {
		struct {
			unsigned maxusers;				/* 監視ポート数       /pwatch   */
			unsigned maxcmd;				/* コマンドテーブル要素数       */
			unsigned maxopt;				/* オプションテーブル要素数     */
			unsigned aprio;					/* １秒待ち最適優先度 /aprio    */
			long start_time;				/* システム起動日時   /cstart   */
			long now_time;					/* 現在時刻           /aprio    */
			config_t   far *config;			/* 設定内容                     */
			flagdat_t  far *flagdat;		/* フラグ                       */
			command_t  far *command;		/* コマンドテーブル             */
			option_t   far *option;			/* 実行許可フラグテーブル       */
			char       far *alias;			/* エイリアステーブル           */
			chatroom_t far *chatroom;		/* チャットルームテーブル       */
			char monitor_off;				/* チャンネルモニタ停止フラグ   */
			char monitor_redraw;			/* チャンネルモニタ再描画要求   */
			char status;					/* システムステータス           */
			char bbslock;					/* BBSファイルロックフラグ      */
			char init_done;					/* 起動時初期化完了フラグ       */
		} p;
		struct {
			char device_name[9];			/* デバイス名             */
			char initstr[INITSTR_MAX + 1];	/* 初期化文字列           */
			char ucmd[9];					/* 現在使用中のコマンド   */
			char logoff;					/* logoff フラグ          */
			userdat udat;					/* ユーザーデータ         */
			dhist dh;						/* 今回の行動             */
			unsigned total_access;			/* totalアクセスカウント  */
			unsigned tlimit;				/* 制限時間(分)           */
			long connect_time;				/* 接続日時               */
			long login_time;				/* ログイン日時           */
			long sequencer;					/* 実効シーケンサ         */
			char open_flag;					/* message read flag      */
			char telegram_received;			/* 電報受信フラグ         */
			notes_t notes;					/* open用                 */
			boardlist blist;				/* open用                 */
			unsigned chat;					/* chat用                 */
		} u;
	} us;
} uid_struct;

#endif	/* USRDAT_H */

