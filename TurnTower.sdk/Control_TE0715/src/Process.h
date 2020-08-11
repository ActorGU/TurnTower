/*
 * Process.h
 *
 *  Created on: 2020年3月2日
 *      Author: asguj
 */

#ifndef SRC_PROCESS_H_
#define SRC_PROCESS_H_

void Data_analysis(void);

//typedef IN16 Xint16 ;
typedef short    IN16;

u16 Yaw_speed;//操纵杆给的航向速度
u16 Pitch_speed;//操纵杆给的俯仰速度
float Yaw_angle_temp;//航向角度
float Pitch_angle_temp;//俯仰角度


void Peripheral_Inquire(void *arg);
void ISR_TTC0(void *arg);
void SelfCheck(void);

void Commend_Optic_analysis(void);
void Commend_Tracker_analysis(void);
void Commend_Servo_analysis(void);

#endif /* SRC_PROCESS_H_ */
