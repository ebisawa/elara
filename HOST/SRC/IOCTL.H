
#if !defined(IOCTL_H)
#define IOCTL_H

#define IOCTL_GETLINESTAT	1			/* �����Ԃ̎擾       */
#define IOCTL_SETLINESTAT	2			/* �����Ԃ̐ݒ�       */
#define IOCTL_GETRESULT		3			/* ���U���g�R�[�h�̎擾 */
#define IOCTL_GETSPEED		4			/* ������x�̐ݒ�       */
#define IOCTL_SETSPEED		5			/* ������x�̎擾       */
#define IOCTL_CLEARBUF		6			/* �o�b�t�@�N���A       */
#define IOCTL_WRITEON		7			/* ������ڑ����o�͋��� */
#define IOCTL_WRITEOFF		8			/* ������ڑ����o�͕s�� */

#define LSTAT_DCD			0x0001		/* CD: ����ڑ���Ԃ� on */
#define LSTAT_DTR			0x0002		/* ER: off �ŉ���ؒf    */
#define LSTAT_DSR			0x0004		/* DR: Data Set Ready    */
#define LSTAT_RTS			0x0008		/* RS: Request To Send   */

#endif	/* IOCTL_H */
