/*
 * device.c - デバイス管理
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include "kernel.h"
#include "xstruc.h"
#include "device.h"
#include "filesys.h"
#include "module.h"
#include "memory.h"

static driver_list *HeadDriver;
static device_list *HeadDevice;

static void ddm_funcinit(ddm_header *ddmhead, driver_list *driver_p);
static driver_list *driver_alloc(char *filename);
static void driver_free(driver_list *p);
static void unchain_driver(driver_list *driver_p);
static driver_list *search_driver_n(char *name);
static device_list *device_alloc(char *device_name);
static void unchain_device(device_list *device_p);
static device_list *search_device_n(char *name);

int install_driver(char *name, char *argv)
{
	int handle;
	char buf[FILENAME_MAX];
	void far *s;
	driver_list *p;
	common_header chead;
	ddm_header ddmhead;

	if (name == 0)
		return E_PARAMERR;

	strcpy(buf, name);
	basename(buf);

	if ((p = search_driver_n(buf)) != 0)
		return E_USING;
	if ((p = driver_alloc(buf)) == 0)
		return E_NOMEM;
	if ((handle = open_file_dos(name, FA_READ)) == 0) {
		driver_free(p);
		return E_NOMDL;
	}
	read_file_dos(&chead, sizeof(chead), handle);
	if (chead.type != 'D' ||
			(p->segm = dosmem_alloc(PARA(chead.memneed))) == 0) {

		close_file_dos(handle);
		driver_free(p);
		return E_NOMDL;
	}
	read_file_dos(&ddmhead, sizeof(ddmhead), handle);

	p->size = PARA(chead.memneed) * 16;
	s = MK_FP(p->segm, 0);
	memset(s, 0, p->size);

	read_file_dos(s, chead.msize, handle);
	close_file_dos(handle);
	ddm_funcinit(&ddmhead, p);
	p->udevs = 0;

	if (p->func.init(argv) != 0) {
		dosmem_free(p->segm);
		driver_free(p);
		return E_DRVERR;
	}

	return 0;
}

int release_driver(char *name)
{
	driver_list *p;

	if ((p = search_driver_n(name)) == 0)
		return E_NODRV;
	if (p->udevs != 0)
		return E_USING;
	if (p->func.term() != 0)
		return E_DRVERR;

	dosmem_free(p->segm);
	driver_free(p);

	return 0;
}

int add_device(char *driver_name, char *device_name)
{
	device_list *p;
	driver_list *q;

	if ((q = search_driver_n(driver_name)) == 0)
		return E_NODRV;
	if ((p = device_alloc(device_name)) == 0)
		return E_NOMEM;

	p->ucount = 0;
	p->driver = q;
	p->device_p = 0;
	p->iolimit = 0;

	return 0;
}

int delete_device(char *device_name)
{
	device_list *p;

	if ((p = search_device_n(device_name)) == 0)
		return E_NODEV;

	unchain_device(p);
	kmem_free(p);
	return 0;
}

device_list *open_device(char *name)
{
	device_list *p;

	if (name == 0)
		return 0;
	if ((p = search_device_n(name)) == 0)
		return 0;
	if (p->ucount++ == 0) {
		if ((p->device_p = p->driver->func.open(name)) == 0) {
			p->ucount = 0;
			return 0;
		}
		p->driver->udevs++;
	}
	return p;
}

void close_device(device_list *device_p)
{
	if (device_p == 0)
		return;
	if (--device_p->ucount == 0) {
		if (device_p->driver) {
			if (device_p->device_p != 0)
				device_p->driver->func.close(device_p->device_p);

			device_p->driver->udevs--;
		}
	}
}

int ioctl_device(char *device_name, unsigned command, void *param)
{
	device_list *p;

	if ((p = search_device_n(device_name)) == 0)
		return E_NODEV;
	if (p->driver->func.ioctl(device_name, command, param) != 0)
		return E_DRVERR;

	return 0;
}

void release_all_driver(void)
{
	driver_list *p;
	device_list *q;

	for (q = HeadDevice; q != 0; q = q->next) {
		if (q->device_p != 0)
			q->driver->func.close(q->device_p);
	}
	for (p = HeadDriver; p != 0; p = p->next)
		p->func.term();
}

int iolimit_device(char *name, unsigned pid)
{
	device_list *p;

	if (name == 0)
		return 0;
	if ((p = search_device_n(name)) == 0)
		return 0;

	p->iolimit = pid;
	return 1;
}


static void ddm_funcinit(ddm_header *ddmhead, driver_list *driver_p)
{
	driver_p->func.init  = MK_FP(driver_p->segm, ddmhead->init);
	driver_p->func.term  = MK_FP(driver_p->segm, ddmhead->term);
	driver_p->func.open  = MK_FP(driver_p->segm, ddmhead->open);
	driver_p->func.close = MK_FP(driver_p->segm, ddmhead->close);
	driver_p->func.read  = MK_FP(driver_p->segm, ddmhead->read);
	driver_p->func.write = MK_FP(driver_p->segm, ddmhead->write);
	driver_p->func.ioctl = MK_FP(driver_p->segm, ddmhead->ioctl);
}

static driver_list *driver_alloc(char *name)
{
	driver_list *p;

	if ((p = search_driver_n(name)) != 0)
		return p;
	if ((p = kmem_alloc(sizeof(driver_list))) == 0)
		return 0;

	strcpy(p->driver_name, name);

	if (HeadDriver == 0)
		p->next = 0;
	else
		p->next = HeadDriver;

	HeadDriver = p;

	return p;
}

static void driver_free(driver_list *p)
{
	unchain_driver(p);
	kmem_free(p);
}

static void unchain_driver(driver_list *driver_p)
{
	driver_list *p;

	if (HeadDriver == driver_p)
		HeadDriver = driver_p->next;
	else {
		for (p = HeadDriver; p->next != 0; p = p->next) {
			if (p->next == driver_p) {
				p->next = driver_p->next;
				break;
			}
		}
	}
}

static driver_list *search_driver_n(char *name)
{
	driver_list *p;

	for (p = HeadDriver; p != 0; p = p->next) {
		if (strcmp(p->driver_name, name) == 0)
			return p;
	}
	return 0;
}

static device_list *device_alloc(char *device_name)
{
	device_list *p;

	if ((p = search_device_n(device_name)) != 0)
		return p;
	if ((p = kmem_alloc(sizeof(device_list))) == 0)
		return 0;
	if (device_name == 0)
		return 0;

	strcpy(p->device_name, device_name);

	if (HeadDevice == 0)
		p->next = 0;
	else
		p->next = HeadDevice;

	HeadDevice = p;

	return p;
}

static void unchain_device(device_list *device_p)
{
	device_list *p;

	if (device_p == 0)
		return;
	if (HeadDevice == device_p)
		HeadDevice = device_p->next;
	else {
		for (p = HeadDevice; p->next != 0; p = p->next) {
			if (p->next == device_p) {
				p->next = device_p->next;
				break;
			}
		}
	}
}

static device_list *search_device_n(char *name)
{
	device_list *p;

	for (p = HeadDevice; p != 0; p = p->next) {
		if (strcmp(p->device_name, name) == 0)
			return p;
	}
	return 0;
}

