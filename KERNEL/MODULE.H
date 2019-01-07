/*
 * module.h - モジュール管理
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(MODULE_H)
#define MODULE_H

typedef struct {
	char type;				/* モジュール種別 ('M' = dlm)     */
	char reserved;			/* 予約                           */
	unsigned msize;			/* モジュールサイズ (CODE + DATA) */
	unsigned memneed;		/* 必要なメモリ量                 */
} common_header;

typedef struct {
	unsigned init;			/* 初期化ルーチンのオフセット */
	unsigned term;			/* 終了ルーチンのオフセット   */
	unsigned open;			/* open                       */
	unsigned close;			/* close                      */
	unsigned read;			/* read                       */
	unsigned write;			/* write                      */
	unsigned ioctl;			/* ioctl                      */
} ddm_header;

#endif	/* MODULE_H */

