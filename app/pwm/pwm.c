// SPDX-License-Identifier: GPL-2.0-only                                        
/*                                                                              
 *  pwm.c                                                         
 *                                                                              
 *  pwm control on the riscv 
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
#define	PWM_EXPORT		"/sys/class/pwm/pwmchip0/export"
#define	PWM_UNEXPORT	"/sys/class/pwm/pwmchip0/unexport"
#define PWM_PATH		"/sys/class/pwm/pwmchip0/pwm"

static int _pwm_export(char *pwm)
{
	int fd, ret, cnt;

	fd = open(PWM_EXPORT, O_WRONLY);
	if (fd < 0) {
		printf("%s:: %s: open error.\n", __func__, PWM_EXPORT);
		return -1;
	}

	cnt = strlen(pwm);

	ret = write(fd, pwm, cnt);
	if (ret < 0) 
		printf("%s:: write() error, pwm:%s\n", __func__, pwm);

	close(fd);
	printf("%s:: pwm:%s, cnt:%d\n", __func__, pwm, cnt);
	usleep(20000);	//20ms

	return ret;
}

static int _pwm_unexport(char *pwm)
{
	int fd, ret, cnt;

	fd = open(PWM_UNEXPORT, O_WRONLY);
	if (fd < 0) {
		printf("%s:: %s: open error.\n", __func__, PWM_UNEXPORT);
		return -1;
	}

	cnt = strlen(pwm);

	ret = write(fd, pwm, cnt);
	if (ret < 0) 
		printf("%s:: write() error, pwm:%s\n", __func__, pwm);

	close(fd);
	printf("%s:: pwm:%s, cnt:%d\n", __func__, pwm, cnt);
	usleep(20000);	//20ms

	return ret;
}

/**
	pwm: pwm number
	target: period, duty_cycle, enable
	value
*/
static int _pwm_set_value(char *pwm, char *target, char *value)
{
	char path[BSIZE], fname[BSIZE];
	int fd, cnt = 0, ret;

	sprintf(path, "%s%s", PWM_PATH, pwm);
	sprintf(fname, "%s/%s", path, target);

_retry:
	fd = open(fname, O_WRONLY);
	if (fd < 0) {
		ret = _pwm_export(pwm);
		cnt++;
		if (ret < 0 || cnt > 3)
			return -1;
		usleep(20000);	//20ms
		goto _retry;
	}

	cnt = strlen(value);
	ret = write(fd, value, cnt);
	if (ret < 0)
		printf("%s:: write() error, fname:%s, pwm:%s\n", __func__, fname, pwm);

	close(fd);
	//printf("%s:: pwm:%s, target:%s, value:%s, cnt:%d\n", __func__, pwm, target, value, cnt);
	return ret;
}

//pwm read
static int _pwm_get_value(char *pwm, char *target)
{
	char path[BSIZE], fname[BSIZE], value[DSIZE] = {0, };
	int fd, ret, cnt = 0;

	sprintf(path, "%s%s", PWM_PATH, pwm);
	sprintf(fname, "%s/%s", path, target);

_retry:
	fd = open(fname, O_RDONLY);
	if (fd < 0) {
		ret = _pwm_export(pwm);
		cnt++;
		if (ret < 0 || cnt > 3) 
			return -1;
		usleep(20000);	//20ms
		goto _retry;
	}

	ret = read(fd, value, DSIZE);
	if (ret < 0)
		printf("%s:: read() error, fname:%s, pwm:%s\n", __func__, fname, pwm);

	close(fd);
	printf("%s:: pwm:%s, target:%s, value:%s\n", __func__, pwm, target, value);
	return ret;
}

int main(int argc, char **argv)
{
	if (argc == 2)
		return _pwm_unexport(argv[1]);

	if (argc == 3)
		return _pwm_get_value(argv[1], argv[2]);

	if (argc == 4)
		return _pwm_set_value(argv[1], argv[2], argv[3]);

	printf("%s:: Usage: pwm <num> [period | duty_cycle | enable] <value>\n", __func__);
	return 0;
}

