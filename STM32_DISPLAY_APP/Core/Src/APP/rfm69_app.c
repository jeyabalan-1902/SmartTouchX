/*
 * rfm69_app.c
 *
 *  Created on: Jul 31, 2025
 *      Author: kjeyabalan
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rf69.h"
#include "main.h"
#include "rfm69_app.h"
#include "stm32f4xx_hal.h"
#include "freeRTOS.h"
#include "user_app.h"
#include "display_ctrl.h"

extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart2;
static uint8_t _DeviceState = STATE_INIT;
static uint8_t PrevState = 0;
uint8_t EncryptionKey[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

void RFM_Task(void)
{
	while(1)
	{
		RF69_ModuleHandler();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void RF69_ModuleHandler(void)
{

	if(_DeviceState == STATE_INIT)
	{
		PrevState = STATE_INIT;
		if(RF69_init() == true)
		{
			_DeviceState = SET_FREQ;
			safe_printf("RF69_InitSuccess\n");
		}
		else
			_DeviceState = FAIL_STATE;
	}
	else if(_DeviceState == SET_FREQ)
	{
		PrevState =	SET_FREQ;
		if(setFrequency(RF69_FREQ) == true)
		{
			_DeviceState = SET_POW;
			safe_printf("RF69_FreqSetSuccess\n");
		}
		else
			_DeviceState = FAIL_STATE;
	}
	else if(_DeviceState == SET_POW)
	{
		PrevState = SET_POW;
		setTxPower(20, true);
		_DeviceState = SET_ENCKEY;
		safe_printf("RF69_PowerSetSuccess\n");
	}
	else if(_DeviceState == SET_ENCKEY)
	{
		PrevState = SET_ENCKEY;
		setEncryptionKey(&EncryptionKey[0]);
		_DeviceState = RX_DATA;
		safe_printf("RF69_setEncryptionKeySuccess\n");
	}
	else if(_DeviceState == TX_DATA)
	{
		PrevState = TX_DATA;
	}
	else if(_DeviceState == RX_DATA)
	{
		PrevState = RX_DATA;

		if(RF69_RxData() != true)
			_DeviceState = SYS_RESTART;
	}
	else if(_DeviceState == FAIL_STATE)
	{
		if(FailIndiCation(PrevState) == true)
		{
			safe_printf("Module Going To Restart\n");
			vTaskDelay(pdMS_TO_TICKS(3000));
			_DeviceState = SYS_RESTART;
		}
	}
	else if(_DeviceState == SYS_RESTART)
	{
		_DeviceState = STATE_INIT;
		safe_printf("SYS_Restarted\n");
	}
	else if(_DeviceState == SYS_REST)
	{
		_DeviceState = SYS_SLEEP;
	}
	else if(_DeviceState == SYS_SLEEP)
	{
		setSleep();
		HAL_SPI_DeInit(&hspi3);
		HAL_UART_DeInit(&huart2);
		HAL_SuspendTick();
		HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON,PWR_SLEEPENTRY_WFI);
		_DeviceState = RX_DATA;
	}
}

bool RF69_TxData(void)
{
	return true; //Never Reach Here
}

bool RF69_RxData(void)
{
//	printf("RF69 StartReceiving\n");
//	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	if (available())
	{

		uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
		uint8_t len = sizeof(buf);
		safe_printf("Data Available..\r\n");
		if (recv1(buf, &len))
		{
			if (!len)
			{
				buf[len] = 0;
			}

			safe_printf("ReceivedData [%d]:%s\n", len, (char*)buf);
			safe_printf("RSSI: %d\n", lastRssi());

			if (strstr((char *)buf, "Control#1"))
			{
				setAllDevicesState(1);
			}
			else if(strstr((char *)buf, "Control#0"))
			{
				setAllDevicesState(0);
			}
		}
		else
		{
			safe_printf("Receive failed\n");
		}
	}
	return true;
}

bool FailIndiCation(Rf69_t FailState)
{
	if(FailState == STATE_INIT)
		safe_printf("ModuleInit_Failed\n");
	else if(FailState == SET_FREQ)
		safe_printf("FreqSetOp_Failed\n");
	else if(FailState == SET_POW)
		safe_printf("PowerSetOp_Failed\n");
	else if(FailState == SET_ENCKEY)
		safe_printf("EncKeySetOp_Failed\n");
	else if(FailState == TX_DATA)
		safe_printf("DataTxOp_Failed\n");
	else if(FailState == RX_DATA)
		safe_printf("DataRxOp_Failed\n");
	return true;
}
