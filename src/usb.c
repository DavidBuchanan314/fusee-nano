#include <dirent.h>
#include <stdio.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "usb.h"

#define SYSFS_DEVICES "/sys/bus/usb/devices"
#define ID_LEN 4

int get_device(int vid, int pid) {
	DIR *d = opendir(SYSFS_DEVICES);
	struct dirent *dir;
	char vid_str[ID_LEN+1];
	char pid_str[ID_LEN+1];
	char read_id[ID_LEN+1];
	char dev_path[PATH_MAX];
	int fd;
	
	snprintf(vid_str, sizeof(vid_str), "%04x", vid);
	snprintf(pid_str, sizeof(pid_str), "%04x", pid);
	
	char *constraints[][2] = {
		{"idVendor", vid_str},
		{"idProduct", pid_str}
	};
	
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == '.')
				continue;
			
			for (int i=0; i < (int)(sizeof(constraints) / sizeof(constraints[0])); i++) {
				snprintf(
					dev_path,
					sizeof(dev_path),
					SYSFS_DEVICES "/%s/%s",
					dir->d_name,
					constraints[i][0]);
				
				if ((fd = open(dev_path, O_RDONLY)) < 0)
					goto fail;
				
				if (read(fd, read_id, ID_LEN) < ID_LEN)
					goto fail;
				
				if (strncmp(read_id, constraints[i][1], ID_LEN) != 0)
					goto fail;
				
				close(fd);
				continue;
				
			fail:
				close(fd);
				goto constraints_failed;
			}
			
			/* if we got this far, the vid/pid matched */
			snprintf(
					dev_path,
					sizeof(dev_path),
					SYSFS_DEVICES "/%s/descriptors",
					dir->d_name);
			
			printf("Found device: %s\n", dev_path); // DEBUG
			
			closedir(d);
			return open(dev_path, O_RDONLY);
			
		constraints_failed:
			continue;
		}
		closedir(d);
	}
	
	return -1;
}
