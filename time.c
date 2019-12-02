#include <reg52.h>
#include <intrins.h>
#define crystal 11059200L
#define T1MS (65536-5000)*12		// 50ms
#define uchar unsigned char
#define uint unsigned int

uint count = 0;

uint seconds1=0,seconds2=0;			//秒的个位数与十位数
uint minutes1=0,minutes2=0;			//分的个位数与十位数

uint seconds = 30;					//设定当前的秒数
uint minutes = 25;					//设定当前的分数
uint hours = 0;						//设定当前的时数

sbit KEY = P3^2;
sbit DAT=P3^0;			// 74HC164 数据输入
sbit CLK=P3^1;			// 74HC164 时钟输入

uchar code table[]={0x81,0xD7,0xC8,0xC2,0x96,0xA2,0xA0,0xC7,0x80,0x82};


void delay(unsigned int mstime)
{
	int i,j;
	for(i=mstime; i>=0; i--)
		for(j=114; j>=0; j--);
}

void initial_TMOD(){
	TMOD = 0x01;   //计时工作模式，gate=0;工作方式1；
	AUXR = 0x7f;   //定时/计数器1的12分频模式
	TL0 =  T1MS;		 //给定高8位计时频率
	TH0 = T1MS >> 8;	 //给定低8位计时频率
	EA = 1;				 //总中断打开
//	EX0 = 1;
	ET0 = 1;			 //打开TF0 计时器0溢出中断请求
	TR0 = 1;			//开启中断
}

void Timer1_ISR (void) interrupt 1{
	count ++;
	
	if(count >= 20){
		count = 0;
		seconds ++;
		if(seconds >= 60){
			minutes++;
			if(minutes >= 60){
				hours++;
				if(hours >= 60){
					hours = 0;
				}
				minutes = 0;
			}
			seconds = 0;
		}
	}
}

void SendByte_74HC164(uchar byte)
{
	uchar num,c;
	num = table[byte];
	for(c=0; c<8; c++)
	{
		DAT=num&0x01;		// P3^0 --> 0000 000x
		CLK=0;				// 制造一个上升沿
		CLK=1;
		num>>=1;			// 将数据发送到寄存器
	}
}

void disp(){
	
	seconds1 = seconds%10;			   //秒的个位
	seconds2 = seconds/10;			   //秒的十位
	
	minutes1 = minutes%10;			   //分的个位
	minutes2 = minutes/10;			   //分的十位

	P1 |= 0x0f;
	SendByte_74HC164(seconds1);
	P1 &= 0xfE;
	delay(6);
	P1 |= 0x0f;

	P1 |= 0x0f;
	SendByte_74HC164(seconds2);
	P1 &= 0xfD;
	delay(6);
	P1 |= 0x0f;

	P1 |= 0x0f;
	SendByte_74HC164(minutes1);
	P1 &= 0xfB;
	delay(6);
	P1 |= 0x0f;

	P1 |= 0x0f;
	SendByte_74HC164(minutes2);
	P1 &= 0xf7;
	delay(6);
	P1 |= 0x0f;
}

void main(){
	initial_TMOD();			  //初始化寄存器
	delay(20);
	while(1){
		delay(10);			  //显示
		disp();
	}
}