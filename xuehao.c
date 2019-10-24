#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char
#define DELAY_TIME 10


sbit DAT=P3^0;			// 74HC164 数据输入
sbit CLK=P3^1;			// 74HC164 时钟输入
//sbit DP=P3^7;			// 小数点
//sbit DP1=P3^5;
sbit DPY0 = P1^0;		// 独立位选
sbit DPY1 = P1^1;
sbit DPY2 = P1^2;
sbit DPY3 = P1^3;
sbit KEY = P3^2;		// INT0 按键,按下为低电平

// 数码管段选数组	数字0~9和全灭
uchar code tab[]={0x01,0x57,0xC8,0xC2,0x96,0xA2,0xA0,0xC7,0x80,0x82,0xff};


void delay_ms(unsigned int mstime)
{
	int i,j;
	for(i=mstime; i>=0; i--)
		for(j=112; j>=0; j--);
}
void SendByte_74HC164(uchar byte)
{
	uchar num,c;
	num=tab[byte];
	for(c=0; c<8; c++)
	{
		DAT=num&0x01;			// P3^0 --> 0000 0001x
		CLK=0;					// 制造一个上升沿
		CLK=1;					// 将数据发送到寄存器
		num>>=1;			
	}
}

void test_light_byte1()
{
		P1 &= 0xf0;							// 关掉位选
		SendByte_74HC164(2);		// 开启段选
		P1 |= 0x08;							// 开启位选
		delay_ms(DELAY_TIME);		// 延时显示
		P1 &= 0xf0;							// 关掉位选
		SendByte_74HC164(0);
		P1 |= 0x04;
		delay_ms(DELAY_TIME);
		P1 &= 0xf0;
		SendByte_74HC164(1);
		P1 |= 0x02;
		delay_ms(DELAY_TIME);
		P1 &= 0xf0;
		SendByte_74HC164(7);
		P1 |= 0x01;
		delay_ms(DELAY_TIME);
}
void test_light_byte2()
{
		P1 &= 0xf0;							// 关掉位选
		SendByte_74HC164(2);		// 开启段选
		P1 |= 0x08;							// 开启位选
		delay_ms(DELAY_TIME);		// 延时显示
		P1 &= 0xf0;							// 关掉位选
		SendByte_74HC164(3);
		P1 |= 0x04;
		delay_ms(DELAY_TIME);
		P1 &= 0xf0;
		SendByte_74HC164(0);
		P1 |= 0x02;
		delay_ms(DELAY_TIME);
		P1 &= 0xf0;
		SendByte_74HC164(1);
		P1 |= 0x01;
		delay_ms(DELAY_TIME);
}
void test_light_byte3()
{
		P1 &= 0xf0;							// 关掉位选
		SendByte_74HC164(0);		// 开启段选
		P1 |= 0x08;							// 开启位选
		delay_ms(DELAY_TIME);		// 延时显示
		P1 &= 0xf0;							// 关掉位选
		SendByte_74HC164(2);
		P1 |= 0x04;
		delay_ms(DELAY_TIME);
		P1 &= 0xf0;
		SendByte_74HC164(4);
		P1 |= 0x02;
		delay_ms(DELAY_TIME);
		P1 &= 0xf0;
		SendByte_74HC164(0);
		P1 |= 0x01;
		delay_ms(DELAY_TIME);
}
void xunhuan1()
{
	int i = 0;
	while(i<100){ test_light_byte1(); i++;}
}
void xunhuan2()
{
	int i = 0;
	while(i<100){ test_light_byte2(); i++;}
}
void xunhuan3()
{
	int i = 0;
	while(i<100){ test_light_byte3(); i++;}
}

void main()
{	
	while(1)
	{
		xunhuan1();
		xunhuan2();
		xunhuan3();
	}
}