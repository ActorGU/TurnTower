/*
 * uart_init.h
 *
 *  Created on: 2019年10月15日
 *      Author: asguj
 */

#ifndef SRC_UART_INIT_H_
#define SRC_UART_INIT_H_
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"



/*ZYNQ工作状态*/
u8 g_uZynqCurrentState;

#define ZYNQ_STATE_CHECK		0x01//上电自检
#define	ZYNQ_STATE_IDLE			0x02//空闲状态
#define ZYNQ_STATE_ATRACE		0x03//跟踪状态
#define ZYNQ_STATE_LOCK			0X04//截获状态
#define ZYNQ_STATE_SERCH		0X05//搜索状态
#define ZYNQ_STATE_OFFSERVO		0x06//关伺服状态

volatile int g_bGetCpuCmd;//PC指令到来标识
volatile int g_bInitComplete;//自检完成标识
volatile int g_bServoCommendUpdate;//伺服指令标识
volatile int g_bCmdAnalysisComplete;//PC指令解析完成标识


/*伺服工作状态*/
u8 g_uServoCurrentState;
u8 g_uServoNewState;
#define SERVO_STATE_IDLE_AB			0x01//双轴空闲
#define SERVO_STATE_SEARCH_AB		0x02//双轴搜索
#define SERVO_STATE_ATRACE_AB		0x03//双轴跟踪
#define SERVO_STATE_POINT_AB		0x04//双轴指向

/*外设ID*/
#define INTC_DEVICE_ID        XPAR_PS7_SCUGIC_0_DEVICE_ID


/*串口通信地址*/
//上位机
#define UART_PC_ADDR_rx			XPAR_UART_PC_S00_AXI_BASEADDR
#define UART_PC_ADDR_tx			XPAR_UART_PC_S01_AXI_BASEADDR
#define UART_HW_ADDR_rx			XPAR_UART_HW_S00_AXI_BASEADDR
#define UART_HW_ADDR_tx			XPAR_UART_HW_S01_AXI_BASEADDR
#define UART_KJG_ADDR_rx		XPAR_UART_KJG_S00_AXI_BASEADDR
#define UART_KJG_ADDR_tx		XPAR_UART_KJG_S01_AXI_BASEADDR
#define UART_SERVOA_ADDR_rx		XPAR_UART_SERVOA_S00_AXI_BASEADDR
#define UART_SERVOA_ADDR_tx		XPAR_UART_SERVOA_S01_AXI_BASEADDR
#define UART_SERVOB_ADDR_rx		XPAR_UART_SERVOB_S00_AXI_BASEADDR
#define UART_SERVOB_ADDR_tx		XPAR_UART_SERVOB_S01_AXI_BASEADDR
#define UART_TAIL_ADDR_rx		XPAR_UART_TAIL_S00_AXI_BASEADDR
#define UART_TAIL_ADDR_tx		XPAR_UART_TAIL_S01_AXI_BASEADDR
#define UART_Tracker_ADDR_rx	XPAR_UART_TRACKER_S00_AXI_BASEADDR
#define UART_Tracker_ADDR_tx	XPAR_UART_TRACKER_S01_AXI_BASEADDR



/*与PC通信数据包ID*/
#define Packet_A1			0x18		//伺服控制常用9字节
#define Packet_A2			0x19		//伺服控制不常用2字节
#define Packet_C1			0x1A		//光学控制常用2字节
#define Packet_C2			0x1B		//光学控制不常用3字节
#define Packet_E1			0x1C		//跟踪器指令常用3字节
#define Packet_E2			0x1D		//跟踪器指令不常用5字节
#define Packet_A3			0x1E		//伺服辅助数据17字节
#define Packet_F1_F2		0x22		//跟踪器状态常用+不常用 = 1+15 = 16字节
#define Packet_B1_B2		0X20		//伺服状态常用+不常用 = 6+11 = 17字节
#define Packet_D2_D1		0X21		//光学状态不常用+常用 = 5+12 = 17字节
#define Packet_G			0X23		//飞控GPS_21字节


/*************************************************************
*与PC通信数据包结构体
*************************************************************/
typedef struct{
	u16 packet_Header0;
	u16 packet_Header1;
	u16 packet_Header2;
	u16 packet_Len;
	u16 packet_ID;
	u8 packet_Data[30];
	u16 packet_Check;

}DataFormat;
DataFormat DataRecv_PC;
DataFormat DataSend_PC[3];// DataSend_PC[0] B包    ;   DataSend_PC[1] D包  ;
u8 DataRecv_PC_tempE1[30];
/*************************************************************
 *
*与各分机通信的数据包
*
*************************************************************/
u8 Commend_ZYNQ_to_KJG_new[7];
u8 Commend_ServoA_to_ZYNQ[32];
u8 Commend_ServoB_to_ZYNQ[32];
u8 Commend_ZYNQ_to_ServoA[32];
u8 Commend_ZYNQ_to_ServoB[32];
u8 Commend_HW_to_ZYNQ[10];
u8 Commend_ZYNQ_to_HW[8];
u8 Commend_Tail_to_ZYNQ[16];//跟踪器422串口数据包
u8 Commend_ZYNQ_to_Tracker[30];//跟踪器232串口数据包




/*************************************************************
*ZYNQ内部工作变量
*************************************************************/


/*上位机数据*/
u32 data_recv_PC[128];//上位机接收数据缓存

/*内部变量_伺服*/
u32 data_recv_servo[128];//伺服航向数据缓存
u32 data_recv_servo_B[128];//伺服俯仰数据缓存
u8 servo_state;//伺服返回的状态字
u8 servo_state_upload;//给上位机上报的伺服状态字
u16 yaw_angle_state;//伺服回传的方位角
u16 yaw_gyro_state;//伺服回传的方位陀螺信号

u16 pitch_angle_state;//伺服回传的俯仰角
u16 pitch_gyro_state;//伺服回传的俯仰陀螺信号

u16 pitch_gyro_recv;

/*内部变量_光学*/
u32 data_recv_HW[128];//红外探测器数据缓存
u8 e_zoom_hw;//红外电子变倍状态
u8 image_state;//图像状态,00 kjg,01 hw
u8 PixelCnt;//图像像素数 ,0x01 1920*1080, 0x04 720*576
int CommendCnt_HW;
/*内部变量_跟踪板*/
u32 data_recv_Tail[128];
u16 Data_send_PC_F[45];
u8 Tail_state;//跟踪器状态
u16 yaw_offset_state;//跟踪板回传的方位目标像素差
u16 pitch_offset_state;//跟踪板回传的俯仰目标像素差

//void uart_tx();
void Uart_Pc_rx(void);
void Uart_Tail_rx(void);
void Uart_Tracker_rx(void);
void Uart_HW_rx(void);
void Uart_KJG_rx(void);
void Uart_ServoA_rx(void);
void Uart_ServoB_rx(void);
void Data_upload(void *arg);
void SendData_BDF();
void SendData_KJG();
void SendData_HW();
void SendData_Servo(void *arg);

u32 *tx_address_PC;
u32 *rx_address_PC;
u32 *tx_address_HW;
u32 *rx_address_HW;
u32 *tx_address_KJG;
u32 *rx_address_KJG;
u32 *rx_address_Tail;
u32 *tx_address_Tail;
u32 *rx_address_ServoA;
u32 *tx_address_ServoA;
u32 *rx_address_ServoB;
u32 *tx_address_ServoB;
u32 *rx_address_Tracker;
u32 *tx_address_Tracker;



#endif /* SRC_UART_INIT_H_ */
