/*
 * uart_init.c

 *
 *  Created on: 2019��10��15��
 *      Author: asguj
 */
#include <stdio.h>
#include "uart_init.h"
#include "Process.h"
#include "xscugic.h"
#include "xparameters.h"
#include "xttcps.h"
#include "Driver_TTCTimer.h"
#include "sleep.h"
u32 *tx_address_PC = (u32*)UART_PC_ADDR_tx;
u32 *rx_address_PC = (u32*)UART_PC_ADDR_rx;
u32 *tx_address_HW = (u32*)UART_HW_ADDR_tx;
u32 *rx_address_HW = (u32*)UART_HW_ADDR_rx;
u32 *tx_address_KJG = (u32*)UART_KJG_ADDR_tx;
u32 *rx_address_KJG = (u32*)UART_KJG_ADDR_rx;
u32 *rx_address_Tail = (u32*)UART_TAIL_ADDR_rx;
u32 *tx_address_Tail = (u32*)UART_TAIL_ADDR_tx;
u32 *rx_address_ServoA = (u32*)UART_SERVOA_ADDR_rx;
u32 *tx_address_ServoA = (u32*)UART_SERVOA_ADDR_tx;
u32 *rx_address_ServoB = (u32*)UART_SERVOB_ADDR_rx;
u32 *tx_address_ServoB = (u32*)UART_SERVOB_ADDR_tx;
u32 *rx_address_Tracker = (u32*)UART_Tracker_ADDR_rx;
u32 *tx_address_Tracker = (u32*)UART_Tracker_ADDR_tx;


void uart_tx(u32 *addr, u16 *sendbuf , int len)
{
	for(int i=0;i<len;i++)
	{
		*addr = sendbuf[i];
	}
}


/*************************************************************
 * �������ƣ�Uart_Pc_rx��Uart_Pc_rx_part
 * �������ܣ�����PC��ָ���
 * ֡��n=���ݰ�����-1+4
 *************************************************************/
void Uart_Pc_rx(void)
{

	int i = 0;
	int j = 0;
	u8 head_index = 0;
	u8 empty = 0;
	u16 data_len = 0;
	u32 data_reg;


	memset(data_recv_PC , 0 ,127);
	/*��ȡfifo������128����*/
	empty = Xil_In32(UART_PC_ADDR_rx+4);
	empty = empty&0x1;
	while(!empty)
	{
		data_reg = Xil_In32(UART_PC_ADDR_rx);
		if(i<128)
		{
			data_recv_PC[i] = data_reg;
		}
		i++;
		empty = Xil_In32(UART_PC_ADDR_rx+4);
		empty = empty&0x1;
	}
	/*Ѱ�Ұ�ͷ����λ��*/
	while(j < 128)
		if((0x55 == data_recv_PC[j])&&(0xaa == data_recv_PC[j+1]))
		{
			DataRecv_PC.packet_Header0 = data_recv_PC[j];//55
			DataRecv_PC.packet_Header1 = data_recv_PC[j+1];//aa
			break;
		}
		else
		{
			j++;
		}
	head_index = j;//��¼��ͷ�����
	DataRecv_PC.packet_Header2 = data_recv_PC[head_index+2];//dc
	DataRecv_PC.packet_Len = data_recv_PC[head_index+3];
	data_len = DataRecv_PC.packet_Len & 0x3f;//֡��������ֽ�,��6Ϊ֡����,ȥ��3����ͷ��ʣ�µĳ���(����У��)
	DataRecv_PC.packet_ID = data_recv_PC[head_index+4];
	for(j = 0 ; j <(data_len-3) ; j++)
	{
		DataRecv_PC.packet_Data[j] = data_recv_PC[head_index+j+5];
	}
	DataRecv_PC.packet_Check = data_recv_PC[data_len+2];


	g_bGetCpuCmd = 1;

}

void Uart_Tracker_rx(void)
{
	int i = 0;
	int j = 0;
	u8 head_index = 0;
	u8 empty = 0;
//	u16 data_len = 0;
	u32 data_reg;
	empty = Xil_In32(UART_Tracker_ADDR_rx+4);
	empty = empty&0x1;
	while(!empty)
	{
		data_reg = Xil_In32(UART_Tracker_ADDR_rx);
		if(i<128)
		{
			data_recv_Tail[i] = data_reg;
		}
		i++;
		empty = Xil_In32(UART_TAIL_ADDR_rx+4);
		empty = empty&0x1;
	}
}


/*************************************************************
 * �������ƣ�Uart_Tail_rx
 * �������ܣ����ո��ٰ�����ݰ�
 *************************************************************/
