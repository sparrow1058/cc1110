#include "ioCC1110.h"

#include "rf_cc1110.h"

#include "hal_cc1110.h"

void InitUART(void)
{
	CLKCON&=~0x40;
	while(!(SLEEP&0x40));
	CLKCON&=~0x47;
	SLEEP|=0x04;
	PERCFG=0x00;
	P0SEL=0x3C;	//P0 User as uart
	U0CSR|=0x80;	//UART Model
	U0GCR|=11;
	U0BAUD|=34;
	UTX0IF=1;
	U0CSR|=0x40;
	IEN0|=0x84;
}
//Send string from uart
void UartSendString(INT8U *data,int len)
{
	int j;
	for(j=0;j<len;j++)
	{
		U0DBUF=*data++;
		while(UTX0IF==0);
		UTX0IF=0;
	}
}
