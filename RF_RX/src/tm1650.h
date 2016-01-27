#ifndef __TM1650_H__
#define __TM1650_H__
#include <iocc1110.h>
#include "mytypedef.h"
#define TM_CLK	P1_3	
#define	TM_SDA	P1_4
#define CLK_H  TM_CLK=1
#define CLK_L  TM_CLK=0
#define DIO_H  TM_SDA = 1
#define DIO_L  TM_SDA = 0
#define	REG_CMD		0x48
#define DIG1		0x68
#define DIG2		0x6A
#define DIG3		0x6C
#define DIG4		0x6E

#define LED_LEVEL	0x51
void TM1650_Write(uchar add,uchar dat);
void TM1650_Set(uchar on);
//uchar Scan_Key(void);
void TM1650_CMD(uchar data);
//void TM1650_Show(uchar dig,uchar num);
void TM1650_Show(INT8U num);

#endif