void Uart_Tail_rx(void)
{
	int i = 0;
	int j = 0;
	u8 head_index = 0;
	u8 empty = 0;
//	u16 data_len = 0;
	u32 data_reg;
	memset(data_recv_Tail , 0 , 127);

	/*��ȡfifo������128����*/
	empty = Xil_In32(UART_TAIL_ADDR_rx+4);
	empty = empty&0x1;
	while(!empty)
	{
		data_reg = Xil_In32(UART_TAIL_ADDR_rx);
		if(i<128)
		{
			data_recv_Tail[i] = data_reg;
		}
		i++;
		empty = Xil_In32(UART_TAIL_ADDR_rx+4);
		empty = empty&0x1;
	}
	/*Ѱ��F1+F2��ͷ����λ��*/
	while(j < 128)
		if((0x55 == data_recv_Tail[j])&&(0xaa == data_recv_Tail[j+1])&&(0x22 == data_recv_Tail[j+4]))
		{

			image_state = data_recv_Tail[j+5]&0x07;//�Ӹ��ٰ巵�ص�F1�����ͼ��״̬
			Tail_state = data_recv_Tail[j+5]&0x18;//�Ӹ��ٰ巵�ص�F1����ø�����״̬
			for(int k = 0 ; k < 45 ; k++ )
			{
				Data_send_PC_F[k] = data_recv_Tail[j+k];

			}
			break;
		}
		else
		{
			j++;
		}
	head_index = j;//��¼��ͷ�����

}

/*************************************************************
 * �������ƣ�Uart_HW_rx
 * �������ܣ�����HW̽���������ݰ�
 *************************************************************/
void Uart_HW_rx(void)
{
	int i = 0;
	int j = 0;
	u8 head_index = 0;
	u8 empty = 0;
	u32 data_reg;
	u16 checksum = 0;
	int CommendCnt_HW = 0;

	memset( data_recv_HW , 0 ,127);
	/*��ȡfifo������128����*/
	empty = Xil_In32(UART_HW_ADDR_rx+4);
	empty = empty&0x1;
	while(!empty)
	{
		data_reg = Xil_In32(UART_HW_ADDR_rx);
		if(i<128)
		{
			data_recv_HW[i] = data_reg;
		}
		i++;
		empty = Xil_In32(UART_HW_ADDR_rx+4);
		empty = empty&0x1;
	}
	/*Ѱ�Ұ�ͷ����λ��*/
	while(j < 128)
		if((0xEB == data_recv_HW[j])&&(0x90 == data_recv_HW[j+1]))
		{
			Commend_HW_to_ZYNQ[0] = data_recv_HW[j];
			Commend_HW_to_ZYNQ[1] = data_recv_HW[j+1];
			CommendCnt_HW += 1;
			/*�յ������һ�����ݺ����ͼ��ת*/
			if (1 == CommendCnt_HW)
			{
				sleep(2);
				Commend_ZYNQ_to_HW[2] = 0x94;
				Commend_ZYNQ_to_HW[3] = 0x00;
				Commend_ZYNQ_to_HW[4] = 0x00;
				SendData_HW();
				Commend_ZYNQ_to_HW[2] = 0x93;
				Commend_ZYNQ_to_HW[3] = 0x00;
				Commend_ZYNQ_to_HW[4] = 0x00;
				SendData_HW();
				g_bInitComplete = 1;
			}
			break;
		}
		else
		{
			j++;
		}
	head_index = j;//��¼��ͷ�����

	for ( j = 0 ; j < 7 ; j++ )
	{
		Commend_HW_to_ZYNQ[j+2] = data_recv_HW[head_index+j+2];
		checksum += Commend_HW_to_ZYNQ[j+2];
	}

	Commend_HW_to_ZYNQ[9] = data_recv_HW[head_index+9];
	//������HW���������һ������,���н���
	if( checksum == Commend_HW_to_ZYNQ[9])
	{
		switch(Commend_HW_to_ZYNQ[7])//����䱶״̬
		{
		case(0x00):
				e_zoom_hw = 0x00;
		break;
		case(0x01):
				e_zoom_hw = 0x01;
		break;
		}


	}

}
/*************************************************************
 * �������ƣ�Uart_KJG_rx
 * �������ܣ�����KJG̽���������ݰ�
 *************************************************************/
void Uart_KJG_rx(void)
{
	//Ӳ��ԭ����ʱ����
}
/*************************************************************
 * �������ƣ�Uart_ServoA_rx
 * �������ܣ�����ServoA�����ݰ�
 * ˵        ����Yaw ��λͨ��
 *************************************************************/
