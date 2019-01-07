
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

typedef struct {
	char *str;
	void (*func)(char far *);
} cnftbl;

int Running;
uid_struct far *Head;

void already_running(void);
void toolong(char far *s);
void adjustpath(char far *s);
void set_bbsdir(char far *s);
void set_etcdir(char far *s);
void set_logdir(char far *s);
void set_msgdir(char far *s);
void set_textdir(char far *s);
void set_helpdir(char far *s);
void set_homedir(char far *s);
void set_menudir(char far *s);
void set_tmpdir(char far *s);
void set_def_shell(char far *s);
void set_def_shprompt(char far *s);
void set_prio_pwatch(char far *s);
void set_prio_tlimit(char far *s);
void set_chatrooms(char far *s);

cnftbl config_table[] = {
	{ "bbsdir",            set_bbsdir        },
	{ "etcdir",            set_etcdir        },
	{ "logdir",            set_logdir        },
	{ "msgdir",            set_msgdir        },
	{ "textdir",           set_textdir       },
	{ "helpdir",           set_helpdir       },
	{ "homedir",           set_homedir       },
	{ "menudir",           set_menudir       },
	{ "tmpdir",            set_tmpdir        },
	{ "def_shell",         set_def_shell     },
	{ "def_shprompt",      set_def_shprompt  },
	{ "prio_pwatch",       set_prio_pwatch   },
	{ "prio_tlimit",       set_prio_tlimit   },
	{ "chatrooms",         set_chatrooms     },
};

void main(void)
{
	int i, handle;
	char far *p, buf[128];
	config_t far *cmem;

	if (Running > 0)
		already_running();

	Running++;
	if ((Head = getuid_n("pwatch")) == 0)
		return;
	if ((cmem = xmalloc(sizeof(config_t))) == 0)
		return;
	memset(cmem, 0, sizeof(config_t));
	if (Head->us.p.config != 0)
		xfree(Head->us.p.config);

	Head->us.p.config = cmem;

	if ((handle = open("config", FA_READ)) == 0)
		return;
	while (fgets(buf, sizeof(buf), handle) != 0) {
		chop(buf);
		if (buf[0] == '#' || buf[0] == 0 || (p = strspl(buf, "\t ")) == 0)
			continue;
		for (i = 0; i < ELEMS(config_table); i++) {
			if (strcmp(config_table[i].str, buf) == 0)
				config_table[i].func(p);
		}
	}
	close(handle);
}

void already_running(void)
{
	exit();
}

void toolong(char far *s)
{
	printf("** %s ‚ª’·‚·‚¬‚Ü‚· **\n", s);
	exit();
}

void adjustpath(char far *s)
{
	int len = strlen(s);

	if (s[len - 1] == '\\')
		s[len - 1] = 0;
}

void set_bbsdir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.bbsdir))
		toolong("bbsdir");
	strcpy(Head->us.p.config->dir.bbsdir, s);
}

void set_etcdir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.etcdir))
		toolong("etcdir");
	strcpy(Head->us.p.config->dir.etcdir, s);
}

void set_helpdir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.helpdir))
		toolong("helpdir");
	strcpy(Head->us.p.config->dir.helpdir, s);
}

void set_logdir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.logdir))
		toolong("logdir");
	strcpy(Head->us.p.config->dir.logdir, s);
}

void set_msgdir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.msgdir))
		toolong("msgdir");
	strcpy(Head->us.p.config->dir.msgdir, s);
}

void set_textdir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.textdir))
		toolong("textdir");
	strcpy(Head->us.p.config->dir.textdir, s);
}

void set_homedir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.homedir))
		toolong("homedir");
	strcpy(Head->us.p.config->dir.homedir, s);
}

void set_menudir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.menudir))
		toolong("menudir");
	strcpy(Head->us.p.config->dir.menudir, s);
}

void set_tmpdir(char far *s)
{
	adjustpath(s);
	if (strlen(s) >= sizeof(Head->us.p.config->dir.tmpdir))
		toolong("tmpdir");
	strcpy(Head->us.p.config->dir.tmpdir, s);
}

void set_def_shell(char far *s)
{
	if (strlen(s) >= sizeof(Head->us.p.config->def_shell))
		toolong("def_shell");

	strcpy(Head->us.p.config->def_shell, s);
}

void set_def_shprompt(char far *s)
{
	if (strlen(s) >= sizeof(Head->us.p.config->def_shprompt))
		toolong("def_shprompt");

	strcpy(Head->us.p.config->def_shprompt, s);
}

void set_prio_pwatch(char far *s)
{
	Head->us.p.config->prio_pwatch = atou(s);
}

void set_prio_tlimit(char far *s)
{
	Head->us.p.config->prio_tlimit = atou(s);
}

void set_chatrooms(char far *s)
{
	Head->us.p.config->chatrooms = atou(s);
}

