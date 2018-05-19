#include <dirent.h>
#include <stdio.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "usb.h"

#define SYSFS_DEVICES "/sys/bus/usb/devices"
#define ID_LEN 4

int get_device(int vid, int pid) {
	DIR *d;
	struct dirent *dir;
	char vid_str[ID_LEN+1];
	char pid_str[ID_LEN+1];
	char read_id[ID_LEN+1];
	char dev_path[PATH_MAX];
	int fd;
	bool constraints_match;
	
	snprintf(vid_str, sizeof(vid_str), "%04x", vid);
	snprintf(pid_str, sizeof(pid_str), "%04x", pid);
	
	char *constraints[][2] = {
		/* file name, contents */
		{"idVendor", vid_str},
		{"idProduct", pid_str}
	};
	
	d = opendir(SYSFS_DEVICES);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == '.')
				continue;
			
			constraints_match = true;
			for (int i = 0; i < (int)(sizeof(constraints) / sizeof(constraints[0])); i++) {
				snprintf(
					dev_path,
					sizeof(dev_path),
					SYSFS_DEVICES "/%s/%s",
					dir->d_name,
					constraints[i][0]);
				
				if ((fd = open(dev_path, O_RDONLY)) < 0) {
					constraints_match = false;
					break;
				}
				
				if (read(fd, read_id, ID_LEN) < ID_LEN) {
					constraints_match = false;
					close(fd);
					break;
				}
				
				close(fd);
				
				if (strncmp(read_id, constraints[i][1], ID_LEN) != 0) {
					constraints_match = false;
					break;
				}
			}
			
			if (!constraints_match)
				continue;
			
			/* if we got this far, the vid/pid matched */
			closedir(d);
			snprintf(
				dev_path,
				sizeof(dev_path),
				SYSFS_DEVICES "/%s/descriptors",
				dir->d_name);
			
			printf("Found device: %s\n", dev_path); // DEBUG
			
			return open(dev_path, O_RDONLY);
		}
		closedir(d);
	}
	return -1;
}
