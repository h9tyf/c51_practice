
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

void func(void)
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
/* mine
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
*/
long aaa_to_time(u8 hour, u8 min, u8 seconds)
{
	long temp = 0;
	temp += hour * 60L * 60L;
	temp += min * 60L;
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
		time = (time + 3600) % (23L*60L*60L + 59*60L + 59L);
	} else if (selected == 1){
		time = (time + 60) % (23L*60L*60L + 59L*60L + 59L);
	} else {
		time = (time + 1) % (23L*60L*60L + 59L*60L + 59L);
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
	time += 24L*60L*60L;
	if(selected == 0){
		time = (time - 3600) % (23L*60L*60L + 59L*60L + 59L);
	} else if (selected == 1){
		time = (time - 60) % (23L*60L*60L + 59L*60L + 59L);
	} else {
		time = (time - 1) % (23L*60L*60L + 59L*60L + 59L);
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
	long time = time_hour * 60 * 60L + time_min * 60L + time_seconds * 1L;
	if(time == 23*60*60L + 59*60L + 59L){
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

sbit L1 = P0^0;  
sbit S7 = P3^0;  
//573????  
void SelectHC573()  
{  
P2 = (P2 & 0x1f) | 0x80;  
}  
  
//============???????================  
unsigned char count = 0;        //??????  
unsigned char pwm_duty = 0; //?????  
void InitTimer0()  
{  
TMOD = 0x01;            //????,???0?16?????,TR0??  
TH0 = (65535 - 100) / 256;  //100us?????,?8?  
TL0 = (65535 - 100) % 256;  //?8?  
  
ET0 = 1;                    //?????  
EA = 1;                 //?????  
}  
  
void ServiceTimer0() interrupt 1  
{  
TH0 = (65535 - 100) / 256;  
TL0 = (65535 - 100) % 256;  
  
count++;  
if(count == pwm_duty)  
{  
L1 = 1;  
}  
else if(count == 100)       //?0,??????  
{  
L1 = 0;  
count = 0;  
}  
}  
//==========================================  
  
//============???????================  
//??  
void Delay(unsigned int t)  
{  
while(t--);  
}  
  
unsigned char stat = 0;     //?????0,??  
void ScanKeys()  
{  
if(S7 == 0)  
{  
Delay(100);  
if(S7 == 0)  
{  
switch(stat)  
{  
case 0:  
L1 = 0;  
TR0 = 1;  
pwm_duty = 10;  
stat = 1;  
break;  
  
case 1:  
pwm_duty = 50;  
stat = 2;  
break;  
  
case 2:  
pwm_duty = 90;  
stat = 3;  
break;  
  
case 3:  
L1 = 1;  
TR0 = 0;  
stat = 0;  
break;  
}  
while(S7 == 0);  
}  
}  
}  
//============================================  
//?????  
void main()  
{  
	SelectHC573();  
	L1 = 1;  
	InitTimer0();  
	while(1)  
	{  
		ScanKeys();  
	}  
}  