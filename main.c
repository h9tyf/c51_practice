
#include "reg52.h"  
#include <intrins.h>

sfr AUXR =0x8E;
int SysTick = 0;

typedef unsigned char u8;

u8 digital_tube[8] = {0, 0, 0, 0, 0, 0, 0, 0};

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
int count = 0;
u8 change = 0;

void func(void) interrupt 1
{
	SysTick++;
	LatchControl(7, display(digital_tube[state]));
	
	LatchControl(6, 1<<(7-state));
	
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
	for(i = 0; i < 8; i++){
		digital_tube[i] = num % 10;
		num = num / 10;
	}
}

void update()
{
	long num = tube_to_num();
	if(num == 99999999){
		num = 0;
	} else {
		num += 1;
	}
	num_to_tube(num);
}

void main(void)
{
	LatchControl(4, 0xff);
	EA = 1;
	ET0 = 1;
	Timer0Init();
  while(1){
		long tickBkp = SysTick;
		
		if(tickBkp % 1000 == 0){
			update();
		}
		
		while(tickBkp == SysTick);
	}
}