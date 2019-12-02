#include <reg52.h>
#include <intrins.h>
#define crystal 11059200L
#define T1MS (65536-5000)*12		// 50ms
#define uchar unsigned char
#define uint unsigned int

uint count = 0;

uint seconds1=0,seconds2=0;			//��ĸ�λ����ʮλ��
uint minutes1=0,minutes2=0;			//�ֵĸ�λ����ʮλ��

uint seconds = 30;					//�趨��ǰ������
uint minutes = 25;					//�趨��ǰ�ķ���
uint hours = 0;						//�趨��ǰ��ʱ��

sbit KEY = P3^2;
sbit DAT=P3^0;			// 74HC164 ��������
sbit CLK=P3^1;			// 74HC164 ʱ������

uchar code table[]={0x81,0xD7,0xC8,0xC2,0x96,0xA2,0xA0,0xC7,0x80,0x82};


void delay(unsigned int mstime)
{
	int i,j;
	for(i=mstime; i>=0; i--)
		for(j=114; j>=0; j--);
}

void initial_TMOD(){
	TMOD = 0x01;   //��ʱ����ģʽ��gate=0;������ʽ1��
	AUXR = 0x7f;   //��ʱ/������1��12��Ƶģʽ
	TL0 =  T1MS;		 //������8λ��ʱƵ��
	TH0 = T1MS >> 8;	 //������8λ��ʱƵ��
	EA = 1;				 //���жϴ�
//	EX0 = 1;
	ET0 = 1;			 //��TF0 ��ʱ��0����ж�����
	TR0 = 1;			//�����ж�
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
		CLK=0;				// ����һ��������
		CLK=1;
		num>>=1;			// �����ݷ��͵��Ĵ���
	}
}

void disp(){
	
	seconds1 = seconds%10;			   //��ĸ�λ
	seconds2 = seconds/10;			   //���ʮλ
	
	minutes1 = minutes%10;			   //�ֵĸ�λ
	minutes2 = minutes/10;			   //�ֵ�ʮλ

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
	initial_TMOD();			  //��ʼ���Ĵ���
	delay(20);
	while(1){
		delay(10);			  //��ʾ
		disp();
	}
}