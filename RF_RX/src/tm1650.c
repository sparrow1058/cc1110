#include"tm1650.h"
uchar digCode[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9显示代码 公阴
void Delay_us(uint n) //us延时
{
	uint i;
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
}
void I2CStart(void)	//开始信号
{
	CLK_H;
	DIO_H;
	Delay_us(5);
	DIO_L;
	
}

void I2Cask(void) //ACK信号
{
	uchar timeout = 1;
	CLK_H;
	Delay_us(5);
	CLK_L;
	while((TM_SDA)&&(timeout<=100))
	{
	  timeout++;	
	}
	Delay_us(5);
	CLK_L;
}

void I2CStop(void) //停止信号
{
	CLK_H;
	DIO_L;
	Delay_us(5);
	DIO_H;
}

void I2CWrByte(uchar oneByte) //写一个字节高位在前，低位在后
{
	uchar i;
	//CLK_L;
	Delay_us(1);
	for(i=0;i<8;i++)
	{	
		if(oneByte&0x80)
                   DIO_H;
                else
                  DIO_L;
 //         TM_SDA=(oneByte&0x80)>>7;
		oneByte = oneByte<<1;
		//TM_SDA = CY;		//进位标志
	
		CLK_L;
		Delay_us(5);
		CLK_H;
		Delay_us(5);
		CLK_L;
	}
}

uchar Scan_Key(void)	  // 按键扫描
{
	uchar i;
	uchar rekey;
	I2CStart();
	I2CWrByte(0x49);//读按键命令
	I2Cask();
	//DIO_H;
	for(i=0;i<8;i++)
	{
	   CLK_H;
	   rekey = rekey<<1;
	   if(TM_SDA)
	   {
	   	rekey++;
	   } 
	   Delay_us(5);
	   CLK_L;	
	}
	I2Cask();
	I2CStop();
	return(rekey);
}
void TM1650_CMD(uchar data)
{
	I2CStart();
	I2CWrByte(data);
	I2Cask();
	I2CStop();
}
void TM1650_Write(uchar add,uchar dat) //数码管显示
{
	//写显存必须从高地址开始写
	I2CStart();
	I2CWrByte(add); //第一个显存地址
	I2Cask();
	I2CWrByte(dat);
	I2Cask();
	I2CStop();
}
void TM1650_Set(uchar on)
{
	if(on)
		TM1650_Write(REG_CMD,LED_LEVEL);
	else
		TM1650_Write(REG_CMD,0x00);
}
void TM1650_Show(INT8U num)
{
	TM1650_Set(1);
	TM1650_Write(DIG1,digCode[num/100]);
	TM1650_Write(DIG2,digCode[(num%100)/10]);
	TM1650_Write(DIG3,digCode[num%10]);
}
/*
void TM1650_Show(uchar dig,uchar num)
{
	switch(dig)
	{
		case 1:
			TM1650_Write(DIG1,digCode[num]);
			break;
		case 2:
			TM1650_Write(DIG2,digCode[num]);
			break;
		case 3:
			TM1650_Write(DIG3,digCode[num]);
			break;
		case 4:
			TM1650_Write(DIG4,digCode[num]);
			break;
		default :
			break;
	}
}
*/


