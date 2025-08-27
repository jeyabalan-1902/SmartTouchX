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
#include "display_spi_app.h"

extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart2;
static uint8_t _DeviceState = STATE_INIT;
static uint8_t PrevState = 0;
uint8_t EncryptionKey[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
int count;

void RFM_Task(void *parameter)
{
	while(1)
	{
		rfmAlive = true;
		RF69_ModuleHandler();
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

void RF_REC_LED_Task(void *parameter)
{
	while(1)
	{
		if(xSemaphoreTake(rfled_semaphore, portMAX_DELAY) == pdTRUE)
		{
			HAL_GPIO_WritePin(RF_REC_LED_GPIO_Port, RF_REC_LED_Pin, GPIO_PIN_SET);
			vTaskDelay(pdMS_TO_TICKS(100));
			HAL_GPIO_WritePin(RF_REC_LED_GPIO_Port, RF_REC_LED_Pin, GPIO_PIN_RESET);
		}
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

bool RF69_RxData(void)
{
//	printf("RF69 StartReceiving\n");
//	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	if (available())
	{

		uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
		uint8_t len = sizeof(buf);
		//safe_printf("Data Available..\r\n");
		if (recv1(buf, &len)) {
		    if (len >= sizeof(buf)) len = sizeof(buf) - 1;
		    buf[len] = '\0'; // always terminate

		    //safe_printf("ReceivedData [%d]:%s\n", len, (char*)buf);
//			safe_printf("RFM RSSI: %d\n", lastRssi());

		    xSemaphoreGive(rfled_semaphore);

			if (strstr((char *)buf, "Control_DOWN")) {
			    ButtonEvent_t evt = BTN_EVENT_DOWN;
			    if(xQueueSend(btnEventQueue, &evt, 0) != pdPASS)
			    {
			    	safe_printf("queue send failed\n");
			    }
			}
			else if (strstr((char *)buf, "Control_UP")) {
			    ButtonEvent_t evt = BTN_EVENT_UP;
			    if(xQueueSend(btnEventQueue, &evt, 0) != pdPASS)
			    {
			    	safe_printf("queue send failed\n");
			    }
			}
			else if (strstr((char *)buf, "Control_ENTER")) {
			    ButtonEvent_t evt = BTN_EVENT_ENTER;
			    if(xQueueSend(btnEventQueue, &evt, 0) != pdPASS)
			    {
			    	safe_printf("queue send failed\n");
			    }
			}
			else if (strstr((char *)buf, "Control_DISP")){
				HAL_GPIO_TogglePin(DISP_BACKLIT_GPIO_Port, DISP_BACKLIT_Pin);
				GPIO_PinState state = HAL_GPIO_ReadPin(DISP_BACKLIT_GPIO_Port, DISP_BACKLIT_Pin);
				safe_printf("RFM: Display Backlit is %s \n",(state == GPIO_PIN_SET) ? "ON" : "OFF");
			}
		}
		else
		{
			safe_printf("Receive failed\n");
		}
		setModeRx();
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
