/*
 * user_app.h
 *
 *  Created on: Jun 27, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_USER_APP_H_
#define INC_USER_APP_H_

#include "main.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "display_app.h"

#define BOOT_CMD    0x50
#define DEVICE_1    0x55
#define DEVICE_2    0X56
#define DEVICE_3    0x57
#define DEVICE_4    0X58


extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern uint8_t uartRxBuffer[1];
extern uint8_t spiRxBuffer[2];

extern volatile uint8_t dataReceived;
extern volatile uint8_t light1State;
extern volatile uint8_t light2State;
extern volatile uint8_t light3State;
extern volatile uint8_t light4State;

void setup_freeRTOS(void);
void SPI_handler(void *param);
void UART_handler(void *param);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_USER_APP_H_ */
