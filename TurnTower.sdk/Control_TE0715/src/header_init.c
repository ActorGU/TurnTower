/*
 * header_init.c
 *
 *  Created on: 2019年10月23日
 *      Author: asguj
 */
#include "header_init.h"
#include "uart_init.h"
#include "process.h"
#include "sleep.h"
void Data_Header()
{
//	u8 data_close[11];

	/*红外数据包包头*/
	memset(Commend_ZYNQ_to_HW , 0 , 7);
	Commend_ZYNQ_to_HW[0] = 0xEB;
	Commend_ZYNQ_to_HW[1] = 0x90;
	CommendCnt_HW = 0;//定义一个变量来判断是否是红外相机反馈的第一包数据,以便用于对红外相机进行图像的翻转

	/*PC数据包包头*/
	DataSend_PC[0].packet_Header0 = 0x55;
	DataSend_PC[0].packet_Header1 = 0xAA;
	DataSend_PC[0].packet_Header2 = 0xDC;
	DataSend_PC[0].packet_Len = 0x00;
	DataSend_PC[0].packet_ID = 0x00;

	DataSend_PC[1].packet_Header0 = 0x55;
	DataSend_PC[1].packet_Header1 = 0xAA;
	DataSend_PC[1].packet_Header2 = 0xDC;
	DataSend_PC[1].packet_Len = 0x00;
	DataSend_PC[1].packet_ID = 0x00;

	DataSend_PC[2].packet_Header0 = 0x55;
	DataSend_PC[2].packet_Header1 = 0xAA;
	DataSend_PC[2].packet_Header2 = 0xDC;
	DataSend_PC[2].packet_Len = 0x00;
	DataSend_PC[2].packet_ID = 0x00;

	for ( int i = 0 ; i < 3 ; i++ )
	{
		memset(DataSend_PC[i].packet_Data , 0 , 29);
		DataSend_PC[i].packet_Check = 0x00;
	}

	/*伺服数据包包头*/
	memset(Commend_ZYNQ_to_ServoA , 0 , 31);
	memset(Commend_ZYNQ_to_ServoB , 0 , 31);
	//航向
	Commend_ZYNQ_to_ServoA[0] = 0x55;
	Commend_ZYNQ_to_ServoA[1] = 0xAA;
	Commend_ZYNQ_to_ServoA[2] = 0x1C;
	//俯仰
	Commend_ZYNQ_to_ServoB[0] = 0x55;
	Commend_ZYNQ_to_ServoB[1] = 0xAA;
	Commend_ZYNQ_to_ServoB[2] = 0x1C;


	/*ZYNQ内部状态初始化*/
	g_uZynqCurrentState = ZYNQ_STATE_IDLE;//上电后伺服为空闲状态
	g_bServoCommendUpdate = 0;
	Yaw_angle_temp = 0;//上电后伺服指向零位
	Pitch_angle_temp = 0;


}
void Commend_once(void)
{

	u8 tail_data_close[11] = {0x55,0xAA,0xDC,0x08,0x1D,0x07,0x00,0x00,0x00,0x00,0x12};
	//	上电后给跟踪板发指令关掉跟经纬度数据显示
	for(int i = 0 ; i < 11 ; i++ )
	{
		*tx_address_Tracker = tail_data_close[i];
	}
}
