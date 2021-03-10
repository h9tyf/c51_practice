
#include "ds1302.h"
#include "var.h"

u8 last_show_state;


unsigned char button_state[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned char button_flag[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char button_count[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned char button_check[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

unsigned char read_pin30(void){return P30;}
unsigned char read_pin31(void){return P31;}
unsigned char read_pin32(void){return P32;}
unsigned char read_pin33(void){return P33;}

unsigned char read_pin34(void){return P34;}
unsigned char read_pin35(void){return P35;}
unsigned char read_pin42(void){return P42;}
unsigned char read_pin44(void){return P44;}

ReadPin read_pins_row[4] = {read_pin33, read_pin32, read_pin31, read_pin30};
ReadPin read_pins_col[4] = {read_pin44, read_pin42, read_pin35, read_pin34};

void check_button()
{
	u8 i, temp;
	u8 row, col;
	
	P3 = 0x0f;
	//P42 = 0;
	//P44 = 0;
	
	if(P3 != 0x0f){
		if((P3 & 0x08) == 0) temp = 0;
		if((P3 & 0x04) == 0) temp = 1;
		if((P3 & 0x02) == 0) temp = 2;
		if((P3 & 0x01) == 0) temp = 3;
		
		P3 = 0xf0;
		P42 = 1;
		P44 = 1;
		
		//if(P44 == 0) temp += 8;
		//if(P42 == 0) temp += 12;
		
		if(P34 == 0) temp += 0;
		if(P35 == 0) temp += 4;
		if(P36 == 0) temp += 8;
		if(P37 == 0) temp += 12;
	}
	
	for(i = 0; i < 16; i++){
		if(i == temp){
			button_check[i] = 0;
		} else {
			button_check[i] = 1;
		}
	}
	
	for(i = 0; i < 16; i++){
		if(button_check[i] != button_state[i]){
			button_count[i]++;
		} else {
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
	u8 i;
	for(i = 0; i < 16; i++){
		if(button_flag[i] == 1){
			button_flag[i] = 0;
			show_num = i + 4;
			return;
		}
	}
	show_num = 99;
}