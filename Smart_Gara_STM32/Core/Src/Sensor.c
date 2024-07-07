/*
 * Sensor.c
 *
 *  Created on: Dec 19, 2023
 *      Author: Admin
 */

#include "Sensor.h"
#include "main.h"

int EMPTY_NUMBER;
int Sensor_buff[5];

int x;

void SensorRead()
{
	EMPTY_NUMBER =5;
	Sensor_buff[0] = HAL_GPIO_ReadPin(SS1_PIN_GPIO_Port, SS1_PIN_Pin);
	Sensor_buff[1] = HAL_GPIO_ReadPin(SS2_PIN_GPIO_Port, SS2_PIN_Pin);
	Sensor_buff[2] = HAL_GPIO_ReadPin(SS3_PIN_GPIO_Port, SS3_PIN_Pin);
	Sensor_buff[3] = HAL_GPIO_ReadPin(SS4_PIN_GPIO_Port, SS4_PIN_Pin);
	Sensor_buff[4] = HAL_GPIO_ReadPin(SS5_PIN_GPIO_Port, SS5_PIN_Pin);

	for(x=0; x<5; x++)
	{
		if(Sensor_buff[x] == 0)
		{
			EMPTY_NUMBER --;
		}
	}
}

