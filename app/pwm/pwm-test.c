// SPDX-License-Identifier: GPL-2.0-only                                        
/*                                                                              
 *  pwm-test.c                                                         
 *                                                                              
 *  pwm test on the riscv, jh7110 
 *                                                                              
 *  JaeJoon Jung <rgbi3307@naver.com> on the www.kernel.bz
 */ 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PWM_CNT			4
#define PWM_PERIOD		1000000UL	//1ms/ns
#define PWM_DUTY_STEP	10000

int main(int argc, char **argv)
{
	int count = -1, step, i;
	char cmd[80];
	unsigned long duty = 0;

	if (argc == 2)
		count = atoi(argv[1]);

	for(i = 0; i < PWM_CNT; i++) { 
		sprintf(cmd, "./pwm %d period %lu", i, PWM_PERIOD);
		system(cmd);
	}

	for(i = 0; i < PWM_CNT; i++) { 
		sprintf(cmd, "./pwm %d enable %d", i, 1);
		system(cmd);
	}

	step = (count < 0) ? 0 : 1;
	printf("pwm loop count:<%d>, step:<%d>\n", count, step);

	i = 0;
	while (count -= step) 
	{
		for(duty = 0; duty <= PWM_PERIOD; duty += PWM_DUTY_STEP) { 
			sprintf(cmd, "./pwm %d duty_cycle %lu", i, duty);
			system(cmd);
		}
		i++;
		if (i == PWM_CNT) {
			for(i = 0; i < PWM_CNT; i++) {
				sprintf(cmd, "./pwm %d duty_cycle %lu", i, 0);
				system(cmd);
			}
		}
		i %= PWM_CNT;

		//printf("pwm loop count:<%d>, step:<%d>\n", count, step);
	}

	for(i = 0; i < PWM_CNT; i++) { 
		sprintf(cmd, "./pwm %d enable %d", i, 0);
		system(cmd);
		//unexport
		sprintf(cmd, "./pwm %d", i);
		system(cmd);
	}
	return 0;
}

