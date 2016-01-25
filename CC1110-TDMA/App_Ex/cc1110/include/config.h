/******************************************************************************
*                                                                             *
*    ****       ******       ****
*     ****     ********     ****
*      ****   ****  ****   ****
*       **** ****    **** ****              wxl
*        *******      *******
*         *****        *****                    成都无线龙通讯科技有限公司
*                                                                             *
*******************************************************************************

Filename:     uart.c                          文 件 名：      uart.c
Target:       cc2510                          使用芯片:       cc2510
Author:       WXL                             程 序 员：      无线龙
data:         1/12-2007                       日    期：      1/12-2007
******************************************************************************/
/******************************************************************************
Description:
Hardware Abstraction Layer, Utility Library.

******************************************************************************/

#ifndef HAL_H
#define HAL_H

#ifdef CC1110
#include "ioCC1110.h"
#endif
#ifdef CC2510
#include "ioCC2510.h"
#endif


/******************************************************************************
*******************              Commonly used types        *******************
******************************************************************************/
typedef unsigned char       BOOL;

// Data
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;

// Unsigned numbers
typedef unsigned char       UINT8;
typedef unsigned char       INT8U;
typedef unsigned short      UINT16;
typedef unsigned short       INT16U;
typedef unsigned long       UINT32;
typedef unsigned long       INT32U;

// Signed numbers
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed long         INT32;

//-----------------------------------------------------------------------------
// Common values
#ifndef FALSE
   #define FALSE 0
#endif

#ifndef TRUE
   #define TRUE 1
#endif

#ifndef NULL
   #define NULL 0
#endif

#ifndef HIGH
   #define HIGH 1
#endif

#ifndef LOW
   #define LOW 0
#endif

#define		K_UP		0X20
#define		K_DOWN		0X21
#define		K_LIFT		0X22
#define		K_RIGHT		0X23
#define		K_CANCEL	0X24
#define		K_OK		0X25

/******************************************************************************
*******************             Port functions/macros       *******************
******************************************************************************/

// Macros for simplifying access to I/O pin setup and usage

//*****************************************************************************


// Macros for configuring IO peripheral location:
// Example usage:
//   IO_PER_LOC_TIMER1_AT_PORT0_PIN234();
//   IO_PER_LOC_TIMER4_AT_PORT2_PIN03();
//   IO_PER_LOC_USART1_AT_PORT0_PIN2345();

#define IO_PER_LOC_TIMER1_AT_PORT0_PIN234() do { PERCFG = (PERCFG&~0x40)|0x00; } while (0)
#define IO_PER_LOC_TIMER1_AT_PORT1_PIN012() do { PERCFG = (PERCFG&~0x40)|0x40; } while (0)

#define IO_PER_LOC_TIMER3_AT_PORT1_PIN34()  do { PERCFG = (PERCFG&~0x20)|0x00; } while (0)
#define IO_PER_LOC_TIMER3_AT_PORT1_PIN67()  do { PERCFG = (PERCFG&~0x20)|0x20; } while (0)

#define IO_PER_LOC_TIMER4_AT_PORT1_PIN01()  do { PERCFG = (PERCFG&~0x10)|0x00; } while (0)
#define IO_PER_LOC_TIMER4_AT_PORT2_PIN03()  do { PERCFG = (PERCFG&~0x10)|0x10; } while (0)

#define IO_PER_LOC_SPI1_AT_PORT0_PIN2345()  do { PERCFG = (PERCFG&~0x02)|0x00; } while (0)
#define IO_PER_LOC_SPI1_AT_PORT1_PIN4567()  do { PERCFG = (PERCFG&~0x02)|0x02; } while (0)

#define IO_PER_LOC_SPI0_AT_PORT0_PIN2345()  do { PERCFG = (PERCFG&~0x01)|0x00; } while (0)
#define IO_PER_LOC_SPI0_AT_PORT1_PIN2345()  do { PERCFG = (PERCFG&~0x01)|0x01; } while (0)

