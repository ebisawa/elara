
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
	unsigned count;							/* アクセスカウント     */
	char id[ID_MAX + 1];					/* ユーザーID           */
	char handle[HANDLE_MAX + 1];			/* ハンドル             */
	long connect_time;						/* コネクト日時         */
	long disconnect_time;					/* 回線切断日時         */
	int channel;							/* チャンネル番号       */
	char disconnect_reason;					/* 接続断の理由         */
	char result[RESULT_MAX + 1];			/* モデムリザルトコード */
	dhist d_hist;								/* ポスト数など         */
	char reserved[46];
} accesslog;

#endif	/* LOGDAT_H */

