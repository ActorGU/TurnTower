/*
 * main.c
 *
 *  Created on: 2019Äê10ÔÂ15ÈÕ
 *      Author: asguj
 */
#include <stdio.h>
#include "platform.h"
#include "uart_init.h"
#include "Driver_TTCTimer.h"
#include "Process.h"



int main(void)
{
	init_platform();
	TTCTimer_setup();

	while (1)
	{

		if(1 == g_bGetCpuCmd)
		{
			g_bGetCpuCmd = 0;
			Data_analysis();
		}

	}
	cleanup_platform();

	return 0;
}

