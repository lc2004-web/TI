#ifndef __OLED_H
#define __OLED_H

void OLED_Write_Clear();
void OLED_Init();
void OLED_Write_ON();
void OLED_Write_OFF();
void OLED_Write_Display(u8 x,u8 y,u8 g,u8 k,u8 *dat);

#endif
