
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
	char esc;								/* �G�X�P�[�v�g�p����   */
	char term_width;						/* �[����ʉ���         */
	char term_height;						/* �[����ʏc��         */
	char up_protocol;						/* upload   �v���g�R��  */
	char down_protocol;						/* download �v���g�R��  */
	char more;								/* more                 */
	char editor_width;						/* �G�f�B�^����         */
	char editor_lm;							/* �G�f�B�^���}�[�W��   */
	char prompt[PROMPT_MAX + 1];			/* �v�����v�g������     */
} terminal;

typedef struct userdat {			/* 128 bytes */
	char id[ID_MAX + 1];					/* ID                   */
	char password[PASSWORD_MAX + 1];		/* Password             */
	char handle[HANDLE_MAX + 1];			/* �n���h��             */
	unsigned pwerr;							/* �p�X���[�h�G���[�� */
	long usrflag;							/* ���[�U�[�t���O       */
	long last_login;						/* �ŏI���O�C������     */
	long last_logout;						/* �ŏI���O�A�E�g����   */
	long last_msgread;						/* �ŏI���b�Z�[�W�ǂ�   */
	unsigned access;						/* �A�N�Z�X��           */
	unsigned post;							/* ���b�Z�[�W�|�X�g��   */
	unsigned upload;						/* �A�b�v���[�h��     */
	unsigned download;						/* �_�E�����[�h��     */
	char expert;							/* �G�L�X�p�[�g���x��   */
	terminal term;							/* �[�����ݒ�         */
	char shell[13];							/* �V�F��               */
	char autosign;							/* �����V�O�l�`���t��   */
	char reserve[15];
} userdat;

typedef struct dhist {
	unsigned post;							/* ���b�Z�[�W�|�X�g��  */
	unsigned read;							/* ���b�Z�[�W���[�h��  */
	unsigned mail;							/* ���[�����M��        */
	unsigned upload;						/* �A�b�v���[�h��    */
	unsigned download;						/* �_�E�����[�h��    */
} dhist;

typedef struct uid_struct {
	int chstat;

	union {
		struct {
			unsigned maxusers;				/* �Ď��|�[�g��       /pwatch   */
			unsigned maxcmd;				/* �R�}���h�e�[�u���v�f��       */
			unsigned maxopt;				/* �I�v�V�����e�[�u���v�f��     */
			unsigned aprio;					/* �P�b�҂��œK�D��x /aprio    */
			long start_time;				/* �V�X�e���N������   /cstart   */
			long now_time;					/* ���ݎ���           /aprio    */
			config_t   far *config;			/* �ݒ���e                     */
			flagdat_t  far *flagdat;		/* �t���O                       */
			command_t  far *command;		/* �R�}���h�e�[�u��             */
			option_t   far *option;			/* ���s���t���O�e�[�u��       */
			char       far *alias;			/* �G�C���A�X�e�[�u��           */
			chatroom_t far *chatroom;		/* �`���b�g���[���e�[�u��       */
			char monitor_off;				/* �`�����l�����j�^��~�t���O   */
			char monitor_redraw;			/* �`�����l�����j�^�ĕ`��v��   */
			char status;					/* �V�X�e���X�e�[�^�X           */
			char bbslock;					/* BBS�t�@�C�����b�N�t���O      */
			char init_done;					/* �N���������������t���O       */
		} p;
		struct {
			char device_name[9];			/* �f�o�C�X��             */
			char initstr[INITSTR_MAX + 1];	/* ������������           */
			char ucmd[9];					/* ���ݎg�p���̃R�}���h   */
			char logoff;					/* logoff �t���O          */
			userdat udat;					/* ���[�U�[�f�[�^         */
			dhist dh;						/* ����̍s��             */
			unsigned total_access;			/* total�A�N�Z�X�J�E���g  */
			unsigned tlimit;				/* ��������(��)           */
			long connect_time;				/* �ڑ�����               */
			long login_time;				/* ���O�C������           */
			long sequencer;					/* �����V�[�P���T         */
			char open_flag;					/* message read flag      */
			char telegram_received;			/* �d���M�t���O         */
			notes_t notes;					/* open�p                 */
			boardlist blist;				/* open�p                 */
			unsigned chat;					/* chat�p                 */
		} u;
	} us;
} uid_struct;

#endif	/* USRDAT_H */
