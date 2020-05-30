/*
 * Driver_TTCTimer.h
 *
 *  Created on: 2019年10月15日
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
/*---------------------------宏定义区---------------------------*/
#define	TICK_TIMER_FREQ_HZ	1000 		//初始定义中断频率

/*---------------------------变量区---------------------------*/
typedef struct {
	u32 OutputHz;					/* Output frequency */
	u16 Interval;					/* Interval value */
	u8 Prescaler;					/* Prescaler value */
	u16 Options;						/* Option settings */
} TmrCntrSetup;

/*---------------------------函数声明区---------------------------*/

/*************************************************************
 * 函数名称：Drv_TTCTimer_Start
 * 功能描述：启动TTC定时器，最多可启动3个
 * 输入参数：uiChannel	: 通道编号
 * 输出参数：无
 * 返回值：无
 *************************************************************/
void Drv_TTCTimer_Start(u32 uiChannel);

/*************************************************************
 * 函数名称：Drv_TTCTimer_Stop
 * 功能描述：停止TTC定时器
 * 输入参数：uiChannel	: 通道编号
 * 输出参数：无
 * 返回值：无
 *************************************************************/
int Drv_TTCTimer_Stop(u32 uiChannel);

/*************************************************************
 * 函数名称：Drv_TTCTimer_ClearIntr
 * 功能描述：清除TTC定时器中断
 * 输入参数：uiChannel	: 通道编号
 * 输出参数：无
 * 返回值：无
 *************************************************************/
void Drv_TTCTimer_ClearIntr(void *CallBackRef);

/*************************************************************
 * 函数名称：Drv_TTCTimer_SetupTicker
 * 功能描述：建立TTC底层配置
 * 输入参数：iDeviceID	: TTC设备ID
 * 输出参数：无
 * 返回值：无
 *************************************************************/
int Drv_TTCTimer_SetupTicker(int iDeviceID);

/*************************************************************
 * 函数名称：Drv_TTCTimer_Init
 * 功能描述：初始化TTC定时器，最多可配3个
 * 输入参数：uiChannel	: 通道编号
 * 		 uiPeriodMs	: 定时周期，单位为ms
 * 		 (*func)()	: 回调函数
 * 输出参数：无
 * 返回值：XST_FAILURE	:	失败
 *      XST_SUCCESS	:	成功
 *************************************************************/
int Drv_TTCTimer_Init(u32 uiChannel, u32 uiPeriodMs, void(*func)());

/*************************************************************
 * 函数名称：Drv_TTCTimer_Bind
 * 功能描述：绑定定时器回调函数
 * 输入参数：uiChannel	: 通道编号
 * 		 (*func)()  : 回调函数
 * 输出参数：无
 * 返回值：XST_FAILURE	:	失败
 *      XST_SUCCESS	:	成功
 *************************************************************/
int Drv_TTCTimer_Bind(u32 uiChannel, void(*func)());

/*************************************************************
 * 函数名称：Drv_TTCTimer_EnableIntr
 * 功能描述：使能TTC定时器中断
 * 输入参数：uiChannel	: 通道编号
 * 输出参数：无
 * 返回值：无
 *************************************************************/
void Drv_TTCTimer_EnableIntr(u32 uiChannel);
s32 TTCTimer_setup(void);
void ISR_TTC0(void *arg);


#endif /* SRC_DRIVER_TTCTIMER_H_ */
