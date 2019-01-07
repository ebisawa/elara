
#include "klib.h"
#include "mlib.h"
#include "ioctl.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

#define CONSOLE_CLS()	console_puts(CLS)

typedef struct device_struct {
	char device_name[9];
	char xwrite;
	int lstat;				/* 信号線エミュレーション */
} device_struct;

int MonitorRedraw, Current, nDevices;
int Status;					/* 0 = monitor, 1 = login?, 2 = shutdown? */
int ShutDown;
int SetReset;
char Result[] = "LOCAL";
device_struct far *Device;
uid_struct far *Head;

int console_getchar(void);
void console_putchar(char c);
void console_puts(char far *buf);
void channel_monitor(void);
device_struct far *get_dptable(char far *device_name);
void change_current(void);
void ioctl_get_linestat(device_struct far *device, unsigned far *stat);
void ioctl_set_linestat(device_struct far *device, unsigned far *stat);
void ioctl_get_result(device_struct far *device, char far * far *p);
void ioctl_get_speed(device_struct far *device, long far *p);
void ioctl_write_on(device_struct far *device, long far *p);
void ioctl_write_off(device_struct far *device, long far *p);

#define SetDS()		setds()
#define ResetDS()	rstds()

int far dev_init(char far *argv)
{
	int i;
	char far *p;

	SetDS();
	nDevices = nelems(argv);
	p = argv;

	if ((Device = xmalloc(sizeof(device_struct) * nDevices)) == 0) {
		ResetDS();
		return -1;
	}
	memset(Device, 0, sizeof(device_struct) * nDevices);

	for (i = 0; i < nDevices; i++) {
		strcpy(Device[i].device_name, p);
		Device[i].lstat = LSTAT_DSR;

		if (adddev("vcons", p) != 0) {
			disp("[");
			disp(p);
			disp("] Can't install device\r\n");
		}
		p = nextelem(p);
	}
	ResetDS();

	return 0;
}

int far dev_term(void)
{
	xfree(Device);
	return 0;
}

void far * far dev_open(char far *name)
{
	int i;
	device_struct far *p;

	SetDS();
	p = get_dptable(name);
	ResetDS();

	return p;
}

void far dev_close(device_struct far *p)
{
	SetDS();
	p->lstat &= ~LSTAT_DCD;
	ResetDS();
}

unsigned far dev_read(char far *buf, unsigned size, device_struct far *p)
{
	int c;
	unsigned i = 0;

	SetDS();
	if (p == &Device[Current] && (Device[Current].lstat & LSTAT_DCD)) {
		for (i = 0; i < size; i++) {
			if ((c = console_getchar()) == -1)
				break;

			if (c == CTRL('Z'))
				p->lstat &= ~LSTAT_DCD;
			else if (c == CTRL('V'))
					change_current();
			else {
				buf[i] = c;
				continue;
			}
			break;
		}
	}
	ResetDS();

	return i;
}

unsigned far dev_write(char far *buf, unsigned size, device_struct far *p)
{
	unsigned i = 0;

	SetDS();
	if (p != &Device[Current]) {
		ResetDS();
		return size;
	}
	if ((Device[Current].lstat & LSTAT_DCD) || Device[Current].xwrite) {
		for (i = 0; i < size; i++)
			console_putchar(buf[i]);
	}
	ResetDS();

	return i;
}

int far dev_ioctl(char far *device_name, unsigned command, void far *param)
{
	device_struct far *device;

	SetDS();
	device = get_dptable(device_name);			/* チェック不要 */

	switch (command) {
	case IOCTL_GETLINESTAT:
		ioctl_get_linestat(device, param);
		break;
	case IOCTL_SETLINESTAT:
		ioctl_set_linestat(device, param);
		break;
	case IOCTL_GETRESULT:
		ioctl_get_result(device, param);
		break;
	case IOCTL_GETSPEED:
		ioctl_get_speed(device, param);
		break;
	case IOCTL_WRITEON:
		ioctl_write_on(device, param);
		break;
	case IOCTL_WRITEOFF:
		ioctl_write_on(device, param);
		break;
	}
	ResetDS();

	return 0;
}

int console_getchar(void)
{
	char c;

	_asm {
		mov		ah, 06h	
		mov		dl, 0ffh
		int		21h
		jz		noinput
		mov		c, al
	}
	return c;
noinput:
	return -1;
}

void console_putchar(char c)
{
	_asm {
		mov		al, c
		int		29h
	}
}

void console_puts(char far *buf)
{
	while (*buf != 0)
		console_putchar(*buf++);
}