void Uart_ServoA_rx(void)
{

	int empty = 0;
	int i = 0 ;
	int j = 0 ;
	u32 data_reg;
	u8 head_index;
	u16 checksum = 0;
	memset(data_recv_servo , 0 ,127);
	/*��ȡfifo������128����*/
	empty = Xil_In32(UART_SERVOA_ADDR_rx+4);
	empty = empty&0x1;
	while(!empty)
	{
		data_reg = Xil_In32(UART_SERVOA_ADDR_rx);
		if(i<128)
		{
			data_recv_servo[i] = data_reg;
		}
		i++;
		empty = Xil_In32(UART_SERVOA_ADDR_rx+4);
		empty = empty&0x1;
	}

	/*Ѱ�Ұ�ͷ����λ��*/
	while(j < 128)
		if((0x55 == data_recv_servo[j])&&(0xaa == data_recv_servo[j+1]))
		{
			Commend_ServoA_to_ZYNQ[0] = data_recv_servo[j];//55
			Commend_ServoA_to_ZYNQ[1] = data_recv_servo[j+1];//aa
			break;
		}
		else
		{
			j++;
		}
	head_index = j;//��¼��ͷ�����
	for ( i = 0 ; i < 29 ; i++ )
	{
		Commend_ServoA_to_ZYNQ[i+2] = data_recv_servo[head_index+2+i];
		checksum += Commend_ServoA_to_ZYNQ[i+2];
		checksum = checksum & 0xff;
	}
	Commend_ServoA_to_ZYNQ[31] = data_recv_servo[head_index+31];
	if( checksum == Commend_ServoA_to_ZYNQ[31])
	{

		/*�ŷ�����״̬,ֻͨ������ͨ����ȡ*/

		servo_state = Commend_ServoA_to_ZYNQ[3];//
		switch(servo_state)
		{
		case(0x05)://˫��ָ�򡪡��涯
				servo_state_upload = 0x03;
		case(0x00)://˫����С������ŷ�
				servo_state_upload = 0x00;
		case(0x0A)://˫�����������ֶ�
				servo_state_upload = 0x01;
		break;
		}
		yaw_angle_state = ((u16)Commend_ServoA_to_ZYNQ[6]<<8 & 0xff00) | (Commend_ServoA_to_ZYNQ[5] & 0x00ff);
		yaw_gyro_state = ((u16)Commend_ServoA_to_ZYNQ[10]<<8 & 0xff00) | (Commend_ServoA_to_ZYNQ[9] & 0x00ff);
		yaw_angle_state = (u16)(int)((float)(((yaw_angle_state/100)*65536)/360));
	}

}


/*************************************************************
 * �������ƣ�Uart_ServoB_rx
 * �������ܣ�����ServoB�����ݰ�
 * ˵        ����Pitch ����ͨ��
 *************************************************************/
void Uart_ServoB_rx(void)
{

	int empty = 0;
	int i = 0 ;
	int j = 0 ;
	u32 data_reg;
	u8 head_index;
	u16 checksum = 0;
	memset(data_recv_servo_B , 0 ,127);
	/*��ȡfifo������128����*/
	empty = Xil_In32(UART_SERVOB_ADDR_rx+4);
	empty = empty&0x1;
	while(!empty)
	{
		data_reg = Xil_In32(UART_SERVOB_ADDR_rx);
		if(i<128)
		{
			data_recv_servo_B[i] = data_reg;
		}
		i++;
		empty = Xil_In32(UART_SERVOB_ADDR_rx+4);
		empty = empty&0x1;
	}

	/*Ѱ�Ұ�ͷ����λ��*/
	while(j < 128)
		if((0x55 == data_recv_servo_B[j])&&(0xaa == data_recv_servo_B[j+1]))
		{
			Commend_ServoB_to_ZYNQ[0] = data_recv_servo_B[j];//55
			Commend_ServoB_to_ZYNQ[1] = data_recv_servo_B[j+1];//aa
			break;
		}
		else
		{
			j++;
		}
	head_index = j;//��¼��ͷ�����
	for ( i = 0 ; i < 29 ; i++ )
	{
		Commend_ServoB_to_ZYNQ[i+2] = data_recv_servo_B[head_index+2+i];
		checksum += Commend_ServoB_to_ZYNQ[i+2];
	}
	Commend_ServoB_to_ZYNQ[31] = data_recv_servo_B[head_index+31];
	if( checksum == Commend_ServoB_to_ZYNQ[31])
	{
		pitch_angle_state = ((u16)Commend_ServoB_to_ZYNQ[6]<<8 & 0xff00) | (Commend_ServoB_to_ZYNQ[5] & 0x00ff);
		pitch_gyro_state = ((u16)Commend_ServoB_to_ZYNQ[10]<<8 & 0xff00) | (Commend_ServoB_to_ZYNQ[9] & 0x00ff);
		pitch_angle_state = (u16)(int)((float)(((yaw_angle_state/100)*65536)/360));
	}

}
/*************************************************************
 * �������ƣ�SendData_HW
 * �������ܣ�����ָ����HW̽����
 *************************************************************/
