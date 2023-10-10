//
//	file name: serial.h
//	comments: Serial Port Module
//	author: Jung,JaeJoon(rgbi3307@nate.com) at the www.kernel.bz
//

#ifndef _SERIAL_H
#define _SERIAL_H

#define BUFSIZE		256
//#define BAUDRATE	B9600
//#define BAUDRATE	B38400
#define BAUDRATE	B115200

#define DEVS0		"/dev/ttyS0"
#define DEVS1		"/dev/ttyS1"
#define DEVS2		"/dev/ttyS2"
#define DEVS3		"/dev/ttyS3"

//Open serial port.
//Returns the file descriptor on success or -1 on error.
int serial_open (char *device, int mode);

//Writing Data to the Port
//return size of writing
int serial_write (int fd, char *buf, int len);

//wait: seconds
int serial_write_select (int fd, char *buf, int len, int wait);
//wait: seconds
int serial_write_poll (int fd, char *buf, int len, int wait);

//Reading Data from the Port
int serial_read (int fd, char *buf, int mode);

//wait: seconds
int serial_read_select (int fd, char *buf, int wait);
//wait: seconds
int serial_read_poll (int fd, char *buf, int wait);

int serial_close (int fd);

void serial_setting(int fd);

#endif
