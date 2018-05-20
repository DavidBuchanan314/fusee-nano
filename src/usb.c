#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <linux/limits.h>
#include <linux/usb/ch9.h>
#include <linux/usbdevice_fs.h>
#include <asm/byteorder.h>
#include <sys/ioctl.h>

#include "usb.h"

#define SYSFS_DEVICE_PATH "/sys/bus/usb/devices"
#define USBFS_PATH "/dev/bus/usb"

/* silly fscanf wrapper that takes a path */
static int scanf_path(const char *path, const char* fmt, ...)
{
	int result;
	FILE *fd;
	va_list args;
	
	va_start(args, fmt);
	if((fd = fopen(path, "r")) == NULL)
		return 0;
	
	result = vfscanf(fd, fmt, args);
	fclose(fd);
	va_end(args);
	return result;
}

/* returns 0 on sucess, -1 on fail */
static int find_sysfs_dir(char *path, size_t pathlen, int vid, int pid)
{
	DIR *d;
	struct dirent *dir;
	char tmp_path[PATH_MAX];
	int found = -1;
	int tmp;
	
	if((d = opendir(SYSFS_DEVICE_PATH)) == NULL)
		return -1;
	
	while ((dir = readdir(d)) != NULL) {
		if (dir->d_name[0] == '.')
			continue;
		
		snprintf(tmp_path,
			sizeof(tmp_path),
			SYSFS_DEVICE_PATH "/%s/idVendor",
			dir->d_name);
		
		if (scanf_path(tmp_path, "%x", &tmp) != 1 || tmp != vid)
			continue;
		
		snprintf(tmp_path,
			sizeof(tmp_path),
			SYSFS_DEVICE_PATH "/%s/idProduct",
			dir->d_name);
		
		if (scanf_path(tmp_path, "%x", &tmp) != 1 || tmp != pid)
			continue;
		
		found = 0; // success!
		snprintf(path,
			pathlen,
			SYSFS_DEVICE_PATH "/%s",
			dir->d_name);
	}
	
	closedir(d);
	return found;
}

int get_device(int vid, int pid)
{
	char sysfs_dir[PATH_MAX];
	char tmp_path[PATH_MAX];
	int busnum, devnum;
	
	if (find_sysfs_dir(sysfs_dir, sizeof(sysfs_dir), vid, pid) < 0) {
		errno = ENXIO; // is this a suitable errno?
		return -1;
	}
	
	strncpy(tmp_path, sysfs_dir, sizeof(tmp_path));
	strncat(tmp_path, "/busnum", sizeof(tmp_path)-1);
	if (scanf_path(tmp_path, "%d", &busnum) != 1) // XXX is this a decimal value?
		return -1;
	
	strncpy(tmp_path, sysfs_dir, sizeof(tmp_path));
	strncat(tmp_path, "/devnum", sizeof(tmp_path)-1);
	if (scanf_path(tmp_path, "%d", &devnum) != 1) // XXX is this a decimal value?
		return -1;
	
	snprintf(tmp_path,
		sizeof(tmp_path),
		USBFS_PATH "/%03d/%03d",
		busnum,
		devnum);
	
	return open(tmp_path, O_RDWR);
}

int claim_interface(int fd)
{
	int ifnum = 0;
	return ioctl(fd, USBDEVFS_CLAIMINTERFACE, &ifnum);
}

int ep_read(int fd,
	unsigned int ep,
	void *data,
	unsigned int len,
	unsigned int timeout)
{
	struct usbdevfs_bulktransfer bulkt = {
		.ep = USB_DIR_IN | ep,
		.len = len,
		.timeout = timeout,
		.data = data,
	};
	
	return ioctl(fd, USBDEVFS_BULK, &bulkt);
}

int ep_write(int fd,
	unsigned int ep,
	void *data,
	unsigned int len,
	unsigned int timeout)
{
	struct usbdevfs_bulktransfer bulkt = {
		.ep = USB_DIR_OUT | ep,
		.len = len,
		.timeout = timeout,
		.data = data,
	};
	
	return ioctl(fd, USBDEVFS_BULK, &bulkt);
}

int ctrl_transfer_unbounded(int fd, int length)
{
	int buf_size = sizeof(struct usb_ctrlrequest) + length;
	char *buffer = calloc(1, buf_size);
	struct usbdevfs_urb *purb;
	
	struct usb_ctrlrequest *ctrl_req = (struct usb_ctrlrequest *) buffer;
	ctrl_req->bRequestType = USB_DIR_IN | USB_RECIP_ENDPOINT;
	ctrl_req->bRequest = USB_REQ_GET_STATUS;
	ctrl_req->wLength = length;
	
	struct usbdevfs_urb urb = {
			.type = USBDEVFS_URB_TYPE_CONTROL,
			.endpoint = 0,
			.buffer = buffer,
			.buffer_length = buf_size,
			.usercontext = (void *) 0x1337,
	};
	
	if (ioctl(fd, USBDEVFS_SUBMITURB, &urb) < 0)
		return -1;
	
	if (ioctl(fd, USBDEVFS_DISCARDURB, &urb) < 0)
		return -2;
	
	if (ioctl(fd, USBDEVFS_REAPURB, &purb) < 0)
		return -3;
	
	if (purb->usercontext != (void *) 0x1337)
		return -4;
	
	free(buffer); // XXX buffer does not get freed under error conditions
	return 0;
}
