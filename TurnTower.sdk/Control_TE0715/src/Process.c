/*
 * Process.c
 *
 *  Created on: 2020年3月2日
 *      Author: asguj
 */
#include <stdio.h>
#include "uart_init.h"
#include "Process.h"


/*************************************************************
 * 函数名称：Data_analysis
 * 函数功能：解析PC机指令
 *************************************************************/
void Data_analysis(void)
{

	int i = 0;
	g_bCmdAnalysisComplete = 0;
	switch(DataRecv_PC.packet_ID)
	{
	/*跟踪器指令*/
		case(Packet_E1):
		case(Packet_E2):
			Commend_Tracker_analysis();
		break;

	/*光学指令*/
		case(Packet_C1):
		case(Packet_C2):
			Commend_Optic_analysis();
		break;

	/*伺服指令*/
		case(Packet_A1):
		case(Packet_A2):
			Commend_Servo_analysis();
		break;
	}

	/*解析完PC指令后清零*/
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
 * 函数名称：Commend_Servo_analysis
 * 函数功能：解析伺服指令
 *************************************************************/
void Commend_Servo_analysis(void)
{

//	int i = 0;
	u8 Servo_state;
	u16 Yaw_angle;//航向
	u16 Pitch_angle;//俯仰
//	u16 Yaw_speed;
//	u16 Pitch_speed;
//	float Yaw_angle_temp;
//	float Pitch_angle_temp;

	memset(Commend_ZYNQ_to_ServoA , 0 , 31);
	memset(Commend_ZYNQ_to_ServoB , 0 , 31);
	Servo_state = DataRecv_PC.packet_Data[0] & 0x0F;// 4bit 0~3 0000 1111
	/*ServoA是航向指令、ServoB是俯仰指令、数组第3位为工作状态、指向都在5/6位、跟踪都在9/10位*/
	/*对于上位机发来的指令,DataRecv_PC.packet_Data包序号对应协议中字节序号-1*/
		switch(Servo_state)
		{
		case(0x00)://关闭伺服__空闲状态
				g_uZynqCurrentState = ZYNQ_STATE_OFFSERVO;
				Commend_ZYNQ_to_ServoA[3] = 0x00;

		break;
		case(0x04)://锁定零位__指向0度
				if(ZYNQ_STATE_ATRACE != g_uZynqCurrentState)//如果不是跟踪状态,则响应指令
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
					g_uZynqCurrentState = ZYNQ_STATE_SERCH;//如果是跟踪状态,则切换搜索状态
				}

		break;

		case(0x01)://手动__伺服搜索
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
		case(0x03)://随动__伺服指向
				g_uZynqCurrentState = ZYNQ_STATE_IDLE;
				//PC指令先高8后低8
				Yaw_angle = ( (DataRecv_PC.packet_Data[5]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[6] & 0x00ff );
				Pitch_angle = ( (DataRecv_PC.packet_Data[7]<<8) & 0xff00 ) | ( DataRecv_PC.packet_Data[8] & 0x00ff );
				//换算角度值
				Yaw_angle_temp = (float)(Yaw_angle*360)/65536;
				Pitch_angle_temp = (float)(Pitch_angle*360)/65536;
				//保留小数点两位
				Yaw_angle_temp = ( (float)( (int)( (Yaw_angle_temp+0.005)*100 ) ) )/100;
				Pitch_angle_temp = ( (float)( (int)( (Pitch_angle_temp+0.005)*100 ) ) )/100;
				if ( Pitch_angle_temp > 180 )
				{
					Pitch_angle_temp = Pitch_angle_temp - 360;
				}
//				//伺服接收先低8后高8
//				Commend_ZYNQ_to_ServoA[3] = 0x05;
//				Commend_ZYNQ_to_ServoA[5] = (u16)(Yaw_angle_temp*100) & 0x00ff;
//				Commend_ZYNQ_to_ServoA[6] = ((u16)(Yaw_angle_temp*100) >> 8)& 0x00ff;
//
//				Commend_ZYNQ_to_ServoB[3] = 0x05;
//				Commend_ZYNQ_to_ServoB[5] = (IN16)(Pitch_angle_temp*100) & 0x00ff;
//				Commend_ZYNQ_to_ServoB[6] = (((IN16)(Pitch_angle_temp*100) >> 8)& 0x00ff);
		break;


		case(0x06)://截获指令
//				g_uServoNewState = SERVO_STATE_ATRACE_AB;
				if(ZYNQ_STATE_ATRACE != g_uZynqCurrentState)//非跟踪状态下,速度清零,指向角度保持不变
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
* 函数名称：Commend_Optic_analysis
* 函数功能：解析光学指令
*************************************************************/
void Commend_Optic_analysis(void)
{
	//			红外探测器    	0x02
	//			可见光探测器	0x01
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
		case(0x00)://无动作
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
		case(0x08)://视场+,
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
		case(0x09)://视场-
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
		case(0x02)://亮度+
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
		case(0x03)://亮度-
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
		case(0x0C)://内部非均匀校正
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x6e;
				}
				else if (0x01 == C1_imager_choose)
				{
				}
		break;
		case(0x0d)://外部非均匀校正
				if(0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x6f;
				}
				else if(0x01 == C1_imager_choose)
				{

				}
		break;
		case(0x0E)://黑白热切换
		case(0x0F):
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x71;
				}
				else if (0x01 == C1_imager_choose)
				{
				}
		break;
		case(0x0A)://调焦+
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x87;
				}
				else if (0x01 == C1_imager_choose)
				{

				}
		break;
		case(0x0B)://调焦-
				if (0x02 == C1_imager_choose)
				{
					Commend_ZYNQ_to_HW[2] = 0x88;
				}
				else if (0x01 == C1_imager_choose)
				{

				}
		break;
		default://默认无动作
			Commend_ZYNQ_to_HW[2] = 0x00;
			Commend_ZYNQ_to_KJG_new[3] = 0x00;
//			Commend_ZYNQ_to_KJG.commend1 = 0x07;
//			Commend_ZYNQ_to_KJG.commend2 = 0x00;
		break;
		}

	//分发到两个相机
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
 * 函数名称：Commend_Tracker_analysis
 * 函数功能：解析跟踪器指令(直接转发)
 *************************************************************/
void Commend_Tracker_analysis(void)
{

	int j = 0;
	int i = 0;
	u8 PC_Packet_Len = 0;
	u8 image_choose_commend = 0;
	memset(Commend_ZYNQ_to_Tracker , 0 , 29);
	//数据转存到Commend_ZYNQ_to_Tracker
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


	//直接转发数据
//	uart_tx(tx_address_Tracker , Commend_ZYNQ_to_Tracker , PC_Packet_Len+3);
	for ( i = 0 ; i < PC_Packet_Len+3 ; i++ )
	{
		*tx_address_Tracker = Commend_ZYNQ_to_Tracker[i];
	}

}

/*************************************************************
 * 函数名称：Peripheral_Inquire
 * 函数功能：20ms定时查询各外设状态
 * 		  红外,可见光,伺服(伺服分机为定时上报模式,不用查询)
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
//	SendData_KJG(0xff);//查询指令
//	Drv_TTCTimer_ClearIntr(arg);


}


void SelfCheck(void)
{

}
