/*
 * uart_init.h
 *
 *  Created on: 2019��10��15��
 *      Author: asguj
 */

#ifndef SRC_UART_INIT_H_
#define SRC_UART_INIT_H_
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"

volatile int g_bGetCpuCmd;
volatile int g_bInitComplete;

/*����ID*/
#define INTC_DEVICE_ID        XPAR_PS7_SCUGIC_0_DEVICE_ID


/*����ͨ�ŵ�ַ*/
//��λ��
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



/*��PCͨ�����ݰ�ID*/
#define Packet_A1			0x18		//�ŷ����Ƴ���9�ֽ�
#define Packet_A2			0x19		//�ŷ����Ʋ�����2�ֽ�
#define Packet_C1			0x1A		//��ѧ���Ƴ���2�ֽ�
#define Packet_C2			0x1B		//��ѧ���Ʋ�����3�ֽ�
#define Packet_E1			0x1C		//������ָ���3�ֽ�
#define Packet_E2			0x1D		//������ָ�����5�ֽ�
#define Packet_A3			0x1E		//�ŷ���������17�ֽ�
#define Packet_F1_F2		0x22		//������״̬����+������ = 1+15 = 16�ֽ�
#define Packet_B1_B2		0X20		//�ŷ�״̬����+������ = 6+11 = 17�ֽ�
#define Packet_D2_D1		0X21		//��ѧ״̬������+���� = 5+12 = 17�ֽ�
#define Packet_G			0X23		//�ɿ�GPS_21�ֽ�


/*************************************************************
*��PCͨ�����ݰ��ṹ��
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
DataFormat DataSend_PC[3];// DataSend_PC[0] B��    ;   DataSend_PC[1] D��  ;  DataSend_PC[2] F��

/*************************************************************
 *
*����ֻ�ͨ�ŵ����ݰ�
*
*************************************************************/
u8 Commend_ZYNQ_to_KJG_new[7];
u8 Commend_ServoA_to_ZYNQ[32];
u8 Commend_ServoB_to_ZYNQ[32];
u8 Commend_ZYNQ_to_ServoA[32];
u8 Commend_ZYNQ_to_ServoB[32];
u8 Commend_HW_to_ZYNQ[10];
u8 Commend_ZYNQ_to_HW[8];
u8 Commend_Tail_to_ZYNQ[16];//������422�������ݰ�
u8 Commend_ZYNQ_to_Tracker[30];//������232�������ݰ�
/*ת�������ٰ�Ŀ���ָ��*/
//u16 sendbuf[30];
//typedef struct{
//	u16 packet_Header0;
//	u16 packet_Header1;
//	u16 packet_Header2;
//	u16 packet_Header1_Inq;
//	u16 packet_Header2_Inq;
//	u16 commend1;
//	u16 commend1_Inq;
//	u16 commend2;
//	u16 packet_End;
//}DataFormat_KJG;
//DataFormat_KJG Commend_ZYNQ_to_KJG;
//typedef struct{
//	u16 packet_Header0;
//	u16 packet_Header1;
//	u16 packet_Data[5];
//	u16 packet_End;
//}DataFormat_KJG1;
//DataFormat_KJG1 Commend_KJG_to_ZYNQ;



/*************************************************************
*ZYNQ�ڲ���������
*************************************************************/

/*��λ������*/
u32 data_recv_PC[128];//��λ���������ݻ���

/*�ڲ�����_�ŷ�*/
u32 data_recv_servo[128];//�ŷ��������ݻ���
u32 data_recv_servo_B[128];//�ŷ��������ݻ���
u8 servo_state;//�ŷ����ص�״̬��
u8 servo_state_upload;//����λ���ϱ����ŷ�״̬��
u16 yaw_angle_state;//�ŷ��ش��ķ�λ��
u16 yaw_gyro_state;//�ŷ��ش��ķ�λ�����ź�
u16 yaw_offset_state;//
u16 pitch_angle_state;//�ŷ��ش��ķ�λ��
u16 pitch_gyro_state;//�ŷ��ش��ķ�λ�����ź�
u16 pitch_offset_state;//
u16 pitch_gyro_recv;

/*�ڲ�����_��ѧ*/
u32 data_recv_HW[128];//����̽�������ݻ���
u8 e_zoom_hw;//������ӱ䱶״̬
u8 image_state;//ͼ��״̬,00 kjg,01 hw

/*�ڲ�����_���ٰ�*/
u32 data_recv_Tail[128];
u16 Data_send_PC_F[45];
u8 Tail_state;//������״̬



void uart_tx();
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
void SendData_Servo();

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
