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
	switch(DataRecv_PC.packet_ID)
	{
	/*������ָ��*/
		case(Packet_E1):
		case(Packet_E2):
			Commend_Tail_analysis();
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
	u16 Yaw_speed;
	u16 Pitch_speed;
	float Yaw_angle_temp;
	float Pitch_angle_temp;
	memset(Commend_ZYNQ_to_ServoA , 0 , 31);
	memset(Commend_ZYNQ_to_ServoB , 0 , 31);
	Servo_state = DataRecv_PC.packet_Data[0] & 0x0F;// 4bit 0~3 0000 1111
	/*ServoA�Ǻ���ָ�ServoB�Ǹ���ָ������3λΪ����״̬��ָ����5/6λ�����ٶ���9/10λ*/
	/*������λ��������ָ��,DataRecv_PC.packet_Data����Ŷ�ӦЭ�����ֽ����-1*/
		switch(Servo_state)
		{
		case(0x00)://�ر��ŷ�__˫�����
				Commend_ZYNQ_to_ServoA[3] = 0x00;
		break;
		case(0x04)://������λ__ָ��0��

				Commend_ZYNQ_to_ServoA[3] = 0x05;
				Commend_ZYNQ_to_ServoA[5] = 0x00;
				Commend_ZYNQ_to_ServoA[6] = 0x00;

				Commend_ZYNQ_to_ServoB[3] = 0x05;
				Commend_ZYNQ_to_ServoB[5] = 0x00;
				Commend_ZYNQ_to_ServoB[6] = 0x00;
		break;

		case(0x01)://�ֶ�__�ŷ�����
				Yaw_speed = ( (DataRecv_PC.packet_Data[1]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[2] & 0x00ff );
				Pitch_speed = ( (DataRecv_PC.packet_Data[3]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[4] & 0x00ff );

				Commend_ZYNQ_to_ServoA[3] = 0x0A;
				Commend_ZYNQ_to_ServoA[9] = (u16)Yaw_speed & 0x00ff;
				Commend_ZYNQ_to_ServoA[10] = ((u16)Yaw_speed >> 8)& 0x00ff;

				Commend_ZYNQ_to_ServoB[3] = 0x0A;
				Commend_ZYNQ_to_ServoB[9] = (u16)Pitch_speed & 0x00ff;
				Commend_ZYNQ_to_ServoB[10] = ((u16)Pitch_speed >> 8)& 0x00ff;
				break;
		case(0x03)://�涯__�ŷ�ָ��

				//PCָ���ȸ�8���8
				Yaw_angle = ( (DataRecv_PC.packet_Data[5]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[6] & 0x00ff );
				Pitch_angle = ( (DataRecv_PC.packet_Data[7]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[8] & 0x00ff );
				//����Ƕ�ֵ
				Yaw_angle_temp = (float)(Yaw_angle*360)/65536;
				Pitch_angle_temp = (float)(Pitch_angle*360)/65536;
				//����С������λ
				Yaw_angle_temp = ( (float)( (int)( (Yaw_angle_temp+0.005)*100 ) ) )/100;
				Pitch_angle_temp = ( (float)( (int)( (Pitch_angle_temp+0.005)*100 ) ) )/100;

//				Yaw_angle_temp = (int)(Yaw_angle_temp+0.5);
//				Pitch_angle_temp = (int)(Pitch_angle_temp+0.5);

				//�ŷ������ȵ�8���8
				Commend_ZYNQ_to_ServoA[3] = 0x05;
				Commend_ZYNQ_to_ServoA[5] = (u16)(Yaw_angle_temp*100) & 0x00ff;
				Commend_ZYNQ_to_ServoA[6] = ((u16)(Yaw_angle_temp*100) >> 8)& 0x00ff;

				Commend_ZYNQ_to_ServoB[3] = 0x05;
				Commend_ZYNQ_to_ServoB[5] = (u16)(Pitch_angle_temp*100) & 0x00ff;
				Commend_ZYNQ_to_ServoB[6] = ((u16)(Pitch_angle_temp*100) >> 8)& 0x00ff;
				break;
		default:
				Commend_ZYNQ_to_ServoA[3] = 0x00;
				Commend_ZYNQ_to_ServoB[3] = 0x00;
				break;
		}

		SendData_Servo();


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
	Commend_ZYNQ_to_KJG.commend1 = 0x00;
	Commend_ZYNQ_to_KJG.commend2 = 0x00;

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
				Commend_ZYNQ_to_KJG.commend1 = 0x07;
				Commend_ZYNQ_to_KJG.commend2 = 0x00;
			}
		break;
		case(0x08)://�ӳ�+,
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x84;
				}
				else if (0x01 == C1_imager_choose)
				{
					Commend_ZYNQ_to_KJG.commend1 = 0x07;
					Commend_ZYNQ_to_KJG.commend2 = 0x02;
				}
		break;
		case(0x09)://�ӳ�-
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x85;
				}
				else if (0x01 == C1_imager_choose)
				{
					Commend_ZYNQ_to_KJG.commend1 = 0x07;
					Commend_ZYNQ_to_KJG.commend2 = 0x03;
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
					Commend_ZYNQ_to_HW[2] = 0x6E;
				}
				else if (0x01 == C1_imager_choose)
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
		default://Ĭ���޶���
			Commend_ZYNQ_to_HW[2] = 0x00;
			Commend_ZYNQ_to_KJG.commend1 = 0x07;
			Commend_ZYNQ_to_KJG.commend2 = 0x00;
		break;
		}

	//�ַ����������
	if (0x02 == C1_imager_choose)
	{
		SendData_HW();
	}
	else if(0x01 == C1_imager_choose)
	{
		SendData_KJG(0x00);
	}

}
/*************************************************************
 * �������ƣ�Commend_Tail_analysis
 * �������ܣ�����������ָ��(ֱ��ת��)
 *************************************************************/
void Commend_Tail_analysis(void)
{

	int j = 0;
	u8 PC_Packet_Len = 0;
	u8 image_choose_commend = 0;
	memset(sendbuf , 0 , 29);
	//����ת�浽sendbuf
	sendbuf[0] = DataRecv_PC.packet_Header0;
	sendbuf[1] = DataRecv_PC.packet_Header1;
	sendbuf[2] = DataRecv_PC.packet_Header2;
	sendbuf[3] = DataRecv_PC.packet_Len;
	PC_Packet_Len = sendbuf[3]&0x3f;
	sendbuf[4] = DataRecv_PC.packet_ID;
	for(j = 0 ; j <(PC_Packet_Len-3) ; j++)
	{
		sendbuf[5+j] = DataRecv_PC.packet_Data[j];
	}
	sendbuf[PC_Packet_Len+2] = DataRecv_PC.packet_Check;
	image_choose_commend = sendbuf[5]&0x07;
//	switch(image_choose_commend)
//	{
//	case(0x01)://kjg
//		image_state = 0x00;
//	break;
//	case(0x02)://hw
//		image_state = 0x01;
//	break;
//	}

	//ֱ��ת������
	uart_tx(tx_address_PC , sendbuf , PC_Packet_Len+3);

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
	Drv_TTCTimer_ClearIntr(arg);


}