
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

typedef struct {
	char *command;
	void (*func)(char far *);
} etable_struct;

void x_device(char far *s);
void x_exec(char far *s);
void x_nowait(char far *s);
void x_stdio(char far *s);

etable_struct etable[] = {
	{ "device", x_device },		/* �f�o�h���g�ݍ��� */
	{ "exec",   x_exec   },		/* ���W���[�����s   */
	{ "nowait", x_nowait },		/* ���W���[�����s(�o�b�N�O���E���h) */
	{ "stdio",  x_stdio  },		/* �W�����o�͂̐ݒ� */
};

void main(void)
{
	int i, handle;
	char buf[1024], far *p;

	if ((handle = open("startup", FA_READ)) == 0) {
		disp("init: �t�@�C�� [startup] ���I�[�v���ł��܂���\r\n");
		exit();
	}
	disp("Starting Elara ");
	spawn(P_WAIT, "version.dlm", 0);
	disp(". please wait...\r\n\n");

	while (fgets(buf, sizeof(buf), handle) != 0) {
		chop(buf);
		if (buf[0] == '#' || (p = strspl(buf, "\t ")) == 0)
			continue;
		for (i = 0; i < ELEMS(etable); i++) {
			if (strcmp(etable[i].command, buf) == 0)
				etable[i].func(p);
		}
	}
	close(handle);
}

void x_device(char far *s)
{
	char far *p;

	if ((p = strchr(s, ' ')) != 0) {
		while (*p == ' ')
			*p++ = 0;
	}
	if (adddrv(s, p) != 0) {
		disp("init: ");
		disp(s);
		disp(" �̑g���݂Ɏ��s���܂���\r\n");
	}
}

void x_exec(char far *s)
{
	char far *p;

	if ((p = strchr(s, ' ')) != 0) {
		while (*p == ' ')
			*p++ = 0;
	}
	if (spawn(P_WAIT, s, p) != 0) {
		disp("init: ");
		disp(s);
		disp(" �̋N���Ɏ��s���܂���\r\n");
	}
}

void x_nowait(char far *s)
{
	char far *p;

	if ((p = strchr(s, ' ')) != 0) {
		while (*p == ' ')
			*p++ = 0;
	}
	if (spawn(P_NOWAIT, s, p) != 0) {
		disp("init: ");
		disp(s);
		disp(" �̋N���Ɏ��s���܂���\r\n");
	}
}

void x_stdio(char far *s)
{
	setstdio(getpid(), s, s);
}
