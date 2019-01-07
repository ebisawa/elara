
#if !defined(LOGDAT_H)
#define LOGDAT_H

#define DR_CARRIER_DOWN		1				/**/
#define DR_TIME_OUT			2				/**/
#define DR_CANNOT_LOGIN		3				/**/
#define DR_LOG_OFF			4				/**/
#define DR_DUPLICATE		5				/**/
#define DR_CHANNEL_ERROR	6
#define DR_SYSTEM_ERROR		7				/**/
#define DR_ABSOLUTE			8

typedef struct accesslog {
	unsigned count;							/* �A�N�Z�X�J�E���g     */
	char id[ID_MAX + 1];					/* ���[�U�[ID           */
	char handle[HANDLE_MAX + 1];			/* �n���h��             */
	long connect_time;						/* �R�l�N�g����         */
	long disconnect_time;					/* ����ؒf����         */
	int channel;							/* �`�����l���ԍ�       */
	char disconnect_reason;					/* �ڑ��f�̗��R         */
	char result[RESULT_MAX + 1];			/* ���f�����U���g�R�[�h */
	dhist d_hist;								/* �|�X�g���Ȃ�         */
	char reserved[46];
} accesslog;

#endif	/* LOGDAT_H */
