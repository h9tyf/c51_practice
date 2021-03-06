
#include "ds1302.h"
#include "var.h"

u8 last_show_state;

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
		if(show_state == TEMPERATURE){
			show_state = last_show_state;
		} else {
			last_show_state = show_state;
			show_state = TEMPERATURE;
		}
		button_flag[0] = 0;
	}
	if(button_flag[1] == 1){//6
		time_hour = end_hour;
		time_min = end_min;
		time_seconds = end_seconds;
		Ds1302_Single_Byte_Write(0x84, res2bcd(end_hour));
		Ds1302_Single_Byte_Write(0x82, res2bcd(end_min));
		Ds1302_Single_Byte_Write(0x80, res2bcd(end_seconds));
		button_flag[1] = 0;
	}
	if(button_flag[2] == 1){//5
		Ds1302_Single_Byte_Write(0x84, 0x00);
		Ds1302_Single_Byte_Write(0x82, 0x00);
		Ds1302_Single_Byte_Write(0x80, 0x00);
		end_min = (end_min + 1) % 3;
		button_flag[2] = 0;
	}
	if(button_flag[3] == 1){//4
		show_state = show_state % 3 + 1;
		button_flag[3] = 0;
	}
}