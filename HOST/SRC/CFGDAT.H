
#if !defined(CFGDAT_H)
#define CFGDAT_H

typedef struct dirconf {
	char bbsdir[80];				/* bbs       ディレクトリ */
	char etcdir[80];				/* etc       ディレクトリ */
	char logdir[80];				/* log       ディレクトリ */
	char msgdir[80];				/* message   ディレクトリ */
	char textdir[80];				/* text      ディレクトリ */
	char helpdir[80];				/* help      ディレクトリ */
	char homedir[80];				/* home      ディレクトリ */
	char menudir[80];				/* menu      ディレクトリ */
	char tmpdir[80];				/* temporary ディレクトリ */
} dirconf;

typedef struct config {
	dirconf dir;					/* ディレクトリ設定             */
	char def_shell[13];				/* シェルプログラム(デフォルト) */
	char def_shprompt[32];			/* シェルプロンプト(デフォルト) */
	int prio_pwatch;				/* pwatch   のプロセス優先度    */
	int prio_tlimit;				/* tlimit   のプロセス優先度    */
	int chatrooms;					/* chatroom 数                  */
} config_t;

typedef struct flagdat {
	char flag_name[FLAGNAME_MAX + 1];	/* フラグ名 */
	char ext[4];						/* 拡張子   */
	unsigned tlimit;					/* 制限時間 */
} flagdat_t;

typedef struct command {
	char name[9];						/* コマンド名 */
	char far *alias;					/* alias か? */
	unsigned long flag;					/* 実行を許可するフラグ */
} command_t;

typedef struct option {
	char name[9];
	unsigned long flag;					/* 実行を許可するフラグ */
} option_t;

#endif	/* CFGDAT_H */

