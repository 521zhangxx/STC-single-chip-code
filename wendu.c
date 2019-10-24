#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
uint temp;							// 温度变量
sbit DS = P1^7;					// DS18B20 数据端口
sbit DAT = P3^0;				// 74HC164 数据输入端口
sbit CLK = P3^1;				// 74HC164 时钟输入端口
sbit DP1 = P3^5;				// 小数点

//定义数码管段选(不加小数点0~9)
unsigned char code table1[]={0x81,0xD7,0xC8,0xC2,0x96,0xA2,0xA0,0xC7,0x80,0x82};


/**********************************************************
*  函数名称：低精度延时函数
*  日期：2019-9-7
*  姓名：ZhangHJ
*  说明：嵌套循环延时
***********************************************************/
void delay(unsigned int mstime)
{
	int i,j;
	for(i=mstime; i>=0; i--)
		for(j=114; j>=0; j--);
}

/**********************************************************
*  函数名称：1ms高精度延时函数(1.004ms)(11.0592MHz)
*  日期：2019-10-4
*  姓名：ZhangHJ
*  说明：嵌套循环延时,延时1.004ms已用示波器测量
***********************************************************/
//void delay_ms()
//{
//	int i,j;
//	for(i=1; i>=0; i--)
//		for(j=123; j>=0; j--);
//}


/**********************************************************
*  函数名称：12us高精度延时函数(12.04us)(11.0592MHz)
*  日期：2019-10-4
*  姓名：ZhangHJ
*  说明：嵌套循环延时,延时12.04us已用示波器测量
***********************************************************/
//void delay_12us()
//{
//	int i;
//	i++;i++;
//}

/**********************************************************
*  函数名称：6us高精度延时函数(6.280us)(11.0592MHz)
*  日期：2019-10-4
*  姓名：ZhangHJ
*  说明：嵌套循环延时,延时6.280us已用示波器测量
***********************************************************/
//void delay_6us()
//{
//	_nop_();
//	_nop_();
//}

//_nop_();			//2.340us


/**********************************************************
*  函数名称：74HC164发送Byte函数
*  日期：2019-9-29
*  姓名：ZhangHJ
*  说明：74HC164移位寄存器输入一字节数据发给寄存器
***********************************************************/
void SendByte_74HC164(uchar byte)
{
	uchar num,c;
	num=table1[byte];
	for(c=0; c<8; c++)
	{
		DAT=num&0x01;		// P3^0 --> 0000 000x
		CLK=0;					// 制造一个上升沿
		CLK=1;
		num>>=1;				// 将数据发送到寄存器
	}
}


/**********************************************************
*  函数名称：发送复位和初始化命令函数 dsinit
*  修改日期：2019-9-9
*  修改人：ZhangHJ
*  说明：1. 对于单片机: 单片机首先发出480-960us的低电平脉冲,
*				 		释放总线为高电平(上拉电阻拉高),在随后的480us进行检测,
*				 		如果出现低电平，说明器件应答正常.B
*				 2. 对于DS18B20: 上电后就检测是否有480/960us的低电平脉冲,
*						如果有低电平,在总线释放之后，等待15-60us,
*						将电平拉低60-240us,告诉主机已经准备好.
***********************************************************/
uchar dsreset(void)					// send reset and initialization command
{
  uint i;
  DS = 0;										// 先将端口拉低
  i=120;										// 维持低电平状态480us~960us
  while(i>0)i--;
  DS = 1;										// 然后释放总线(将总线拉高),若DS18B20做出反应,将会将在15us~60us后将总线拉低
	
	i = 0;
	while(DS)									// 在DS高电平时等待
	{
		i++;
		if(i > 50000)						// 等待时间大于60us,说明响应失败
		{
			return 0;
		}
	}
	return 1;
}



/**********************************************************
*  函数名称：读1bit数据函数
*  修改日期：2019-9-9
*  修改人：ZhangHJ
*  说明：1.首先单片机端口拉低 1 us (i++),
*				 2.然后释放总线,拉高总线,
*				 3.等待几位秒，是为了让 DS18B20 数据稳定,
*				 4.在15 us 内读取DS数据,
*				 5.接下来进行延时等待采样周期完成。
*				 详见DS18B20资料2.8.3.1读/写时间片
***********************************************************/
bit tmpreadbit(void)				// read a bit data
{
	uint i;
	bit dat;									// 定义位数据 (dat = 0 or 1)
	DS = 0;										// 先将端口拉低
	_nop_();									// 延时 2us ,要求至少保持1us
	DS = 1;										// 再将端口拉高
	i=8;while(i>0)i--;				// 等待DS数据稳定,要求的至少延时15us以上
	dat = DS;									// 数据传输
	i=15;while(i>0)i--;				// 等待数据采样周期完成,要求不低于60us
	return (dat);
}



/**********************************************************
*  函数名称：读1Byte数据函数
*  修改日期：2019-9-9
*  修改人：ZhangHJ
*  说明：1.首先初始化字节数据变量dat为 0
*				 2.循环 8 次,调用tmpreadbit函数,读 8bit 数据
*				 3.读出的数据暂存到 j ,之后进行移位和按位或操作
*				 4.效果是 j 先读入的数据,放到了dat的低位,读8次正好是1byte.
*				 5.最后返回读到的字节数据
***********************************************************/
uchar tmpread(void)					// read a byte date
{
	uchar i,j,dat;
  dat = 0;									// 初始化数据变量为 0
  for(i=1;i<=8;i++)					// 循环 8 次,调用tmpreadbit函数,读 8bit 数据
  {
    j = tmpreadbit();				// 读出的数据暂存到 j
    dat = (dat>>1)|(j<<7);	// 效果是 j 先读入的数据,放到了dat的低位,读8次正好是1byte.
  }
  return(dat);							// 返回读到的字节
}



