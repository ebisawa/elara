/*
 * device.h - デバイス管理
 * Copyright (C) S.Ebisawa, 1995. All rights reserved.
 */
#if !defined(DEVICE_H)
#define DEVICE_H

#include "xstruc.h"

int install_driver(char *name, char *argv);
int release_driver(char *name);
int add_device(char *driver_name, char *drvice_name);
int delete_device(char *device_name);
device_list *open_device(char *name);
void close_device(device_list *device_p);
int ioctl_device(char *device_name, unsigned command, void *param);
void release_all_driver(void);
int iolimit_device(char *name, unsigned pid);

#endif	/* DEVICE_H */

