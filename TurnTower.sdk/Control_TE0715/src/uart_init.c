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


//void uart_tx(u32 *addr, u16 *sendbuf , int len)
//{
//	for(int i=0;i<len;i++)
//	{
//		*addr = sendbuf[i];
//	}
//}


/*************************************************************
 * 函数名称：Uart_Pc_rx、Uart_Pc_rx_part
 * 函数功能：接收PC的指令包
 * 帧长n=数据包长度-1+4
 * 说        明：PC_A、C、E ——> Zynq
 *************************************************************/
void Uart_Pc_rx(void)
{

	int i = 0;
	int j = 0;
	int k = 0;

	u8 head_index = 0;
	u8 head_index_1 = 0;
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


	g_bGetCpuCmd = 1;//第一小包指令标识


//	/*判断是否有第二包数据*/
	while( k < 4 )//最多响应4包
	{
		if((0x55 == data_recv_PC[head_index+data_len+3])&&(0xaa == data_recv_PC[head_index+data_len+4]))
		{
			head_index = head_index+data_len+3;//包头位置后移
			DataRecv_PC.packet_Len = data_recv_PC[head_index+3];
			data_len = DataRecv_PC.packet_Len & 0x3f;
			DataRecv_PC.packet_ID = data_recv_PC[head_index+4];
			for(j = 0 ; j <(data_len-3) ; j++)
			{
				DataRecv_PC.packet_Data[j] = data_recv_PC[head_index+j+5];
			}
			DataRecv_PC.packet_Check = data_recv_PC[head_index+data_len+2];


			g_bGetCpuCmd = 1;//第二包以后的指令标识

			head_index = head_index+data_len+3;
			k++;
		}
		else
		{
			break;
		}

	}


//	if((DataRecv_PC.packet_ID == 0x18)&&(DataRecv_PC.packet_Data[0] == 0x06))//如果是A1包的跟踪指令
//		{
//			if(data_recv_PC[data_len+3] == 0x55)//且接着有其他包数据
//			{
//				while(k < 128)//寻找E1包位置
//					if((0x55 == data_recv_PC[k])&&(0xaa == data_recv_PC[k+1])&&(0x1C == data_recv_PC[k+4]))
//					{
//
//						for( i = 0 ; i < ((data_recv_PC[k+3] + 3)& 0x3f) ; i++)
//						{
//							DataRecv_PC_tempE1[i] = data_recv_PC[k+i];
//						}
//						break;
//					}
//					else
//					{
//						k++;
//					}
//	//			//E1包直接转发
//				for(  i = 0 ; i < ((data_recv_PC[k+3] + 3)& 0x3f) ; i++ )
//				{
//
//					*tx_address_Tracker = DataRecv_PC_tempE1[i];
//				}
//			}
//		}
//
//		g_bGetCpuCmd = 1;

}

void Uart_Tail_rx(void)
{
//	int i = 0;
//	int j = 0;
//	u8 head_index = 0;
//	u8 empty = 0;
////	u16 data_len = 0;
//	u32 data_reg;
//	empty = Xil_In32(UART_Tracker_ADDR_rx+4);
//	empty = empty&0x1;
//	while(!empty)
//	{
//		data_reg = Xil_In32(UART_Tracker_ADDR_rx);
//		if(i<128)
//		{
//			data_recv_Tail[i] = data_reg;
//		}
//		i++;
//		empty = Xil_In32(UART_TAIL_ADDR_rx+4);
//		empty = empty&0x1;
//	}
}


/*************************************************************
 * 函数名称：Uart_Tail_rx
 * 函数功能：接收跟踪板的数据包
 * 说        明：Tail_A、C、F ——> Zynq
 *************************************************************/