/**********************************************************
*  函数名称：写入1Byte数据函数
*  修改日期：2019-9-11
*  修改人：ZhangHJ
*  说明：1.将对待写入数据dat进行位操作,将dat末位数值赋值给位数据testb
*				 2.通过判断testb得到写 0 还是写 1
*				 3.若是写 0 操作,将 DS 拉低,进行<60us的延时,再将DS拉高,进行>1us的延时
*				 4.若是写 1 操作,将 DS 拉低,进行>1us的延时,再将DS拉高,进行<60us的延时
*				 5.循环执行2、3、4操作8次,写入1字节数据
*				 详见18B20资料“2.8.3.1读/写时间片”章节
***********************************************************/
void tmpwritebyte(uchar dat)   //write a byte to ds18b20
{
  uint i;
  uchar j;
  bit testb;
  for(j=1;j<=8;j++)
  {
    testb = dat&0x01;
    dat = dat>>1;
    if(testb)     						// write 1
    {
      DS=0;
      i=8;while(i>0)i--;;			// 延时要求15~60us内
      DS=1;
      i=15;while(i>0)i--;			// 要求不低于60us
    }
    else
    {
      DS = 0;       					// write 0
      i=15;while(i>0)i--;
      DS = 1;
      i++;i++;
    }
  }
}



/**********************************************************
*  函数名称：18B20温度转换函数
*  修改日期：2019-9-11
*  修改人：ZhangHJ
*  说明：1.首先进行18B20初始化
*				 2.进行适当延时
*				 3.发送跳过光刻ROM指令
*				 4.发送RAM指令,进行温度转换
*				 详见18B20资料“2.8.3.1存储器操作命令”章节
***********************************************************/
void tmpchange(void)					// DS18B20 begin change
{
  while(dsreset() == 0)
	{
		SendByte_74HC164(0);
		P1 &= 0xfB;
	}
  delay(1);
  tmpwritebyte(0xCC);					// 跳过 ROM 操作
  tmpwritebyte(0x44);					// 启动一次温度转换
}



/**********************************************************
*  函数名称：18B20温度转换完整过程函数
*  修改日期：2019-9-11
*  修改人：ZhangHJ
*  说明：1.首先进行18B20初始化
*				 2.进行适当延时
*				 3.发送跳过光刻ROM指令
*				 4.发送RAM指令,进行温度转换
*				 5.读取两个8位数据,放到16位寄存器 temp 中
*				 6.将读取到的二进制数据(默认为正数),转换为十进制数据
*				 7.返回温度数据
*				 详见18B20资料“2.8.3.1存储器操作命令”章节
***********************************************************/
uint tmp()										// get the temperature
{
  float tt;
  uchar high,low;
	//P1 |= 0x0f;
  while(dsreset() == 0)
	{
		SendByte_74HC164(0);
		P1 &= 0xfB;
	}
  delay(1);
	//tmpchange();
  tmpwritebyte(0xCC);					// 跳过 ROM 操作
  tmpwritebyte(0xBE);					// 读暂存寄存器
  low=tmpread();
  high=tmpread();
  temp=high;
  temp<<=8;										// two byte compose a int variable
  temp=temp|low;
  tt=temp*0.0625;
  temp=tt*10+0.5;
  return temp;
}



/**********************************************************
*  函数名称：数码管温度数据显示函数
*  修改日期：2019-9-11
*  修改人：ZhangHJ
*  说明：1.temp表示需要显示的温度数值(百位数值)
*				 2.A1、A2、A3分别了百位、十位、个位数值
*				 3.控制段选信号dula和位选信号wela,以使数码管显示
***********************************************************/
void display(uint temp)				// 显示程序
{
	uchar A1,A2,A2t,A3,ser;
	ser=temp/10;
	SBUF=ser;
	A1=temp/100;								// A1 --> 百位
	A2t=temp%100;								// A2t --> 后两位
	A2=A2t/10;									// A2 --> 十位
	A3=A2t%10;									// A3 --> 个位
	
	DP1 = 0;										// 小数点
   
	// 控制数码管显示温度数值
	P1 &= 0x00;
	SendByte_74HC164(A1);
	P1 |= 0xf4;
	delay(6);
	P1 &= 0x00;
	
	SendByte_74HC164(A2);
	P1 |= 0xf2;
	delay(6);
	P1 &= 0x00;
	
	SendByte_74HC164(A3);
	P1 |= 0xf1;
	delay(6);
	P1 &= 0x00;
	
	// 位选高电平
//	P1 &= 0x00;
//	SendByte_74HC164(A1);
//	P1 |= 0xf4;
//	delay(6);
//	P1 &= 0x00;
//	
//	SendByte_74HC164(A2);
//	P1 |= 0xf2;
//	delay(6);
//	P1 &= 0x00;
//	
//	SendByte_74HC164(A3);
//	P1 |= 0xf1;
//	delay(6);
}


// 主函数功能: 在数码管显示温度(一位小数)
void main()
{
	uchar a;
	while(1)
	{
		tmpchange();							//首次温度转换
		for(a=50;a>0;a--)					//延时,保持连续显示
		{
			display(tmp());					//进行温度转换和数值显示
		}
	}
}