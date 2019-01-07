
#if !defined(CFGDAT_H)
#define CFGDAT_H

typedef struct dirconf {
	char bbsdir[80];				/* bbs       �f�B���N�g�� */
	char etcdir[80];				/* etc       �f�B���N�g�� */
	char logdir[80];				/* log       �f�B���N�g�� */
	char msgdir[80];				/* message   �f�B���N�g�� */
	char textdir[80];				/* text      �f�B���N�g�� */
	char helpdir[80];				/* help      �f�B���N�g�� */
	char homedir[80];				/* home      �f�B���N�g�� */
	char menudir[80];				/* menu      �f�B���N�g�� */
	char tmpdir[80];				/* temporary �f�B���N�g�� */
} dirconf;

typedef struct config {
	dirconf dir;					/* �f�B���N�g���ݒ�             */
	char def_shell[13];				/* �V�F���v���O����(�f�t�H���g) */
	char def_shprompt[32];			/* �V�F���v�����v�g(�f�t�H���g) */
	int prio_pwatch;				/* pwatch   �̃v���Z�X�D��x    */
	int prio_tlimit;				/* tlimit   �̃v���Z�X�D��x    */
	int chatrooms;					/* chatroom ��                  */
} config_t;

typedef struct flagdat {
	char flag_name[FLAGNAME_MAX + 1];	/* �t���O�� */
	char ext[4];						/* �g���q   */
	unsigned tlimit;					/* �������� */
} flagdat_t;

typedef struct command {
	char name[9];						/* �R�}���h�� */
	char far *alias;					/* alias ��? */
	unsigned long flag;					/* ���s��������t���O */
} command_t;

typedef struct option {
	char name[9];
	unsigned long flag;					/* ���s��������t���O */
} option_t;

#endif	/* CFGDAT_H */
