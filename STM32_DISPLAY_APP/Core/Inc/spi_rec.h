/*
 * spi_rec.h
 *
 *  Created on: Jul 17, 2025
 *      Author: kjeya
 */

#ifndef INC_SPI_REC_H_
#define INC_SPI_REC_H_

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

#define SPI_RING_BUFFER_SIZE   128

extern uint8_t spiRingBuffer[SPI_RING_BUFFER_SIZE];
extern volatile uint16_t spiHead;
extern volatile uint16_t spiTail;
extern uint8_t spiRxByte;

extern SPI_HandleTypeDef hspi2;
void SPI_Handler(void *param);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void process_spi_json(uint8_t *jsonBuffer);

#endif /* INC_SPI_REC_H_ */
