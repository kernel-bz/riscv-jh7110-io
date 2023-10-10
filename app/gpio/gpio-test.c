// SPDX-License-Identifier: GPL-2.0-only                                        
/*                                                                              
 *  gpio-test.c                                                         
 *                                                                              
 *  gpio test on the riscv, jh7110 
 *                                                                              
 *  JaeJoon Jung <rgbi3307@naver.com> on the www.kernel.bz
 */ 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define GPIO_CNT		4

int main(int argc, char **argv)
{
	int count = -1, step, i;
	int gpio[] = { 51, 50, 61, 44 };
	char cmd[80];

	if (argc == 2)
		count = atoi(argv[1]);

	step = (count < 0) ? 0 : 1;
	printf("gpio loop count:<%d>, step:<%d>\n", count, step);

	for (i = 0; i < GPIO_CNT; i++) {
		sprintf(cmd, "./gpio %d direction out", gpio[i]);
		system(cmd);
	}

	while (count -= step) {
		for (i = 0; i < GPIO_CNT; i++) {
			sprintf(cmd, "./gpio %d value 1", gpio[i]);
			system(cmd);
			sleep(1);
			sprintf(cmd, "./gpio %d value 0", gpio[i]);
			system(cmd);
		}
		//printf("gpio loop count:<%d>, step:<%d>\n", count, step);
	}

	//unexport
	for (i = 0; i < GPIO_CNT; i++) {
		sprintf(cmd, "./gpio %d", gpio[i]);
		system(cmd);
	}

	return 0;
}

