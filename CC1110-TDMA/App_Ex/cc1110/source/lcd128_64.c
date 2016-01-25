#include "app_ex_main.h"
#include "app_ex_util.h"
#include "RF04EB.h"
#include "hal.h"
#include "Font.h"

#define RECALIB_TIMEOUT     12000 // Every 2 minutes
#define MSG_PING           0x00
#define MSG_SEND           0x10
#define MSG_RECIVE         0x20
#define MSG_NEEDSEND       0x30


INT8U ContrastValue = 0x38;
INT8U BT_State;

void ClearScreen(void);
void Print(INT8U y, INT8U x, INT8U ch[], INT16U yn);
void Print6(INT8U xx,INT8U  yy,INT8U ch1[],INT8U yn);


/*******************************************************************************
//函数名：void delaylcd (int16u x)
//功能：廷时
//输入：时间
//输出：无
********************************************************************************/
void delaylcd (INT16U x)
{
	INT8U j;
	while (x--)
	{
		for (j=0;j<115;j++);
	}
}
/*******************************************************************************
//函数名：void fdelay(unsigned int n)
//功能：廷时
//输入：时间
//输出：无
********************************************************************************/
void fdelay(unsigned int n) /* wait n seconds*/
{
	INT16U i;
	INT16U j;
	for (i=0;i<5;i++)
	for (j=0;j<n*2;j++) ;
}


/*******************************************************************************
//函数名：void Lcdwritecom(int8u com)
//功能：lcd写指令
//输入：com指令
//输出：无
********************************************************************************/
void Lcdwritecom(INT8U com)
{
	INT8U i, temp;
	LCD_CLK = 0;
	LCD_CS = 0;
	LCD_RS = 0;
	temp = com;

	for(i=0; i<8; i++)
	{
		if(temp & 0x80)
		{
			LCD_SDO = 1;
		}
		else LCD_SDO = 0;
		temp <<= 1;
		LCD_CLK = 1;
		asm("nop");
		asm("nop");
		LCD_CLK = 0;
	}
	LCD_CS = 1;
}

/*******************************************************************************
//函数名：void Lcdwritedata(int8u dat)
//功能：lcd写数据
//输入：dat数据
//输出：无
********************************************************************************/
void Lcdwritedata(INT8U dat)
{
	INT8U i, temp;
	LCD_CS = 0;
	LCD_RS = 1;
	temp = dat;

	for(i=0; i<8; i++)
	{
		if(temp & 0x80)
		{
			LCD_SDO = 1;
		}
		else LCD_SDO = 0;
		temp <<= 1;
		LCD_CLK = 1;
		asm("nop");
		asm("nop");
		LCD_CLK = 0;
	}
	LCD_CS = 1;
}

/*******************************************************************************
//函数名：void Prog_Reset(void)
//功能：lcd复位
//输入：无
//输出：无
********************************************************************************/
void Prog_Reset(void)
{
	LCD_RST = 0;
	delaylcd(100);
	LCD_RST = 1;
}
/*******************************************************************************
//函数名：void Resetchip(void)
//功能：lcd软件复位
//输入：无
//输出：无
********************************************************************************/
void Resetchip(void)
{
	Prog_Reset();
}

/*******************************************************************************
//函数名：void SetRamAddr (INT8U Page, INT8U Col)
//功能：lcd位置选择
//输入：Page-页，Col-列
//输出：无
********************************************************************************/
void SetRamAddr (INT8U Page, INT8U Col)
{
	Lcdwritecom(0xB0 + Page);
	Lcdwritecom(Col & 0x0f); //Set lower column address
	Lcdwritecom(0x10 | ((Col & 0xf0) >> 4)); //Set higher column address
}
/*******************************************************************************
//函数名：void SetContrast(INT8U Gain, INT8U Step)
//功能：lcd对比度设定
//输入：Page-页，Col-列
//输出：无
********************************************************************************/
void SetContrast(INT8U Gain, INT8U Step)
{
	Lcdwritecom(0x81);
	Lcdwritecom(Step);
}

void LcdPortInit(void)
{
    IO_DIR_PORT_PIN(0,0,IO_OUT);			//
    IO_DIR_PORT_PIN(0,1,IO_OUT);			//
    IO_DIR_PORT_PIN(1,3,IO_OUT);			//
    IO_DIR_PORT_PIN(1,4,IO_OUT);			//
    IO_DIR_PORT_PIN(1,5,IO_OUT);
    IO_DIR_PORT_PIN(1,6,IO_OUT);
}


