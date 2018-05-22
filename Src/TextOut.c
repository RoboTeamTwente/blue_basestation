/*
 * TextOut.c
 *
 *  Created on: 14 sep. 2016
 *      Author: Hans-van-der-Heide
 */

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "TextOut.h"
#include "usbd_cdc.h"
#include "usb_device.h"
char smallStrBuffer[1024];


void TextOut(char *str){
	HAL_GPIO_WritePin(LD0_GPIO_Port,LD0_Pin, 0);

	int length = strlen(str);
	HexOut((uint8_t*)str, length);

	HAL_GPIO_WritePin(LD0_GPIO_Port,LD0_Pin, 1);
}

void HexOut(uint8_t data[], uint8_t length){
	while(((USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData)->TxState != 0);
	memcpy(smallStrBuffer, data, length);
	CDC_Transmit_FS((uint8_t*)smallStrBuffer, length);
}
