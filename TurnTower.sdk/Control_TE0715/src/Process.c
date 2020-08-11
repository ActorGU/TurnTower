/*
 * Process.c
 *
 *  Created on: 2020��3��2��
 *      Author: asguj
 */
#include <stdio.h>
#include "uart_init.h"
#include "Process.h"


/*************************************************************
 * �������ƣ�Data_analysis
 * �������ܣ�����PC��ָ��
 *************************************************************/
void Data_analysis(void)
{

	int i = 0;
	g_bCmdAnalysisComplete = 0;
	switch(DataRecv_PC.packet_ID)
	{
	/*������ָ��*/
		case(Packet_E1):
		case(Packet_E2):
			Commend_Tracker_analysis();
		break;

	/*��ѧָ��*/
		case(Packet_C1):
		case(Packet_C2):
			Commend_Optic_analysis();
		break;

	/*�ŷ�ָ��*/
		case(Packet_A1):
		case(Packet_A2):
			Commend_Servo_analysis();
		break;
	}

	/*������PCָ�������*/
	DataRecv_PC.packet_Header0 = 0x00;
	DataRecv_PC.packet_Header1 = 0x00;
	DataRecv_PC.packet_Header2 = 0x00;
	DataRecv_PC.packet_Len = 0x00;
	DataRecv_PC.packet_ID = 0x00;
	for (i = 0 ; i < 30 ; i++)
	{
		DataRecv_PC.packet_Data[i] = 0x00;
	}
	DataRecv_PC.packet_Check = 0x00;

	g_bCmdAnalysisComplete = 1;
}

/*************************************************************
 * �������ƣ�Commend_Servo_analysis
 * �������ܣ������ŷ�ָ��
 *************************************************************/
