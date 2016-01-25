#include "app_ex.h"
#include "hal.h"
#include "lcd128_64.h"
#include "string.h"
#include "Font.h"

INT8U NowItem = 0;
INT8U FirstItem = 0;
INT8U TopDisp = 1;
unsigned char MenuItem[4][17];					//菜单缓冲区
extern INT8U ContrastValue;
extern void RfTestDisp( void );
extern void uart_main(void);
extern void temp_sensor_main(void);
extern INT8U ScanKey(void);

void Page1Display(INT8U ss, INT8U tt);
INT8U  DrawMenu(INT8U MenuItem[][17] , INT8U MenuNo);
void MenuMenuDisp(void);
void Page0Display(void);
void SettingDisp(void);
void DoSetContrast(void);
void LanguageDisp(void);
void BackLampDisp(void);
//*********************************************************************************
//函数名:INT8U  DrawMenu(INT8U MenuItem[][16] , INT8U MenuNo)
//功能:菜单显示
//输入：无
//输出：无
//*********************************************************************************
INT8U  DrawMenu(INT8U MenuItem[][17] , INT8U MenuNo)
{
	INT8U ii;
	INT8U key;
	INT8U NowScreen = 1;
	ClearScreen();
	Page0Display();
	while(1)
	{	
		if(NowScreen)
		{	
			NowScreen = 0;
			if(MenuNo > 2)
			{
				for(ii = 0 ; ii < 3 ; ii++)
				{
					if(NowItem != ii)
					{				
						Print((ii*2+2),0,MenuItem[FirstItem + ii],1);
					}
					else
					{	
						Print((ii*2+2),0,MenuItem[FirstItem + ii],0);				
					}
				}
			}
			else
			{
				for(ii = 0 ; ii < 2 ; ii++)
				{
					if(NowItem != ii)
					{				
						Print((ii*2+2),0,MenuItem[FirstItem + ii],1);
					}
					else
					{	
						Print((ii*2+2),0,MenuItem[FirstItem + ii],0);				
					}
				}
			}
                        while(ScanKey() != 0xff);
                        halWait(5);
		}
		key = ScanKey();
		if(K_UP == key)
		{
			while(ScanKey() == K_UP);
			if(NowItem > 0) NowItem -= 1;
			else if(FirstItem > 0) FirstItem -= 1;
			NowScreen = 1;
		}
		if(K_DOWN == key)
		{
			while(ScanKey() == K_DOWN);
			if(MenuNo > 2)
			{
				if(NowItem < 2) 	NowItem += 1;
				else if((FirstItem + 3) < MenuNo) 	FirstItem += 1;
				NowScreen = 1;
			}
			else
			{
				if(NowItem < 1) 	NowItem += 1;
				NowScreen = 1;
			}
		}
		if(K_OK == key)
		{
			while(ScanKey() == K_OK);
                        halWait(5);
                        return FirstItem + NowItem;
		}
		if(K_CANCEL == key)
		{
			while(ScanKey() == K_CANCEL);
                        halWait(5);
                        return 0xff;
		}
	}
}
//*********************************************************************************
//函数名:void TurnOnDisp(void)
//功能:开机画面显示
//输入：无
//输出：无
//*********************************************************************************
void TurnOnDisp(void)
{
        INT8U i;
        ClearScreen();
        Rectangle(2 , 2 , 127 , 7);
        Print(3,13,"成都无线龙",1);
        Print(5,13,"-CC1110DEMO",1);
        for(i=0; i<12; i++)
        halWait(200);
}

