//
//	file name: recv.c
//	comments: Serial Communication Main
//	author: Jung,JaeJoon(rgbi3307@nate.com) at the www.kernel.bz
//

#include <stdio.h>
#include <string.h>
#include <termios.h>	//terminal control definitions
#include <stdlib.h>

#include "serial.h"

int input_data (char *buf)
{
	int i = 0, c;

	printf (">Send: ");
	memset (buf, 0, BUFSIZE);
	while ((c = getchar()) != (int)'\n' && i < BUFSIZE-2) buf[i++] = c;

	buf[i] = '\n';
	buf[i+1] = 0;

	return (i == 0) ? 0 : i + 1;
}

int main (int argc, char **argv)
{
	int fd, n, len, idx = 0;
	char buf[BUFSIZE];
	struct termios options;
	char *dev[] = { DEVS0, DEVS1, DEVS2, DEVS3 };                               
                                                                                
    if (argc > 1)                                                               
        idx = atoi(argv[1]);                                                    
                                                                                
    fd = serial_open (dev[idx], 0);                                             
    if (fd < 0) return -1;               

	//get current serial port settings
	tcgetattr(fd, &options);

	serial_setting (fd);

	printf ("sizeof buffer=%d\n", sizeof(buf));
	printf ("waiting...\n");

	while (1) {
		//receiving....
		n = serial_read_select (fd, buf, 5);	//select for 7 seconds
		if (n < 0) break;
		if (!n) n = serial_read_poll (fd, buf, 5);	//poll for 7 seconds
		if (n <= 0) break;

		printf ("<Recv: %s\n", buf);

		//sending....
		len = input_data (buf);
		if (len <= 0 || buf[0] < 1) break;

		//if (serial_write (fd, buf) < 0) break;
		n = serial_write_select (fd, buf, len, 2);
		if (n < 0) break;
		if (!n) n = serial_write_poll (fd, buf, len, 2);
		if (n <= 0) break;
	}

	//restore current serial port settings
	tcsetattr(fd, TCSANOW, &options);

	serial_close (fd);

	return 0;
}
