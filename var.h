#ifndef __VAR_H
#define __VAR_H

typedef unsigned char u8;
extern u8 temperature;

extern u8 time_hour;
extern u8 time_min;
extern u8 time_seconds;

extern u8 alarm_hour;
extern u8 alarm_min;
extern u8 alarm_seconds;

extern u8 alarm_end_hour;
extern u8 alarm_end_min;
extern u8 alarm_end_seconds;

//main
u8 display(int i);
void LatchControl(int num, u8 value);
void func(void);
void Timer0Init(void);
long tube_to_num();
void num_to_tube(long num);
long aaa_to_time();
void time_to_aaa(long time);
void add_time();
void sub_time();
u8 check_alarm_time();
void time_adddd();
u8 bcd2res(u8 bcd);
u8 res2bcd(u8 res);
void change_state();
void update_alarm_end_time();


//button
extern unsigned char button_state[4];
extern unsigned char button_flag[4];
extern unsigned char button_count[4];

extern unsigned char read_pin30(void);
extern unsigned char read_pin31(void);
extern unsigned char read_pin32(void);
extern unsigned char read_pin33(void);

typedef unsigned char (*ReadPin)(void);

extern ReadPin read_pins[4];

void respond_to_button();

void check_button();

//show

extern unsigned char digital_tube[8];
extern unsigned char selected;//1:hour 2:min 3:seconds

extern unsigned char led_state;//0: 1:light
extern unsigned char led_sum_time;
extern unsigned char led_time;
extern unsigned char digital_state;//0:light 1:

enum show
{
	SHOW_TIME=1, SET_TIME, SET_ALARM, TEMPERATURE, ALARMING
};

extern enum show show_state;

void show_temperature();
void change_show();
void set_show();


#endif