//*********************************************************************************
//函数名:void MenuMenuDisp( void )
//功能:主菜单显示
//输入：无
//输出：无
//*********************************************************************************
void MenuMenuDisp( void )
{
	INT8U sel;
        if(LanguageSel == 1)
        {
                strcpy((char*)MenuItem[0] ,"1:RF TEST       ");
	        strcpy((char*)MenuItem[1] ,"2:UART          ");
	        strcpy((char*)MenuItem[2] ,"3:TEMPERATURE   ");
                strcpy((char*)MenuItem[3] ,"4:SETTING       ");
        }
        else
        {
                strcpy((char*)MenuItem[0] ,"1:无线测试      ");
	        strcpy((char*)MenuItem[1] ,"2:串口测试      ");
	        strcpy((char*)MenuItem[2] ,"3:温度测试      ");
                strcpy((char*)MenuItem[3] ,"4:表演板设定    ");
        }

	FirstItem = 0;
	NowItem = 0;
        TopDisp = 1;
	sel = DrawMenu(MenuItem , 4);
	
        while(ScanKey() != 0xff);
        halWait(5);
	switch(sel)
	{
		case 0:
		{
			TopDisp = 2;
                        RfTestDisp();
		}break;
		case 1:
		{
			uart_main();
		}break;
		case 2:
		{
			temp_sensor_main();
		}break;
                case 3:
                {
                        TopDisp = 3;
                        SettingDisp();
                }break;
                default:
                break;
	}
		
}
//*********************************************************************************
//函数名:void SettingDisp(void)
//功能:参数设定菜单显示
//输入：无
//输出：无
//*********************************************************************************
void SettingDisp(void)
{
        INT8U sel;
        if(LanguageSel == 1)
        {
                strcpy((char*)MenuItem[0] ,"1:Contrast      ");
	        strcpy((char*)MenuItem[1] ,"2:Language      ");
        }
        else
        {
                strcpy((char*)MenuItem[0] ,"1:对比度设定    ");
	        strcpy((char*)MenuItem[1] ,"2:语言设定      ");
        }

	FirstItem = 0;
	NowItem = 0;
        while(1)
        {
	        sel = DrawMenu(MenuItem , 2);
	
	        switch(sel)
	        {
		        case 0:
		        {
                                DoSetContrast();
		        }break;
		        case 1:
		        {
			        LanguageDisp();
		        }break;
                        case 0xff:
		        {
                                return;
                        }
	        }
        }
		
}

//*********************************************************************************
//函数名:void BackLampDisp(void)
//功能:背光设定菜单显示
//输入：无
//输出：无
//*********************************************************************************
void BackLampDisp(void)
{
        INT8U sel;
        if(LanguageSel == 1)
        {
                strcpy((char*)MenuItem[0] ,"1:ON            ");
	        strcpy((char*)MenuItem[1] ,"2:OFF           ");
        }
        else
        {
                strcpy((char*)MenuItem[0] ,"1:开            ");
	        strcpy((char*)MenuItem[1] ,"2:关            ");
        }

	FirstItem = 0;
	NowItem = 0;

	sel = DrawMenu(MenuItem , 2);
	
	switch(sel)
	{
		case 0:
	        {
                         P0_4 = 0;
	         }break;
		 case 1:
		 {
			 P0_4 = 1;
		 }break;
                 case 0xff:
		 {
                        ;
                 }break;
        }	
        if(LanguageSel == 1)
        {
                strcpy((char*)MenuItem[0] ,"1:Contrast      ");
	        strcpy((char*)MenuItem[2] ,"2:Language      ");
        }
        else
        {
                strcpy((char*)MenuItem[0] ,"1:对比度设定    ");
	        strcpy((char*)MenuItem[2] ,"2:语言设定      ");
        }

	FirstItem = 0;
	NowItem = 0;
}

//*********************************************************************************
//函数名:void LanguageDisp(void)
//功能:语言设定菜单显示
//输入：无
//输出：无
//*********************************************************************************
void LanguageDisp(void)
{
        INT8U sel;
        if(LanguageSel == 1)
        {
                strcpy((char*)MenuItem[0] ,"1:SimpChinese   ");
	        strcpy((char*)MenuItem[1] ,"2:Eenglish      ");
        }
        else
        {
                strcpy((char*)MenuItem[0] ,"1:中文显示      ");
	        strcpy((char*)MenuItem[1] ,"2:英文显示      ");
        }

	FirstItem = 0;
	NowItem = 0;

	sel = DrawMenu(MenuItem , 2);
	
	switch(sel)
	{
		case 0:
	        {
                        LanguageSel = 0;
	         }break;
		 case 1:
		 {
			LanguageSel = 1;;
		 }break;
                 case 0xff:
		 {
                        ;
                 }break;
        }	
        if(LanguageSel == 1)
        {
                strcpy((char*)MenuItem[0] ,"1:Contrast      ");
	        strcpy((char*)MenuItem[1] ,"2:Language      ");
        }
        else
        {
                strcpy((char*)MenuItem[0] ,"1:对比度设定    ");
	        strcpy((char*)MenuItem[1] ,"2:语言设定      ");
        }

	FirstItem = 0;
	NowItem = 0;
}
			