void Commend_Servo_analysis(void)
{

//	int i = 0;
	u8 Servo_state;
	u16 Yaw_angle;//����
	u16 Pitch_angle;//����
//	u16 Yaw_speed;
//	u16 Pitch_speed;
//	float Yaw_angle_temp;
//	float Pitch_angle_temp;

	memset(Commend_ZYNQ_to_ServoA , 0 , 31);
	memset(Commend_ZYNQ_to_ServoB , 0 , 31);
	Servo_state = DataRecv_PC.packet_Data[0] & 0x0F;// 4bit 0~3 0000 1111
	/*ServoA�Ǻ���ָ�ServoB�Ǹ���ָ������3λΪ����״̬��ָ����5/6λ�����ٶ���9/10λ*/
	/*������λ��������ָ��,DataRecv_PC.packet_Data����Ŷ�ӦЭ�����ֽ����-1*/
		switch(Servo_state)
		{
		case(0x00)://�ر��ŷ�__����״̬
				g_uZynqCurrentState = ZYNQ_STATE_OFFSERVO;
				Commend_ZYNQ_to_ServoA[3] = 0x00;

		break;
		case(0x04)://������λ__ָ��0��
				if(ZYNQ_STATE_ATRACE != g_uZynqCurrentState)//������Ǹ���״̬,����Ӧָ��
				{
					g_uZynqCurrentState = ZYNQ_STATE_IDLE;
//					Commend_ZYNQ_to_ServoA[3] = 0x05;
//					Commend_ZYNQ_to_ServoA[5] = 0x00;
//					Commend_ZYNQ_to_ServoA[6] = 0x00;
//
//					Commend_ZYNQ_to_ServoB[3] = 0x05;
//					Commend_ZYNQ_to_ServoB[5] = 0x00;
//					Commend_ZYNQ_to_ServoB[6] = 0x00;
					Yaw_angle_temp = 0;
					Pitch_angle_temp = 0;
				}
				else if(ZYNQ_STATE_ATRACE == g_uZynqCurrentState)
				{
					g_uZynqCurrentState = ZYNQ_STATE_SERCH;//����Ǹ���״̬,���л�����״̬
				}

		break;

		case(0x01)://�ֶ�__�ŷ�����
				g_uZynqCurrentState = ZYNQ_STATE_SERCH;
				Yaw_speed = ( (DataRecv_PC.packet_Data[1]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[2] & 0x00ff );
				Pitch_speed = ( (DataRecv_PC.packet_Data[3]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[4] & 0x00ff );

//				Commend_ZYNQ_to_ServoA[3] = 0x0A;
//				Commend_ZYNQ_to_ServoA[9] = (u16)Yaw_speed & 0x00ff;
//				Commend_ZYNQ_to_ServoA[10] = ((u16)Yaw_speed >> 8)& 0x00ff;
//
//				Commend_ZYNQ_to_ServoB[3] = 0x0A;
//				Commend_ZYNQ_to_ServoB[9] = (u16)Pitch_speed & 0x00ff;
//				Commend_ZYNQ_to_ServoB[10] = ((u16)Pitch_speed >> 8)& 0x00ff;
				break;
		case(0x03)://�涯__�ŷ�ָ��
				g_uZynqCurrentState = ZYNQ_STATE_IDLE;
				//PCָ���ȸ�8���8
				Yaw_angle = ( (DataRecv_PC.packet_Data[5]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[6] & 0x00ff );
				Pitch_angle = ( (DataRecv_PC.packet_Data[7]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[8] & 0x00ff );
				//����Ƕ�ֵ
				Yaw_angle_temp = (float)(Yaw_angle*360)/65536;
				Pitch_angle_temp = (float)(Pitch_angle*360)/65536;
				//����С������λ
				Yaw_angle_temp = ( (float)( (int)( (Yaw_angle_temp+0.005)*100 ) ) )/100;
				Pitch_angle_temp = ( (float)( (int)( (Pitch_angle_temp+0.005)*100 ) ) )/100;
				if ( Pitch_angle_temp > 180 )
				{
					Pitch_angle_temp = Pitch_angle_temp - 360;
				}
//				//�ŷ������ȵ�8���8
//				Commend_ZYNQ_to_ServoA[3] = 0x05;
//				Commend_ZYNQ_to_ServoA[5] = (u16)(Yaw_angle_temp*100) & 0x00ff;
//				Commend_ZYNQ_to_ServoA[6] = ((u16)(Yaw_angle_temp*100) >> 8)& 0x00ff;
//
//				Commend_ZYNQ_to_ServoB[3] = 0x05;
//				Commend_ZYNQ_to_ServoB[5] = (IN16)(Pitch_angle_temp*100) & 0x00ff;
//				Commend_ZYNQ_to_ServoB[6] = (((IN16)(Pitch_angle_temp*100) >> 8)& 0x00ff);
		break;


		case(0x06)://�ػ�ָ��
//				g_uServoNewState = SERVO_STATE_ATRACE_AB;
				if(ZYNQ_STATE_ATRACE != g_uZynqCurrentState)//�Ǹ���״̬��,�ٶ�����,ָ��Ƕȱ��ֲ���
				{
					g_uZynqCurrentState = ZYNQ_STATE_LOCK;
					Yaw_speed = 0;
					Pitch_speed = 0;
				}
		break;


		}

//			g_bServoCommendUpdate = 1;




}

/*************************************************************
* �������ƣ�Commend_Optic_analysis
* �������ܣ�������ѧָ��
*************************************************************/
void Commend_Optic_analysis(void)
{
	//			����̽����    	0x02
	//			�ɼ���̽����	0x01
	u8 C1_imager_choose = 0x00;
//	u8 C1_commend_para;
	u8 C1_commend1_low = 0x00;
	u8 C1_commend1_high = 0x00;
	u8 C1_commend1 = 0x00;
	memset(Commend_ZYNQ_to_HW , 0 , 7);
	memset(Commend_ZYNQ_to_KJG_new , 0 , 6);
//	Commend_ZYNQ_to_KJG.commend1 = 0x00;
//	Commend_ZYNQ_to_KJG.commend2 = 0x00;

	C1_imager_choose = DataRecv_PC.packet_Data[1] & 0x07;//3bit 0~2 0000 0111
//	C1_commend_para = (DataRecv_PC.packet_Data[1] & 0x38)>>3;//3bit 3~5  0011 1000
	C1_commend1_low = (DataRecv_PC.packet_Data[1] & 0xC0)>>6;//2bit 	 1100 0000
	C1_commend1_high = DataRecv_PC.packet_Data[0] & 0x1F;//5bit 	 0001 1111
	C1_commend1 = C1_commend1_high<<2|C1_commend1_low;//2+5=7bit 6~12

		switch(C1_commend1)
		{
		case(0x00)://�޶���
			if (0x02 == C1_imager_choose)
			{
				Commend_ZYNQ_to_HW[2] = 0x86;
			}
			else if(0x01 == C1_imager_choose)
			{
//				Commend_ZYNQ_to_KJG.commend1 = 0x07;
//				Commend_ZYNQ_to_KJG.commend2 = 0x00;
				Commend_ZYNQ_to_KJG_new[3] = 0x00;
			}
		break;
		case(0x08)://�ӳ�+,
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x85;
				}
				else if (0x01 == C1_imager_choose)
				{
//					Commend_ZYNQ_to_KJG.commend1 = 0x07;
//					Commend_ZYNQ_to_KJG.commend2 = 0x02;
					Commend_ZYNQ_to_KJG_new[3] = 0x40;
				}
		break;
		case(0x09)://�ӳ�-
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x84;
				}
				else if (0x01 == C1_imager_choose)
				{
//					Commend_ZYNQ_to_KJG.commend1 = 0x07;
//					Commend_ZYNQ_to_KJG.commend2 = 0x03;
					Commend_ZYNQ_to_KJG_new[3] = 0x20;
				}
		break;
		case(0x02)://����+
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x74;
				}
				else if (0x01 == C1_imager_choose)
				{
//					Commend_ZYNQ_to_KJG.commend1 = 0x07;
//					Commend_ZYNQ_to_KJG.commend2 = 0x03;
				}
		break;
		case(0x03)://����-
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x75;
				}
				else if (0x01 == C1_imager_choose)
				{
//					Commend_ZYNQ_to_KJG.commend1 = 0x07;
//					Commend_ZYNQ_to_KJG.commend2 = 0x03;
				}
		break;
		case(0x0C)://�ڲ��Ǿ���У��
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x6e;
				}
				else if (0x01 == C1_imager_choose)
				{
				}
		break;
		case(0x0d)://�ⲿ�Ǿ���У��
				if(0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x6f;
				}
				else if(0x01 == C1_imager_choose)
				{

				}
		break;
		case(0x0E)://�ڰ����л�
		case(0x0F):
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x71;
				}
				else if (0x01 == C1_imager_choose)
				{
				}
		break;
		case(0x0A)://����+
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x87;
				}
				else if (0x01 == C1_imager_choose)
				{

				}
		break;
		case(0x0B)://����-
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x88;
				}
				else if (0x01 == C1_imager_choose)
				{

				}
		break;
		default://Ĭ���޶���
			Commend_ZYNQ_to_HW[2] = 0x00;
			Commend_ZYNQ_to_KJG_new[3] = 0x00;
