
#if !defined(CONFIG_H)
#define CONFIG_H

#define BOARDNAME_MAX		32				/* ノートファイル名 (最大) */
#define BOARDTITLE_MAX		64				/* ポリシーノート   (最大) */
#define NOTETITLE_MAX		40				/* ノートタイトル   (最大) */
#define ID_MAX				8				/* ID               (最大) */
#define PASSWORD_MAX		16				/* パスワード       (最大) */
#define HANDLE_MAX			16				/* ハンドル         (最大) */
#define PROMPT_MAX			20				/* プロンプト       (最大) */
#define FLAGNAME_MAX		20				/* フラグ名         (最大) */
#define INITSTR_MAX			128				/* 初期化文字列     (最大) */
#define RESULT_MAX			32				/* リザルトコード   (最大) */
#define FILENAME_MAX		40				/* ファイル名       (最大) */

#define DEF_EDITOR_F		79				/* エディタ /F: 初期値 */
#define DEF_EDITOR_M		0				/* エディタ /M: 初期値 */
#define DEF_TERM_X			80				/* 画面桁数     初期値 */
#define DEF_TERM_Y			24				/* 画面行数     初期値 */

#define DEF_PRIO_PWATCH		1000
#define DEF_PRIO_TLIMIT		1000

#endif	/* CONFIG_H */