/******************************************************************************/
//函数名称:DoSetContrast
//功能:设置液晶对比度
//Input Vector:NONE
//Output Vector :NONE
/*******************************************************************************/
void DoSetContrast(void)
{

	INT8U key;
	INT8U ii;
	INT8U Temp_ContrastValue;
	INT8U state_key = 1;

	Temp_ContrastValue = ContrastValue;
	ClearScreen();
	Rectangle(10 , 4 , 120 , 6);
	if(LanguageSel == 1)
	{
		Print6(2, 28, "SetContrast", 1);
	}
	else
	{
		Print(2, 32, "对比度", 1);
	}
	while(1)
	{
		if(state_key == 1)
		{
			state_key = 0;
			Printn(7 , 80 , Temp_ContrastValue , 1 , 2);
			SetContrast(1 , Temp_ContrastValue);
			SetRamAddr(5 ,11);
			for(ii = 11; ii < 120 ;ii++)
			{
				Lcdwritedata(0x00);
			}

			SetRamAddr(5 ,11);
			for(ii = 11; ii < (Temp_ContrastValue ) + 11 ;ii++)
			{
				Lcdwritedata(0xff);
			}
		}
		key = ScanKey();
		if(K_OK == key)
		{	
			SetContrast(1 , Temp_ContrastValue);		
			ContrastValue = Temp_ContrastValue;
			return;
		}
		else if(K_RIGHT == key)
		{
			if(Temp_ContrastValue < 100)
			{
				Temp_ContrastValue += 1;
			}
			if(Temp_ContrastValue>=100)
			{
				Temp_ContrastValue=20;
			}
			state_key = 1;
		}
		else if(K_CANCEL == key)
		{
			ClearScreen();
			return;
		}
		else if(K_LIFT == key)
		{
			if(Temp_ContrastValue > 20)
			{
				Temp_ContrastValue -= 1;
			}
			else
			{
				Temp_ContrastValue = 100;
			}
			state_key = 1;
		}
                while(ScanKey() != 0xff);
	}
}

/*********************************************************************************/
//函数名:void Page1Display(INT8U ss, INT8U tt)
//功能:第七页显示内容.
//输入：ss:当前的选择项   tt:该二维数组的项数
//输出：无
/*********************************************************************************/
void Page1Display(INT8U ss, INT8U tt)
{
	INT8U sel,temp;
	sel = ss;
	temp = tt-2;

	Print6(1,10,"OK",1);
	if(sel > 0)
	{
		Print6(1,30,"UP",1);
	}
	else
	{
		Print6(1,30,"  ",1);
	}
	if(sel < temp)
	{
		Print6(1,56,"DOWN",1);
	}
	else
	{
		Print6(1,56,"    ",1);
	}
	Print6(1,89,"ESC",1);
}


/*********************************************************************************/
//函数名:void Page0Display(INT8U ss, INT8U tt)
//功能:第七页显示内容.
//输入：ss:当前的选择项   tt:该二维数组的项数
//输出：无
/*********************************************************************************/
void Page0Display(void)
{
	switch(TopDisp)
        {
                case 1:
                {
                        Print(0,5,"---MAIN MENU---",1);
                }break;
                case 2:
                {
                        Print(0,5," ---RF TEST--- ",1);
                }break;
                case 3:
                {
                        Print(0,5," ---SETTING--- ",1);
                }break;
                case 7:
                {
                    Print(0,5,"--CLOCKMODE--",1);
                }break;
                case 8:
                {
                    Print(0,5,"---FLASH TEST---",1);
                }
                case 10:
                {
                    Print(0,5,"POWERMODE TEST",1);
                }break;
        }
}