#define IO_PER_LOC_UART1_AT_PORT0_PIN2345() do { PERCFG = (PERCFG&~0x02)|0x00; } while (0)
#define IO_PER_LOC_UART1_AT_PORT1_PIN4567() do { PERCFG = (PERCFG&~0x02)|0x02; } while (0)

#define IO_PER_LOC_UART0_AT_PORT0_PIN2345() do { PERCFG = (PERCFG&~0x01)|0x00; } while (0)
#define IO_PER_LOC_UART0_AT_PORT1_PIN2345() do { PERCFG = (PERCFG&~0x01)|0x01; } while (0)


// Actual MCU pin configuration:
//
// Peripheral I/O signal     Alt1       Alt2
// -------------------------------------------
// Timer1 channel0           P0.2       P1.2
// Timer1 channel1           P0.3       P1.1
// Timer1 channel2           P0.4       P1.0
// Timer3 channel0           P1.3       P1.6
// Timer3 channel1           P1.4       P1.7
// Timer4 channel0           P1.0       P2.0
// Timer4 channel1           P1.1       P2.3
// USART0 TXD/MOSI           P0.3       P1.5
// USART0 RXD/MISO           P0.2       P1.4
// USART0 RTS/SCK            P0.5       P1.3
// USART0 CTS/SS_N           P0.4       P1.2
// USART1 TXD/MOSI           P0.4       P1.6
// USART1 RXD/MISO           P0.5       P1.7
// USART1 RTS/SCK            P0.3       P1.5
// USART1 CTS/SS_N           P0.2       P1.4

// Macros for configuring IO direction:
// Example usage:
//   IO_DIR_PORT_PIN(0, 3, IO_IN);    // Set P0_3 to input
//   IO_DIR_PORT_PIN(2, 1, IO_OUT);   // Set P2_1 to output

#define IO_DIR_PORT_PIN(port, pin, dir)  \
   do {                                  \
      if (dir == IO_OUT)                 \
         P##port##DIR |= (0x01<<(pin));  \
      else                               \
         P##port##DIR &= ~(0x01<<(pin)); \
   }while(0)

// Where port={0,1,2}, pin={0,..,7} and dir is one of:
#define IO_IN   0
#define IO_OUT  1

// Macros for configuring IO input mode:
// Example usage:
//   IO_IMODE_PORT_PIN(0,0,IO_IMODE_PUD);
//   IO_IMODE_PORT_PIN(2,0,IO_IMODE_TRI);
//   IO_IMODE_PORT_PIN(1,3,IO_IMODE_PUD);

#define IO_IMODE_PORT_PIN(port, pin, imode) \
   do {                                     \
      if (imode == IO_IMODE_TRI)            \
         P##port##INP |= (0x01<<(pin));     \
      else                                  \
         P##port##INP &= ~(0x01<<(pin));    \
   } while (0)

// where imode is one of:
#define IO_IMODE_PUD  0 // Pull-up/pull-down
#define IO_IMODE_TRI  1 // Tristate

// Macro for configuring IO drive mode:
// Example usage:
//   IIO_PUD_PORT(0, IO_PULLUP);
//   IIO_PUD_PORT(1, IO_PULLDOWN);
//   IIO_PUD_PORT(2, IO_PULLUP);

#define IO_PUD_PORT(port, pud)        \
   do {                               \
      if (pud == IO_PULLDOWN)         \
         P2INP |= (0x01 << (port+5)); \
      else                            \
         P2INP &= ~(0x01 << (port+5));\
   } while (0)

#define IO_PULLUP          0
#define IO_PULLDOWN        1

// Macros for function select (General purpose I/O / Peripheral function):
// Example usage:
//   IO_FUNC_PORT0_PIN0(0, 0, IO_FUNC_PERIPH);
//   IO_FUNC_PORT0_PIN1(0, 1, IO_FUNC_GIO);
//   IO_FUNC_PORT2_PIN3(2, 3, IO_FUNC_PERIPH);

