#ifndef USB_H
#define USB_H

#include <stdint.h>

/* Returns the fd of the USB device with the corresponding vid/pid */
int get_device(int vid, int pid);

int claim_interface(int fd, int ifnum);

int ep_read(int fd,
	unsigned int ep,
	void *data,
	unsigned int len,
	unsigned int timeout);

int ep_write(int fd,
	unsigned int ep,
	void *data,
	unsigned int len,
	unsigned int timeout);

int ctrl_transfer_unbounded(int fd, int length);

#endif /* USB_H */
