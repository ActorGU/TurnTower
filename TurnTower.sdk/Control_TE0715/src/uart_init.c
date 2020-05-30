/*
 * uart_init.c

 *
 *  Created on: 2019年10月15日
 *      Author: asguj
 */
#include <stdio.h>
#include "uart_init.h"
#include "Process.h"
#include "xscugic.h"
#include "xparameters.h"
#include "xttcps.h"
#include "Driver_TTCTimer.h"
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


void uart_tx(u32 *addr, u16 *sendbuf , int len)
{
	for(int i=0;i<len;i++)
	{
		*addr = sendbuf[i];
	}
}


/*************************************************************
 * 函数名称：Uart_Pc_rx、Uart_Pc_rx_part
 * 函数功能：接收PC的指令包
 * 帧长n=数据包长度-1+4
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
	/*读取fifo中所有128个数*/
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
	/*寻找包头所在位置*/
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
	head_index = j;//记录包头的序号
	DataRecv_PC.packet_Header2 = data_recv_PC[head_index+2];//dc
	DataRecv_PC.packet_Len = data_recv_PC[head_index+3];
	data_len = DataRecv_PC.packet_Len & 0x3f;//帧长与计数字节,低6为帧长度,去掉3个包头后剩下的长度(包括校验)
	DataRecv_PC.packet_ID = data_recv_PC[head_index+4];
	for(j = 0 ; j <(data_len-3) ; j++)
	{
		DataRecv_PC.packet_Data[j] = data_recv_PC[head_index+j+5];
	}
	DataRecv_PC.packet_Check = data_recv_PC[data_len+2];


	g_bGetCpuCmd = 1;

}

/*************************************************************
 * 函数名称：Uart_Tail_rx
 * 函数功能：接收跟踪板的数据包
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

	/*读取fifo中所有128个数*/
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
	/*寻找F1+F2包头所在位置*/
	while(j < 128)
		if((0x55 == data_recv_Tail[j])&&(0xaa == data_recv_Tail[j+1])&&(0x22 == data_recv_Tail[j+4]))
		{
//			DataSend_PC[2].packet_Data[12] = data_recv_Tail[j+17];
//			DataSend_PC[2].packet_Data[13] = data_recv_Tail[j+18];
//			DataSend_PC[2].packet_Data[14] = data_recv_Tail[j+19];
//			DataSend_PC[2].packet_Data[15] = data_recv_Tail[j+20];
			image_state = data_recv_Tail[j+5]&0x07;
			for(int k = 0 ; k < 45 ; k++ )
			{
//				DataSend_PC[2].packet_Data[k] = data_recv_Tail[j+k+5];
				Data_send_PC_F[k] = data_recv_Tail[j+k];

			}
			break;
		}
		else
		{
			j++;
		}
	head_index = j;//记录包头的序号

}

/*************************************************************
 * 函数名称：Uart_HW_rx
 * 函数功能：接收HW探测器的数据包
 *************************************************************/
