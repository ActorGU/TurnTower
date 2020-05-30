/*
 * Process.h
 *
 *  Created on: 2020Äê3ÔÂ2ÈÕ
 *      Author: asguj
 */

#ifndef SRC_PROCESS_H_
#define SRC_PROCESS_H_

void Data_analysis(void);

void Peripheral_Inquire(void *arg);
void ISR_TTC0(void *arg);

void Commend_Optic_analysis(void);
void Commend_Tail_analysis(void);
void Commend_Servo_analysis(void);

#endif /* SRC_PROCESS_H_ */
