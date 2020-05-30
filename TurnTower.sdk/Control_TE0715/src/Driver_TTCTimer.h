/*
 * Driver_TTCTimer.h
 *
 *  Created on: 2019��10��15��
 *      Author: asguj
 */

#ifndef SRC_DRIVER_TTCTIMER_H_
#define SRC_DRIVER_TTCTIMER_H_
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xstatus.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xil_printf.h"
/*---------------------------�궨����---------------------------*/
#define	TICK_TIMER_FREQ_HZ	1000 		//��ʼ�����ж�Ƶ��

/*---------------------------������---------------------------*/
typedef struct {
	u32 OutputHz;					/* Output frequency */
	u16 Interval;					/* Interval value */
	u8 Prescaler;					/* Prescaler value */
	u16 Options;						/* Option settings */
} TmrCntrSetup;

/*---------------------------����������---------------------------*/

/*************************************************************
 * �������ƣ�Drv_TTCTimer_Start
 * ��������������TTC��ʱ������������3��
 * ���������uiChannel	: ͨ�����
 * �����������
 * ����ֵ����
 *************************************************************/
void Drv_TTCTimer_Start(u32 uiChannel);

/*************************************************************
 * �������ƣ�Drv_TTCTimer_Stop
 * ����������ֹͣTTC��ʱ��
 * ���������uiChannel	: ͨ�����
 * �����������
 * ����ֵ����
 *************************************************************/
int Drv_TTCTimer_Stop(u32 uiChannel);

/*************************************************************
 * �������ƣ�Drv_TTCTimer_ClearIntr
 * �������������TTC��ʱ���ж�
 * ���������uiChannel	: ͨ�����
 * �����������
 * ����ֵ����
 *************************************************************/
void Drv_TTCTimer_ClearIntr(void *CallBackRef);

/*************************************************************
 * �������ƣ�Drv_TTCTimer_SetupTicker
 * ��������������TTC�ײ�����
 * ���������iDeviceID	: TTC�豸ID
 * �����������
 * ����ֵ����
 *************************************************************/
int Drv_TTCTimer_SetupTicker(int iDeviceID);

/*************************************************************
 * �������ƣ�Drv_TTCTimer_Init
 * ������������ʼ��TTC��ʱ����������3��
 * ���������uiChannel	: ͨ�����
 * 		 uiPeriodMs	: ��ʱ���ڣ���λΪms
 * 		 (*func)()	: �ص�����
 * �����������
 * ����ֵ��XST_FAILURE	:	ʧ��
 *      XST_SUCCESS	:	�ɹ�
 *************************************************************/
int Drv_TTCTimer_Init(u32 uiChannel, u32 uiPeriodMs, void(*func)());

/*************************************************************
 * �������ƣ�Drv_TTCTimer_Bind
 * �����������󶨶�ʱ���ص�����
 * ���������uiChannel	: ͨ�����
 * 		 (*func)()  : �ص�����
 * �����������
 * ����ֵ��XST_FAILURE	:	ʧ��
 *      XST_SUCCESS	:	�ɹ�
 *************************************************************/
int Drv_TTCTimer_Bind(u32 uiChannel, void(*func)());

/*************************************************************
 * �������ƣ�Drv_TTCTimer_EnableIntr
 * ����������ʹ��TTC��ʱ���ж�
 * ���������uiChannel	: ͨ�����
 * �����������
 * ����ֵ����
 *************************************************************/
void Drv_TTCTimer_EnableIntr(u32 uiChannel);
s32 TTCTimer_setup(void);
void ISR_TTC0(void *arg);


#endif /* SRC_DRIVER_TTCTIMER_H_ */