#define IO_FUNC_PORT_PIN(port, pin, func)  \
   do {                                    \
      if((port == 2) && (pin == 3)){       \
         if (func) {                       \
            P2SEL |= 0x02;                 \
         } else {                          \
            P2SEL &= ~0x02;                \
         }                                 \
      }                                    \
      else if((port == 2) && (pin == 4)){  \
         if (func) {                       \
            P2SEL |= 0x04;                 \
         } else {                          \
            P2SEL &= ~0x04;                \
         }                                 \
      }                                    \
      else{                                \
         if (func) {                       \
            P##port##SEL |= (0x01<<(pin)); \
         } else {                          \
            P##port##SEL &= ~(0x01<<(pin));\
        }                                  \
      }                                    \
   } while (0)

// where func is one of:
#define IO_FUNC_GIO     0 // General purpose I/O
#define IO_FUNC_PERIPH  1 // Peripheral function

// Macros for configuring the ADC input:
// Example usage:
//   IO_ADC_PORT0_PIN(0, IO_ADC_EN);
//   IO_ADC_PORT0_PIN(4, IO_ADC_DIS);
//   IO_ADC_PORT0_PIN(6, IO_ADC_EN);

#define IO_ADC_PORT0_PIN(pin, adcEn) \
   do {                              \
      if (adcEn)                     \
         ADCCFG |= (0x01<<pin);      \
      else                           \
         ADCCFG &= ~(0x01<<pin); }   \
   while (0)

// where adcEn is one of:
#define IO_ADC_EN           1 // ADC input enabled
#define IO_ADC_DIS          0 // ADC input disab

/******************************************************************************
*******************         Common USART functions/macros   *******************
*******************************************************************************

The macros in this section are available for both SPI and UART operation.
******************************************************************************/

// Example usage:
//   USART0_FLUSH();
#define USART_FLUSH(num)              (U##num##UCR |= 0x80)
#define USART0_FLUSH()                USART_FLUSH(0)
#define USART1_FLUSH()                USART_FLUSH(1)

// Example usage:
//   if (USART0_BUSY())
//     ...
#define USART_BUSY(num)               (U##num##CSR & 0x01 == 0x01)
#define USART0_BUSY()                 USART_BUSY(0)
#define USART1_BUSY()                 USART_BUSY(1)

// Example usage:
//   while(!USART1_BYTE_RECEIVED())
//     ...
#define USART_BYTE_RECEIVED(num)      ((U##num##CSR & 0x04) == 0x04)
#define USART0_BYTE_RECEIVED()        USART_BYTE_RECEIVED(0)
#define USART1_BYTE_RECEIVED()        USART_BYTE_RECEIVED(1)

// Example usage:
//   if(USART1_BYTE_TRANSMITTED())
//     ...
#define USART_BYTE_TRANSMITTED(num)   ((U##num##CSR & 0x02) == 0x02)
#define USART0_BYTE_TRANSMITTED()     USART_BYTE_TRANSMITTED(0)
#define USART1_BYTE_TRANSMITTED()     USART_BYTE_TRANSMITTED(1)


/******************************************************************************
*******************  USART-UART specific functions/macros   *******************
******************************************************************************/


#define BAUD_E(baud, clkDivPow)( \
   (baud ==   2400) ?   6 +clkDivPow : \
   (baud ==   4800) ?   7 +clkDivPow : \
   (baud ==   9600) ?   8 +clkDivPow : \
   (baud ==  14400) ?   9 +clkDivPow : \
   (baud ==  19200) ?   9 +clkDivPow : \
   (baud ==  28800) ?  10 +clkDivPow : \
   (baud ==  38400) ?  10 +clkDivPow : \
   (baud ==  57600) ?  11 +clkDivPow : \
   (baud ==  76800) ?  11 +clkDivPow : \
   (baud == 115200) ?  12 +clkDivPow : \
   (baud == 230400) ?  13 +clkDivPow : \
   (baud == 307200) ?  13 +clkDivPow : \
   (baud == 460800) ?  14 +clkDivPow : \
   0)

