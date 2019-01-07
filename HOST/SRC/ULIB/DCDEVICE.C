
#include "klib.h"
#include "../ioctl.h"
#include "../config.h"
#include "../bbsdat.h"
#include "../cfgdat.h"
#include "../usrdat.h"
#include "../logdat.h"
#include "ulib.h"

void disconnect_device(char far *device)
{
	unsigned lstat;

	lineoff(device);
	do {
		ioctl(device, IOCTL_GETLINESTAT, &lstat);
	} while (lstat & LSTAT_DCD);
	/*
	 * DTR �� off �ɂ����܂܂ɂ��Ă���
	 * ��� sendinit ������������ on �ɂ���
	 *
	 * �������Ȃ��� pwatch ������ؒf���m�F����O�ɒ��M��������
	 * �]���r�ɂȂ�\��������
	 */
}
