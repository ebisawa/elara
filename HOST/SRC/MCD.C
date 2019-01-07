
#include "klib.h"
#include "mlib.h"
#include "ioctl.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"

#define TYPE_NONE		0
#define TYPE_MODEM		1
#define TYPE_DIRECT		2

/* -------------------------------------------------------------------------
 *    +---------------------------------------------------------------+
 *    | RI|CTS|DCD| x |RXF|OER|FER|PER|DSR|DBK|RTS| x | x | x |DTR| x |
 *    +---------------------------------------------------------------+
 *      15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
 * ---------------------------------------------------------------------- */
#define MCD_LSTAT_DCD	0x2000		/* get */
#define MCD_LSTAT_DSR	0x0080		/* get */
#define MCD_LSTAT_RTS	0x0020		/* get, set */
#define MCD_LSTAT_DTR	0x0002		/* get, set */

typedef struct device_struct {
	char device_name[9];
	char ucount;
	char result[RESULT_MAX + 1];	/* modem result code */
	char xwrite;
	int type;

	unsigned (pascal far *getlstat)(void);
	void (pascal far *setlstat)(unsigned);
	void (pascal far *setbufstat)(unsigned);
	unsigned (pascal far *read)(char far *, unsigned);
	unsigned (pascal far *write)(char far *, unsigned);
	int (pascal far *getc)(void);
	int (pascal far *putc)(int c);
	long (pascal far *getspeed)(void);
	void (pascal far *setspeed)(long);
} device_struct;

typedef struct getid_packet {		/*  8 bytes */
	int funcnum;
	char minor;
	char major;
	char far *id;
} getid_packet;

typedef struct getpent_packet {		/* 16 bytes */
	int funcnum;
	unsigned far *addr;
	unsigned cseg;
	unsigned tblsize;
	char reserved[6];
} getpent_packet;

void error(char far *device, char far *msg);
int mcd_exist(char far *device_name);
device_struct far *mcd_getpent(char far *device_name, int tab);
void mcd_disconnect(device_struct far *device);
int mcd_gets(char far *buf, device_struct far *device);
void set_result(device_struct far *device);
device_struct far *get_dptable(char far *device_name);
void ioctl_get_linestat(device_struct far *device, unsigned far *stat);
void ioctl_set_linestat(device_struct far *device, unsigned far *stat);
void ioctl_get_result(device_struct far *device, void far * far *result);
void ioctl_get_speed(device_struct far *device, long far *speed);
void ioctl_set_speed(device_struct far *device, long far *speed);
void ioctl_clear_buffer(device_struct far *device, long far *p);
void ioctl_write_on(device_struct far *device, long far *p);
void ioctl_write_off(device_struct far *device, long far *p);

int nDevices;
device_struct far *Device;


int far dev_init(char far *argv)
{
	int i;
	char far *p;

	setds();
	nDevices = nelems(argv);
	p = argv;

	if ((Device = xmalloc(sizeof(device_struct) * nDevices)) == 0) {
		rstds();
		return -1;
	}
	memset(Device, 0, sizeof(device_struct) * nDevices);

	for (i = 0; i < nDevices; i++) {
		if (!mcd_exist(p))
			error(p, "is not a MCD device.");
		else {
			if (mcd_getpent(p, i) == 0)
				error(p, "DPC is not supported.");
			else {
				if (adddev("mcd", p) != 0)
					error(p, "Can't install.");
			}
		}
		p = nextelem(p);
	}
	rstds();

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

	setds();
	if ((p = get_dptable(name)) != 0)
		p->ucount++;
	rstds();

	return p;
}

void far dev_close(device_struct far *device)
{
	setds();
	device->ucount--;
	mcd_disconnect(device);
	rstds();
}

unsigned far dev_read(char far *buf, unsigned size, device_struct far *device)
{
	int c;
	unsigned i;

	setds();
	i = device->read(buf, size);
	rstds();

	return i;
}

unsigned far dev_write(char far *buf, unsigned size, device_struct far *device)
{
	unsigned i = 0;

	setds();
	if (device->type != 0 || device->xwrite)
		i = device->write(buf, size);

	rstds();

	return i;
}

int far dev_ioctl(char far *device_name, unsigned command, void far *param)
{
	device_struct far *device;

	setds();
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
	case IOCTL_SETSPEED:
		ioctl_set_speed(device, param);
		break;
	case IOCTL_CLEARBUF:
		ioctl_clear_buffer(device, param);
		break;
	case IOCTL_WRITEON:
		ioctl_write_on(device, param);
		break;
	case IOCTL_WRITEOFF:
		ioctl_write_off(device, param);
		break;
	}
	rstds();

	return 0;
}

void error(char far *device, char far *msg)
{
	disp("mcd: device '");
	disp(device);
	disp("' ");
	disp(msg);
	disp("\r\n");
}

int dos_ioctl_call(int handle, void far *packet, int size)
{
	_asm {
		push	ds
		mov		ax, 4402h
		mov		bx, handle
		mov		cx, size
		lds		dx, [packet]
		int		21h
		pop		ds
	}
}

int mcd_exist(char far *device_name)
{
	int handle, r;
	getid_packet packet;

	if ((handle = open(device_name, FA_READ)) == 0)
		return 0;

	packet.funcnum = 9;
	packet.id = 0;
	r = dos_ioctl_call(fileno(handle), &packet, sizeof(packet));
	close(handle);

	if (r == sizeof(packet) && strcmp(packet.id, "MCD") == 0)
		return 1;

	return 0;
}

