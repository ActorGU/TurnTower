/*
 * Driver_TTCTimer.c
 *
 *  Created on: 2019年10月15日
 *      Author: asguj
 */
#include "Driver_TTCTimer.h"
#include "xscugic.h"
#include "xparameters.h"
#include "xparameters_ps.h"
#include "uart_init.h"
#include "xstatus.h"
#include "header_init.h"
#include "Process.h"
TmrCntrSetup SettingsTable=
	{TICK_TIMER_FREQ_HZ, 0, 0, 0};		/* Ticker timer counter initial setup,only output freq */

XTtcPs TtcPsInst[3];  					/* Timer counter instance */
static XScuGic Intc;

extern u32 *tx_address_KJG;
/*************************************************************
 * 函数名称：Drv_TTCTimer_EnableIntr
 * 功能描述：使能TTC定时器中断
 * 输入参数：uiChannel	: 通道编号
 * 输出参数：无
 * 返回值：无
 *************************************************************/
void Drv_TTCTimer_EnableIntr(u32 uiChannel)
{
	int iDevIntr;
	XTtcPs *TtcPsTick;
	int iDevID;
	switch(uiChannel)
	{
		case 0:
			iDevID 		= XPAR_XTTCPS_0_DEVICE_ID;
			iDevIntr 	= XPAR_XTTCPS_0_INTR;
			break;
		case 1:
			iDevID 		= XPAR_XTTCPS_1_DEVICE_ID;
			iDevIntr 	= XPAR_XTTCPS_1_INTR;
			break;
		case 2:
			iDevID 		= XPAR_XTTCPS_2_DEVICE_ID;
			iDevIntr 	= XPAR_XTTCPS_2_INTR;
			break;
		default:
			iDevID 		= XPAR_XTTCPS_0_DEVICE_ID;
			iDevIntr 	= XPAR_XTTCPS_0_INTR;
			break;
	}

	TtcPsTick = &TtcPsInst[iDevID];
	XScuGic_Enable(&Intc, iDevIntr);
	XTtcPs_EnableInterrupts(TtcPsTick, XTTCPS_IXR_INTERVAL_MASK);

}
/*************************************************************
 * 函数名称：Drv_TTCTimer_Bind
 * 功能描述：绑定定时器回调函数
 * 输入参数：uiChannel	: 通道编号
 * 		 (*func)()  : 回调函数
 * 输出参数：无
 * 返回值：XST_FAILURE	:	失败
 *      XST_SUCCESS	:	成功
 *************************************************************/
int Drv_TTCTimer_Bind(u32 uiChannel, void(*func)())
{
	int iStatus;
	XTtcPs *TtcPsTick;
	int iDevID;
	int iDevIntr;
	switch(uiChannel)
	{
		case 0:
			iDevID 		= XPAR_XTTCPS_0_DEVICE_ID;
			iDevIntr 	= XPAR_XTTCPS_0_INTR;
			TtcPsTick = &TtcPsInst[iDevID];
			break;
		case 1:
			iDevID 		= XPAR_XTTCPS_1_DEVICE_ID;
			iDevIntr 	= XPAR_XTTCPS_1_INTR;
			TtcPsTick = &TtcPsInst[iDevID];
			break;
		case 2:
			iDevID 		= XPAR_XTTCPS_2_DEVICE_ID;
			iDevIntr 	= XPAR_XTTCPS_2_INTR;
			break;
		default:
			iDevID 		= XPAR_XTTCPS_0_DEVICE_ID;
			iDevIntr 	= XPAR_XTTCPS_0_INTR;
			break;
	}

	TtcPsTick = &TtcPsInst[iDevID];
	/*
	 * Connect to the interrupt controller
	 */
	iStatus = XScuGic_Connect(&Intc, iDevIntr,
		(Xil_InterruptHandler)func, (void *)TtcPsTick);

	return iStatus;
}
/*************************************************************
 * 函数名称：Drv_TTCTimer_Start
 * 功能描述：启动TTC定时器，最多可启动3个
 * 输入参数：uiChannel	: 通道编号
 * 输出参数：无
 * 返回值：无
 *************************************************************/
void Drv_TTCTimer_Start(u32 uiChannel)
{
	switch(uiChannel)
	{
		case 0:
			XTtcPs_Start(&TtcPsInst[XPAR_XTTCPS_0_DEVICE_ID]);
			break;
		case 1:
			XTtcPs_Start(&TtcPsInst[XPAR_XTTCPS_1_DEVICE_ID]);
			break;
		case 2:
			XTtcPs_Start(&TtcPsInst[XPAR_XTTCPS_2_DEVICE_ID]);
			break;
		default:
			break;
	}
}

