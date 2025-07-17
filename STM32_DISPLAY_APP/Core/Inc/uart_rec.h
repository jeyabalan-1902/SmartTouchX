/*
 * uart_rec.h
 *
 *  Created on: Jul 17, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_UART_REC_H_
#define INC_UART_REC_H_

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

#include "cJSON.h"

#define UART_RING_BUFFER_SIZE 256
#define BOOT_CMD              0x50
#define JSON_BUFFER_SIZE      150

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern uint8_t uartRingBuffer[UART_RING_BUFFER_SIZE];
extern volatile uint16_t uartHead;
extern volatile uint16_t uartTail;
extern uint8_t uartRxByte;

void UART_Handler(void *param);
void send_json_response(cJSON *jsonObj);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void handle_get_status_request(void);
void handle_device_control(cJSON *json);
void process_json(uint8_t *jsonBuffer);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_UART_REC_H_ */
