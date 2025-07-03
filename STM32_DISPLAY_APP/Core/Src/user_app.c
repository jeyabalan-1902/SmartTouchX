/*
 * user_app.c
 *
 *  Created on: Jun 27, 2025
 *      Author: kjeyabalan
 */

#include "user_app.h"

QueueHandle_t spiQueue;
BaseType_t status;

static uint8_t uartRingBuffer[UART_RING_BUFFER_SIZE];
static volatile uint16_t uartHead = 0;
static volatile uint16_t uartTail = 0;
static uint8_t uartRxByte;

uint8_t spiRxBuffer[2];

void setup_freeRTOS(void)
{
	spiQueue = xQueueCreate(10, sizeof(spiRxBuffer));
	configASSERT(spiQueue != NULL);

	HAL_SPI_Receive_IT(&hspi2, spiRxBuffer, sizeof(spiRxBuffer));
	HAL_UART_Receive_IT(&huart3, &uartRxByte, 1);


	status = xTaskCreate(SPI_handler, "SPIHandler", 512, NULL, 5, NULL);
	configASSERT(status == pdPASS);

	status = xTaskCreate(UART_handler, "UARTHandler", 1024, NULL, 4, NULL);
	configASSERT(status == pdPASS);

	vTaskStartScheduler();
}