void channel_monitor(void)
{
	int i;
	char buf[128];

	if (Head == 0) {
		if ((Head = getuid_n("pwatch")) == 0)
			return;
	}
	if (Head->us.p.monitor_off)
		return;
	if (Head->us.p.monitor_redraw == 0 && MonitorRedraw == 0)
		return;
	if (Device[Current].lstat & LSTAT_DCD)
		return;

	CONSOLE_CLS();
	for (i = 1; i <= Head->us.p.maxusers; i++) {
		sprintf(buf, "[%2d] %-8s : ", i, FAR(Head[i].us.u.device_name));
		console_puts(buf);
		switch (Head[i].chstat) {
		case CS_WAIT:
			console_puts("waiting");
			if (i == getchnum(Device[Current].device_name, Head))
				console_puts("  <current console>");

			break;
		case CS_INIT:
			console_puts("initialize");
			break;
		case CS_OFF:
		case CS_SHUTDOWN:
			console_puts("off");
			break;
		case CS_LOGIN:
			console_puts("login");
			break;
		case CS_USING:
			sprintf(buf, "%-8s (%-16s)",
				FAR(Head[i].us.u.udat.id),
				FAR(Head[i].us.u.udat.handle));

			console_puts(buf);
			break;
		case CS_SHORT:
			console_puts("short");
			break;
		}
		console_puts("\r\n");
	}
	console_puts("\r\n");

	Head->us.p.monitor_redraw = 0;
	MonitorRedraw = 0;
}

device_struct far *get_dptable(char far *device_name)
{
	int i;

	for (i = 0; i < nDevices; i++) {
		if (strcmp(Device[i].device_name, device_name) == 0)
			return &Device[i];
	}
	return 0;
}

void change_current(void)
{
	uid_struct far *uid;
	char buf[128];

again:
	if (++Current >= nDevices)
		Current = 0;
	if (Head == 0) {
		if ((Head = getuid_n("pwatch")) == 0)
			return;
	}
	if ((uid = getuid_d(Device[Current].device_name, Head)) == 0)
		return;
	if ((Device[Current].lstat & LSTAT_DTR) == 0)
		goto again;

	if (uid->chstat != CS_WAIT) {
		CONSOLE_CLS();
		if (uid->chstat == CS_LOGIN) {
			sprintf(buf, "[ Ch%d (%s): login ]\r\n",
				getchnum(uid->us.u.device_name, Head),
				FAR(Device[Current].device_name));
		} else {
			sprintf(buf, "[ Ch%d (%s): %s (%s) ]\r\n",
				getchnum(uid->us.u.device_name, Head),
				FAR(Device[Current].device_name),
				uid->us.u.udat.id, uid->us.u.udat.handle);
		}
		console_puts(buf);
	}
	MonitorRedraw = 1;
}

void ioctl_get_linestat(device_struct far *device, unsigned far *lstat)
{
	int c;

	if ((device->lstat & LSTAT_DCD) == 0 && device == &Device[Current]) {
		if (Status == 0) {
			if ((c = console_getchar()) == ' ') {
				if (device->lstat & LSTAT_DTR) {
					console_puts("ログインしますか ? (y/[n]) : ");
					Status = 1;
				}
			} else if (c == 'e') {
				console_puts("終了しますか ? (y/[n]) : ");
#if 0
				if (ShutDown == 0)
					console_puts("終了しますか ? (y/[n]) : ");
				else
					console_puts("中止しますか ? (y/[n]) : ");
#endif
				Status = 2;
			} else if (c == CTRL('V'))
				change_current();
			else
				channel_monitor();
		} else {
			if ((c = console_getchar()) != -1) {
				if (c == 'y' || c == 'Y') {
					switch (Status) {
					case 1:
						device->lstat |= LSTAT_DCD;
						break;
					case 2:
						spawn(P_NOWAIT, "shutdown.dlm", "-s");
						ShutDown ^= 1;
						break;
					}
				}
				Status = 0;
				MonitorRedraw = 1;
				CONSOLE_CLS();
			}
		}
	}
	*lstat = device->lstat;
}

void ioctl_set_linestat(device_struct far *device, unsigned far *stat)
{
	device->lstat = *stat;
	if ((device->lstat & LSTAT_DTR) == 0)
		device->lstat &= ~LSTAT_DCD;
}

void ioctl_get_result(device_struct far *device, char far * far *p)
{
	*p = Result;
}

void ioctl_get_speed(device_struct far *device, long far *p)
{
	*p = 0;
}

void ioctl_write_on(device_struct far *device, long far *p)
{
	device->xwrite = 1;
}

void ioctl_write_off(device_struct far *device, long far *p)
{
	device->xwrite = 0;
}