/*******************************************************************************
//函数名：void InitLcd(void)
//功能：lcd初始化
//输入：无
//输出：无
********************************************************************************/
void InitLcd(void)
{
	LcdPortInit();
        LCD_PWR = 0;    //OLED POWER ON
	LCD_CS = 1;
	LCD_RS = 1;
	LCD_RST = 1;
	LCD_CLK = 1;
	LCD_SDO = 1;


	Resetchip();
	Lcdwritecom(0xAE);
	Lcdwritecom(0xAD);	//dc-dc off
	Lcdwritecom(0x8a);
	halWait(100);
	Lcdwritecom(0x00);
	Lcdwritecom(0x10);
	Lcdwritecom(0x40);
	Lcdwritecom(0x81);
	Lcdwritecom(ContrastValue);
	Lcdwritecom(0xA0);
	Lcdwritecom(0xA4);
	Lcdwritecom(0xA6);
	Lcdwritecom(0xA8);
	Lcdwritecom(0x3f);
	Lcdwritecom(0xD3);
	Lcdwritecom(0x00);
	Lcdwritecom(0xD5);
	Lcdwritecom(0x20);
	Lcdwritecom(0xD8);
	Lcdwritecom(0x00);
	Lcdwritecom(0xDA);
	Lcdwritecom(0x12);
	Lcdwritecom(0xDB);
	Lcdwritecom(0x00);
	Lcdwritecom(0xD9);
	Lcdwritecom(0x22);
	Lcdwritecom(0xc8);
	Lcdwritecom(0xAF);
	ClearScreen();
}


/*******************************************************************************
//函数名：void ClearScreen(void)
//功能：清屏
//输入：无
//输出：无
********************************************************************************/
void ClearScreen(void)
{
	INT8U i , j;
	
	for (i = 0 ; i < 8 ; i++)
	{
		SetRamAddr(i,0);
		for (j=0;j<132; j++) Lcdwritedata(0x00);
	}
}


/*******************************************************************************
//函数名：void Printn(INT8U xx ,INT8U yy , INT32U no,INT8U yn,INT8U le)
//功能：显示一个6*8无符号数据
//输入：xx , yy屏幕当中位置,no待显示数据 yn=0正常显示 yn=1反黑显示  le有效位
//输出：无
********************************************************************************/
void Printn(INT8U xx ,INT8U yy , INT32U no,INT8U yn,INT8U le)
{
	INT8U ch2[6];
	INT8U ii;

	for(ii = 1 ; ii <= le ;)
	{
		ch2[le - ii] = no % 10 + 0x30;
		no /= 10;
		ii += 1;
	}
	ch2[le] = '\0';
	Print6(xx ,yy ,ch2 ,yn);
}




/*******************************************************************************
//函数名：void Printn8(INT8U xx ,INT8U yy , INT32U no,INT8U yn,INT8U le)
//功能：显示8*8一个无符号数据
//输入：xx , yy屏幕当中位置,no待显示数据 yn=0正常显示 yn=1反黑显示  le有效位
//输出：无
********************************************************************************/
void Printn8(INT8U xx ,INT8U yy , INT32U no,INT8U yn,INT8U le)
{
	INT8U ch2[6];
	INT8U ii;

	for(ii = 1 ; ii <= le ;){
		ch2[le - ii] = no % 10 + 0x30;
		no /= 10;
		ii += 1;
	}
	ch2[le] = '\0';
	Print(xx ,yy ,ch2 ,yn);
}



/*******************************************************************************
//函数名：void Print6(INT8U xx, INT8U yy, INT8U ch1[], INT8U yn)
//功能：显示6*8字符串
//输入：xx ,yy 坐标,ch1待显示的字符串,yn是否反黑
//输出：无
********************************************************************************/
void Print6(INT8U xx, INT8U yy, INT8U ch1[], INT8U yn)		
{
	INT8U ii = 0;
	INT8U bb = 0;
	unsigned int index = 0 ;	

	SetRamAddr(xx , yy);		
	while(ch1[bb] != '\0')
	{
                index = (unsigned int)(ch1[bb] - 0x20);
		index = (unsigned int)index*6;		
		for(ii=0;ii<6;ii++)
		{
			if(yn)
			{
				Lcdwritedata(FontSystem6x8[index]);
			}
			else
			{
				Lcdwritedata(~FontSystem6x8[index]);
			}		
			index += 1;
		}		
		bb += 1;
	}
}


/*******************************************************************************
//函数名：void Print8(INT16U y,INT16U x, INT8U ch[],INT16U yn)
//功能：显示8*8字符串
//输入：xx ,yy 坐标,ch1待显示的字符串,yn是否反黑
//输出：无
********************************************************************************/
void Print8(INT16U y,INT16U x, INT8U ch[],INT16U yn)
{
	INT8U wm ,ii = 0;
	INT16U adder;

	while(ch[ii] != '\0')
	{
		adder = (ch[ii] - 0x20) * 16;

		SetRamAddr(y , x);
		for(wm = 0;wm < 8;wm++)
		{
			if(yn == 0)
			{
				Lcdwritedata(~Font8X8[adder]);
			}
			else
			{
				Lcdwritedata(Font8X8[adder]);
			}
			adder += 1;
		}
		SetRamAddr(y + 1 , x);
		for(wm = 0;wm < 8;wm++)
		{
			if(yn == 0)
			{
				Lcdwritedata(~Font8X8[adder]);	
			}
			else
			{
				Lcdwritedata(Font8X8[adder]);	
			}
			adder += 1;
		}
		ii += 1;
		x += 8;
	}

}


