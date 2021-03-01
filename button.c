
#include "ds1302.h"
#include "var.h"

unsigned char button_state[4] = {1, 1, 1, 1};
unsigned char button_flag[4] = {0, 0, 0, 0};
unsigned char button_count[4] = {0, 0, 0, 0};

unsigned char read_pin30(void){return P30;}
unsigned char read_pin31(void){return P31;}
unsigned char read_pin32(void){return P32;}
unsigned char read_pin33(void){return P33;}

ReadPin read_pins[4] = {read_pin30, read_pin31, read_pin32, read_pin33};

void check_button()
{
	u8 i;
	for(i = 0;i < 4; i++){
		if(read_pins[i]() != button_state[i]){
			button_count[i]++;
		}else{
			button_count[i] = 0;
		}
		if(button_count[i] == 5){
			button_count[i] = 0;
			button_state[i] = 1 - button_state[i];
			if(button_state[i] == 0){
				button_flag[i] = 1;
			}
		}
	}
	
}
void respond_to_button()
{
	if(button_flag[0] == 1){//7
		switch(show_state){
			case SHOW_TIME:
				show_state = SET_TIME;
				selected = 0;
				break;
			case SET_TIME:
				show_state = selected == 2?SHOW_TIME:SET_TIME;
				selected = (selected + 1)%3;
				break;
			case ALARMING:
				show_state = SHOW_TIME;
				break;
			case SET_ALARM:
				break;
			default:
				show_state = SET_TIME;
				selected = 0;
				break;
		}
		button_flag[0] = 0;
	}
	if(button_flag[1] == 1){//6
		switch(show_state){
			case SET_ALARM:
				show_state = selected == 2?SHOW_TIME:SET_ALARM;
				selected = (selected + 1)%3;
				break;
			case ALARMING:
				show_state = SHOW_TIME;
				break;
			case SET_TIME:
				break;
			default:
				show_state = SET_ALARM;
				selected = 0;
		}
		button_flag[1] = 0;
	}
	if(button_flag[2] == 1){//5
		switch(show_state){
			case SET_TIME:
				add_time();
				break;
			case SET_ALARM:
				add_time();
				break;
			case ALARMING:
				show_state = SHOW_TIME;
				break;
		}
		button_flag[2] = 0;
	}
	if(button_flag[3] == 1){//4
		switch(show_state){
			case SET_TIME:
				sub_time();
				break;
			case SET_ALARM:
				sub_time();
				break;
			case ALARMING:
				show_state = SHOW_TIME;
				break;
		}
		button_flag[3] = 0;
	}
}