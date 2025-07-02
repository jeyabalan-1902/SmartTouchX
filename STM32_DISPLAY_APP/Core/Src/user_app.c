/*
 * user_app.c
 *
 *  Created on: Jun 27, 2025
 *      Author: kjeyabalan
 */

#include "user_app.h"

QueueHandle_t spiQueue;
QueueHandle_t uartQueue;

BaseType_t status;
xSemaphoreHandle xUartMutex;
xSemaphoreHandle xSpiMutex;

uint8_t uartRxBuffer[1];
uint8_t spiRxBuffer[2];

volatile uint8_t dataReceived = 0;
volatile uint8_t light1State;
volatile uint8_t light2State;
volatile uint8_t light3State;
volatile uint8_t light4State;

void setup_freeRTOS(void)
{
	spiQueue = xQueueCreate(10, sizeof(spiRxBuffer));
	uartQueue = xQueueCreate(10, sizeof(uartRxBuffer));
	configASSERT(spiQueue != NULL && uartQueue != NULL);

	// Start SPI and UART interrupt-based reception
	HAL_SPI_Receive_IT(&hspi2, spiRxBuffer, sizeof(spiRxBuffer));
	HAL_UART_Receive_IT(&huart3, uartRxBuffer, sizeof(uartRxBuffer));

	xUartMutex = xSemaphoreCreateMutex();

	status = xTaskCreate(SPI_handler, "SPIHandler", 512, NULL, 5, NULL);
	configASSERT(status == pdPASS);

	status = xTaskCreate(UART_handler, "UARTHandler", 256, NULL, 4, NULL);
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
            	print_To_display("Device 1 operation");
                HAL_GPIO_TogglePin(TOUCH_LED1_GPIO_Port, TOUCH_LED1_Pin);
                state = HAL_GPIO_ReadPin(TOUCH_LED1_GPIO_Port, TOUCH_LED1_Pin);
                deviceState = (state == GPIO_PIN_SET) ? 1 : 0;

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
            	print_To_display("Device 2 operation");
            	HAL_GPIO_TogglePin(TOUCH_LED2_GPIO_Port, TOUCH_LED2_Pin);
            	state = HAL_GPIO_ReadPin(TOUCH_LED2_GPIO_Port, TOUCH_LED2_Pin);
				deviceState = (state == GPIO_PIN_SET) ? 1 : 0;

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
            	print_To_display("Device 3 operation");
				HAL_GPIO_TogglePin(TOUCH_LED3_GPIO_Port, TOUCH_LED3_Pin);
				state = HAL_GPIO_ReadPin(TOUCH_LED3_GPIO_Port, TOUCH_LED3_Pin);
				deviceState = (state == GPIO_PIN_SET) ? 1 : 0;

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
            	print_To_display("Device 4 operation");
				HAL_GPIO_TogglePin(TOUCH_LED4_GPIO_Port, TOUCH_LED4_Pin);
				state = HAL_GPIO_ReadPin(TOUCH_LED4_GPIO_Port, TOUCH_LED4_Pin);
				deviceState = (state == GPIO_PIN_SET) ? 1 : 0;

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
    while (1) {
        if (xQueueReceive(uartQueue, uartRxBuffer, portMAX_DELAY) == pdTRUE) {
            printf("UART Data Received: %s\n", uartRxBuffer);
            if(uartRxBuffer[0] == BOOT_CMD)
            {
            	NVIC_SystemReset();
            }
            else if (uartRxBuffer[0] == DEVICE_1)
            {
            	HAL_GPIO_TogglePin(TOUCH_LED1_GPIO_Port, TOUCH_LED1_Pin);
            	printf("command L processed\r\n");
            }
            else if(uartRxBuffer[0] == DEVICE_2)
            {
            	HAL_GPIO_TogglePin(TOUCH_LED2_GPIO_Port, TOUCH_LED2_Pin);
            	printf("command M processed\r\n");
            }
            else if(uartRxBuffer[0] == DEVICE_3)
            {
				HAL_GPIO_TogglePin(TOUCH_LED3_GPIO_Port, TOUCH_LED3_Pin);
				printf("command N processed \r\n");
			}
            else if(uartRxBuffer[0] == DEVICE_4)
            {
				HAL_GPIO_TogglePin(TOUCH_LED4_GPIO_Port, TOUCH_LED4_Pin);
				printf("command O processed\r\n");
			}
            else
            {
            	printf("command does not found , Junk data received on UART\r\n");
            }
        }
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI2) {
    	printf("SPI Interrupt Triggered\n");
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
    	printf("UART Interrupt Triggered\n");
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(uartQueue, uartRxBuffer, &xHigherPriorityTaskWoken);
        memset(uartRxBuffer, 0, sizeof(uartRxBuffer));
        HAL_UART_Receive_IT(&huart3, uartRxBuffer, sizeof(uartRxBuffer));
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

