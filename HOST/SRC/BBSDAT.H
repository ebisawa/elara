
#if !defined(BBSDAT_H)
#define BBSDAT_H

#define MSG_DELETED		0x0001
#define MSG_AWO			0x0002
#define MSG_IMPORTANT	0x0004
#define MSG_CLOSED		0x0008
#define MSG_SUBJECT		0x0010

typedef struct boardlist {			/* 128 bytes */
	char realname[BOARDNAME_MAX + 1];		/* ノートファイル名           */
	char desc[BOARDTITLE_MAX+1];			/* description                */
	char dosname[9];						/* DOS のディレクトリ名       */
	long last_update;						/* 最終メッセージのポスト日時 */
	unsigned long msgcount;					/* メッセージ数               */
	long read;								/* 進入許可フラグ             */
	long write;								/* 書き込み許可フラグ         */
	long basenote;							/* ノート作成許可フラグ       */
	char reserved[1];
} boardlist;

typedef struct msgidx {				/* 128 bytes */
	char title[NOTETITLE_MAX + 1];			/* Bas:NoteTitle, Res:Subject */
	long post_time;							/* ポスト日時                 */
	long last_update;						/* Bas: 最終resポスト日時     */
	char author[ID_MAX + 1];				/* ID                         */
	char handle[HANDLE_MAX + 1];			/* handle                     */
	long msgpos;
	long msglen;
	unsigned flags;							/* フラグ                     */
	unsigned response;						/* Bas: レスポンス数          */
	char reserved[41];
} msgidx;

typedef struct {
	int status;
	int note;
	int response;
	unsigned board_number;
	unsigned data_note_number;
	unsigned data_res_number;
	msgidx basidx;
	msgidx residx;
	unsigned open_flags;
	unsigned note_flags;
	long board_sequencer;
	long note_sequencer;
} notes_t;

#endif	/* BBSDAT_H */

