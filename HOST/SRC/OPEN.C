
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "event.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

#define DISP_MESSAGE	0
#define SILENT			1
#define NEXT_NOTEFILE	2
#define SCAN_ABORT		3

#define ESC				0x1b

#define InternalError()	internal_error( __FILE__, __LINE__ )

#define INDEX			0
#define OPEN			1

#define BASIDX			"basenote.idx"
#define MSGCMP			"message.cmp"

#define OFLAG_A			0x0001
#define OFLAG_S			0x0002
#define OFLAG_X			0x0004
#define OFLAG_N			0x0008
#define OFLAG_NEW		0x0010

#define NFLAG_A			0x0001
#define NFLAG_N			0x0002

typedef struct {
	int cmdcode;
	int (*func)(int, notes_t far *, boardlist far *, uid_struct far *, uid_struct far *);
} cmdtab_t;

int post_basenote(int key, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head);
int post_bn_awo(int key, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head);
int post_message(int key, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head);
int quit(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int basenote_num(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int indexmode(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int response_num(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int next_response(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int next_newres(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int prev_response(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int goto_basenote(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int next_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int prev_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int next_newnote(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int redraw_message(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int last_response(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int open_newnote(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int edit_title(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int scan_abort(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int nidxopen(int notenum, char far *bbsdir, char far *dosname, unsigned mode);
int allnew_board(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int current_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int disp_help0(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int disp_help1(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int disp_help2(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int disp_title(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int disp_alltitle(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int allnew_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int close_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int set_subject(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int nonstop_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int nonstop_all(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int delete(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int set_important(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int set_awo(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);
int disp_important(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head);

cmdtab_t cmdtable[2][50] = {
/* INDEX mode */ {
		{ '0',       basenote_num   },
		{ '1',       basenote_num   },
		{ '2',       basenote_num   },
		{ '3',       basenote_num   },
		{ '4',       basenote_num   },
		{ '5',       basenote_num   },
		{ '6',       basenote_num   },
		{ '7',       basenote_num   },
		{ '8',       basenote_num   },
		{ '9',       basenote_num   },
		{ ESC,       quit           },
		{ CTRL('D'), scan_abort     },
		{ CTRL('L'), redraw_message },
		{ CTRL('R'), set_subject    },
		{ '\n',      current_note   },
		{ '\t',      open_newnote   },
		{ '?',       disp_help0     },
		{ '!',       set_important  },
		{ '%',       close_note     },
		{ 'a',       allnew_board   },
		{ 'd',       delete         },
		{ 'g',       post_bn_awo    },
		{ 'l',       open_newnote   },
		{ 'q',       quit           },
		{ 'v',       redraw_message },
		{ 'w',       post_basenote  },
		{ 0, 0 }	/* end of table */
	},

/* OPEN mode */ {
		{ '0',       response_num   },
		{ '1',       response_num   },
		{ '2',       response_num   },
		{ '3',       response_num   },
		{ '4',       response_num   },
		{ '5',       response_num   },
		{ '6',       response_num   },
		{ '7',       response_num   },
		{ '8',       response_num   },
		{ '9',       response_num   },
		{ ESC,       quit           },
		{ CTRL('D'), scan_abort     },
		{ CTRL('E'), prev_response  },
		{ CTRL('G'), post_bn_awo    },
		{ CTRL('L'), redraw_message },
		{ CTRL('R'), set_subject    },
		{ CTRL('W'), post_basenote  },
		{ CTRL('X'), next_response  },
		{ '\b',      prev_response  },
		{ '\n',      next_response  },
		{ '\t',      next_newres    },
		{ '?',       disp_help1     },
		{ '&',       disp_help2     },
		{ '!',       set_important  },
		{ '%',       close_note     },
		{ '>',       next_response  },
		{ '<',       prev_response  },
		{ ' ',       next_response  },
		{ '=',       goto_basenote  },
		{ '*',       last_response  },
		{ '+',       next_note      },
		{ 'j',       next_note      },
		{ '-',       prev_note      },
		{ 'a',       allnew_note    },
		{ 'b',       basenote_num   },
		{ 'c',       nonstop_note   },
		{ 'C',       nonstop_all    },
		{ 'd',       delete         },
		{ 'e',       edit_title     },
		{ 'G',       set_awo        },
		{ 'i',       indexmode      },
		{ 'I',       disp_important },
		{ 'l',       next_newres    },
		{ 'L',       next_newnote   },
		{ 'q',       quit           },
		{ 't',       disp_title     },
		{ 'T',       disp_alltitle  },
		{ 'v',       redraw_message },
		{ 'w',       post_message   },
		{ 0, 0 }	/* end of table */
	}
};

char *prompt[2] = {
	"INDEX>", "OPEN>"
};

int noteopen(notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head);
char far *option(char far *argv, notes_t far *notes);
void internal_error(char far *file, unsigned lineno);
void disp_index(notes_t far *notes, boardlist far *blist, uid_struct far *uid, config_t far *conf);
void tmpfilename(char far *name, uid_struct far *uid, uid_struct far *head);
void disp_message(notes_t far *notes, boardlist far *blist, uid_struct far *uid,  uid_struct far *head);
void print_message(msgidx far *idx, boardlist far *blist,uid_struct far *head);
void print_text(long msgpos, long msglen, boardlist far *blist, uid_struct far *head);
void write_str(char far *buf, int len);
int write_textmsg(msgidx far *idx, boardlist far *blist, uid_struct far *uid, uid_struct far *head);
unsigned getmaxnotes(boardlist far *blist, uid_struct far *head);
unsigned getmaxres(unsigned note, boardlist far *blist, uid_struct far *head);
unsigned adjust_notenum(unsigned notenum, boardlist far *blist, uid_struct far *head);
unsigned adjust_resnum(unsigned resnum, notes_t far *notes);
void update_boardlist(boardlist far *blist, long newtime, config_t far *conf);
unsigned read_basenote(msgidx far *idx,  notes_t far *notes, boardlist far *blist, uid_struct far *head);
void write_basenote(msgidx far *idx, notes_t far *notes, boardlist far *blist, uid_struct far *head);
void append_basenote(msgidx far *idx, boardlist far *blist,config_t far *conf);
unsigned read_response(msgidx far *idx, notes_t far *notes, boardlist far *blist, uid_struct far *head);
void write_response(msgidx far *idx, notes_t far *notes, boardlist far *blist, uid_struct far *head);
void append_response(msgidx far *idx, unsigned notenum, boardlist far *blist, config_t far *conf);
void update_basenote(long newtime, notes_t far *notes, boardlist far *blist, uid_struct far *head);
int query_grq(void);
int search_newnote(unsigned note, notes_t far *notes, boardlist far *blist, uid_struct far *head);
void print_title(unsigned fmask, unsigned resnum, notes_t far *notes, boardlist far *blist, config_t far *conf);
void print_title2(msgidx far *idx);
int doyouset(char far *msg, int set, uid_struct far *uid);
void set_noteflag(char far *str, unsigned flag, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head);
void set_resflag(char far *str, unsigned flag, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head);
int make_newnote(unsigned flags, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head);
void update_basenote_mem(msgidx far *idx, notes_t far *notes, uid_struct far *head);
void update_response_mem(msgidx far *idx, notes_t far *notes, uid_struct far *head);


void main(void)
{
	int handle;
	unsigned board_number;
	char far *argv, far *target;
	notes_t far *notes;
	boardlist far *blist;
	uid_struct far *head, far *uid;

	if ((head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;
	if ((argv = getargv()) == 0)
		return;

	notes = &uid->us.u.notes;
	blist = &uid->us.u.blist;
	uid->us.u.open_flag = 1;
	memset(notes, 0, sizeof(notes_t));
	target = option(argv, notes);

	handle = dopen(head->us.p.config->dir.bbsdir, "blist.dat", FA_READ);
	if (handle == 0)
		return;

	board_number = 0;
	while (read(blist, sizeof(boardlist), handle)) {
		board_number++;
		if (wildcmp(blist->realname, target) == 0) {
			if ((blist->read & uid->us.u.udat.usrflag) == 0)
				continue;

			notes->status = INDEX;
			notes->note = 1;
			notes->response = 0;
			notes->board_number = board_number;

			printf("<%s>\n", FAR(blist->realname));
			if ((notes->open_flags & OFLAG_S)
			 					&& !(notes->open_flags & OFLAG_A)) {
				if (blist->last_update <= uid->us.u.sequencer)
					continue;
			}
			if (noteopen(notes, blist, uid, head) != 0) {
				if (notes->open_flags & OFLAG_NEW)
					puts("-- SCAN ABORTED --");

				close(handle);
				exit();
			}
		}
	}
	close(handle);
	if (notes->open_flags & OFLAG_NEW)
		puts("-- SCAN COMPLETED --");
}

int noteopen(notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head)
{
	int i, c, newnote;

	notes->board_sequencer = uid->us.u.sequencer;
	notes->note_sequencer = uid->us.u.sequencer;

	if (notes->open_flags & OFLAG_S || notes->open_flags & OFLAG_X) {
		newnote = search_newnote(0, notes, blist, head);
		if (newnote == 0) {
			if (notes->open_flags & OFLAG_S)
				return 0;
		} else {
			notes->status = OPEN;
			notes->note = newnote;
		}
	}
	for (;;) {
		switch (notes->status) {
		case INDEX:
			disp_index(notes, blist, uid, head->us.p.config);
			break;
		case OPEN:
			disp_message(notes, blist, uid, head);
			break;
		}

	label1:
		if ((notes->open_flags & OFLAG_N) && notes->status == OPEN) {
			if (getchar() != -1)
				notes->open_flags &= ~(OFLAG_N | OFLAG_S);
			else
				c = 'l';
		} else if ((notes->note_flags & NFLAG_N) && notes->status == OPEN)
			if (getchar() != -1)
				notes->note_flags &= ~NFLAG_N;
			else
				c = 'l';
		else {
			print(prompt[notes->status]);
			while ((c = getchar()) == -1)
				;
			/* if (notes->status == OPEN)
				putchar('\n'); */
		}
	label2:
		for (i = 0; cmdtable[notes->status][i].cmdcode != 0; i++) {
			if (cmdtable[notes->status][i].cmdcode == c) {
				/* if (notes->status == INDEX) */
					putchar('\n');
				switch (cmdtable[notes->status][i].func(c, notes, blist, uid, head)) {
				case SILENT:
					goto label1;
				case NEXT_NOTEFILE:
					return 0;	/* 次のボードへ */
				case SCAN_ABORT:
					return 1;	/* scan abort */
				}
				goto noerror;
			}
		}
		cputs("Type [?] for help.", CYAN, uid);
		goto label1;
	noerror:
		;
	}
}

char far *option(char far *argv, notes_t far *notes)
{
	if (argv == 0)
		return 0;
	while (*argv != 0) {
		if (*argv == '-') {
			if (*++argv == 0)
				break;
			*argv = tolower(*argv);
			while (*argv != ' ' && *argv != 0) {
				switch(*argv) {
				case 's':
					notes->open_flags |= OFLAG_S;
					break;
				case 'x':
					notes->open_flags |= OFLAG_X;
					break;
				case 'n':
					notes->open_flags |= OFLAG_N;
					break;
				case 'a':
					notes->open_flags |= OFLAG_A;
					break;
				case '@':
					notes->open_flags |= OFLAG_NEW;
					break;
				}
				argv++;
			}
		} else if (!isspace(*argv)) {
			return argv;
		} else
			argv++;
	}
	return 0;
}


void internal_error(char far *file, unsigned lineno)
{
	printf("\nopen: internal error at file %s, line %u\n\n", file, lineno);
	exit();
}

void disp_index(notes_t far *notes, boardlist far *blist, uid_struct far *uid, config_t far *conf)
{
	int c, handle;
	unsigned note;
	msgidx idx;

	putchar('\n');
	if (uid->us.u.udat.term.esc & ESC_CURSOR)
		print(CLS);
	printf("[ INDEX ]  %s  (%s)\n", FAR(blist->desc), FAR(blist->realname));
	puts(" Num__ Date____ Last____  Res Author__ Title___________________________________");

	handle = dopen2(conf->dir.bbsdir, blist->dosname, BASIDX, FA_READ);
	if (handle == 0) {
		puts("-- END --");
		return;
	}
	note = 1;
	for (;;) {
		c = ' ';
		if (read(&idx, sizeof(msgidx), handle) == 0) {
			puts("-- END --");
			break;
		}
		if (idx.flags & MSG_DELETED)
			goto contin;
		if (note == notes->note)
			c = '>';
		else if (idx.flags & MSG_IMPORTANT)
			c = '!';
		else if (idx.flags & MSG_CLOSED)
			c = '%';
		else if (idx.flags & MSG_AWO)
			c = '*';

		printf("%c%5u ", c, note);
		dispdate(idx.post_time);
		putchar(' ');
		dispdate(idx.last_update);
		printf("%5u %-8s %-s\n",
			idx.response, FAR(idx.author), FAR(idx.title));
	contin:
		note++;
	}
	close(handle);
}

void tmpfilename(char far *name, uid_struct far *uid, uid_struct far *head)
{
	int ch;

	ch = getchnum(uid->us.u.device_name, head);
	sprintf(name, "%s\\opentmp0.%03d", FAR(head->us.p.config->dir.tmpdir), ch);
}

void disp_message(notes_t far *notes, boardlist far *blist, uid_struct far *uid,  uid_struct far *head)
{
	int maxres;
	msgidx idx;

	if (notes->response == 0) {
		if (notes->note != notes->data_note_number) {
			notes->note_flags = 0;
			notes->note_sequencer = notes->board_sequencer;
		}
		if (read_basenote(&idx, notes, blist, head) == 0)
			InternalError();
	} else {
		maxres = notes->basidx.response;
		if (read_response(&idx, notes, blist, head) == 0)
			InternalError();
	}

	if (!(notes->open_flags & OFLAG_N || notes->note_flags & NFLAG_N)) {
		if (uid->us.u.udat.term.esc & ESC_CURSOR)
			print(CLS);
	}
	printf("Note %-5d   %s  (%s)\n", notes->note,
								FAR(blist->desc), FAR(blist->realname));
	if (notes->response != 0)
		printf("[ RESPONSE:%4u of%4u ]", notes->response, maxres);
	else {
		printf("[ BASENOTE with%4uRes ]", idx.response);
	}
	if (idx.flags & MSG_IMPORTANT)
		print("  ** Important **");
	else if (notes->basidx.flags & MSG_CLOSED)
		print("  ** Closed **");
	else if (notes->basidx.flags & MSG_AWO)
		print("  ** Author Write Only **");

	putchar('\n');
	if (idx.flags & MSG_DELETED)
		puts("** 削除されています **\n");
	else {
		printf("Title: %s\n", FAR(notes->basidx.title));
		if (notes->response != 0 && idx.flags & MSG_SUBJECT)
			printf("Subject: %s\n", FAR(idx.title));

		print_message(&idx, blist, head);
		uid->us.u.dh.read++;
	}
}

void print_message(msgidx far *idx, boardlist far *blist, uid_struct far *head)
{
	printf("Bytes: %-6ld Date : ", idx->msglen);
	dispdate(idx->post_time);
	putchar(' ');
	disptime(idx->post_time);
	printf("  Author: %s (%s)\n\n", FAR(idx->author), FAR(idx->handle));

	print_text(idx->msgpos, idx->msglen, blist, head);
	putchar('\n');
}

void print_text(long msgpos, long msglen, boardlist far *blist, uid_struct far *head)
{
	int i, handle, rlen;
	char buf[256];
	config_t far *conf = head->us.p.config;

	handle = dopen2(conf->dir.bbsdir, blist->dosname, MSGCMP, FA_READ);
	if (handle == 0)
		return;

	seek(handle, msgpos, FS_SET);
	while (msglen > 0) {
		if ((rlen = read(buf, sizeof(buf), handle)) == 0)
			InternalError();
		if (msglen < sizeof(buf))
			rlen = (unsigned)msglen;

		write_str(buf, rlen);
		msglen -= rlen;
	}
	close(handle);
}

void write_str(char far *buf, int len)
{
	unsigned wlen;

	while ((wlen = write(buf, len, 0)) < len) {
		buf += wlen;
		len -= wlen;
	}
}

int write_textmsg(msgidx far *idx, boardlist far *blist, uid_struct far *uid, uid_struct far *head)
{
	char tmpfile[128];
	int handle, tmphandle;
	config_t far *conf = head->us.p.config;

	tmpfilename(tmpfile, uid, head);
	remove(tmpfile);			/* 万が一テンポラリが残ってしまった時のため */
	spawn(P_WAIT, "textedit.dlm", tmpfile);
	idx->post_time = time();
	idx->last_update = time();
	strcpy(idx->author, uid->us.u.udat.id);
	strcpy(idx->handle, uid->us.u.udat.handle);

	handle = dopen2(conf->dir.bbsdir, blist->dosname, MSGCMP, FA_APPEND);
	if (handle == 0)
		InternalError();
	if ((tmphandle = open(tmpfile, FA_READ | FA_AUTODEL)) == 0) {
		close(handle);
		return 1;
	}
	idx->msglen = filesize(tmpfile);
	idx->msgpos = seek(handle, 0L, FS_CUR);

	filecopy_h(tmphandle, handle);
	close(handle);
	close(tmphandle);

	return 0;
}

unsigned getmaxnotes(boardlist far *blist, uid_struct far *head)
{
	char filename[128];
	config_t far *conf = head->us.p.config;
	ldiv_t d;

	sprintf(filename, "%s\\%s\\%s",
		FAR(conf->dir.bbsdir), FAR(blist->dosname), FAR(BASIDX));

	d = ldiv(filesize(filename), sizeof(msgidx));

	return (unsigned)d.quot;
}

#if 0
/* 念のためとっておく */
unsigned getmaxres(unsigned note, boardlist far *blist, uid_struct far *head)
{
	char filename[128];
	config_t far *conf = head->us.p.config;
	ldiv_t d;

	sprintf(filename, "%s\\%s\\note%04x.idx",
			conf->dir.bbsdir, blist->dosname, note);
	d = ldiv(filesize(filename), sizeof(msgidx));

	return (unsigned)d.quot;
}
#endif

unsigned adjust_notenum(unsigned notenum, boardlist far *blist, uid_struct far *head)
{
	unsigned maxnotes;

	maxnotes = getmaxnotes(blist, head);
	if (notenum < 1)
		return 1;
	if (notenum > maxnotes)
		return maxnotes;

	return notenum;
}

unsigned adjust_resnum(unsigned resnum, notes_t far *notes)
{
	unsigned maxres;

	maxres = notes->basidx.response;
	if (resnum > maxres)
		return maxres;

	return resnum;
}

void update_boardlist(boardlist far *blist, long newtime, config_t far *conf)
{
	blist->last_update = newtime;
	blist->msgcount++;
	save_boardlist(blist, blist->realname, conf);
}

unsigned read_basenote(msgidx far *idx,  notes_t far *notes, boardlist far *blist, uid_struct far *head)
{
	int handle;
	unsigned rcount;
	config_t far *conf = head->us.p.config;

	memset(idx, 0, sizeof(msgidx));
	handle = dopen2(conf->dir.bbsdir, blist->dosname, BASIDX, FA_READ);
	if (handle == 0)
		return 0;

	seek(handle, sizeof(msgidx) * (notes->note - 1), FS_SET);
	rcount = read(idx, sizeof(msgidx), handle);
	close(handle);
	notes->data_note_number = notes->note;
	if (notes->response == 0)
		notes->data_res_number = 0;
	update_basenote_mem(idx, notes, head);

	return rcount;
}

void write_basenote(msgidx far *idx, notes_t far *notes, boardlist far *blist, uid_struct far *head)
{
	int handle;
	config_t far *conf = head->us.p.config;

	handle = dopen2(conf->dir.bbsdir, blist->dosname, BASIDX, FA_RW);
	if (handle == 0)
		InternalError();

	seek(handle, sizeof(msgidx) * (notes->note - 1), FS_SET);
	write(idx, sizeof(msgidx), handle);
	close(handle);
	update_basenote_mem(idx, notes, head);
}

void append_basenote(msgidx far *idx, boardlist far *blist,config_t far *conf)
{
	int handle;

	handle = dopen2(conf->dir.bbsdir, blist->dosname, BASIDX, FA_APPEND);
	if (handle == 0)
		InternalError();

	write(idx, sizeof(msgidx), handle);
	close(handle);
}

unsigned read_response(msgidx far *idx, notes_t far *notes, boardlist far *blist, uid_struct far *head)
{
	int handle;
	unsigned rcount;
	config_t far *conf = head->us.p.config;

	memset(idx, 0, sizeof(msgidx));
	handle = nidxopen(notes->note, conf->dir.bbsdir, blist->dosname, FA_READ);
	if (handle == 0)
		return 0;

	seek(handle, sizeof(msgidx) * (notes->response - 1), FS_SET);
	rcount = read(idx, sizeof(msgidx), handle);
	close(handle);
	notes->data_res_number = notes->response;
	update_response_mem(idx, notes, head);

	return rcount;
}

void write_response(msgidx far *idx, notes_t far *notes, boardlist far *blist, uid_struct far *head)
{
	int handle;
	config_t far *conf = head->us.p.config;

	handle = nidxopen(notes->note, conf->dir.bbsdir, blist->dosname, FA_RW);
	if (handle == 0)
		InternalError();

	seek(handle, sizeof(msgidx) * (notes->response - 1), FS_SET);
	write(idx, sizeof(msgidx), handle);
	close(handle);
	update_response_mem(idx, notes, head);
}

void append_response(msgidx far *idx, unsigned notenum, boardlist far *blist, config_t far *conf)
{
	int handle;

	handle = nidxopen(notenum, conf->dir.bbsdir, blist->dosname, FA_APPEND);
	if (handle == 0)
		InternalError();

	write(idx, sizeof(msgidx), handle);
	close(handle);
}

void update_basenote(long newtime, notes_t far *notes, boardlist far *blist, uid_struct far *head)
{
	notes->basidx.last_update = newtime;
	notes->basidx.response++;
	write_basenote(&notes->basidx, notes, blist, head);
}

int query_grq(void)
{
	int c;

	print("(l/TAB) go next  (BS) return  (^D) quit : ");
	for (;;) {
		while ((c = getchar()) == -1)
			;
		switch (c) {
		case 'l': case 'L': case '\t':
			putchar('\n');
			return NEXT_NOTEFILE;
		case '\b':
			putchar('\n');
			return SILENT;
		case CTRL('D'):
			putchar('\n');
			return SCAN_ABORT;
		}
	}
}

int nidxopen(int notenum, char far *bbsdir, char far *dosname, unsigned mode)
{
	char filename[13];

	sprintf(filename, "note%04x.idx", notenum);
	return dopen2(bbsdir, dosname, filename, mode);
}

int search_newnote(unsigned note, notes_t far *notes, boardlist far *blist, uid_struct far *head)
{
	int handle;
	config_t far *conf = head->us.p.config;
	msgidx idx;

	handle = dopen2(conf->dir.bbsdir, blist->dosname, BASIDX, FA_READ);
	if (handle == 0)
		return 0;

	seek(handle, sizeof(msgidx) * note, FS_SET);
	note++;

	for (;;) {
		if (read(&idx, sizeof(msgidx), handle) == 0) {
			close(handle);
			return 0;
		}
		if (idx.last_update > notes->note_sequencer)
			break;
		if (notes->open_flags & OFLAG_A)
			break;
		if (notes->note_flags & NFLAG_A)
			break;

		note++;
	}
	close(handle);

	return note;
}

void print_title(unsigned fmask, unsigned resnum, notes_t far *notes, boardlist far *blist, config_t far *conf)
{
	int handle;
	msgidx idx;

	if (resnum == 0) {
		handle = dopen2(conf->dir.bbsdir, blist->dosname, BASIDX, FA_READ);
		if (handle == 0)
			return;
		seek(handle, sizeof(msgidx) * (notes->note - 1), FS_SET);
		read(&idx, sizeof(msgidx), handle);
		if (fmask == 0 || (idx.flags & fmask)) {
			printf(">>");
			print_title2(&idx);
		}
		close(handle);
		resnum++;
	}
	handle = nidxopen(notes->note, conf->dir.bbsdir, blist->dosname, FA_READ);
	if (handle == 0)
		return;
	seek(handle, sizeof(msgidx) * (resnum - 1), FS_SET);
	while (read(&idx, sizeof(msgidx), handle)) {
		if (getchar() != -1)
			break;
		if (fmask == 0 || (idx.flags & fmask)) {
			printf("%u)", resnum);
			print_title2(&idx);
		}
		resnum++;
	}
	close(handle);
}

void print_title2(msgidx far *idx)
{
	printf(" %-40s ", FAR(idx->title));
	dispdate(idx->post_time);
	printf(" %-8s (%-.12s)\n", FAR(idx->author), FAR(idx->handle));
}

int doyouset(char far *msg, int set, uid_struct far *uid)
{
	char buf[128];

	if (set)
		sprintf(buf, "%s 設定しますか", msg);
	else
		sprintf(buf, "%s 解除しますか", msg);

	return cquery(buf, Q_YES, uid);
}

void set_noteflag(char far *str, unsigned flag, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head)
{
	int set;

	set = notes->basidx.flags & flag;
	if (doyouset(str, !set, uid) == Q_YES) {
		if (set)
			notes->basidx.flags &= ~flag;
		else
			notes->basidx.flags |= flag;
		write_basenote(&notes->basidx, notes, blist, head);
		puts("-- 変更しました --");
	} else
		puts("-- 中止しました --");
}

void set_resflag(char far *str, unsigned flag, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head)
{
	int set;

	set = notes->residx.flags & flag;
	if (doyouset(str, !set, uid) != Q_YES) {
		puts("-- 中止しました --");
		return;
	}
	if (set)
		notes->residx.flags &= ~flag;
	else
		notes->residx.flags |= flag;

	write_response(&notes->residx, notes, blist, head);
	puts("-- 変更しました --");
}

int make_newnote(unsigned flags, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head)
{
	msgidx idx;
	unsigned maxnotes;

	if ((blist->basenote & uid->us.u.udat.usrflag) == 0)
		return SILENT;

	maxnotes = getmaxnotes(blist, head);
	if (maxnotes == 65535) {
		puts("** too many notes **");
		return SILENT;
	}
	memset(&idx, 0, sizeof(msgidx));
	cputs("----  Basenote  ----", GREEN, uid);
	print("Title : ");
	if (gets(idx.title, sizeof(idx.title)) == 0) {
		puts("-- 中止しました --");
		return SILENT;
	}
	if (write_textmsg(&idx, blist, uid, head) == 1)
		return SILENT;

	idx.flags = flags;
	lockbbs(uid, head);
	append_basenote(&idx, blist, head->us.p.config);
	update_boardlist(blist, idx.post_time, head->us.p.config);
	unlockbbs(uid, head);

	notes->status = OPEN;
	notes->note = maxnotes + 1;
	notes->response = 0;
	uid->us.u.dh.post++;
	uid->us.u.udat.post++;

	puts("-- 書き込み完了 --");

	return DISP_MESSAGE;
}

void update_basenote_mem(msgidx far *idx, notes_t far *notes, uid_struct far *head)
{
	int i;

	for (i = 1; i <= head->us.p.maxusers; i++) {
		if (head[i].us.u.notes.board_number  == notes->board_number &&
			head[i].us.u.notes.data_note_number == notes->data_note_number) {
			memcpy(&head[i].us.u.notes.basidx, idx, sizeof(msgidx));
			if (head[i].us.u.notes.data_res_number == 0)
				memcpy(&head[i].us.u.notes.residx, idx, sizeof(msgidx));
		}
	}
}

void update_response_mem(msgidx far *idx, notes_t far *notes, uid_struct far *head)
{
	int i;

	for (i = 1; i <= head->us.p.maxusers; i++) {
		if (head[i].us.u.notes.board_number  == notes->board_number &&
			head[i].us.u.notes.data_note_number == notes->data_note_number
			&& head[i].us.u.notes.data_res_number == notes->data_res_number) {
			memcpy(&head[i].us.u.notes.residx, idx, sizeof(msgidx));
		}
	}
}


int post_basenote(int key, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head)
{
	return make_newnote(0, notes, blist, uid, head);
}

int post_bn_awo(int key, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head)
{
	puts("-- Author Write Only --");
	return make_newnote(MSG_AWO, notes, blist, uid, head);
}

int post_message(int key, notes_t far *notes, boardlist far *blist, uid_struct far *uid, uid_struct far *head)
{
	msgidx idx;
	unsigned maxres;

	if (notes->basidx.flags & MSG_CLOSED) {
		puts("** 書き込めません **");
		return SILENT;
	}
	if ((blist->write & uid->us.u.udat.usrflag) == 0) {
		puts("** 書き込めません **");
		return SILENT;
	}
	if ((notes->basidx.flags & MSG_AWO) &&
					stricmp(notes->basidx.author, uid->us.u.udat.id) != 0) {
		puts("** 書き込めません **");
		return SILENT;
	}
	maxres = notes->basidx.response;
	if (maxres == 65534) {
		puts("** too many responses **");
		return SILENT;
	}
	memset(&idx, 0, sizeof(msgidx));
	cputs("----  Response  ----", CYAN, uid);
	if (notes->basidx.flags & MSG_SUBJECT) {
		print("Subject : ");
		if (gets(idx.title, sizeof(idx.title)) == 0) {
			puts("-- 中止しました --");
			return SILENT;
		}
		idx.flags |= MSG_SUBJECT;
	} else {
		sprintf(idx.title, "Re :%-.36s", notes->basidx.title);
	}
	if (iskanji1(idx.title[NOTETITLE_MAX - 1]))
		idx.title[NOTETITLE_MAX - 1] = 0;
	if (write_textmsg(&idx, blist, uid, head) == 1)
		return SILENT;

	lockbbs(uid, head);
	append_response(&idx, notes->note, blist, head->us.p.config);
	update_boardlist(blist, idx.post_time, head->us.p.config);
	update_basenote(idx.post_time, notes, blist, head);
	unlockbbs(uid, head);

	uid->us.u.dh.post++;
	uid->us.u.udat.post++;

	puts("-- 書き込み完了 --");
	return SILENT;
}

int quit(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	return NEXT_NOTEFILE;
}

int basenote_num(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	char buf[6];
	unsigned notenum = 0;

	print("Basenote番号 : ");
	if (isdigit(key))
		ungetch(key);
	if (gets(buf, sizeof(buf)) == 0)
		return SILENT;

	notenum = atou(buf);
	notes->note = adjust_notenum(notenum, blist, head);
	notes->status = OPEN;
	notes->response = 0;

	return DISP_MESSAGE;
}

int indexmode(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	notes->status = INDEX;
	notes->response = 0;
	return DISP_MESSAGE;
}

int response_num(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	char buf[6];
	unsigned resnum = 0;

	print("Response番号 : ");
	if (isdigit(key))
	ungetch(key);
	if (gets(buf, sizeof(buf)) == 0)
		return SILENT;

	resnum = atou(buf);
	notes->response = adjust_resnum(resnum, notes);

	return DISP_MESSAGE;
}

int next_response(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	unsigned maxres;

	maxres = notes->basidx.response;
	if (notes->response + 1 > maxres)
		return next_note(key, notes, blist, uid, head);

	notes->response++;
	return DISP_MESSAGE;
}

int next_newres(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	int handle;
	unsigned res;
	config_t far *conf = head->us.p.config;
	msgidx idx;

	handle = nidxopen(notes->note, conf->dir.bbsdir, blist->dosname, FA_READ);
	if (handle == 0)
		return next_newnote(key, notes, blist, uid, head);

	seek(handle, sizeof(msgidx) * notes->response, FS_SET);
	res = notes->response + 1;
	for (;;) {
		if (read(&idx, sizeof(msgidx), handle) == 0) {
			close(handle);
			return next_newnote(key, notes, blist, uid, head);
		}
		if (idx.last_update > notes->note_sequencer)
			break;
		if (notes->open_flags & OFLAG_A)
			break;
		if (notes->note_flags & NFLAG_A)
			break;

		res++;
	}
	close(handle);
	notes->response = res;

	return DISP_MESSAGE;
}

int prev_response(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (notes->response > 0)
		notes->response--;
	else
		return prev_note(key, notes, blist, uid, head);

	return DISP_MESSAGE;
}

int goto_basenote(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	notes->response = 0;
	return DISP_MESSAGE;
}

int next_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	unsigned maxnotes;

	maxnotes = getmaxnotes(blist, head);
	if (notes->note + 1 > maxnotes)
		return query_grq();

	notes->note++;
	notes->response = 0;

	return DISP_MESSAGE;
}

int prev_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (notes->note > 1) {
		notes->note--;
		notes->response = 0;
		return DISP_MESSAGE;
	}
	return SILENT;
}

int next_newnote(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	unsigned newnote;

	if (notes->note_flags & NFLAG_N) {
		notes->note_flags &= ~NFLAG_N;
		return SILENT;
	}
	newnote = search_newnote(notes->note, notes, blist, head);
	if (newnote == 0) {
		if (notes->open_flags & OFLAG_N)
			return NEXT_NOTEFILE;

		return query_grq();
	}
	notes->note = newnote;
	notes->response = 0;

	return DISP_MESSAGE;
}

int redraw_message(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	return DISP_MESSAGE;
}

int last_response(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	notes->response = notes->basidx.response;
	return DISP_MESSAGE;
}

int open_newnote(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	int r, ntmp;

	ntmp = notes->note;
	notes->note = 0;	/* note1 から検索させるため */
	if ((r = next_newnote(key, notes, blist, uid, head)) != 0)
		notes->note = ntmp;
	else
		notes->status = OPEN;

	return r;
}

int edit_title(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	char buf[NOTETITLE_MAX + 1];

	if (stricmp(notes->residx.author, uid->us.u.udat.id) != 0)
		return SILENT;

	printf("New Title : ");
	gets(buf, sizeof(buf));
	if (iskanji1(buf[NOTETITLE_MAX - 1]))
		buf[NOTETITLE_MAX - 1] = 0;

	if (cquery("変更しますか", Q_NONE, uid) != Q_YES) {
		puts("-- 中止しました --");
		return SILENT;
	}
	if (notes->response == 0) {
		strcpy(notes->basidx.title, buf);
		write_basenote(&notes->basidx, notes, blist, head);
	} else {
		strcpy(notes->residx.title, buf);
		write_response(&notes->residx, notes, blist, head);
	}
	puts("-- 変更しました --");
	return SILENT;
}

int scan_abort(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	return SCAN_ABORT;
}

int allnew_board(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (notes->open_flags & OFLAG_A) {
		puts("-- 未読設定を通常に戻しました --");
		notes->open_flags &= ~OFLAG_A;
	} else {
		puts("-- 全メッセージを未読とします --");
		notes->open_flags |= OFLAG_A;
	}
	return SILENT;
}

int current_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	notes->status = OPEN;
	notes->response = 0;

	return DISP_MESSAGE;
}

int disp_help0(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	typetext(head->us.p.config->dir.helpdir, "open0.hlp");
	return SILENT;
}

int disp_help1(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	typetext(head->us.p.config->dir.helpdir, "open1.hlp");
	return SILENT;
}

int disp_help2(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	typetext(head->us.p.config->dir.helpdir, "open2.hlp");
	return SILENT;
}

int disp_title(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	print_title(0, notes->response, notes, blist, head->us.p.config);
	return SILENT;
}

int disp_alltitle(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	print_title(0, 0, notes, blist, head->us.p.config);
	return SILENT;
}

int allnew_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (notes->note_flags & NFLAG_A) {
		puts("-- 未読設定を通常に戻しました --");
		notes->note_flags &= ~NFLAG_A;
	} else {
		puts("-- このノートの全メッセージを未読とします --");
		notes->note_flags |= NFLAG_A;
	}
	return SILENT;
}

int close_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (stricmp(notes->basidx.author, uid->us.u.udat.id) == 0)
		set_noteflag("Close", MSG_CLOSED, notes, blist, uid, head);

	return SILENT;
}

int set_subject(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (stricmp(notes->basidx.author, uid->us.u.udat.id) == 0)
		set_noteflag("Subject", MSG_SUBJECT, notes, blist, uid, head);

	return SILENT;
}

int nonstop_note(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	notes->note_flags |= NFLAG_N;
	return SILENT;
}

int nonstop_all(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	notes->open_flags |= (OFLAG_N | OFLAG_S);
	return SILENT;
}

int delete(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (stricmp(notes->residx.author, uid->us.u.udat.id) != 0)
		return SILENT;
	if (notes->response == 0)
		set_noteflag("Delete", MSG_DELETED, notes, blist, uid, head);
	else
		set_resflag("Delete", MSG_DELETED, notes, blist, uid, head);

	return SILENT;
}

int set_important(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (notes->response == 0)
		set_noteflag("Important", MSG_IMPORTANT, notes, blist, uid, head);
	else
		set_resflag("Important", MSG_IMPORTANT, notes, blist, uid, head);

	return SILENT;
}

int set_awo(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	if (stricmp(notes->basidx.author, uid->us.u.udat.id) == 0)
		set_noteflag("Author Write Only", MSG_AWO, notes, blist, uid, head);

	return SILENT;
}

int disp_important(int key, notes_t far *notes, boardlist far *blist , uid_struct far *uid, uid_struct far *head)
{
	print_title(MSG_IMPORTANT, notes->response,notes,blist,head->us.p.config);
	return SILENT;FLAGED