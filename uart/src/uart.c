#include <iocc1110.h>
#include <string.h>
#define uint unsigned int
#define uchar unsigned char
#define FALSE	0
#define TRUE	1
#define led1	P1_0
#define led2	P1_1
void Delay(uint);
void InitUART(void);
void InitialAD(void);
void UartSendString(uchar *data,int len);
uchar Recdata[30]="hdy test";
uchar RFlag=1;
uchar temp;
uint datanumber=0;
uint stringlen;
void Delay(uint n)
{
	uint i;
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
}
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
void InitADC(void)
{
	P1DIR=0x03;
	led1=1;
	led2=1;
	ADCH&=0x00;		//Clear eoc flag
	ADCCON3=0xbf;	//Reference voltage 
	ADCCON1=0x30;	//stop adc
	ADCCON1|=0x40;	//star adc
}
//Send string from uart
void UartSendString(uchar *data,int len)
{
	int j;
	for(j=0;j<len;j++)
	{
		U0DBUF=*data++;
		while(UTX0IF==0);
		UTX0IF=0;
	}
}
void main(void)
{
	P1DIR=0x03;
	led1=1;
	led2=1;
	InitUART();
	stringlen=strlen((uchar *)Recdata);
	UartSendString(Recdata,stringlen);
	while(1)
	{
		if(RFlag==1)
		{
			led2=0;
			if(temp!=0)
			{ 
				if((temp!='#')&&(datanumber<10))
				{
					Recdata[datanumber++]=temp;
				}
				else
				{
					RFlag=3;
				}
				temp=0;
			}
		}
		if(RFlag==3)
		{
			led2=1;
			led1=0;
			U0CSR&=~0x40;
			UartSendString(Recdata,datanumber);
			U0CSR|=0x40;
			RFlag=1;
			datanumber=0;
			led1=1;
		}
	}
}
#pragma vector=URX0_VECTOR
__interrupt void UART0_ISR(void)
{
	URX0IF=0;
	temp=U0DBUF;
}
