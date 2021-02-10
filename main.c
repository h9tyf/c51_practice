
//#include "reg52.h"  
//#include <intrins.h>

#include "ds1302.h"
#include "onewire.h"

int SysTick = 0;

typedef unsigned char u8;

u8 digital_tube[8] = {0, 0, 0, 0, 0, 0, 0, 0};

u8 button_state[4] = {1, 1, 1, 1};
u8 button_flag[4] = {0, 0, 0, 0};
u8 button_count[4] = {0, 0, 0, 0};

u8 read_pin30(void){return P30;}
u8 read_pin31(void){return P31;}
u8 read_pin32(void){return P32;}
u8 read_pin33(void){return P33;}

typedef unsigned char (*ReadPin)(void);

ReadPin read_pins[4] = {read_pin30, read_pin31, read_pin32, read_pin33};

u8 s_10;//seconds
u8 s_1;

u8 temperature;

int display(int i){
	u8 a[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
	return a[i];
}


void LatchControl(int num, int value)
{
	P0 = value;
	_nop_();
	P2 = ((P2&0x1f)|(num<<5));
	_nop_();
	P2 &= 0x1f;
}

u8 state = 0;

void func(void) interrupt 1
{
	SysTick++;
	
	
	if(state == 8){
		LatchControl(7, ~0x80);
		LatchControl(6, 1<<6);
	} else {
		LatchControl(7, display(digital_tube[state]));
		LatchControl(6, 1<<(7-state));
	}
	
	state = (state + 1) % 8;
}

void Timer0Init(void)
{
	AUXR |= 0x80;  
  TMOD &= 0xF0;
	//TMOD |= 0x01;
  TL0 = 0xCD;		//??????
	TH0 = 0xD4;		//??????
  TF0 = 0;
  TR0 = 1;
  IE0 = 1;
  ET0 = 1;
}

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
	long num = tube_to_num();
	if(button_flag[0] == 1){
		num++;
		button_flag[0] = 0;
	}
	if(button_flag[1] == 1){
		num--;
		button_flag[1] = 0;
	}
	num_to_tube(num);
}

void read_seconds()
{
	u8 time = Ds1302_Single_Byte_Read(0x81);
	s_1 = time & 0x0f;
	s_10 = time>>4;
}

void show_time()
{
	long time = s_10 * 10 + s_1;
	num_to_tube(time);
}
/*
void get_temperature()
{
	u8 low, high;
	int temp;
	
	Init_DS18B20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	Delay_OneWire(1300);
	
	Init_DS18B20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	
	low = Read_DS18B20();
	high = Read_DS18B20();
	temp = high<<4;
	temp = temp<<8 + low;
	temperature = temp * 0.625;
}
*/

void show_temperature()
{
	long temp = temperature;
	num_to_tube(temp);
}


void main(void)
{
	LatchControl(4, 0xff);
	EA = 1;
	ET0 = 1;
	P30 = 1;
	P31 = 1;
	P32 = 1;
	P33 = 1;
	//Ds1302_Single_Byte_Write(0x80, 0x23);
	Timer0Init();
  while(1){
		long tickBkp = SysTick;
		
		//check_button();
		//respond_to_button();
		
		//read_seconds();
		//show_time();
		if(tickBkp % 1000 == 0){
			EA = 0;
			temperature = rd_temperature();
			show_temperature();
			EA = 1;
		}
		
		
		while(tickBkp == SysTick);
	}
}