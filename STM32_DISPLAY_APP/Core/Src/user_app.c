/*
 * user_app.c
 *
 *  Created on: Jun 27, 2025
 *      Author: kjeyabalan
 */

#include "user_app.h"

QueueHandle_t spiQueue;
QueueHandle_t jsonTxQueue;
SemaphoreHandle_t deviceStateMutex;
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

	initializeMenu();
	setup_freeRTOS();
}

void Display_Handler(void *param)
{
	while(1)
	{
		Menu_Handler();
	}
}

void SPI_handler(void *param)
{
    uint8_t localSpiRxBuffer[3] = {0};
    uint8_t deviceState;
    GPIO_PinState state;
    int deviceIndex = -1;

    while (1) {
        if (xQueueReceive(spiQueue, localSpiRxBuffer, portMAX_DELAY) == pdTRUE)
        {
            if (strcmp((char *)localSpiRxBuffer, "L1") == 0)
            {
            	if(xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
            	{
            		deviceIndex = 0;
            		HAL_GPIO_TogglePin(TOUCH_LED1_GPIO_Port, TOUCH_LED1_Pin);
					state = HAL_GPIO_ReadPin(TOUCH_LED1_GPIO_Port, TOUCH_LED1_Pin);
					deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
                    global_device_states[deviceIndex] = deviceState;
                    xSemaphoreGive(deviceStateMutex);
            	}

                cJSON *jsonObj = cJSON_CreateObject();
				if (jsonObj != NULL)
				{
					cJSON_AddNumberToObject(jsonObj, "device1", deviceState);
					char *jsonStr = cJSON_PrintUnformatted(jsonObj);
					if (jsonStr != NULL)
					{
						HAL_UART_Transmit(&huart3, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
						free(jsonStr);
					}

					cJSON_Delete(jsonObj);
				}
            }
            else if(strcmp((char *)localSpiRxBuffer, "L2") == 0)
            {
            	if(xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
				{
					deviceIndex = 1;
					HAL_GPIO_TogglePin(TOUCH_LED2_GPIO_Port, TOUCH_LED2_Pin);
					state = HAL_GPIO_ReadPin(TOUCH_LED2_GPIO_Port, TOUCH_LED2_Pin);
					deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
					global_device_states[deviceIndex] = deviceState;
					xSemaphoreGive(deviceStateMutex);
				}

				cJSON *jsonObj = cJSON_CreateObject();
				if (jsonObj != NULL)
				{
					cJSON_AddNumberToObject(jsonObj, "device2", deviceState);

					char *jsonStr = cJSON_PrintUnformatted(jsonObj);
					if (jsonStr != NULL)
					{
						HAL_UART_Transmit(&huart3, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
						free(jsonStr);
					}

					cJSON_Delete(jsonObj);
				}
            }
            else if(strcmp((char *)localSpiRxBuffer, "L3") == 0)
            {
            	if(xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
				{
					deviceIndex = 2;
					HAL_GPIO_TogglePin(TOUCH_LED3_GPIO_Port, TOUCH_LED3_Pin);
					state = HAL_GPIO_ReadPin(TOUCH_LED3_GPIO_Port, TOUCH_LED3_Pin);
					deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
					global_device_states[deviceIndex] = deviceState;
					xSemaphoreGive(deviceStateMutex);
				}

				cJSON *jsonObj = cJSON_CreateObject();
				if (jsonObj != NULL)
				{
					cJSON_AddNumberToObject(jsonObj, "device3", deviceState);

					char *jsonStr = cJSON_PrintUnformatted(jsonObj);
					if (jsonStr != NULL)
					{
						HAL_UART_Transmit(&huart3, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
						free(jsonStr);
					}

					cJSON_Delete(jsonObj);
				}
			}
            else if(strcmp((char *)localSpiRxBuffer, "L4") == 0)
            {
            	if(xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
				{
					deviceIndex = 3;
					HAL_GPIO_TogglePin(TOUCH_LED4_GPIO_Port, TOUCH_LED4_Pin);
					state = HAL_GPIO_ReadPin(TOUCH_LED4_GPIO_Port, TOUCH_LED4_Pin);
					deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
					global_device_states[deviceIndex] = deviceState;
					xSemaphoreGive(deviceStateMutex);
				}

				cJSON *jsonObj = cJSON_CreateObject();
				if (jsonObj != NULL)
				{
					cJSON_AddNumberToObject(jsonObj, "device4", deviceState);

					char *jsonStr = cJSON_PrintUnformatted(jsonObj);
					if (jsonStr != NULL)
					{
						HAL_UART_Transmit(&huart3, (uint8_t *)jsonStr, strlen(jsonStr), HAL_MAX_DELAY);
						HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
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
            printf("TOUCH: Device %d set to %s\n", deviceIndex + 1, global_device_states[deviceIndex] ? "ON" : "OFF");
            updateToDisplayMenu();
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
                        		cJSON *resp = cJSON_CreateObject();

                        		if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
									for (int i = 0; i < 4; i++)
									{
										cJSON_AddNumberToObject(resp, devices[i], global_device_states[i]);
									}
									xSemaphoreGive(deviceStateMutex);
								}

								char *respStr = cJSON_PrintUnformatted(resp);
								if (respStr)
								{
									HAL_UART_Transmit(&huart3, (uint8_t *)respStr, strlen(respStr), HAL_MAX_DELAY);
									HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
									free(respStr);
								}

								cJSON_Delete(resp);
                        	}
                        	else
                        	{
                        		if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
                        		{
                        			cJSON *resp = cJSON_CreateObject();
									for (int i = 0; i < 4; i++)
									{
										cJSON *item = cJSON_GetObjectItem(json, devices[i]);
										if (cJSON_IsNumber(item))
										{
											HAL_GPIO_WritePin(ports[i], pins[i], item->valueint ? GPIO_PIN_SET : GPIO_PIN_RESET);
											GPIO_PinState state = HAL_GPIO_ReadPin(ports[i], pins[i]);
											global_device_states[i] = (state == GPIO_PIN_SET) ? 1 : 0;
											cJSON_AddNumberToObject(resp, devices[i], global_device_states[i]);
											printf("ESP: Device %d set to %s\n", i + 1, global_device_states[i] ? "ON" : "OFF");
										}
									}
									xSemaphoreGive(deviceStateMutex);
									char *respStr = cJSON_PrintUnformatted(resp);
									if (respStr)
									{
										HAL_UART_Transmit(&huart3, (uint8_t *)respStr, strlen(respStr), HAL_MAX_DELAY);
										HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
										free(respStr);
									}
									cJSON_Delete(resp);
									updateToDisplayMenu();
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

void updateToDisplayMenu(void)
{
	if(current_menu == MENU_TOTAL_CONTROL)
	{
		last_selection = current_selection;
		displayTotalControlMenu();
	}
	else if(current_menu == MENU_SEPARATE_CONTROL)
	{
		last_selection = current_selection;
		displaySeparateControlMenu();
	}
	else if(current_menu == MENU_DEVICE_CONTROL)
	{
		last_selection = current_selection;
		displayDeviceControlMenu();
	}
}