//			Commend_ZYNQ_to_KJG.commend1 = 0x07;
//			Commend_ZYNQ_to_KJG.commend2 = 0x00;
		break;
		}

	//�ַ����������
	if (0x02 == C1_imager_choose)
	{
		SendData_HW();
	}
	else if(0x01 == C1_imager_choose)
	{
		SendData_KJG();
	}

}
/*************************************************************
 * �������ƣ�Commend_Tracker_analysis
 * �������ܣ�����������ָ��(ֱ��ת��)
 *************************************************************/
void Commend_Tracker_analysis(void)
{

	int j = 0;
	int i = 0;
	u8 PC_Packet_Len = 0;
	u8 image_choose_commend = 0;
	memset(Commend_ZYNQ_to_Tracker , 0 , 29);
	//����ת�浽Commend_ZYNQ_to_Tracker
	Commend_ZYNQ_to_Tracker[0] = DataRecv_PC.packet_Header0;
	Commend_ZYNQ_to_Tracker[1] = DataRecv_PC.packet_Header1;
	Commend_ZYNQ_to_Tracker[2] = DataRecv_PC.packet_Header2;
	Commend_ZYNQ_to_Tracker[3] = DataRecv_PC.packet_Len;
	PC_Packet_Len = Commend_ZYNQ_to_Tracker[3]&0x3f;
	Commend_ZYNQ_to_Tracker[4] = DataRecv_PC.packet_ID;
	for(j = 0 ; j <(PC_Packet_Len-3) ; j++)
	{
		Commend_ZYNQ_to_Tracker[5+j] = DataRecv_PC.packet_Data[j];
	}
	Commend_ZYNQ_to_Tracker[PC_Packet_Len+2] = DataRecv_PC.packet_Check;
	image_choose_commend = Commend_ZYNQ_to_Tracker[5]&0x07;


	//ֱ��ת������
//	uart_tx(tx_address_Tracker , Commend_ZYNQ_to_Tracker , PC_Packet_Len+3);
	for ( i = 0 ; i < PC_Packet_Len+3 ; i++ )
	{
		*tx_address_Tracker = Commend_ZYNQ_to_Tracker[i];
	}

}

/*************************************************************
 * �������ƣ�Peripheral_Inquire
 * �������ܣ�20ms��ʱ��ѯ������״̬
 * 		  ����,�ɼ���,�ŷ�(�ŷ��ֻ�Ϊ��ʱ�ϱ�ģʽ,���ò�ѯ)
 *************************************************************/
void Peripheral_Inquire(void *arg)
{

	Commend_ZYNQ_to_HW[2] = 0x00;
	Commend_ZYNQ_to_HW[3] = 0x00;
	Commend_ZYNQ_to_HW[4] = 0x00;
	SendData_HW();

//	Commend_ZYNQ_to_KJG.packet_Header1_Inq = 0x09;
//	Commend_ZYNQ_to_KJG.packet_Header2_Inq = 0x04;
//	Commend_ZYNQ_to_KJG.commend1_Inq = 0x47;
//	SendData_KJG(0xff);//��ѯָ��
//	Drv_TTCTimer_ClearIntr(arg);


}


void SelfCheck(void)
{

}
