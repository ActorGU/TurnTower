/*
 * header_init.c
 *
 *  Created on: 2019��10��23��
 *      Author: asguj
 */
#include "header_init.h"
#include "uart_init.h"
#include "process.h"
#include "sleep.h"
void Data_Header()
{
//	u8 data_close[11];

	/*�������ݰ���ͷ*/
	memset(Commend_ZYNQ_to_HW , 0 , 7);
	Commend_ZYNQ_to_HW[0] = 0xEB;
	Commend_ZYNQ_to_HW[1] = 0x90;
	CommendCnt_HW = 0;//����һ���������ж��Ƿ��Ǻ�����������ĵ�һ������,�Ա����ڶԺ����������ͼ��ķ�ת

	/*PC���ݰ���ͷ*/
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

	/*�ŷ����ݰ���ͷ*/
	memset(Commend_ZYNQ_to_ServoA , 0 , 31);
	memset(Commend_ZYNQ_to_ServoB , 0 , 31);
	//����
	Commend_ZYNQ_to_ServoA[0] = 0x55;
	Commend_ZYNQ_to_ServoA[1] = 0xAA;
	Commend_ZYNQ_to_ServoA[2] = 0x1C;
	//����
	Commend_ZYNQ_to_ServoB[0] = 0x55;
	Commend_ZYNQ_to_ServoB[1] = 0xAA;
	Commend_ZYNQ_to_ServoB[2] = 0x1C;


	/*ZYNQ�ڲ�״̬��ʼ��*/
	g_uZynqCurrentState = ZYNQ_STATE_IDLE;//�ϵ���ŷ�Ϊ����״̬
	g_bServoCommendUpdate = 0;
	Yaw_angle_temp = 0;//�ϵ���ŷ�ָ����λ
	Pitch_angle_temp = 0;


}
void Commend_once(void)
{

	u8 tail_data_close[11] = {0x55,0xAA,0xDC,0x08,0x1D,0x07,0x00,0x00,0x00,0x00,0x12};
	//	�ϵ������ٰ巢ָ��ص�����γ��������ʾ
	for(int i = 0 ; i < 11 ; i++ )
	{
		*tx_address_Tracker = tail_data_close[i];
	}
}