#define BAUD_M(baud) ( \
   (baud ==   2400) ?  131 : \
   (baud ==   4800) ?  131 : \
   (baud ==   9600) ?  131 : \
   (baud ==  14400) ?   34 : \
   (baud ==  19200) ?  131 : \
   (baud ==  28800) ?   34 : \
   (baud ==  38400) ?  131 : \
   (baud ==  57600) ?   34 : \
   (baud ==  76800) ?  131 : \
   (baud == 115200) ?   34 : \
   (baud == 230400) ?   34 : \
   (baud == 307200) ?  131 : \
   (baud == 460800) ?   34 : \
   0)


//*****************************************************************************

// Macro for setting up a UART transfer channel. The macro sets the appropriate
// pins for peripheral operation, sets the baudrate, and the desired options of
// the selected uart. _uart_ indicates which uart to configure and must be
// either 0 or 1. _baudRate_ must be one of 2400, 4800, 9600, 14400, 19200,
// 28800, 38400, 57600, 76800, 115200, 153600, 230400 or 307200. Possible
// options are defined below.
//
// Example usage:
//
//      UART_SETUP(0,115200,HIGH_STOP);
//
// This configures uart 0 for contact with "hyperTerminal", setting:
//      Baudrate:           115200
//      Data bits:          8
//      Parity:             None
//      Stop bits:          1
//      Flow control:       None
//

#define UART_SETUP(uart, baudRate, options)      \
   do {                                          \
      if ((options) & FLOW_CONTROL_ENABLE){     \
         if((uart) == 0){      /* USART0       */\
            if(PERCFG & 0x01){ /* Alt 2        */\
               P1SEL |= 0x3C;                    \
            } else {           /* Alt 1        */\
               P0SEL |= 0x3C;                    \
            }                                    \
         }                                       \
         else {                /* USART1       */\
            if(PERCFG & 0x02){ /* Alt 2        */\
               P1SEL |= 0xF0;                    \
            } else {           /* Alt 1        */\
               P0SEL |= 0x3C;                    \
            }                                    \
         }                                       \
      }                                          \
      else{                    /* Flow Ctrl Dis*/\
         if((uart) == 0){      /* USART0       */\
            if(PERCFG & 0x01){ /* Alt 2        */\
               P1SEL |= 0x30;                    \
            } else {           /* Alt 1        */\
               P0SEL |= 0x0C;                    \
            }                                    \
         }                                       \
         else {                /* USART1       */\
            if(PERCFG & 0x02){ /* Alt 2        */\
               P1SEL |= 0xC0;                    \
            } else {           /* Alt 1        */\
               P0SEL |= 0x30;                    \
            }                                    \
         }                                       \
      }                                          \
                                                 \
      U##uart##GCR = BAUD_E((baudRate), CLKSPD); \
      U##uart##BAUD = BAUD_M(baudRate);          \
                                                 \
      U##uart##CSR |= 0x80;                      \
                                                 \
      U##uart##UCR |= ((options) | 0x80);        \
                                                 \
      if((options) & TRANSFER_MSB_FIRST){        \
         U##uart##GCR |= 0x20;                   \
      }                                          \
   } while(0)


// Options for UART_SETUP macro
#define FLOW_CONTROL_ENABLE         0x40
#define FLOW_CONTROL_DISABLE        0x00
#define EVEN_PARITY                 0x20
#define ODD_PARITY                  0x00
#define NINE_BIT_TRANSFER           0x10
#define EIGHT_BIT_TRANSFER          0x00
#define PARITY_ENABLE               0x08
#define PARITY_DISABLE              0x00
#define TWO_STOP_BITS               0x04
#define ONE_STOP_BITS               0x00
#define HIGH_STOP                   0x02
#define LOW_STOP                    0x00
#define HIGH_START                  0x01
#define TRANSFER_MSB_FIRST          0x80
#define TRANSFER_MSB_LAST           0x00


