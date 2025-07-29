/*
 * user_app.h
 *
 *  Created on: Jun 27, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_APP_USER_APP_H_
#define INC_APP_USER_APP_H_

#include <cJSON/cJSON.h>
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "freeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define SPI_BUFFER_SIZE       64
#define DEVICE_COUNT          4
#define KEEP_ALIVE_INTERVAL   60


extern const char *devices[DEVICE_COUNT];
extern GPIO_TypeDef* led_ports[DEVICE_COUNT];
extern uint16_t led_pins[DEVICE_COUNT];
extern GPIO_TypeDef* relay_ports[DEVICE_COUNT];
extern uint16_t relay_pins[DEVICE_COUNT];
extern volatile int global_device_states[4];
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern SemaphoreHandle_t deviceStateMutex;
extern uint32_t lastKeepAliveTime;

void setup_freeRTOS(void);
void SPI_handler(void *param);
void Display_Handler(void *param);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void updateToDisplayMenu(void);
void user_app_init(void);
void print_task_info(void);
void safe_printf(const char *fmt, ...);

#endif /* INC_APP_USER_APP_H_ */
