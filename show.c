#include "var.h"

unsigned char digital_tube[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char selected;//1:hour 2:min 3:seconds

unsigned char led_state;//0:light 1:
unsigned char led_sum_time = 5000;
unsigned char led_time;
unsigned char digital_state;//0:light 1:

enum show show_state = SHOW_TIME;

long tube_to_num(){
	long ans = 0;
	long n = 1;
	u8 i;
	for(i = 0; i < 8; i++){
		ans += digital_tube[i] * n;
		n *= 10;
	}
	return ans;
}

void num_to_tube(long num){
	u8 i;
	long temp = num;
	if(temp < 0){
		temp = 0;
	}
	for(i = 0; i < 8; i++){
		digital_tube[i] = temp % 10;
		temp = temp / 10;
	}
}

void show_temperature()
{
	long temp = temperature;
	num_to_tube(temp);
}

void show_time(u8 hour, u8 min, u8 seconds)
{
	digital_tube[0] = seconds % 10;
	digital_tube[1] = seconds / 10;
	digital_tube[2] = 0xfe;
	digital_tube[3] = min % 10;
	digital_tube[4] = min / 10;
	digital_tube[5] = 0xfe;
	digital_tube[6] = hour % 10;
	digital_tube[7] = hour / 10;
}

void change_show()
{
	u8 i;
	if(show_state == SHOW_TIME){
		LatchControl(4, 0xff);
		show_time(time_hour, time_min, time_seconds);
	} else if(show_state == SET_TIME){
		LatchControl(4, 0xff);
		digital_tube[0] = selected == 2 && digital_state == 0? 0xff :time_seconds % 10;
		digital_tube[1] = selected == 2 && digital_state == 0? 0xff :time_seconds / 10;
		digital_tube[2] = 0xfe;
		digital_tube[3] = selected == 1 && digital_state == 0? 0xff :time_min % 10;
		digital_tube[4] = selected == 1 && digital_state == 0? 0xff :time_min / 10;
		digital_tube[5] = 0xfe;
		digital_tube[6] = selected == 0 && digital_state == 0? 0xff :time_hour % 10;
		digital_tube[7] = selected == 0 && digital_state == 0? 0xff :time_hour / 10;
	}	else if(show_state == SET_ALARM){
		LatchControl(4, 0xff);
		digital_tube[0] = selected == 2 && digital_state == 0? 0xff :alarm_seconds % 10;
		digital_tube[1] = selected == 2 && digital_state == 0? 0xff :alarm_seconds / 10;
		digital_tube[2] = 0xfe;
		digital_tube[3] = selected == 1 && digital_state == 0? 0xff :alarm_min % 10;
		digital_tube[4] = selected == 1 && digital_state == 0? 0xff :alarm_min / 10;
		digital_tube[5] = 0xfe;
		digital_tube[6] = selected == 0 && digital_state == 0? 0xff :alarm_hour % 10;
		digital_tube[7] = selected == 0 && digital_state == 0? 0xff :alarm_hour / 10;
	}else if(show_state == TEMPERATURE){
		long temp = temperature;
		LatchControl(4, 0xff);
		digital_tube[0] = 10;
		digital_tube[1] = temp % 10;
		digital_tube[2] = temp / 10;
		for(i = 3; i < 8; i++){
			digital_tube[i] = 0xff;
		}
	}else{
		show_time(time_hour, time_min, time_seconds);
		
		if(led_state == 0){
			LatchControl(4, 0xfe);
		} else {
			LatchControl(4, 0xff);
		}
		
	}
}
	
