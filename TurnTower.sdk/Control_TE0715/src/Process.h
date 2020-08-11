/*
 * Process.h
 *
 *  Created on: 2020��3��2��
 *      Author: asguj
 */

#ifndef SRC_PROCESS_H_
#define SRC_PROCESS_H_

void Data_analysis(void);

//typedef IN16 Xint16 ;
typedef short    IN16;

u16 Yaw_speed;//���ݸ˸��ĺ����ٶ�
u16 Pitch_speed;//���ݸ˸��ĸ����ٶ�
float Yaw_angle_temp;//����Ƕ�
float Pitch_angle_temp;//�����Ƕ�


void Peripheral_Inquire(void *arg);
void ISR_TTC0(void *arg);
void SelfCheck(void);

void Commend_Optic_analysis(void);
void Commend_Tracker_analysis(void);
void Commend_Servo_analysis(void);

#endif /* SRC_PROCESS_H_ */
