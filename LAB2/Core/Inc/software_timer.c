/*
 * software_timer.c
 *
 *  Created on: Oct 4, 2023
 *      Author: ADMIN
 */


#include "software_timer.h"

int timer1_counter = 0;
int timer2_counter = 0;
int timer3_counter = 0;
int timer_flag1 = 0;
int timer_flag2 = 0;
int timer_flag3 = 0;
void set_Timer1(int duration){
	timer1_counter = duration;
	timer_flag1 = 0;
}

void set_Timer2(int duration){
	timer2_counter = duration;
	timer_flag2 = 0;
}

void set_Timer3(int duration){
	timer3_counter = duration;
	timer_flag3 = 0;
}
int get_timer1_counter(){
	return timer1_counter;
}

void timerRun(){
	timer1_counter--;
	timer2_counter--;
	timer3_counter--;
	if(timer1_counter <= 0){
		timer_flag1 = 1;
	}
	if(timer2_counter <= 0){
		timer_flag2 = 1;
	}
	if(timer3_counter <= 0){
		timer_flag3 = 1;
	}
}
