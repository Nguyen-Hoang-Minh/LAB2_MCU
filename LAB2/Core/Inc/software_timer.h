/*
 * software_timer.h
 *
 *  Created on: Oct 4, 2023
 *      Author: ADMIN
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

extern int timer_flag1;
extern int timer_flag2;
extern int timer_flag3;
void set_Timer1(int duration);
void set_Timer2(int duration);
void set_Timer3(int duration);
int get_timer1_counter();
void timerRun();


#endif /* INC_SOFTWARE_TIMER_H_ */
