// SPDX-License-Identifier: GPL-2.0-only                                        
/*                                                                              
 *  gpio.c                                                         
 *                                                                              
 *  gpio control on the riscv 
 *                                                                              
 *  JaeJoon Jung <rgbi3307@naver.com> on the www.kernel.bz
 */ 

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define DSIZE			20
#define BSIZE			80
#define	GPIO_EXPORT		"/sys/class/gpio/export"
#define	GPIO_UNEXPORT	"/sys/class/gpio/unexport"
#define GPIO_PATH		"/sys/class/gpio/gpio"

static int _gpio_export(char *gpio)
{
	int fd, ret, cnt;

	fd = open(GPIO_EXPORT, O_WRONLY);
	if (fd < 0) {
		printf("%s:: %s: open error.\n", __func__, GPIO_EXPORT);
		return -1;
	}

	cnt = strlen(gpio);

	ret = write(fd, gpio, cnt);
	if (ret < 0) 
		printf("%s:: write() error, gpio:%s\n", __func__, gpio);

	close(fd);
	printf("%s:: gpio:%s, cnt:%d\n", __func__, gpio, cnt);
	usleep(20000);	//20ms

	return ret;
}

static int _gpio_unexport(char *gpio)
{
	int fd, ret, cnt;

	fd = open(GPIO_UNEXPORT, O_WRONLY);
	if (fd < 0) {
		printf("%s:: %s: open error.\n", __func__, GPIO_UNEXPORT);
		return -1;
	}

	cnt = strlen(gpio);

	ret = write(fd, gpio, cnt);
	if (ret < 0) 
		printf("%s:: write() error, gpio:%s\n", __func__, gpio);

	close(fd);
	printf("%s:: gpio:%s, cnt:%d\n", __func__, gpio, cnt);
	usleep(20000);	//20ms

	return ret;
}

/**
	gpio: gpio number
	target: direction, value
	value
*/
static int _gpio_set_value(char *gpio, char *target, char *value)
{
	char path[BSIZE], fname[BSIZE];
	int fd, cnt = 0, ret;

	sprintf(path, "%s%s", GPIO_PATH, gpio);
	sprintf(fname, "%s/%s", path, target);

_retry:
	fd = open(fname, O_WRONLY);
	if (fd < 0) {
		ret = _gpio_export(gpio);
		cnt++;
		if (ret < 0 || cnt > 3)
			return -1;
		usleep(20000);	//20ms
		goto _retry;
	}

	cnt = strlen(value);
	ret = write(fd, value, cnt);
	if (ret < 0)
		printf("%s:: write() error, fname:%s, gpio:%s\n", __func__, fname, gpio);

	close(fd);
	//printf("%s:: gpio:%s, target:%s, value:%s, cnt:%d\n", __func__, gpio, target, value, cnt);
	return ret;
}

//gpio read
static int _gpio_get_value(char *gpio, char *target)
{
	char path[BSIZE], fname[BSIZE], value[DSIZE] = {0, };
	int fd, ret, cnt = 0;

	sprintf(path, "%s%s", GPIO_PATH, gpio);
	sprintf(fname, "%s/%s", path, target);

_retry:
	fd = open(fname, O_RDONLY);
	if (fd < 0) {
		ret = _gpio_export(gpio);
		cnt++;
		if (ret < 0 || cnt > 3)
			return -1;
		usleep(20000);	//20ms
		goto _retry;
	}

	ret = read(fd, value, DSIZE);
	if (ret < 0)
		printf("%s:: read() error, fname:%s, gpio:%s\n", __func__, fname, gpio);

	close(fd);
	printf("%s:: gpio:%s, target:%s, value:%s\n", __func__, gpio, target, value);
	return ret;
}

int main(int argc, char **argv)
{
	if (argc == 2)
		return _gpio_unexport(argv[1]);

	if (argc == 3)
		return _gpio_get_value(argv[1], argv[2]);

	if (argc == 4)
		return _gpio_set_value(argv[1], argv[2], argv[3]);

	printf("%s:: Usage: gpio <num> [direction | value] <value>\n", __func__);
	return 0;
}

