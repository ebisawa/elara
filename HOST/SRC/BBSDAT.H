
#if !defined(BBSDAT_H)
#define BBSDAT_H

#define MSG_DELETED		0x0001
#define MSG_AWO			0x0002
#define MSG_IMPORTANT	0x0004
#define MSG_CLOSED		0x0008
#define MSG_SUBJECT		0x0010

typedef struct boardlist {			/* 128 bytes */
	char realname[BOARDNAME_MAX + 1];		/* �m�[�g�t�@�C����           */
	char desc[BOARDTITLE_MAX+1];			/* description                */
	char dosname[9];						/* DOS �̃f�B���N�g����       */
	long last_update;						/* �ŏI���b�Z�[�W�̃|�X�g���� */
	unsigned long msgcount;					/* ���b�Z�[�W��               */
	long read;								/* �i�����t���O             */
	long write;								/* �������݋��t���O         */
	long basenote;							/* �m�[�g�쐬���t���O       */
	char reserved[1];
} boardlist;

typedef struct msgidx {				/* 128 bytes */
	char title[NOTETITLE_MAX + 1];			/* Bas:NoteTitle, Res:Subject */
	long post_time;							/* �|�X�g����                 */
	long last_update;						/* Bas: �ŏIres�|�X�g����     */
	char author[ID_MAX + 1];				/* ID                         */
	char handle[HANDLE_MAX + 1];			/* handle                     */
	long msgpos;
	long msglen;
	unsigned flags;							/* �t���O                     */
	unsigned response;						/* Bas: ���X�|���X��          */
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
