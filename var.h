#ifndef __VAR_H
#define __VAR_H

typedef unsigned char u8;
extern u8 temperature;

extern u8 time_hour;
extern u8 time_min;
extern u8 time_seconds;

extern u8 end_hour;
extern u8 end_min;
extern u8 end_seconds;

extern long left_time;

extern u8 pwm_duty;

//main
void LatchControl(int num, u8 value);
void func(void);
void Timer0Init(void);
long tube_to_num();
void num_to_tube(long num);
long aaa_to_time(u8 a, u8 b, u8 c);
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

enum show
{
	SLEEP=1, NATURE, STEADY, TEMPERATURE
};

extern enum show show_state;
extern u8 work_state;

void show_temperature();
void change_show();
void set_show();
void cal_left_time();


#endif