/*******************************************************************************
//函数名：void Print16(INT16U y,INT16U x,INT8U ch[],INT16U yn)
//功能：在屏幕上显示汉字
//输入：x ,y 坐标,ch[]待显示的汉字,yn是否反黑
//输出：无
********************************************************************************/
void Print16(INT16U y,INT16U x,INT8U ch[],INT16U yn)
{
	INT8U wm ,ii = 0;
	INT16U adder;

	wm = 0;
	adder = 1;
	while(FontNew8X16_Index[wm] > 100)
	{
		if(FontNew8X16_Index[wm] == ch[ii])
		{
			if(FontNew8X16_Index[wm + 1] == ch[ii + 1])
			{
				adder = wm * 14;
				break;
			}
		}
		wm += 2;				//找到汉字在索引中的位置
	}
	SetRamAddr(y , x);

	if(adder != 1)					//找到汉字，显示出来	
	{
		SetRamAddr(y , x);
		for(wm = 0;wm < 14;wm++)
		{
			if(yn == 0)
			{
				Lcdwritedata(~FontNew16X16[adder]);
			}
			else
			{
				Lcdwritedata(FontNew16X16[adder]);
			}
			adder += 1;
		}
                for(wm = 0;wm < 2;wm++)
		{
			if(yn == 0)
			{
				Lcdwritedata(0xff);
			}
			else
			{
				Lcdwritedata(0x00);
			}
		}
		SetRamAddr(y + 1 , x);

		for(wm = 0;wm < 14;wm++)
		{
			if(yn == 0)
			{
				Lcdwritedata(~FontNew16X16[adder]);
			}
			else
			{
				Lcdwritedata(FontNew16X16[adder]);
			}
			adder += 1;
		}
                for(wm = 0;wm < 2;wm++)
		{
			if(yn == 0)
			{
				Lcdwritedata(0xff);
			}
			else
			{
				Lcdwritedata(0x00);
			}
		}


	}
	else						//找不到字显示空格			
	{
		ii += 1;SetRamAddr(y , x);
		for(wm = 0;wm < 16;wm++)
		{
			if(yn == 0)
			{
				Lcdwritedata(0xff);
			}
			else
			{
				Lcdwritedata(0x00);
			}
		}
		SetRamAddr(y + 1 , x);
		for(wm = 0;wm < 16;wm++)
		{
			if(yn == 0)
			{
				Lcdwritedata(0xff);
			}
			else
			{
				Lcdwritedata(0x00);
			}
		}
	}
}
/*******************************************************************************
//函数名：void Print(INT8U y, INT8U x, INT8U ch[], INT16U yn)
//功能：实现汉字及字母混合显示
//输入：x ,y 坐标,ch[]待显示的汉字或字母,yn是否反黑
//输出：无
********************************************************************************/
void Print(INT8U y, INT8U x, INT8U ch[], INT16U yn)
{
	INT8U ch2[3];
	INT8U ii;
        ii = 0;
	while(ch[ii] != '\0')
	{
		if(ch[ii] > 127)
		{
			ch2[0] = ch[ii];
	 		ch2[1] = ch[ii + 1];
			ch2[2] = '\0';			//汉字为两个字节
			Print16(y , x , ch2 , yn);	//显示汉字
			x += 16;
			ii += 2;
		}
		else
		{
			ch2[0] = ch[ii];	
			ch2[1] = '\0';			//字母占一个字节
			Print8(y , x , ch2 , yn);	//显示字母
			x += 8;
			ii += 1;
		}
	}
}

/*******************************************************************************
//函数名：void Rectangle(INT8U x1,INT8U y1,INT8U x2,INT8U y2)
//功能：画直线函数，本函数目前只能画水平和垂直线
//输入：x1,y1(第一个点)   x2,y2第二个点
//输出：无
********************************************************************************/
void Rectangle(INT8U x1,INT8U y1,INT8U x2,INT8U y2)
{

	INT8U ii;

	for(ii=x1; ii<x2; ii++)
	{		
		SetRamAddr(y1,ii);
		Lcdwritedata(0x08);
		SetRamAddr(y2,ii);
		Lcdwritedata(0x08);		//画横线
	}
	SetRamAddr(y1,x1);
	Lcdwritedata(0xF0);
	SetRamAddr(y1,x2);
	Lcdwritedata(0xF0);

	for(ii = y1+1;ii<y2;ii++)
	{		
	 	SetRamAddr(ii,x1);
		Lcdwritedata(0xff);
	 	SetRamAddr(ii,x2);
		Lcdwritedata(0xff);		//画竖线
	}

	SetRamAddr(y2,x1);
	Lcdwritedata(0x0F);
	SetRamAddr(y2,x2);
	Lcdwritedata(0x0F);
}


	