/*************************************************************
 * 函数名称：Drv_TTCTimer_Stop
 * 功能描述：停止TTC定时器
 * 输入参数：uiChannel	: 通道编号
 * 输出参数：无
 * 返回值：无
 *************************************************************/
int Drv_TTCTimer_Stop(u32 uiChannel)
{
	switch(uiChannel)
	{
		case 0:
			XTtcPs_Stop(&TtcPsInst[XPAR_XTTCPS_0_DEVICE_ID]);
			break;
		case 1:
			XTtcPs_Stop(&TtcPsInst[XPAR_XTTCPS_1_DEVICE_ID]);
			break;
		case 2:
			XTtcPs_Stop(&TtcPsInst[XPAR_XTTCPS_2_DEVICE_ID]);
			break;
		default:
			break;
	}
	return 0;
}

/*************************************************************
 * 函数名称：Drv_TTCTimer_ClearIntr
 * 功能描述：清除TTC定时器中断
 * 输入参数：uiChannel	: 通道编号
 * 输出参数：无
 * 返回值：无
 *************************************************************/
void Drv_TTCTimer_ClearIntr(void *CallBackRef)
{
	u32 StatusEvent;
	/*
	 * Read the interrupt status, then write it back to clear the interrupt.
	 */
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);

}

/*************************************************************
 * 函数名称：Drv_TTCTimer_SetupTicker
 * 功能描述：建立TTC底层配置
 * 输入参数：iDeviceID	: TTC设备ID
 * 输出参数：无
 * 返回值：无
 *************************************************************/
