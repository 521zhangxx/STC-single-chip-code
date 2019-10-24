#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char
#define DELAY_TIME 10


sbit DAT=P3^0;			// 74HC164 ��������
sbit CLK=P3^1;			// 74HC164 ʱ������
//sbit DP=P3^7;			// С����
//sbit DP1=P3^5;
sbit DPY0 = P1^0;		// ����λѡ
sbit DPY1 = P1^1;
sbit DPY2 = P1^2;
sbit DPY3 = P1^3;
sbit KEY = P3^2;		// INT0 ����,����Ϊ�͵�ƽ

// ����ܶ�ѡ����	����0~9��ȫ��
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
		CLK=0;					// ����һ��������
		CLK=1;					// �����ݷ��͵��Ĵ���
		num>>=1;			
	}
}

void test_light_byte4(){
	P1 &= 0xf0;
	SendByte_74HC164(0);
	P1 |= 0x08;
	P1 &= 0xf0;
}
void test_light_byte3(){
	P1 &= 0xf0;
	SendByte_74HC164(2);
	P1 |= 0x04;
	P1 &= 0xf0;
}
void test_light_byte2(){
	P1 &= 0xf0;
	SendByte_74HC164(4);
	P1 |= 0x02;
	P1 &= 0xf0;
}
void test_light_byte1(){
	P1 &= 0xf0;
	SendByte_74HC164(0);
	P1 |= 0x01;
	P1 &= 0xf0;
}

void main()
{
	
	while(1){
		while(KEY==0);
		while(1){
			 test_light_byte4();
			 if(KEY==0){break;}
		}
		while(KEY==0);
		while(1){
			 test_light_byte3();
			 if(KEY==0){break;}
		}
		while(KEY==0);
		while(1){
			 test_light_byte2();
			 if(KEY==0){break;}
		}
		while(KEY==0);
		while(1){
			 test_light_byte1();
			 if(KEY==0){ break;}
		} 
	}
}