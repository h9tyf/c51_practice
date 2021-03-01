
//#include "reg52.h"  
//#include <intrins.h>

#include "onewire.h"
#include "var.h"

int SysTick = 0;
u8 temperature;

u8 time_hour = 23;
u8 time_min = 59;
u8 time_seconds = 50;

u8 alarm_hour;
u8 alarm_min;
u8 alarm_seconds;

u8 alarm_end_hour;
u8 alarm_end_min;
u8 alarm_end_seconds;
void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}


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

void func(void) interrupt 1
{
	SysTick++; 
	
	if(state == 8){
		LatchControl(7, ~0x80);
		LatchControl(6, 1<<6);
	} else {
		if(digital_tube[state]==0xff){
			LatchControl(7, 0xff);
		} else if(digital_tube[state] == 0xfe){ 
			LatchControl(7, 0xbf);
		} else if(digital_tube[state] == 10){
			LatchControl(7, 0xc6);
		} else{
			LatchControl(7, display(digital_tube[state]));
		}	
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

long aaa_to_time(u8 hour, u8 min, u8 seconds)
{
	long temp = 0;
	temp += hour * 60 * 60;
	temp += min * 60;
	temp += seconds;
	return  temp;
}

void time_to_aaa(long time)
{
	long temp = time;
	
	if(show_state == SET_TIME){
		time_hour = temp / 3600;	
		Ds1302_Single_Byte_Write(0x84, res2bcd(time_hour));
		temp = temp % 3600;
		time_min = temp / 60;
		Ds1302_Single_Byte_Write(0x82, res2bcd(time_min));
		time_seconds = temp % 60;
		Ds1302_Single_Byte_Write(0x80, res2bcd(time_seconds));
	}else if(show_state == SET_ALARM){
		alarm_hour = temp / 3600;
		temp = temp % 3600;
		alarm_min = temp / 60;
		alarm_seconds = temp % 60;
	}else;
}

void add_time()
{
	long time;
	if(show_state == SET_TIME){
		time = aaa_to_time(time_hour, time_min, time_seconds);
	}else if(show_state == SET_ALARM){
		time = aaa_to_time(alarm_hour, alarm_min, alarm_seconds);
	}else;
	
	if(selected == 0){
		time = (time + 3600) % (23*60*60 + 59*60 + 59);
	} else if (selected == 1){
		time = (time + 60) % (23*60*60 + 59*60 + 59);
	} else {
		time = (time + 1) % (23*60*60 + 59*60 + 59);
	}
	
	time_to_aaa(time);
	update_alarm_end_time();
}

void sub_time()
{
	long time;
	if(show_state == SET_TIME){
		time = aaa_to_time(time_hour, time_min, time_seconds);
	}else if(show_state == SET_ALARM){
		time = aaa_to_time(alarm_hour, alarm_min, alarm_seconds);
	}else;
	time += 24*60*60;
	if(selected == 0){
		time = (time - 3600) % (23*60*60 + 59*60 + 59);
	} else if (selected == 1){
		time = (time - 60) % (23*60*60 + 59*60 + 59);
	} else {
		time = (time - 1) % (23*60*60 + 59*60 + 59);
	}
	
	time_to_aaa(time);
	update_alarm_end_time();
}

void update_alarm_end_time()
{
	long alarm_time;
	alarm_time = aaa_to_time(alarm_hour, alarm_min, alarm_seconds);
	alarm_time = (alarm_time + 5) % (23*60*60 + 59*60 + 59);
	
	alarm_end_hour = alarm_time / 3600;
	alarm_time = alarm_time % 3600;
	alarm_end_min = alarm_time / 60;
	alarm_end_seconds = alarm_time % 60;
}

u8 check_alarm_time()
{
	if(time_hour == alarm_hour && 
		time_min == alarm_min &&
		time_seconds == alarm_seconds){
			return 1;
		} else return 0;
}

void time_adddd()
{
	long temp;
	long time = time_hour * 60 * 60 + time_min * 60 + time_seconds;
	if(time == 23*60*60 + 59*60 + 59){
		time = 0;
	} else {
		time += 1;
	}
	temp = time;
	
	time_hour = temp / 3600;
	temp = temp % 3600;
	time_min = temp / 60;
	time_seconds = temp % 60;
}

u8 bcd2res(u8 bcd){
	return (bcd >> 4) * 10 + (bcd & 0x0f);
}

u8 res2bcd(u8 res){
	return ((res / 10) << 4) + (res % 10);
}

void change_state()
{
	if(show_state == SHOW_TIME && button_state[3] == 0){
			show_state = TEMPERATURE;
		return;
	}
	if(show_state == TEMPERATURE && button_state[3] == 1){
			show_state = SHOW_TIME;
		return;
	}
		
	if(check_alarm_time() == 1 && show_state != SET_TIME && show_state != SET_ALARM){
		show_state = ALARMING;
		return;
	} 
	
	if(show_state == ALARMING && time_hour == alarm_end_hour 	&& time_min == alarm_end_min&&time_seconds == alarm_end_seconds){
			show_state = SHOW_TIME;
		return;
	}
	
	check_button();
	respond_to_button();
}


void main(void)
{
	u8 temp, i;
	LatchControl(4, 0xff);
	EA = 1;
	ET0 = 1;
	P30 = 1;
	P31 = 1;
	P32 = 1;
	P33 = 1;
	Ds1302_Single_Byte_Write(0x84, 0x23);
	Ds1302_Single_Byte_Write(0x82, 0x59);
	Ds1302_Single_Byte_Write(0x80, 0x50);
	Timer0Init();
	update_alarm_end_time();
	//show_state = 1;
  while(1){
		long tickBkp = SysTick;
		
		if(tickBkp % 200 == 0){
			EA = 0;
			led_state = 1 - led_state;
			EA = 1;
		}
		
		if(tickBkp % 1000 == 0){
			EA = 0;
			digital_state = 1 - digital_state;
			EA = 1;
		}
		
		if(tickBkp % 50 == 0){
			u8 addr = 0xbf;
			EA = 0;
			
			RST_CLR;			/*RST脚置低，实现DS1302的初始化*/
			SCK_CLR;			/*SCK脚置低，实现DS1302的初始化*/

			RST_SET;	/*启动DS1302总线,RST=1电平置高 */	
			addr = addr | 0x01;	 
			Write_Ds1302_Byte(addr); /*写入目标地址：addr,保证是读操作,写之前将最低位置高*/
			time_seconds = bcd2res(Read_Ds1302_Byte());
			time_min = bcd2res(Read_Ds1302_Byte());
			time_hour = bcd2res(Read_Ds1302_Byte());
			RST_CLR;	/*停止DS1302总线*/
			/*
			time_seconds = bcd2res(Ds1302_Single_Byte_Read(0x81));
			time_min = bcd2res(Ds1302_Single_Byte_Read(0x83));
			time_hour = bcd2res(Ds1302_Single_Byte_Read(0x85));
			*/
			
			EA = 1;
		}
		
		if(tickBkp % 2000 == 0){
			EA = 0;
			temperature = rd_temperature();
			EA = 1;
		}
		
		change_state();
		change_show();
		
		
		while(tickBkp == SysTick);
	}
}