// SPDX-License-Identifier: GPL-2.0-only
/*
 *	spi.c
 *
 *	Editted by Jung,JaeJoon <rgbi3307@naver.com> on the www.kernel.bz
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>	//-lpthread

#define CONFIG_DEBUG

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define SPI_DEV			"/dev/spidev1.0"
#define SPI_BITS		8
//#define SPI_SPEED		800000
#define SPI_SPEED		500000

static uint8_t txbuf[] = {
	0x11, 0x33, 0x77, 0xFF, 0x11, 0x33, 0x77, 0xFF 
};
static uint8_t rxbuf[ARRAY_SIZE(txbuf)] = {0, };
static uint8_t txflag = 0;

struct spi_ioc_transfer trx;


static void pabort(const char *s)
{
	if (errno != 0)
		perror(s);
	else
		printf("%s\n", s);
	abort();
}

static void hex_dump(const void *src, size_t length, size_t line_size,
		     char *prefix)
{
	int i = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" |");
			while (line < address) {
				c = *line++;
				printf("%c", (c < 32 || c > 126) ? '.' : c);
			}
			printf("|\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}

static void spi_buffer_set(uint32_t mode, uint8_t bits, uint32_t speed
							, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	trx.tx_buf = (unsigned long)tx;
	trx.rx_buf = (unsigned long)rx;
	trx.len = len;
	trx.delay_usecs = 0;
	trx.speed_hz = speed;
	trx.bits_per_word = bits;

	if (mode & SPI_TX_OCTAL)
		trx.tx_nbits = 8;
	else if (mode & SPI_TX_QUAD)
		trx.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		trx.tx_nbits = 2;
	if (mode & SPI_RX_OCTAL)
		trx.rx_nbits = 8;
	else if (mode & SPI_RX_QUAD)
		trx.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		trx.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_OCTAL | SPI_TX_QUAD | SPI_TX_DUAL))
			trx.rx_buf = 0;
		else if (mode & (SPI_RX_OCTAL | SPI_RX_QUAD | SPI_RX_DUAL))
			trx.tx_buf = 0;
	}
}

static void spi_send(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;

	trx.tx_buf = (unsigned long)tx;
	trx.rx_buf = (unsigned long)rx;
	trx.len = len;

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &trx);
	if (ret < 1)
		pabort("can't send spi message");

#ifdef CONFIG_DEBUG
	hex_dump(tx, len, 8, ">TX");
	hex_dump(rx, len, 8, ">RX");
#endif

	txflag = 1;
}

void *spi_recv_thread(void *arg)
{
	int fd, ret;
	uint8_t buf[16];
	const size_t len = 8;

	fd = *(int *)arg; 
	printf("%s:: fd:<%d>\n", __func__, fd);

	while (1) {
		if (txflag) {
			memset(buf, 0, sizeof(buf));

			ret = read(fd, buf, len);
			printf("%s:: read length:<%d>\n", __func__, ret);

			if (ret == len) {
				txflag = 0;
#ifdef CONFIG_DEBUG
				hex_dump(buf, len, len, "<<RX");
#endif
				printf("\n");
			}
		}
		usleep(20000);	//20ms
	}
}

static int spi_config_set(int fd, uint32_t mode, uint8_t bits, uint32_t speed)
{
	int ret;

	//spi mode
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	//bits per word
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	//max speed hz
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	return ret;
}

static int spi_config_get(int fd, uint32_t *mode, uint8_t *bits, uint32_t *speed)
{
	int ret;

	//spi mode
	ret = ioctl(fd, SPI_IOC_RD_MODE32, mode);
	if (ret == -1)
		pabort("can't get spi mode");

	//bits per word
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, bits);
	if (ret == -1)
		pabort("can't get bits per word");

	//max speed hz
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("%s:: mode:[%X], bits:<%u>, speed:<%u>Hz, <%u>KHz\n\n"
			, __func__, *mode, *bits, *speed, *speed/1000);

	return ret;
}

int main(int argc, char **argv)
{
	int fd, ret, cnt = 10;
	uint8_t bits;
	uint32_t mode = 0, speed;
	pthread_t tid;

	if (argc > 1)
		cnt = atoi(argv[1]);	//loop count

	fd = open(SPI_DEV, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	ret = spi_config_set(fd, mode, SPI_BITS, SPI_SPEED);
	if (ret < 0)
		return ret;
	ret = spi_config_get(fd, &mode, &bits, &speed);
	if (ret < 0)
		return ret;

	ret = pthread_create(&tid, NULL, spi_recv_thread, (void *)&fd);
	if (ret) {
		pabort("can't create spi_recv_thread()");
		return -1;
	} else
		printf("spi_recv_thread:<%u>, fd:<%d>\n", tid, fd);

	spi_buffer_set(mode, bits, speed, txbuf, rxbuf, sizeof(txbuf));

	//send & recv loop test
	while(cnt--) {
		spi_send(fd, txbuf, rxbuf, sizeof(txbuf));; 
		usleep(2000000);	//2000ms(2s)
	}

	close(fd);
	return ret;
}
