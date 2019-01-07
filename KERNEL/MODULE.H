/*
 * module.h - ���W���[���Ǘ�
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(MODULE_H)
#define MODULE_H

typedef struct {
	char type;				/* ���W���[����� ('M' = dlm)     */
	char reserved;			/* �\��                           */
	unsigned msize;			/* ���W���[���T�C�Y (CODE + DATA) */
	unsigned memneed;		/* �K�v�ȃ�������                 */
} common_header;

typedef struct {
	unsigned init;			/* ���������[�`���̃I�t�Z�b�g */
	unsigned term;			/* �I�����[�`���̃I�t�Z�b�g   */
	unsigned open;			/* open                       */
	unsigned close;			/* close                      */
	unsigned read;			/* read                       */
	unsigned write;			/* write                      */
	unsigned ioctl;			/* ioctl                      */
} ddm_header;

#endif	/* MODULE_H */
