#ifndef _LCD_H_
#define _LCD_H_



void delay_us(unsigned int Time);
void LCD_Enable(void);
void LCD_Send4Bit(unsigned char Data);
void LCD_SendCommand(unsigned char command);
void LCD_Init();
void LCD_Gotoxy(unsigned char x, unsigned char y);
void LCD_Clear();
void LCD_PutChar(unsigned char Data);
void LCD_Puts(char *s);
void hien_thi();

#endif