void SendData_HW()
{
	int i = 0 ;
	u16 checkout_HW = 0;

	Commend_ZYNQ_to_HW[0] = 0xEB;
	Commend_ZYNQ_to_HW[1] = 0x90;
	Commend_ZYNQ_to_HW[5] = 0x00;
	Commend_ZYNQ_to_HW[6] = 0x00;


	/*����У��ͣ�3-7�ֽ�*/
	for( i = 0 ; i < 5 ; i++ )
	{
		checkout_HW += Commend_ZYNQ_to_HW[i+2];
	}
	Commend_ZYNQ_to_HW[7] = checkout_HW;
	for( i = 0 ; i < 8 ; i++ )
	{
		*tx_address_HW = Commend_ZYNQ_to_HW[i];
	}

//	memset(Commend_ZYNQ_to_HW)

}
/*************************************************************
 * �������ƣ�SendData_KJG
 * �������ܣ�����ָ�����ɼ���
 * ˵����Inq_flag�ǲ�ѯָ���־,0xff�����ѯָ��,0x00�������ָ��
 *************************************************************/
void SendData_KJG()
{
//	if (0xff == Inq_flag)
//	{
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header0;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header1_Inq;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header2_Inq;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.commend1_Inq;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_End;
//	}
//	else if (0x00 == Inq_flag)
//	{
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header0;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header1;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header2;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.commend1;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.commend2;
//		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_End;
//	}
	u16 checkout_KJG;
	checkout_KJG = 0;
	Commend_ZYNQ_to_KJG_new[0] = 0xff;
	Commend_ZYNQ_to_KJG_new[1] = 0x01;
	Commend_ZYNQ_to_KJG_new[2] = 0x00;
	//����У���
	for ( int i = 1 ; i < 6 ; i ++ )
	{
		checkout_KJG += Commend_ZYNQ_to_KJG_new[i];
	}
	Commend_ZYNQ_to_KJG_new[6] = checkout_KJG;
	//�������ɼ���
	for(int j = 0 ; j < 7 ; j++ )
	{
		*tx_address_KJG = Commend_ZYNQ_to_KJG_new[j];
	}
}
/*************************************************************
 * �������ƣ�SendData_Servo
 * �������ܣ�����ָ�����ŷ�
 *************************************************************/
void SendData_Servo()
{
	int i ;
	u8 CheckSumA = 0;
	u8 CheckSumB = 0;

	Commend_ZYNQ_to_ServoA[0] = 0x55;
	Commend_ZYNQ_to_ServoA[1] = 0xAA;
	Commend_ZYNQ_to_ServoA[2] = 0x1C;
	Commend_ZYNQ_to_ServoB[0] = 0x55;
	Commend_ZYNQ_to_ServoB[1] = 0xAA;
	Commend_ZYNQ_to_ServoB[2] = 0x1C;
	//����У���
	for( i = 2 ; i < 31 ; i++ )
	{
		CheckSumA += Commend_ZYNQ_to_ServoA[i];
		CheckSumB += Commend_ZYNQ_to_ServoB[i];
	}

	Commend_ZYNQ_to_ServoA[31] = CheckSumA;
	Commend_ZYNQ_to_ServoB[31] = CheckSumB;
	//�������ŷ�A��Bͨ��
	for( i = 0 ; i < 32 ; i++ )
	{
		*tx_address_ServoA = Commend_ZYNQ_to_ServoA[i];
		*tx_address_ServoB = Commend_ZYNQ_to_ServoB[i];
	}


}

/*************************************************************
 * �������ƣ�Data_upload
 * �������ܣ�20ms��ʱ�ϱ���PC
 * 		    �ϱ�����ΪB1+B2�ŷ�״̬����D2+D1��ѧ״̬��
 *************************************************************/
