// SPDX-License-Identifier: GPL-2.0-only                                        
/*                                                                              
 *  i2c.c                                                         
 *                                                                              
 *  i2c control on the riscv 
 *                                                                              
 *  JaeJoon Jung <rgbi3307@naver.com> on the www.kernel.bz
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define I2C_SLAVE_ADDR_BH1721	0x23	//digit: 35
#define I2C_CMD_BH1721			0x10

#define I2C_SLAVE_ADDR_HT30		0x44	//digit: 68
#define I2C_CMD_HT30_MSB		0x2C	//enable
#define I2C_CMD_HT30_LSB		0x06	//repeat read

/**
	bh1721:		Light Sensor(value rage 2bytes: 1 ~ 65528)
	i2c_num:	i2c adapter number(/dev/i2c-#)
	i2c_addr:	i2c slave address(BH1721 is 0x23)
	loop_cnt:	looping count for read
*/
static int _i2c_bh1721_test(int i2c_num, int i2c_addr, int loop_cnt)
{
	int fd, ret;
	char dev[40];

	sprintf(dev, "/dev/i2c-%d", i2c_num);

    // open device handle to i2c device
    fd = open(dev, O_RDWR);
    if (fd < 0) {
        printf("%s:: Failed to open %s, errno: <%i>\n", __func__, dev, errno); 
		return -1;
    }

    // set the mode and address of the chip we want to talk to
    if (ioctl(fd, I2C_SLAVE, i2c_addr) < 0) {
        printf("%s:: Failed to set I2C_SLAVE for addr: [%X], errno: <%i>\n"
				, __func__, i2c_addr, errno); 
		goto _error;
    }

    //communicate with i2c device
    char buf[4] = {0, };
	int i, value;

    buf[0] = I2C_CMD_BH1721;

    ret = write(fd, buf, 1);
	if (ret != 1) {
        printf("%s:: Failed to write %s, ret: <%d>\n", __func__, dev, ret); 
		goto _error;
    }

	while (loop_cnt--) 
	{
		ret = read(fd, buf, 2);
		if (ret != 2) {
			printf("%s:: Failed to read %s, ret: <%d>\n", __func__, dev, ret); 
			goto _error;
		}
		value = buf[0] << 8 | buf[1];

		printf(">Reading Data Count <%d>: ", ret);
		for (i = 0; i < ret; i++) 
			printf("[%02X] ", buf[i]);
		printf(": Value: %d\n", value);
		sleep(1);
	}

	close(fd);
	return 0;

_error:
	close(fd);
	return -2;
}

/**
	ht30:		Humidity and Temperature Sensor
	i2c_num:	i2c adapter number(/dev/i2c-#)
	i2c_addr:	i2c slave address(ht30 is 0x44)
	loop_cnt:	looping count for read
*/
static int _i2c_ht30_test(int i2c_num, int i2c_addr, int loop_cnt)
{
	int fd, ret;
	char dev[40];

	sprintf(dev, "/dev/i2c-%d", i2c_num);

    // open device handle to i2c device
    fd = open(dev, O_RDWR);
    if (fd < 0) {
        printf("%s:: Failed to open %s, errno: <%i>\n", __func__, dev, errno); 
		return -1;
    }

    // set the mode and address of the chip we want to talk to
    if (ioctl(fd, I2C_SLAVE, i2c_addr) < 0) {
        printf("%s:: Failed to set I2C_SLAVE for addr: [%X], errno: <%i>\n"
				, __func__, i2c_addr, errno); 
		goto _error;
    }

    //communicate with i2c device
    char buf[8] = {0, };
	int i, temp, humi;
	float tf, hf;

	while (loop_cnt--) 
	{
		buf[0] = I2C_CMD_HT30_MSB;
		buf[1] = I2C_CMD_HT30_LSB;

		ret = write(fd, buf, 2);
		if (ret != 2) {
			printf("%s:: Failed to write %s, ret: <%d>\n", __func__, dev, ret); 
			goto _error;
		}

		ret = read(fd, buf, 6);
		if (ret != 6) {
			printf("%s:: Failed to read %s, ret: <%d>\n", __func__, dev, ret); 
			goto _error;
		}
		temp = buf[0] << 8 | buf[1];
		humi = buf[3] << 8 | buf[4];

		//Conversion
		//tf = -45 + 175 * (float)temp / 65535;
		tf = -50 + 175 * (float)temp / 65535;
		hf = 100 * (float)humi / 65535;

		printf(">Reading Data Count <%d>: ", ret);
		for (i = 0; i < ret; i++) 
			printf("[%02X] ", buf[i]);

		printf(": Temperature: %.2f", tf);
		printf(", Humidity: %.2f\n", hf);

		sleep(1);
	}

	close(fd);
	return 0;

_error:
	close(fd);
	return -2;
}

int main(int argc, char **argv) 
{
	int i2c_num, i2c_addr, loop_cnt;
	int ret;

	if (argc == 4) {
		i2c_num = atoi(argv[1]);
		i2c_addr = atoi(argv[2]);
		loop_cnt = atoi(argv[3]);
		if (i2c_addr == I2C_SLAVE_ADDR_HT30)
			ret = _i2c_ht30_test(i2c_num, i2c_addr, loop_cnt);
		else
			ret = _i2c_bh1721_test(i2c_num, i2c_addr, loop_cnt);
		return ret;
	}
	ret = _i2c_ht30_test(0, I2C_SLAVE_ADDR_HT30, 10);
	return ret;

	printf("Usage: i2c <num> <addr> <loop_cnt>\n");
}
