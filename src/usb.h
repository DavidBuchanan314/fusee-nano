#ifndef USB_H
#define USB_H

/* Returns the fd of the USB device with the corresponding vid/pid */
int get_device(int vid, int pid);

#endif /* USB_H */
