#include "lcd.h"
#include "main.h"
#include <Park.h>
#include <Sensor.h>
#include "stm32f1xx.h"

extern TIM_HandleTypeDef htim2;

void delay_us(unsigned int Time)
{
	unsigned int i,j;
	for(i=0;i<Time;i++)
	{
		for(j=0;j<2;j++);
	}
} 

void LCD_Enable(void)
{
    HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin,GPIO_PIN_SET);
    HAL_Delay(1);	 	
    HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);
}

void LCD_Send4Bit(unsigned char Data)
{
  HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, Data & 0x01);
  HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (Data>>1)&1);
  HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (Data>>2)&1);
  HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (Data>>3)&1);
}

void LCD_SendCommand(unsigned char command)
{
  LCD_Send4Bit(command >> 4);
  LCD_Enable();
  LCD_Send4Bit(command);
  LCD_Enable();

}



void LCD_Init()
{

  LCD_Send4Bit(0x00);
	HAL_Delay(200);
	
  HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
	
  LCD_Send4Bit(0x03);
  LCD_Enable();
  LCD_Enable();
  LCD_Enable();
  LCD_Send4Bit(0x02);
  LCD_Enable();
  LCD_SendCommand(0x28); // giao thuc 4 bit, hien thi 2 hang, ki tu 5x8
  LCD_SendCommand(0x0C); // cho phep hien thi man hinh9
  LCD_SendCommand(0x06); // tang ID, khong dich khung hinh
  LCD_SendCommand(0x01); // xoa toan bo khung hinh
}

void LCD_Gotoxy(unsigned char x, unsigned char y)
{
  unsigned char address;
  if(y == 0)address=(0x80 + x);
  else if(y == 1) address=(0xc0 + x);
  LCD_SendCommand(address);
}

void LCD_Clear()
{
  LCD_SendCommand(0x01);
  HAL_Delay(2);
}

void LCD_PutChar(unsigned char Data)
{
  HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin,GPIO_PIN_SET);
  LCD_SendCommand(Data);
  HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin,GPIO_PIN_RESET);
}

void LCD_Puts(char *s)
{
  while (*s)
  {
    LCD_PutChar(*s);
    s++;
  }
}

void hien_thi(void)
{
	// Xe vào hoặc xe ra mã thẻ đúng
	if(barie == 1)
	{
		buzzer(2);
		if(save_status == 1)
		{
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 10);
		}

		else
		{
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 10);
		}
		LCD_Clear();
		LCD_Gotoxy(3, 0);
		LCD_Puts("BARIE OPEN");

		HAL_Delay(6000);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 20);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 20);
		barie = 0;
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 20);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 20);
		LCD_Gotoxy(3, 0);
		LCD_Puts("BARIE CLOSE");
	}

	// Xe ra nhưng mã thẻ sai
	if(save_status == 1 && overlap_status == 0)
	{
		LCD_Clear();
		LCD_Gotoxy(6, 0);
		LCD_Puts("ERROR");
		buzzer(6);
		HAL_Delay(5000);
		LCD_Clear();
		LCD_Gotoxy(3, 0);
		LCD_Puts("BARIE CLOSE");
		save_status = 0;
	}

	// Hiển thị số vị trí trống
	LCD_Gotoxy(0, 1);
	LCD_Puts("EMPTY: ");
	LCD_Gotoxy(7, 1);
	LCD_PutChar(EMPTY_NUMBER + 48);
	LCD_Gotoxy(9, 1);
	LCD_Puts("- ");

	// Hiển thị cụ thể vị trí trống
	LCD_Gotoxy(11, 1);
	LCD_PutChar(!Sensor_buff[4] + 48);
	LCD_Gotoxy(12, 1);
	LCD_PutChar(!Sensor_buff[3] + 48);
	LCD_Gotoxy(13, 1);
	LCD_PutChar(!Sensor_buff[2] + 48);
	LCD_Gotoxy(14, 1);
	LCD_PutChar(!Sensor_buff[1] + 48);
	LCD_Gotoxy(15, 1);
	LCD_PutChar(!Sensor_buff[0] + 48);
}
