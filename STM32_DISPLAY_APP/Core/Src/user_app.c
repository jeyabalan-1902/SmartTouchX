/*
 * user_app.c
 *
 *  Created on: Jun 27, 2025
 *      Author: kjeyabalan
 */

#include "user_app.h"
#include "uart_rec.h"
#include "spi_rec.h"
#include "display_ctrl.h"
#include "onwords_logo.h"


SemaphoreHandle_t deviceStateMutex;
BaseType_t status;

const char *devices[DEVICE_COUNT] = {"device1", "device2", "device3", "device4"};
GPIO_TypeDef* ports[DEVICE_COUNT] = {TOUCH_LED1_GPIO_Port, TOUCH_LED2_GPIO_Port, TOUCH_LED3_GPIO_Port, TOUCH_LED4_GPIO_Port};
uint16_t pins[DEVICE_COUNT] = {TOUCH_LED1_Pin, TOUCH_LED2_Pin, TOUCH_LED3_Pin, TOUCH_LED4_Pin};
volatile int global_device_states[4] = {0, 0, 0, 0};


void setup_freeRTOS(void)
{
	HAL_SPI_Receive_IT(&hspi2, &spiRxByte, 1);
	HAL_UART_Receive_IT(&huart3, &uartRxByte, 1);

	status = xTaskCreate(SPI_Handler, "SPIHandler", 512, NULL, 5, NULL);
	configASSERT(status == pdPASS);

	status = xTaskCreate(UART_Handler, "UARTHandler", 1024, NULL, 4, NULL);
	configASSERT(status == pdPASS);

	status = xTaskCreate(Display_Handler, "DisplayHandler", 1024, NULL, 3, NULL);
	configASSERT(status == pdPASS);

	vTaskStartScheduler();
}

void user_app_init(void)
{
	ST7735_Init(0);
	ST7735_SetRotation(1);
	fillScreen(BLACK);
	showStartupLogoAndMenu();
	deviceStateMutex = xSemaphoreCreateMutex();
	for (int i = 0; i < 4; i++) {
	  global_device_states[i] = 0;
	  device_states[i] = 0;
	}
	HAL_GPIO_WritePin(TI_SS_GPIO_Port, TI_SS_Pin, GPIO_PIN_SET);
	initializeMenu();
	setup_freeRTOS();
}