void Uart_Tracker_rx(void)
{
	int i = 0;
	int j = 0;
	u8 head_index = 0;
	u8 empty = 0;
//	u16 data_len = 0;
	u32 data_reg;
	memset(data_recv_Tail , 0 , 127);

	/*读取fifo中所有128个数*/
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
		empty = Xil_In32(UART_Tracker_ADDR_rx+4);
		empty = empty&0x1;
	}
	/*寻找F1+F2包头所在位置*/
	while(j < 128)
		if((0x55 == data_recv_Tail[j])&&(0xaa == data_recv_Tail[j+1])&&(0x22 == data_recv_Tail[j+4]))
		{

			image_state = data_recv_Tail[j+5]&0x07;//从跟踪板返回的F1包获得图像状态
			Tail_state = (data_recv_Tail[j+5]&0x18)>>3;//从跟踪板返回的F1包获得跟踪器状态
			yaw_offset_state = (data_recv_Tail[j+17]<<8)|data_recv_Tail[j+18];//从跟踪板返回的F2包获得方位像素差
			pitch_offset_state = (data_recv_Tail[j+19]<<8)|data_recv_Tail[j+20];//从跟踪板返回的F2包获得俯仰像素差
			for(int k = 0 ; k < 45 ; k++ )
			{
				Data_send_PC_F[k] = data_recv_Tail[j+k];

			}
			//跟踪板反馈图像状态,控制显示字符
			if(0x00 == image_state)
			{
				PixelCnt = 0x01;//1920*1080
			}
			else
			{
				PixelCnt = 0x04;//720*576
			}
			//根据跟踪板反馈控制伺服跟踪
			switch(Tail_state)
			{
			case(0x02)://跟踪

				if(ZYNQ_STATE_LOCK == g_uZynqCurrentState)
				{
					g_uZynqCurrentState = ZYNQ_STATE_ATRACE;//在主控板为截获状态时，跟踪板返回跟踪指令,则主控切为跟踪状态
				}

				if(0x00 == image_state)//判断跟踪的探测器,可见光
				{
					/*跟踪系数需要试验*/
					yaw_offset_state = (u16)((float)yaw_offset_state*5.0);
					pitch_offset_state = (u16)((float)pitch_offset_state*5.0);
				}
				else if(0x01 == image_state)//红外
				{
					yaw_offset_state = (u16)((float)yaw_offset_state*3.0);
					pitch_offset_state = (u16)((float)pitch_offset_state*3.0);
				}

				break;
			case(0x01)://搜索
					if(ZYNQ_STATE_SERCH == g_uZynqCurrentState)
					{
						//判断是否为搜索状态
					}
				break;

			case(0x00)://跟踪板停止跟踪后,主控转为搜索状态
					if(ZYNQ_STATE_ATRACE == g_uZynqCurrentState)//如果主控状态为跟踪
					{
						g_uZynqCurrentState = ZYNQ_STATE_SERCH;
					}
				break;
			case(0x03)://丢失后切为搜索状态
					if(ZYNQ_STATE_ATRACE == g_uZynqCurrentState)
					{
						g_uZynqCurrentState = ZYNQ_STATE_SERCH;

					}

				break;

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
		if((0xEB == data_recv_HW[j])&&(0x90 == data_recv_HW[j+1]))
		{
			Commend_HW_to_ZYNQ[0] = data_recv_HW[j];
			Commend_HW_to_ZYNQ[1] = data_recv_HW[j+1];
			/*收到红外第一包数据后进行图像翻转*/
			if( 1 == CommendCnt_HW )
			{
				CommendCnt_HW = 35;
			}
			else if( 0 == CommendCnt_HW )
			{
				CommendCnt_HW = 1;
			}


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
	float yaw_angle_state_temp = 0 ;
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
		break;
		case(0x00)://双轴空闲——关伺服
				servo_state_upload = 0x00;
		break;
		case(0x0A)://双轴搜索——手动
				servo_state_upload = 0x01;
		break;
		case(0x0F)://双轴跟踪
				servo_state_upload = 0x06;
		break;
		}
		yaw_angle_state = ((u16)Commend_ServoA_to_ZYNQ[6]<<8 & 0xff00) | (Commend_ServoA_to_ZYNQ[5] & 0x00ff);
		yaw_gyro_state = ((u16)Commend_ServoA_to_ZYNQ[10]<<8 & 0xff00) | (Commend_ServoA_to_ZYNQ[9] & 0x00ff);

		if( yaw_angle_state > 18000)
		{
			yaw_angle_state_temp = (int)yaw_angle_state-36000;
			yaw_angle_state_temp = ((float)yaw_angle_state)/100;
		}
		else
		{
			yaw_angle_state_temp = (float)yaw_angle_state/100;
		}
		yaw_angle_state = (u16)(int)((yaw_angle_state_temp*65536)/360.0);
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
	float pitch_angle_state_temp = 0;
	memset(data_recv_servo_B , 0 ,127);
	/*读取fifo中所有128个数*/
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

	/*寻找包头所在位置*/
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
	head_index = j;//记录包头的序号
	for ( i = 0 ; i < 29 ; i++ )
	{
		Commend_ServoB_to_ZYNQ[i+2] = data_recv_servo_B[head_index+2+i];
		checksum += Commend_ServoB_to_ZYNQ[i+2];
		checksum = checksum & 0xff;
	}
	Commend_ServoB_to_ZYNQ[31] = data_recv_servo_B[head_index+31];
	if( checksum == Commend_ServoB_to_ZYNQ[31])
	{
		pitch_angle_state = ((u16)(Commend_ServoB_to_ZYNQ[6]<<8 & 0xff00) | (Commend_ServoB_to_ZYNQ[5] & 0x00ff));
		pitch_gyro_state = ((u16)Commend_ServoB_to_ZYNQ[10]<<8 & 0xff00) | (Commend_ServoB_to_ZYNQ[9] & 0x00ff);


		if(pitch_angle_state > 53536)//俯仰角度为负时
		{
			pitch_angle_state_temp = (int)(pitch_angle_state - 0xffff);
			pitch_angle_state_temp = ((float)pitch_angle_state_temp)/100;
		}
		else
		{
			pitch_angle_state_temp = (float)pitch_angle_state/100;
		}

//		pitch_angle_state = (u16)(int)((float)(((pitch_angle_state/100)*65536)/360));
		pitch_angle_state = (u16)((int)((pitch_angle_state_temp*65536.0)/360.0));
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
	Commend_ZYNQ_to_HW[5] = 0x00;
	Commend_ZYNQ_to_HW[6] = 0x00;


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
void SendData_KJG()
{

	u16 checkout_KJG;
	checkout_KJG = 0;
	Commend_ZYNQ_to_KJG_new[0] = 0xff;
	Commend_ZYNQ_to_KJG_new[1] = 0x01;
	Commend_ZYNQ_to_KJG_new[2] = 0x00;
	//计算校验和
	for ( int i = 1 ; i < 6 ; i ++ )
	{
		checkout_KJG += Commend_ZYNQ_to_KJG_new[i];
	}
	Commend_ZYNQ_to_KJG_new[6] = checkout_KJG;
	//发送至可见光
	for(int j = 0 ; j < 7 ; j++ )
	{
		*tx_address_KJG = Commend_ZYNQ_to_KJG_new[j];
	}
}
/*************************************************************
 * 函数名称：SendData_Servo
 * 函数功能：发送指令至伺服
 *************************************************************/
void SendData_Servo(void *arg)
{
	int i ;
	u8 CheckSumA = 0;
	u8 CheckSumB = 0;

//	if( 1 == g_bServoCommendUpdate)//新伺服指令
//	{
//		g_bServoCommendUpdate = 0;
//		if(g_uServoNewState != g_uServoCurrentState)//伺服状态更改
//		{
//			g_uServoCurrentState = g_uServoNewState;
//		}
//	}

	switch(g_uZynqCurrentState)//根据当前主控板状态周期发送指令
	{
	case(ZYNQ_STATE_LOCK)://截获状态,伺服保持搜索状态,直到跟踪板反馈跟踪指令
	case(ZYNQ_STATE_SERCH)://搜索状态
			memset(Commend_ZYNQ_to_ServoA , 0 , 31);
			memset(Commend_ZYNQ_to_ServoB , 0 , 31);
			Commend_ZYNQ_to_ServoA[3] = 0x0A;
			Commend_ZYNQ_to_ServoA[9] = (u16)Yaw_speed & 0x00ff;
			Commend_ZYNQ_to_ServoA[10] = ((u16)Yaw_speed >> 8)& 0x00ff;
			Commend_ZYNQ_to_ServoB[3] = 0x0A;//双轴搜索
			Commend_ZYNQ_to_ServoB[9] = (u16)Pitch_speed & 0x00ff;
			Commend_ZYNQ_to_ServoB[10] = ((u16)Pitch_speed >> 8)& 0x00ff;
			break;
	case(ZYNQ_STATE_ATRACE)://跟踪
			memset(Commend_ZYNQ_to_ServoA , 0 , 31);
			memset(Commend_ZYNQ_to_ServoB , 0 , 31);
			Commend_ZYNQ_to_ServoA[3] = 0x0F;
			Commend_ZYNQ_to_ServoA[9] = yaw_offset_state&0x00ff;
			Commend_ZYNQ_to_ServoA[10] = (yaw_offset_state>>8)&0x00ff;
			Commend_ZYNQ_to_ServoB[3] = 0x0F;
			Commend_ZYNQ_to_ServoB[9] = pitch_offset_state&0x00ff;
			Commend_ZYNQ_to_ServoB[10] = (pitch_offset_state>>8)&0x00ff;
			break;
	case(ZYNQ_STATE_IDLE)://空闲_伺服指向
			memset(Commend_ZYNQ_to_ServoA , 0 , 31);
			memset(Commend_ZYNQ_to_ServoB , 0 , 31);
			//伺服接收先低8后高8
			Commend_ZYNQ_to_ServoA[3] = 0x05;
			Commend_ZYNQ_to_ServoA[5] = (u16)(Yaw_angle_temp*100) & 0x00ff;
			Commend_ZYNQ_to_ServoA[6] = ((u16)(Yaw_angle_temp*100) >> 8)& 0x00ff;

			Commend_ZYNQ_to_ServoB[3] = 0x05;
			Commend_ZYNQ_to_ServoB[5] = (IN16)(Pitch_angle_temp*100) & 0x00ff;
			Commend_ZYNQ_to_ServoB[6] = (((IN16)(Pitch_angle_temp*100) >> 8)& 0x00ff);
			break;
	case(ZYNQ_STATE_OFFSERVO)://关伺服
			memset(Commend_ZYNQ_to_ServoA , 0 , 31);
			memset(Commend_ZYNQ_to_ServoB , 0 , 31);
			break;
	}





	Commend_ZYNQ_to_ServoA[0] = 0x55;
	Commend_ZYNQ_to_ServoA[1] = 0xAA;
	Commend_ZYNQ_to_ServoA[2] = 0x1C;
	Commend_ZYNQ_to_ServoB[0] = 0x55;
	Commend_ZYNQ_to_ServoB[1] = 0xAA;
	Commend_ZYNQ_to_ServoB[2] = 0x1C;
	//计算校验和
	for( i = 2 ; i < 31 ; i++ )
	{
		CheckSumA += Commend_ZYNQ_to_ServoA[i];
		CheckSumB += Commend_ZYNQ_to_ServoB[i];
	}

	Commend_ZYNQ_to_ServoA[31] = CheckSumA;
	Commend_ZYNQ_to_ServoB[31] = CheckSumB;
	//发送至伺服A、B通道
	for( i = 0 ; i < 32 ; i++ )
	{
		*tx_address_ServoA = Commend_ZYNQ_to_ServoA[i];
		*tx_address_ServoB = Commend_ZYNQ_to_ServoB[i];
	}


	Drv_TTCTimer_ClearIntr(arg);
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
	DataSend_PC[0].packet_Len = 0x14;//数据17字节+包头3字节=20
	DataSend_PC[0].packet_Data[0] = servo_state_upload<<4;//伺服状态只从航向通道获取
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
	DataSend_PC[1].packet_Len = 0x14;//数据17字节+包头3字节=20
	DataSend_PC[1].packet_Data[0] = DataSend_PC[1].packet_Data[0]|( ( image_state & 0x07) << 3);//成像状态
	DataSend_PC[1].packet_Data[1] = DataSend_PC[1].packet_Data[1]|( ( image_state & 0x07) << 5);//成像状态
	DataSend_PC[1].packet_Data[1] = DataSend_PC[1].packet_Data[1]|(PixelCnt & 0x1f);//图像像素数
	SendData_BDF(tx_address_Tail);

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
	/*如果发送地址是上位机，则发送增加F包*/
	if(tx_address_PC == addr_send_BD)//给上位机发送B+D+F包数据
	{

		for(i = 0 ; i < 45 ; i++ )
		{
			*addr_send_BD = Data_send_PC_F[i];
		}
	}

}