void SPI_handler(void *param)
{
    uint8_t localSpiRxBuffer[3] = {0};
    uint8_t deviceState;
    GPIO_PinState state;

    while (1) {
        if (xQueueReceive(spiQueue, localSpiRxBuffer, portMAX_DELAY) == pdTRUE)
        {
            printf("SPI Data Received: %s\n", localSpiRxBuffer);
            if (strcmp((char *)localSpiRxBuffer, "L1") == 0)
            {
                HAL_GPIO_TogglePin(TOUCH_LED1_GPIO_Port, TOUCH_LED1_Pin);
                state = HAL_GPIO_ReadPin(TOUCH_LED1_GPIO_Port, TOUCH_LED1_Pin);
                deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
                if(deviceState == 1) print_To_display("Device 1 ON");
                else print_To_display("Device 1 OFF");

                cJSON *jsonObj = cJSON_CreateObject();
				if (jsonObj != NULL)
				{
					cJSON_AddNumberToObject(jsonObj, "device1", deviceState);
					char *jsonStr = cJSON_PrintUnformatted(jsonObj);
					if (jsonStr != NULL)
					{
						HAL_UART_Transmit(&huart3, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
						printf("JSON Sent via UART: %s\n", jsonStr);
						free(jsonStr);
					}

					cJSON_Delete(jsonObj);
				}
            }
            else if(strcmp((char *)localSpiRxBuffer, "L2") == 0)
            {
            	HAL_GPIO_TogglePin(TOUCH_LED2_GPIO_Port, TOUCH_LED2_Pin);
            	state = HAL_GPIO_ReadPin(TOUCH_LED2_GPIO_Port, TOUCH_LED2_Pin);
				deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
				if(deviceState == 1) print_To_display("Device 2 ON");
				else print_To_display("Device 2 OFF");

				cJSON *jsonObj = cJSON_CreateObject();
				if (jsonObj != NULL)
				{
					cJSON_AddNumberToObject(jsonObj, "device2", deviceState);

					char *jsonStr = cJSON_PrintUnformatted(jsonObj);
					if (jsonStr != NULL)
					{
						HAL_UART_Transmit(&huart3, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
						printf("JSON Sent via UART: %s\n", jsonStr);
						free(jsonStr);
					}

					cJSON_Delete(jsonObj);
				}
            }
            else if(strcmp((char *)localSpiRxBuffer, "L3") == 0)
            {
				HAL_GPIO_TogglePin(TOUCH_LED3_GPIO_Port, TOUCH_LED3_Pin);
				state = HAL_GPIO_ReadPin(TOUCH_LED3_GPIO_Port, TOUCH_LED3_Pin);
				deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
				if(deviceState == 1) print_To_display("Device 3 ON");
				else print_To_display("Device 3 OFF");

				cJSON *jsonObj = cJSON_CreateObject();
				if (jsonObj != NULL)
				{
					cJSON_AddNumberToObject(jsonObj, "device3", deviceState);

					char *jsonStr = cJSON_PrintUnformatted(jsonObj);
					if (jsonStr != NULL)
					{
						HAL_UART_Transmit(&huart3, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
						printf("JSON Sent via UART: %s\n", jsonStr);
						free(jsonStr);
					}

					cJSON_Delete(jsonObj);
				}
			}
            else if(strcmp((char *)localSpiRxBuffer, "L4") == 0)
            {
				HAL_GPIO_TogglePin(TOUCH_LED4_GPIO_Port, TOUCH_LED4_Pin);
				state = HAL_GPIO_ReadPin(TOUCH_LED4_GPIO_Port, TOUCH_LED4_Pin);
				deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
				if(deviceState == 1) print_To_display("Device 4 ON");
				else print_To_display("Device 4 OFF");

				cJSON *jsonObj = cJSON_CreateObject();
				if (jsonObj != NULL)
				{
					cJSON_AddNumberToObject(jsonObj, "device4", deviceState);

					char *jsonStr = cJSON_PrintUnformatted(jsonObj);
					if (jsonStr != NULL)
					{
						HAL_UART_Transmit(&huart3, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
						printf("JSON Sent via UART: %s\n", jsonStr);
						free(jsonStr);
					}

					cJSON_Delete(jsonObj);
				}
			}
            else
            {
            	printf("junk data received on SPI\r\n");
            }
            memset(localSpiRxBuffer, 0, sizeof(localSpiRxBuffer));
        }
    }
}

void UART_handler(void *param)
{
    uint8_t jsonBuffer[128];
    uint8_t index = 0;
    bool collecting = false;

    while (1)
    {
        if (uartHead != uartTail)
        {
            uint8_t byte = uartRingBuffer[uartTail];
            uartTail = (uartTail + 1) % UART_RING_BUFFER_SIZE;

            if (byte == '{')
            {
                collecting = true;
                index = 0;
                jsonBuffer[index++] = byte;
            }
            else if (collecting)
            {
                if (index < sizeof(jsonBuffer) - 1)
                {
                    jsonBuffer[index++] = byte;

                    if (byte == '}')
                    {
                        jsonBuffer[index] = '\0';
                        printf("JSON received: %s\n", jsonBuffer);

                        cJSON *json = cJSON_Parse((char *)jsonBuffer);
                        if (json)
                        {
                        	const char *devices[] = {"device1", "device2", "device3", "device4"};
							GPIO_TypeDef* ports[] = {TOUCH_LED1_GPIO_Port, TOUCH_LED2_GPIO_Port, TOUCH_LED3_GPIO_Port, TOUCH_LED4_GPIO_Port};
							uint16_t pins[] = {TOUCH_LED1_Pin, TOUCH_LED2_Pin, TOUCH_LED3_Pin, TOUCH_LED4_Pin};

                        	cJSON *getCurrentStatus = cJSON_GetObjectItem(json, "request");
                        	if(strcmp(getCurrentStatus->valuestring, "getCurrentStatus") == 0)
                        	{
                        		for (int i = 0; i < 4; i++)
                        		{
                        			GPIO_PinState state = HAL_GPIO_ReadPin(ports[i], pins[i]);
									cJSON *resp = cJSON_CreateObject();
									cJSON_AddNumberToObject(resp, devices[i], (state == GPIO_PIN_SET) ? 1 : 0);
									char *respStr = cJSON_PrintUnformatted(resp);
									if (respStr)
									{
										HAL_UART_Transmit(&huart3, (uint8_t *)respStr, strlen(respStr), HAL_MAX_DELAY);
										HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
										free(respStr);
									}
									cJSON_Delete(resp);
                        		}
                        	}
                        	else
                        	{
                        		for (int i = 0; i < 4; i++)
								{
									cJSON *item = cJSON_GetObjectItem(json, devices[i]);
									if (cJSON_IsNumber(item))
									{
										HAL_GPIO_WritePin(ports[i], pins[i], item->valueint ? GPIO_PIN_SET : GPIO_PIN_RESET);
										GPIO_PinState state = HAL_GPIO_ReadPin(ports[i], pins[i]);

										cJSON *resp = cJSON_CreateObject();
										cJSON_AddNumberToObject(resp, devices[i], (state == GPIO_PIN_SET) ? 1 : 0);
										char *respStr = cJSON_PrintUnformatted(resp);
										if (respStr)
										{
											HAL_UART_Transmit(&huart3, (uint8_t *)respStr, strlen(respStr), HAL_MAX_DELAY);
											HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
											free(respStr);
										}
										cJSON_Delete(resp);
										char displayMsg[32];
										snprintf(displayMsg, sizeof(displayMsg), "Device %d %s", i + 1, (state == GPIO_PIN_SET) ? "ON" : "OFF");
										print_To_display(displayMsg);
									}
								}
                        	}
                            cJSON_Delete(json);
                        }
                        else
                        {
                            printf("Invalid JSON!\n");
                        }
                        collecting = false;
                        index = 0;
                    }
                }
                else
                {
                    collecting = false;
                    index = 0;
                }
            }
            else if (byte == BOOT_CMD)
            {
                NVIC_SystemReset();
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}


void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI2) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(spiQueue, spiRxBuffer, &xHigherPriorityTaskWoken);
        memset(spiRxBuffer, 0, sizeof(spiRxBuffer));
        HAL_SPI_Receive_IT(&hspi2, spiRxBuffer, sizeof(spiRxBuffer));
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    __HAL_SPI_CLEAR_OVRFLAG(hspi);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3) {
        uint16_t nextHead = (uartHead + 1) % UART_RING_BUFFER_SIZE;

        if (nextHead != uartTail) {
            uartRingBuffer[uartHead] = uartRxByte;
            uartHead = nextHead;
        } else {
            printf("UART ring buffer overflow!\n");
        }

        HAL_UART_Receive_IT(&huart3, &uartRxByte, 1);
    }
}





