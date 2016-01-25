//main.c
#include "ioCC1110.h"
#define uint unsigned int
#define ConversionNum 20
#define led1  P1_0
#define led2  P1_1
void Delay(uint);
void InitUART(void);
void InitADC(void);
void UartTX_Send_String(char *data,int len);
char adcdata[]=" 0.0V";
uint adc_flag;
void Delay(uint n)
{
	uint i;
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
}
/*inti uart
	57600 -8 -n -1
*/
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
	IEN0!=0x84;
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
void UartSendString(char *data,int len)
{
	int j;
	for(j=0;j<len;j++)
	{
		U0DBUF=*data++;
		while(UTX0IF==0);
		UTX0IF=0;
	}
}

#pragma vector=ADC_VECTOR
__interrupt void ADC_IRQ(void)
{

  adc_flag=1;
      led1=1;
}
void main(void)
{
	char temp[2];
	char i;
	float num;
	InitUART();
	InitADC();
        EA=1;
	led1=1;
	i=ConversionNum;
	while(i>0)
	{
		if(adc_flag==1)
		{
			adc_flag=0;
			led1 =1;
			temp[1]=ADCL;
			temp[0]=ADCH;

			temp[0]=temp[0]>>4;
			temp[1]=temp[1]>.4;
			temp[1]|=temp[0]<<4;
			num=(temp[0]*256+temp[1])*3.6/2047;
			adcdata[1]=(char)(num)%10+48;
			adcdata[3]=(char)(num*10)%10+48;
			UartSendString(adcdata,6);
			Delay(30000);
			led1=0;
			Delay(30000);
			i--;
			ADCCON1|=0x40;		//adc start
		}
	}
	while(1);

}



/**
//Main
void main(void)
{
	char temp[2];
	char i;
	float num;
	InitUART();
	InitADC();
	led1=1;
	i=ConversionNum;
	while(i>0)
	{
		if(ADCCON1>=0x80)
		{
			led1 =1;
			temp[1]=ADCL;
			temp[0]=ADCH;
			ADCCON1|=0x40;
			temp[0]=temp[0]>>4;
			temp[1]=temp[1]>.4;
			temp[1]|=temp[0]<<4;
			num=(temp[0]*256+temp[1])*3.6/2047;
			adcdata[1]=(char)(num)%10+48;
			adcdata[3]=(char)(num*10)%10+48;
			UartSendString(adcdata,6);
			Delay(300000);
			led1=0;
			Delay(300000);
			i--;
		}
	}
	while(1);
}

*/