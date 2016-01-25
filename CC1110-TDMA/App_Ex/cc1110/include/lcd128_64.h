#ifndef _LCD_H
#define _LCD_H

extern void InitDisplay(void);
extern void InitLcd(void);
//extern void LoadICO(INT8U y , INT8U x , INT8U n);
extern void ClearScreen(void);
extern void Printn(INT8U xx ,INT8U yy , INT32U no,INT8U yn,INT8U le);
extern void Printn8(INT8U xx ,INT8U yy , INT32U no,INT8U yn,INT8U le);
extern void Print6(INT8U xx, INT8U yy, INT8U ch1[], INT8U yn);
extern void Print8(INT16U y,INT16U x, INT8U ch[],INT16U yn);
extern void Print16(INT16U y,INT16U x,INT8U ch[],INT16U yn);
extern void Print(INT8U y, INT8U x, INT8U ch[], INT16U yn);
extern void ClearCol(INT8U Begin , INT8U End);
extern void Rectangle(INT8U x1,INT8U y1,INT8U x2,INT8U y2);
extern void DoSetContrast(void);
extern void SetContrast(INT8U Gain, INT8U Step);
extern void SetRamAddr (INT8U Page, INT8U Col);
extern void Lcdwritedata(INT8U dat);
extern void LoadICO(void);
//void MenuMenuDisp(void);


#endif
