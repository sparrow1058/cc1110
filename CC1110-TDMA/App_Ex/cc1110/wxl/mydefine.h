////////////////////////////////////////////////////////////////////////
/*
-----------------------------__WXL__-----------------------------------
-----------------------------@2007-12----------------------------------
----------------------------__by__SU__---------------------------------
*/
////////////////////////////////////////////////////////////////////////

#ifndef __mydefine_h
#define __mydefine_h


typedef unsigned char uchar;
typedef unsigned char UCHAR;
typedef unsigned char uint8;
typedef unsigned char INT8U;

#define false 0
#define true  1


#define SET_PIN(port,pin) {P##port |= 1<<pin}
#define CLR_PIN(port,pin) {P##port &= ~(1<<pin)}



#define ENABLE_ALL_INTERRUPT() (IEN0 |= 0x80)
#define DISABLE_ALL_INTERRUPT() (IEN0 &= ~0x80)

//定义帧长度
#define FRAMELENTH  20
#define UART_TIMER_OVER 50//ms
#define FRAME_HEAD1 0X68
#define FRAME_HEAD2 0XAA


typedef union x{
	UCHAR RxBuf[FRAMELENTH];
	struct y{
		uchar FrameHead[2];
		uchar Order[3];
		uchar Id[4];
		uchar Data[9];
		uchar CRC;
		uchar FrameEnd;
		}AF_Struct;
	}AF;									//全局命令
extern AF AF_Data;
struct u{
	uchar Buf[FRAMELENTH];
	uchar count;
	uchar Flag;
	};	
typedef struct u Uart_Rx;//接收数据区
extern Uart_Rx Uart_R;//串口接收数据 、命令单元
enum n{
	EMPTY,
	FULL,
	RXING,
	CLEARING,
	TIMEROVER,	
	RXOVER,
	};
typedef enum n Uart_State;//状态表现

extern Uart_State Uart_S;
	
//2007-12-14 14:06:28
//wxlapp.c
extern void Uart_int(unsigned int baud);
extern void Timer_int(void);
extern Uart_State Chick_Uart_State(void);
	
//2007-12-14 15:14:24
//uart.c
extern void ChangUartBaund(unsigned int temp);
extern void initUART(unsigned int baud);
extern void UARTxISRopen(void);
extern void uart_state_int(void);
extern void UartSendWord(uchar *msg,unsigned int lenth);

//2007-12-14 16:41:56
//timer.c
extern uchar timer3_int(void);//
extern void init_T3(void);
extern void RestTimer(unsigned char timercount);
extern unsigned int ReadTimer(unsigned char timercount);

//2007-12-25 13:48:53
//nixie_light.c
extern void nixie_light_int(void);
extern void disp_nixie_light(uchar i,uchar ch);

#ifdef AP_IS_DATA_HUB
//font.c
//2007-12-26 15:03:24
extern void clr_yz(void);
extern void disp_yz(unsigned char h,unsigned char l);
extern unsigned char di_yz[8][128];
extern void disp_open_wil(void);
extern void Print_word(unsigned char *add,unsigned char h,unsigned char l,unsigned char len);
extern void Print_words(unsigned char add[],unsigned char h,unsigned char l,unsigned char len);
//extern void Print_word(unsigned char *add,unsigned char h,unsigned char l);
#endif

#endif










