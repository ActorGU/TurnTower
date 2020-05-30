/*
 * header_init.c
 *
 *  Created on: 2019年10月23日
 *      Author: asguj
 */
#include "header_init.h"

void Data_Header()
{

	/*红外数据包包头*/
	memset(Commend_ZYNQ_to_HW , 0 , 7);
	Commend_ZYNQ_to_HW[0] = 0xEB;
	Commend_ZYNQ_to_HW[1] = 0x90;


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

	/*可见光数据包包头*/
	Commend_ZYNQ_to_KJG.packet_Header0 = 0x81;
	Commend_ZYNQ_to_KJG.packet_Header1 = 0x01;
	Commend_ZYNQ_to_KJG.packet_Header2 = 0x04;
	Commend_ZYNQ_to_KJG.packet_End = 0xFF;

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


}
