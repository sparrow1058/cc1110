#ifndef _MENU_H
#define _MENU_H

#include "lcd128_64.h"

extern unsigned char MenuItem[4][17];
extern INT8U NowItem;
extern INT8U FirstItem;
extern INT8U TopDisp;

extern INT8U  DrawMenu(INT8U MenuItem[][17] , INT8U MenuNo);
extern void MenuMenuDisp( void );
extern void Page7Display(INT8U ss, INT8U tt);
extern void TurnOnDisp(void);
#endif