int Drv_TTCTimer_SetupTicker(int iDeviceID)
{
	int Status;
	XTtcPs_Config *Config;
	XTtcPs *Timer;
	TmrCntrSetup *TimerSetup;

	TimerSetup = &SettingsTable;

	TimerSetup->Options |= (XTTCPS_OPTION_INTERVAL_MODE |
					      XTTCPS_OPTION_WAVE_DISABLE);

	Timer = &TtcPsInst[iDeviceID];
	/*
	 * Stop the timer first
	 */
	XTtcPs_Stop(Timer);

	/*
	 * Look up the configuration based on the device identifier
	 */
	Config = XTtcPs_LookupConfig(iDeviceID);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	/*
	 * Initialize the device
	 */
	Status = XTtcPs_CfgInitialize(Timer, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the options
	 */
	XTtcPs_SetOptions(Timer, TimerSetup->Options);

	/*
	 * Timer frequency is preset in the TimerSetup structure,
	 * however, the value is not reflected in its other fields, such as
	 * IntervalValue and PrescalerValue. The following call will map the
	 * frequency to the interval and prescaler values.
	 */
	XTtcPs_CalcIntervalFromFreq(Timer, TimerSetup->OutputHz,
		&(TimerSetup->Interval), &(TimerSetup->Prescaler));

	/*
	 * Set the interval and prescale
	 */
	XTtcPs_SetInterval(Timer, TimerSetup->Interval);
	XTtcPs_SetPrescaler(Timer, TimerSetup->Prescaler);

	return XST_SUCCESS;
}

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
int Drv_TTCTimer_Init(u32 uiChannel, u32 uiPeriodMs, void(*func)())
{
	int iStatus;

	SettingsTable.OutputHz = (u32)((float)1000/uiPeriodMs);
	switch(uiChannel)
	{
		case 0:
			iStatus = Drv_TTCTimer_SetupTicker(XPAR_XTTCPS_0_DEVICE_ID);
			break;
		case 1:
			iStatus = Drv_TTCTimer_SetupTicker(XPAR_XTTCPS_1_DEVICE_ID);
			break;
		case 2:
			iStatus = Drv_TTCTimer_SetupTicker(XPAR_XTTCPS_2_DEVICE_ID);
			break;
		default:
			iStatus = XST_FAILURE;
			break;
	}

	if(iStatus == XST_FAILURE)
	{
		return iStatus;
	}

	iStatus = Drv_TTCTimer_Bind(uiChannel, func);	//绑定回调函数
	Drv_TTCTimer_EnableIntr(uiChannel);				//使能中断
	return iStatus;
}


void ISR_TTC0(void *arg)
{
//	if (DataRecv_PC[0].packet_Check)
//	{
//		Data_analysis();
//	}

	u8 send[] = {0x55 , 0xAA , 0xBB , 0xCC , 0xdd , 0xee , 0xff};
	uart_tx(tx_address_PC , send , 7);
	Drv_TTCTimer_ClearIntr(arg);
}


s32 TTCTimer_setup()
{
	int Status;
	XScuGic_Config *IntcConfig;

	IntcConfig = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);

	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(&Intc, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/*串口中断配置*/
	//UART_PC
	XScuGic_SetPriorityTriggerType(&Intc , XPAR_FABRIC_UART_PC_RXD_ENDOFPACKET_INTR , 0xb0, 0x3);
	Status = XScuGic_Connect(&Intc, XPAR_FABRIC_UART_PC_RXD_ENDOFPACKET_INTR,
				(Xil_InterruptHandler)Uart_Pc_rx,(void*)&Intc);
	XScuGic_Enable(&Intc, XPAR_FABRIC_UART_PC_RXD_ENDOFPACKET_INTR);
	//UART_HW
	XScuGic_SetPriorityTriggerType(&Intc , XPAR_FABRIC_UART_HW_RXD_ENDOFPACKET_INTR , 0xb0, 0x3);
	Status = XScuGic_Connect(&Intc, XPAR_FABRIC_UART_HW_RXD_ENDOFPACKET_INTR,
				(Xil_InterruptHandler)Uart_HW_rx,(void*)&Intc);
	XScuGic_Enable(&Intc, XPAR_FABRIC_UART_HW_RXD_ENDOFPACKET_INTR);
	//UART_KJG
	XScuGic_SetPriorityTriggerType(&Intc , XPAR_FABRIC_UART_KJG_RXD_ENDOFPACKET_INTR , 0xb0, 0x3);
	Status = XScuGic_Connect(&Intc, XPAR_FABRIC_UART_KJG_RXD_ENDOFPACKET_INTR,
					(Xil_InterruptHandler)Uart_KJG_rx,(void*)&Intc);
	XScuGic_Enable(&Intc, XPAR_FABRIC_UART_KJG_RXD_ENDOFPACKET_INTR);
	//UART_ServoA yaw
	XScuGic_SetPriorityTriggerType(&Intc , XPAR_FABRIC_UART_SERVOA_RXD_ENDOFPACKET_INTR , 0xb0, 0x3);
	Status = XScuGic_Connect(&Intc, XPAR_FABRIC_UART_SERVOA_RXD_ENDOFPACKET_INTR,
					(Xil_InterruptHandler)Uart_ServoA_rx,(void*)&Intc);
	XScuGic_Enable(&Intc, XPAR_FABRIC_UART_SERVOA_RXD_ENDOFPACKET_INTR);
	//UART_ServoB pitch
	XScuGic_SetPriorityTriggerType(&Intc , XPAR_FABRIC_UART_SERVOB_RXD_ENDOFPACKET_INTR , 0xb0, 0x3);
	Status = XScuGic_Connect(&Intc, XPAR_FABRIC_UART_SERVOB_RXD_ENDOFPACKET_INTR,
					(Xil_InterruptHandler)Uart_ServoB_rx,(void*)&Intc);
	XScuGic_Enable(&Intc, XPAR_FABRIC_UART_SERVOB_RXD_ENDOFPACKET_INTR);
	//Uart_Tail_rx
	XScuGic_SetPriorityTriggerType(&Intc , XPAR_FABRIC_UART_TAIL_RXD_ENDOFPACKET_INTR , 0xb0, 0x3);
	Status = XScuGic_Connect(&Intc, XPAR_FABRIC_UART_TAIL_RXD_ENDOFPACKET_INTR,
					(Xil_InterruptHandler)Uart_Tail_rx,(void*)&Intc);
	XScuGic_Enable(&Intc, XPAR_FABRIC_UART_TAIL_RXD_ENDOFPACKET_INTR);

	//定时器中断配置
	Drv_TTCTimer_Init(0, 20, Peripheral_Inquire);
	Drv_TTCTimer_Init(1, 20 , Data_upload);
	Drv_TTCTimer_Start(0);
	Drv_TTCTimer_Start(1);
	//中断异常处理函数
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler,
			&Intc);
	Xil_ExceptionEnable();

	g_bGetCpuCmd = 0;
	Data_Header();

	return 0;

}
