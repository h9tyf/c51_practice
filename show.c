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

void change_show()
{
	u8 i, temp;
	temp = show_num;
	
	for(i = 0; i < 8; i++){
		if(temp != 0){
			digital_tube[i] = temp % 10;
			temp /= 10;
		} else {
			digital_tube[i] = 0xff;
		}
	}
	
}
	