void Uart_HW_rx(void)
{
	int i = 0;
	int j = 0;
	u8 head_index = 0;
	u8 empty = 0;
	u32 data_reg;
	u16 checksum = 0;

	memset( data_recv_HW , 0 ,127);
	/*读取fifo中所有128个数*/
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
	/*寻找包头所在位置*/
	while(j < 128)
		if((0xEB == data_recv_PC[j])&&(0x90 == data_recv_PC[j+1]))
		{
			Commend_HW_to_ZYNQ[0] = data_recv_HW[j];
			Commend_HW_to_ZYNQ[1] = data_recv_HW[j+1];
			break;
		}
		else
		{
			j++;
		}
	head_index = j;//记录包头的序号
	for ( j = 0 ; j < 7 ; j++ )
	{
		Commend_HW_to_ZYNQ[j+2] = data_recv_HW[head_index+j+2];
		checksum += Commend_HW_to_ZYNQ[j+2];
	}

	Commend_HW_to_ZYNQ[9] = data_recv_HW[head_index+9];
	//接收完HW相机反馈的一包数据,进行解析
	if( checksum == Commend_HW_to_ZYNQ[9])
	{
		switch(Commend_HW_to_ZYNQ[7])//红外变倍状态
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
 * 函数名称：Uart_KJG_rx
 * 函数功能：接收KJG探测器的数据包
 *************************************************************/
void Uart_KJG_rx(void)
{
	//硬件原因暂时不加
}
/*************************************************************
 * 函数名称：Uart_ServoA_rx
 * 函数功能：接收ServoA的数据包
 * 说        明：Yaw 方位通道
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
	/*读取fifo中所有128个数*/
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

	/*寻找包头所在位置*/
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
	head_index = j;//记录包头的序号
	for ( i = 0 ; i < 29 ; i++ )
	{
		Commend_ServoA_to_ZYNQ[i+2] = data_recv_servo[head_index+2+i];
		checksum += Commend_ServoA_to_ZYNQ[i+2];
		checksum = checksum & 0xff;
	}
	Commend_ServoA_to_ZYNQ[31] = data_recv_servo[head_index+31];
	if( checksum == Commend_ServoA_to_ZYNQ[31])
	{

		/*伺服工作状态,只通过航向通道读取*/

		servo_state = Commend_ServoA_to_ZYNQ[3];//
		switch(servo_state)
		{
		case(0x05)://双轴指向——随动
				servo_state_upload = 0x03;
		case(0x00)://双轴空闲——关伺服
				servo_state_upload = 0x00;
		case(0x0A)://双轴搜索——手动
				servo_state_upload = 0x01;
		break;
		}
		yaw_angle_state = ((u16)Commend_ServoA_to_ZYNQ[6]<<8 & 0xff00) | (Commend_ServoA_to_ZYNQ[5] & 0x00ff);
		yaw_gyro_state = ((u16)Commend_ServoA_to_ZYNQ[10]<<8 & 0xff00) | (Commend_ServoA_to_ZYNQ[9] & 0x00ff);
		yaw_angle_state = (u16)(int)((float)(((yaw_angle_state/100)*65536)/360));
	}

}


/*************************************************************
 * 函数名称：Uart_ServoB_rx
 * 函数功能：接收ServoB的数据包
 * 说        明：Pitch 俯仰通道
 *************************************************************/
void Uart_ServoB_rx(void)
{

	int empty = 0;
	int i = 0 ;
	int j = 0 ;
	u32 data_reg;
	u8 head_index;
	u16 checksum = 0;
	memset(data_recv_servo , 0 ,127);
	/*读取fifo中所有128个数*/
	empty = Xil_In32(UART_SERVOB_ADDR_rx+4);
	empty = empty&0x1;
	while(!empty)
	{
		data_reg = Xil_In32(UART_SERVOB_ADDR_rx);
		if(i<128)
		{
			data_recv_servo[i] = data_reg;
		}
		i++;
		empty = Xil_In32(UART_SERVOB_ADDR_rx+4);
		empty = empty&0x1;
	}

	/*寻找包头所在位置*/
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
	head_index = j;//记录包头的序号
	for ( i = 0 ; i < 29 ; i++ )
	{
		Commend_ServoA_to_ZYNQ[i+2] = data_recv_servo[head_index+2+i];
		checksum += Commend_ServoA_to_ZYNQ[i+2];
	}
	Commend_ServoA_to_ZYNQ[31] = data_recv_servo[head_index+31];
	if( checksum == Commend_ServoA_to_ZYNQ[31])
	{
		pitch_angle_state = ((u16)Commend_ServoB_to_ZYNQ[6]<<8 & 0xff00) | (Commend_ServoB_to_ZYNQ[5] & 0x00ff);
		pitch_gyro_state = ((u16)Commend_ServoB_to_ZYNQ[10]<<8 & 0xff00) | (Commend_ServoB_to_ZYNQ[9] & 0x00ff);
		pitch_angle_state = (u16)(int)((float)(((yaw_angle_state/100)*65536)/360));
	}

}
/*************************************************************
 * 函数名称：SendData_HW
 * 函数功能：发送指令至HW探测器
 *************************************************************/
void SendData_HW()
{
	int i = 0 ;
	u16 checkout_HW = 0;

	Commend_ZYNQ_to_HW[0] = 0xEB;
	Commend_ZYNQ_to_HW[1] = 0x90;

	/*计算校验和，3-7字节*/
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
 * 函数名称：SendData_KJG
 * 函数功能：发送指令至可见光
 * 说明：Inq_flag是查询指令标志,0xff代表查询指令,0x00代表控制指令
 *************************************************************/
void SendData_KJG(u8 Inq_flag)
{
	if (0xff == Inq_flag)
	{
		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header0;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header1_Inq;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header2_Inq;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.commend1_Inq;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_End;
	}
	else if (0x00 == Inq_flag)
	{
		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header0;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header1;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_Header2;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.commend1;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.commend2;
		*tx_address_KJG = Commend_ZYNQ_to_KJG.packet_End;
	}

}
/*************************************************************
 * 函数名称：SendData_Servo
 * 函数功能：发送指令至伺服
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

	for( i = 2 ; i < 31 ; i++ )
	{
		CheckSumA += Commend_ZYNQ_to_ServoA[i];
		CheckSumB += Commend_ZYNQ_to_ServoB[i];
	}

	Commend_ZYNQ_to_ServoA[31] = CheckSumA;
	Commend_ZYNQ_to_ServoB[31] = CheckSumB;

	for( i = 0 ; i < 32 ; i++ )
	{
		*tx_address_ServoA = Commend_ZYNQ_to_ServoA[i];
		*tx_address_ServoB = Commend_ZYNQ_to_ServoB[i];
	}


}

/*************************************************************
 * 函数名称：Data_upload
 * 函数功能：20ms定时上报至PC
 * 		    上报内容为B1+B2伺服状态包和D2+D1光学状态包
 *************************************************************/
void Data_upload(void *arg)
{
	int i = 0;
	//伺服
	DataSend_PC[0].packet_ID = Packet_B1_B2;
	DataSend_PC[0].packet_Len = 0x14;//6+11=17字节
	DataSend_PC[0].packet_Data[0] = servo_state_upload<<4;
//	DataSend_PC[0].packet_Data[0] = 0x18;
	DataSend_PC[0].packet_Data[2] = (yaw_angle_state & 0xff00)>>8;
	DataSend_PC[0].packet_Data[3] = yaw_angle_state & 0x00ff;
	DataSend_PC[0].packet_Data[4] = (pitch_angle_state & 0xff00)>>8;
	DataSend_PC[0].packet_Data[5] = pitch_angle_state & 0x00ff;

	DataSend_PC[0].packet_Data[13] = (yaw_gyro_state & 0xff00)>>8;
	DataSend_PC[0].packet_Data[14] = yaw_gyro_state & 0x00ff;
	DataSend_PC[0].packet_Data[15] = (pitch_gyro_state & 0xff00)>>8;
	DataSend_PC[0].packet_Data[16] = pitch_gyro_state & 0x00ff;
	//光学
	DataSend_PC[1].packet_ID = Packet_D2_D1;
	DataSend_PC[1].packet_Len = 0x14;//5+12=17字节
	DataSend_PC[1].packet_Data[5] = DataSend_PC[1].packet_Data[5]|( image_state & 0x07);//成像状态
	DataSend_PC[1].packet_Data[5] = DataSend_PC[1].packet_Data[5]|(( e_zoom_hw & 0x03 )<<5);//红外变倍
	SendData_BDF(tx_address_Tail);


	//跟踪
	DataSend_PC[2].packet_ID = Packet_F1_F2;
	DataSend_PC[2].packet_Len = 0x13;//1+15=16字节
	SendData_BDF(tx_address_PC);
	//上报数据后清零
	for ( i = 0 ; i < 3 ; i++ )
	{
		memset(DataSend_PC[i].packet_Data , 0 , 29);
		DataSend_PC[i].packet_Check = 0x00;
	}


	Drv_TTCTimer_ClearIntr(arg);
}


/*************************************************************
 * 函数名称：SendData_BDF
 * 函数功能：发送BDF包到PC
 * 		     发送BD包到跟踪板
 *************************************************************/
void SendData_BDF( u32 *addr_send_BD )
{
	int i;
	u8 CheckOut_B = 0;
	u8 Packet_Len_B = 0;
	u8 CheckOut_D = 0;
	u8 Packet_Len_D = 0;
	u8 CheckOut_F = 0;
	u8 Packet_Len_F = 0;
	//发送B包数据
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
	//发送D包数据
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

	if(tx_address_PC == addr_send_BD)//给上位机发送B+D+F包数据
	{
		//发送F包数据
//		*addr_send_BD = DataSend_PC[2].packet_Header0;
//		*addr_send_BD = DataSend_PC[2].packet_Header1;
//		*addr_send_BD = DataSend_PC[2].packet_Header2;
//		*addr_send_BD = DataSend_PC[2].packet_Len;
//		*addr_send_BD = DataSend_PC[2].packet_ID;
//		Packet_Len_F = DataSend_PC[2].packet_Len & 0x3F;//0011 1111
//		CheckOut_F = DataSend_PC[2].packet_Len ^DataSend_PC[2].packet_ID;
//		for(i = 0 ; i <(Packet_Len_F-3) ; i++)
//		{
//			*addr_send_BD = DataSend_PC[2].packet_Data[i];
//			CheckOut_F = CheckOut_F^DataSend_PC[2].packet_Data[i];
//		}
//		DataSend_PC[2].packet_Check = CheckOut_F;
//		*addr_send_BD = DataSend_PC[2].packet_Check;
		for(i = 0 ; i < 45 ; i++ )
		{
			*addr_send_BD = Data_send_PC_F[i];
		}
	}

}

