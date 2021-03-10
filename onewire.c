/*
  程序说明: 单总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台
  日    期: 2011-8-9
*/

#include "onewire.h"

//单总线延时函数
void Delay_OneWire_0(unsigned int t)
{
  while(t--);
}
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--)
	{
		for(i=0;i<10;i++);
	}
}

//DS18B20芯片初始化
bit Init_DS18B20(void)
{
	bit initflag = 0;
		ET0=0;

	DQ = 1;
	Delay_OneWire(12);
	DQ = 0;
	Delay_OneWire(80); 
	DQ = 1;
	Delay_OneWire(10); 
	initflag = DQ;    
	Delay_OneWire(5);
  ET0=1;
	return initflag;
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
		ET0=0;

	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
	ET0=1;
}

//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  ET0=0;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	ET0=1;
	return dat;
}

//DS18B20??????:??
unsigned char rd_temperature(void)
{
    unsigned char low,high;
  	char temp;
  
  	Init_DS18B20();
  	Write_DS18B20(0xCC);
  	Write_DS18B20(0x44); //??????
  	Delay_OneWire(200);

  	Init_DS18B20();
  	Write_DS18B20(0xCC);
  	Write_DS18B20(0xBE); //?????

  	low = Read_DS18B20(); //???
  	high = Read_DS18B20(); //???
  
  	temp = high<<4;
  	temp |= (low>>4);
  
  	return temp;
}