void Data_upload(void *arg)
{
	int i = 0;
	//�ŷ�
	DataSend_PC[0].packet_ID = Packet_B1_B2;
	DataSend_PC[0].packet_Len = 0x14;//����17�ֽ�+��ͷ3�ֽ�=20
	DataSend_PC[0].packet_Data[0] = servo_state_upload<<4;//�ŷ�״ֻ̬�Ӻ���ͨ����ȡ
	DataSend_PC[0].packet_Data[2] = (yaw_angle_state & 0xff00)>>8;
	DataSend_PC[0].packet_Data[3] = yaw_angle_state & 0x00ff;
	DataSend_PC[0].packet_Data[4] = (pitch_angle_state & 0xff00)>>8;
	DataSend_PC[0].packet_Data[5] = pitch_angle_state & 0x00ff;

	DataSend_PC[0].packet_Data[13] = (yaw_gyro_state & 0xff00)>>8;
	DataSend_PC[0].packet_Data[14] = yaw_gyro_state & 0x00ff;
	DataSend_PC[0].packet_Data[15] = (pitch_gyro_state & 0xff00)>>8;
	DataSend_PC[0].packet_Data[16] = pitch_gyro_state & 0x00ff;
	//��ѧ
	DataSend_PC[1].packet_ID = Packet_D2_D1;
	DataSend_PC[1].packet_Len = 0x14;//����17�ֽ�+��ͷ3�ֽ�=20
	DataSend_PC[1].packet_Data[5] = DataSend_PC[1].packet_Data[5]|( image_state & 0x07);//����״̬
	DataSend_PC[1].packet_Data[5] = DataSend_PC[1].packet_Data[5]|(( e_zoom_hw & 0x03 )<<5);//����䱶
	SendData_BDF(tx_address_Tracker);

	SendData_BDF(tx_address_PC);
	//�ϱ����ݺ�����
	for ( i = 0 ; i < 3 ; i++ )
	{
		memset(DataSend_PC[i].packet_Data , 0 , 29);
		DataSend_PC[i].packet_Check = 0x00;
	}


	Drv_TTCTimer_ClearIntr(arg);
}


/*************************************************************
 * �������ƣ�SendData_BDF
 * �������ܣ�����BDF����PC
 * 		     ����BD�������ٰ�
 *************************************************************/
void SendData_BDF( u32 *addr_send_BD )
{
	int i;
	u8 CheckOut_B = 0;
	u8 Packet_Len_B = 0;
	u8 CheckOut_D = 0;
	u8 Packet_Len_D = 0;
	//����B������
	*addr_send_BD = DataSend_PC[0].packet_Header0;
	*addr_send_BD = DataSend_PC[0].packet_Header1;
	*addr_send_BD = DataSend_PC[0].packet_Header2;
	*addr_send_BD = DataSend_PC[0].packet_Len;
	*addr_send_BD = DataSend_PC[0].packet_ID;
	Packet_Len_B = DataSend_PC[0].packet_Len & 0x3F;//0011 1111
	CheckOut_B = DataSend_PC[0].packet_Len ^DataSend_PC[0].packet_ID;
	for(i = 0 ; i <(Packet_Len_B-3) ; i++)
	{
		*addr_send_BD = DataSend_PC[0].packet_Data[i];
		CheckOut_B = CheckOut_B^DataSend_PC[0].packet_Data[i];
	}
	DataSend_PC[0].packet_Check = CheckOut_B;
	*addr_send_BD = DataSend_PC[0].packet_Check;
	//����D������
	*addr_send_BD = DataSend_PC[1].packet_Header0;
	*addr_send_BD = DataSend_PC[1].packet_Header1;
	*addr_send_BD = DataSend_PC[1].packet_Header2;
	*addr_send_BD = DataSend_PC[1].packet_Len;
	*addr_send_BD = DataSend_PC[1].packet_ID;
	Packet_Len_D = DataSend_PC[1].packet_Len & 0x3F;//0011 1111
	CheckOut_D = DataSend_PC[1].packet_Len ^DataSend_PC[1].packet_ID;
	for(i = 0 ; i <(Packet_Len_D-3) ; i++)
	{
		*addr_send_BD = DataSend_PC[1].packet_Data[i];
		CheckOut_D = CheckOut_D^DataSend_PC[1].packet_Data[i];
	}
	DataSend_PC[1].packet_Check = CheckOut_D;
	*addr_send_BD = DataSend_PC[1].packet_Check;
	/*������͵�ַ����λ������������F��*/
	if(tx_address_PC == addr_send_BD)//����λ������B+D+F������
	{

		for(i = 0 ; i < 45 ; i++ )
		{
			*addr_send_BD = Data_send_PC_F[i];
		}
	}

}