device_struct far *mcd_getpent(char far *device_name, int tab)
{
	int handle, r;
	getpent_packet packet;
	device_struct far *p;

	if ((handle = open(device_name, FA_READ)) == 0)
		return 0;

	packet.funcnum = 22;
	r = dos_ioctl_call(fileno(handle), &packet, sizeof(packet));
	close(handle);

	if (r != sizeof(packet))
		return 0;

	p = &Device[tab];
	memset(p, 0, sizeof(device_struct));
	strcpy(p->device_name, device_name);
	p->getlstat   = MK_FP(packet.cseg, packet.addr[3]);
	p->setlstat   = MK_FP(packet.cseg, packet.addr[4]);
	p->setbufstat = MK_FP(packet.cseg, packet.addr[6]);
	p->read       = MK_FP(packet.cseg, packet.addr[7]);
	p->write      = MK_FP(packet.cseg, packet.addr[8]);
	p->getc       = MK_FP(packet.cseg, packet.addr[14]);
	p->putc       = MK_FP(packet.cseg, packet.addr[15]);
	p->getspeed   = MK_FP(packet.cseg, packet.addr[21]);
	p->setspeed   = MK_FP(packet.cseg, packet.addr[22]);

	return p;
}

void mcd_disconnect(device_struct far *device)
{
	unsigned lstat;

	switch (device->type) {
	case TYPE_MODEM:
		lstat = device->getlstat();
		lstat &= ~MCD_LSTAT_DTR;
		device->setlstat(lstat);

		while (lstat & MCD_LSTAT_DCD)
			lstat = device->getlstat();

		lstat |= MCD_LSTAT_DTR;
		device->setlstat(lstat);
		break;
	case TYPE_DIRECT:
		lstat = device->getlstat();
		lstat &= ~MCD_LSTAT_DTR;
		device->setlstat(lstat);	/* DTR を一瞬落とす */
		lstat |= MCD_LSTAT_DTR;
		device->setlstat(lstat);
		break;
	}
}

int mcd_gets(char far *buf, device_struct far *device)
{
	int i, c = 0;

	for (i = 0; c != '\n'; i++) {
		while ((c = device->getc()) == -1)
			;

		*buf++ = c;
	}
	return i;
}

void set_result(device_struct far *device)
{
	char far *s, buf[128];

	if (device->type == TYPE_DIRECT)		/* DIRECT */
		sprintf(device->result, "%ld/DIR", device->getspeed());
	else {
		device->result[0] = 0;				/* MODEM */
		for (;;) {
			if (mcd_gets(buf, device) == 0)
				break;
			if ((s = strchr(buf, ' ')) != 0) {
				*s++ = 0;
				if (strcmp(buf, "CONNECT") == 0) {
					if (strlen(s) < sizeof(device->result))
						strcpy(device->result, s);
				}
				break;
			}
		}
	}
	device->setbufstat(0x0003);		/* 送信・受信バッファクリア */
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

void ioctl_get_linestat(device_struct far *device, unsigned far *stat)
{
	unsigned lstat;

	lstat = device->getlstat();

	if (device->ucount == 0) {
		if (lstat & MCD_LSTAT_DCD) {
			if (device->type == 0) {
				device->type = TYPE_MODEM;
				set_result(device);
			}
		} else {
			if (device->type == 0 && lstat & MCD_LSTAT_DSR
					&& device->getc() != -1) {
				device->type = TYPE_DIRECT;	/* DCD が落ちてるのに入力あり */
				set_result(device);			/*  -> 直結端末からの接続要求 */
			}
		}
	}
	if ((lstat & MCD_LSTAT_DSR) == 0 && device->type == TYPE_DIRECT)
		device->type = 0;					/* 相手側からの直結回線強制切断 */
	if (device->type == TYPE_DIRECT) {
		if (lstat & MCD_LSTAT_DCD)
			device->type = 0;				/* 直結中に DCD が on になった */
		else
			lstat |= MCD_LSTAT_DCD;			/* DCD エミュレーション */
	}
	*stat = 0;
	*stat |= (lstat & MCD_LSTAT_DCD) ? LSTAT_DCD : 0;
	*stat |= (lstat & MCD_LSTAT_DTR) ? LSTAT_DTR : 0;
	*stat |= (lstat & MCD_LSTAT_DSR) ? LSTAT_DSR : 0;
	*stat |= (lstat & MCD_LSTAT_RTS) ? LSTAT_RTS : 0;
}

void ioctl_set_linestat(device_struct far *device, unsigned far *stat)
{
	unsigned lstat;

	lstat = device->getlstat();

	if (*stat & LSTAT_DTR)
		lstat |= MCD_LSTAT_DTR;
	else {
		if (device->type != 0)
			device->setbufstat(0x0003);		/* 送信・受信バッファクリア */
		if (device->type == TYPE_DIRECT)
			device->type = 0;				/* 直結強制切断 */

		lstat &= ~MCD_LSTAT_DTR;
	}

	if (*stat & LSTAT_RTS)
		lstat |= MCD_LSTAT_RTS;
	else
		lstat &= ~MCD_LSTAT_RTS;

	device->setlstat(lstat);
}

void ioctl_get_result(device_struct far *device, void far * far *result)
{
	*result = device->result;
}

void ioctl_get_speed(device_struct far *device, long far *speed)
{
	*speed = device->getspeed();
}

void ioctl_set_speed(device_struct far *device, long far *speed)
{
	device->setspeed(*speed);
}

void ioctl_clear_buffer(device_struct far *device, long far *p)
{
	device->setbufstat(0x0003);
}

void ioctl_write_on(device_struct far *device, long far *p)
{
	device->xwrite = 1;
}

void ioctl_write_off(device_struct far *device, long far *p)
{
	device->xwrite = 0;
}