// Example usage:
//   if(UART0_PARERR())
//     ...
#define UART_PARERR(num)      ((U##num##CSR & 0x08) == 0x08)
#define UART0_PARERR()        UART_PARERR(0)
#define UART1_PARERR()        UART_PARERR(1)

// Example usage:
//   if(UART1_FRAMEERR())
//     ...
#define UART_FRAMEERR(num)    ((U ##num## CSR & 0x10) == 0x10)
#define UART0_FRAMEERR()      UART_FRAMEERR(0)
#define UART1_FRAMEERR()      UART_FRAMEERR(1)


// Example usage:
//   char ch = 'A';
//   UART1_SEND(ch);
//   ...
//   UART1_RECEIVE(ch);
#define UART_SEND(num, x)   U##num##DBUF = x
#define UART0_SEND(x)       UART_SEND(0, x)
#define UART1_SEND(x)       UART_SEND(1, x)

#define UART_RECEIVE(num, x)  x = U##num##DBUF
#define UART0_RECEIVE(x)      UART_RECEIVE(0, x)
#define UART1_RECEIVE(x)      UART_RECEIVE(1, x)

/******************************************************************************
*******************      Power and clock management        ********************
*******************************************************************************

These macros are used to set power-mode, clock source and clock speed.

******************************************************************************/

// Macro for getting the clock division factor
#define CLKSPD  (CLKCON & 0x07)

// Macro for getting the timer tick division factor.
#define TICKSPD ((CLKCON & 0x38) >> 3)

// Macro for checking status of the crystal oscillator
#define XOSC_STABLE (SLEEP & 0x40)

// Macro for checking status of the high frequency RC oscillator.
#define HIGH_FREQUENCY_RC_OSC_STABLE    (SLEEP & 0x20)


// Macro for setting power mode
#define SET_POWER_MODE(mode)                   \
         if(mode == 0)        { SLEEP &= ~0x03; } \
      else if (mode == 3)  { SLEEP |= 0x03;  } \
      else { SLEEP &= ~0x03; SLEEP |= mode;  } \
      PCON |= 0x01;                            \
     // asm("NOP");                              \

// Where _mode_ is one of
#define POWER_MODE_0  0x00  // Clock oscillators on, voltage regulator on
#define POWER_MODE_1  0x01  // 32.768 KHz oscillator on, voltage regulator on
#define POWER_MODE_2  0x02  // 32.768 KHz oscillator on, voltage regulator off
#define POWER_MODE_3  0x03  // All clock oscillators off, voltage regulator off

// Macro for setting the 32 KHz clock source
#define SET_32KHZ_CLOCK_SOURCE(source) \
   do {                                \
      if( source ) {                   \
         CLKCON |= 0x80;               \
      } else {                         \
         CLKCON &= ~0x80;              \
      }                                \
   } while (0)

// Where _source_ is one of
#define CRYSTAL 0x00
#define RC      0x01

// Macro for setting the main clock oscillator source,
//turns off the clock source not used
//changing to XOSC will take approx 150 us


// Macro for setting the main clock division,
#define SET_MAIN_CLOCK_SPEED(frequency)   \
   do {                                   \
        CLKCON = ((CLKCON & ~0x07) | (frequency & 0x07));     \
   }while (0)

// where frequency is one of
#define MHZ_26          0x00
#define MHZ_13          0x01
#define MHZ_6_5         0x02
#define MHZ_3_25        0x03
#define MHZ_1_62        0x04
#define MHZ_0_81        0x05
#define MHZ_0_40        0x06
#define MHZ_0_20        0x07
#endif
