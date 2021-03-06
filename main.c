
//#include "reg52.h"  
//#include <intrins.h>

#include "onewire.h"
#include "var.h"

int SysTick = 0;
u8 temperature;

u8 time_hour;
u8 time_min;
u8 time_seconds;

u8 end_hour;
u8 end_min;
u8 end_seconds;

long left_time;

sbit pwm_out = P3^4;
u8 pwm_duty = 0;

u8 display(int i){
	u8 a[10] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
	return a[i];
}

void LatchControl(int num, u8 value)
{
	P0 = value;
	_nop_();
	P2 = ((P2&0x1f)|(num<<5));
	_nop_();
	P2 &= 0x1f;
}

u8 state = 0;
u8 count = 0;

void func(void) interrupt 1
{
	SysTick++; 
	
	if(state == 8){
		LatchControl(7, 0xff);
	
		LatchControl(7, ~0x80);
		LatchControl(6, 1<<6);
	} else {
		LatchControl(7, 0xff);
		LatchControl(6, 1<<(7-state));
		if(digital_tube[state]==0xff){
			LatchControl(7, 0xff);
		} else if(digital_tube[state] == 0xfe){ 
			LatchControl(7, 0xbf);
		} else if(digital_tube[state] == 10){
			LatchControl(7, 0xc6);
		} else{
			LatchControl(7, display(digital_tube[state]));
		}	
		
	}
	
	state = (state + 1) % 8;
	
	count++;  
	if(count == pwm_duty)  
	{  
		pwm_out = 0;
	} else if(count == 100){  
		pwm_out = 1;
		count = 0;  
	} 
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

long aaa_to_time(u8 hour, u8 min, u8 seconds)
{
	long temp = 0;
	temp += hour * 60L * 60L;
	temp += min * 60L;
	temp += seconds;
	return  temp;
}

u8 bcd2res(u8 bcd){
	return (bcd >> 4) * 10 + (bcd & 0x0f);
}

u8 res2bcd(u8 res){
	return ((res / 10) << 4) + (res % 10);
}

void change_state()
{	
	check_button();
	respond_to_button();
}

void main()  
{  
	Timer0Init();
	LatchControl(4, 0xff);
	EA = 1;
	ET0 = 1;
	P30 = 1;
	P31 = 1;
	P32 = 1;
	P33 = 1;
	while(1)  
	{
		long tickBkp = SysTick;
		if(tickBkp % 50 == 0){
			u8 addr = 0xbf;
			EA = 0;
			
			RST_CLR;
			SCK_CLR;

			RST_SET;
			addr = addr | 0x01;	 
			Write_Ds1302_Byte(addr);
			time_seconds = bcd2res(Read_Ds1302_Byte());
			time_min = bcd2res(Read_Ds1302_Byte());
			time_hour = bcd2res(Read_Ds1302_Byte());
			RST_CLR;
			
			EA = 1;
		}
		
		if(tickBkp % 2000 == 0){
			EA = 0;
			temperature = rd_temperature();
			EA = 1;
		}
		
		EA = 0;
		change_state();
		change_show();
		EA = 1;
	}
}  