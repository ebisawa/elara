
#if !defined(IOCTL_H)
#define IOCTL_H

#define IOCTL_GETLINESTAT	1			/* 回線状態の取得       */
#define IOCTL_SETLINESTAT	2			/* 回線状態の設定       */
#define IOCTL_GETRESULT		3			/* リザルトコードの取得 */
#define IOCTL_GETSPEED		4			/* 回線速度の設定       */
#define IOCTL_SETSPEED		5			/* 回線速度の取得       */
#define IOCTL_CLEARBUF		6			/* バッファクリア       */
#define IOCTL_WRITEON		7			/* 回線未接続時出力許可 */
#define IOCTL_WRITEOFF		8			/* 回線未接続時出力不可 */

#define LSTAT_DCD			0x0001		/* CD: 回線接続状態で on */
#define LSTAT_DTR			0x0002		/* ER: off で回線切断    */
#define LSTAT_DSR			0x0004		/* DR: Data Set Ready    */
#define LSTAT_RTS			0x0008		/* RS: Request To Send   */

#endif	/* IOCTL_H */

