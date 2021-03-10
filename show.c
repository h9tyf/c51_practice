#include "var.h"

unsigned char digital_tube[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char selected;//1:hour 2:min 3:seconds

enum show show_state = SLEEP;

void cal_left_time(){
	long time, end_t;
	time = aaa_to_time(time_hour, time_min, time_seconds);
	end_t = aaa_to_time(end_hour, end_min, end_seconds);
	left_time = time >= end_t?0:(end_t - time);
}

#define OFF 12
#define HYPHEN 11

void change_show()
{
	u8 i;
	digital_tube[7] = HYPHEN;
	digital_tube[5] = HYPHEN;
	digital_tube[4] = OFF;
	if(show_state == TEMPERATURE){
		long temp = temperature;
		digital_tube[0] = 10;
		digital_tube[1] = temp % 10;
		digital_tube[2] = temp / 10;
		digital_tube[3] = OFF;
		
		digital_tube[6] = 4;	
	} else {
		long temp;
		cal_left_time();
		temp = left_time;
		for(i = 0; i<4; i++){
			digital_tube[i] = temp % 10;
			temp /= 10;
		}
		
		if(show_state == SLEEP){
			digital_tube[6] = 1;
		} else if(show_state == NATURE){
			digital_tube[6] = 2;
		} else {
			digital_tube[6] = 3;
		}
	}
	
	if(show_state == SLEEP){
		LatchControl(4, 0xfe);
		pwm_duty = 20;
	} else if(show_state == NATURE){
		LatchControl(4, 0xfd);
		pwm_duty = 30;
	} else if(show_state == STEADY){
		LatchControl(4, 0xfb);
		pwm_duty = 70;
	} else {
		pwm_duty = 0;
	}
	
	
}
	
