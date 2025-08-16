/*
 * spi_rec.c
 *
 *  Created on: Jul 17, 2025
 *      Author: kjeyabalan
 */

#include "user_app.h"
#include "touch_spi_app.h"
#include "display_spi_app.h"
#include "esp32_uart_app.h"

uint8_t spiRingBuffer[SPI_RING_BUFFER_SIZE];
volatile uint16_t spiHead = 0;
volatile uint16_t spiTail = 0;
uint8_t spiRxByte;

typedef enum
{
	SPI_IDLE,
	SPI_COLLECT_JSON,
	SPI_PROCESS_JSON
}SpiState_t;

static SpiState_t spiState = SPI_IDLE;

void SPI_Handler(void *param)
{
    uint8_t jsonBuffer[SPI_RING_BUFFER_SIZE];
    uint8_t index = 0;

    while (1)
    {
        if (spiHead != spiTail)
        {
            uint8_t byte = spiRingBuffer[spiTail];
            spiTail = (spiTail + 1) % SPI_RING_BUFFER_SIZE;

            switch (spiState)
            {
                case SPI_IDLE:
                    if (byte == '{') {
                        index = 0;
                        jsonBuffer[index++] = byte;
                        spiState = SPI_COLLECT_JSON;
                    }
                    break;

                case SPI_COLLECT_JSON:
                    if (index < SPI_RING_BUFFER_SIZE - 1) {
                        jsonBuffer[index++] = byte;

                        if (byte == '}') {
                            jsonBuffer[index] = '\0';
                            spiState = SPI_PROCESS_JSON;
                        }
                    }
                    else {
                        safe_printf("SPI JSON buffer overflow, resetting...\n");
                        index = 0;
                        spiState = SPI_IDLE;
                    }
                    break;

                case SPI_PROCESS_JSON:
                    process_spi_json(jsonBuffer);
                    spiState = SPI_IDLE;
                    break;
            }
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}


void process_spi_json(uint8_t *jsonBuffer)
{
	uint8_t deviceState;
	GPIO_PinState state;
	int deviceIndex = -1;
	char msg[128];
	cJSON *jsonObj = cJSON_Parse((char *)jsonBuffer);
	if(jsonObj)
	{
		cJSON *device = cJSON_GetObjectItem(jsonObj, "device");
		if(device)
		{
			if(strcmp(device->valuestring, "L1") == 0) deviceIndex = 0;
			else if(strcmp(device->valuestring, "L2") == 0) deviceIndex = 1;
			else if(strcmp(device->valuestring, "L3") == 0) deviceIndex = 2;
			else if(strcmp(device->valuestring, "L4") == 0) deviceIndex = 3;
			else if(strcmp(device->valuestring, "D1") == 0)
			{
				HAL_GPIO_TogglePin(DISP_BACKLIT_GPIO_Port, DISP_BACKLIT_Pin);
				GPIO_PinState state = HAL_GPIO_ReadPin(DISP_BACKLIT_GPIO_Port, DISP_BACKLIT_Pin);
				safe_printf("TOUCH: Display Backlit is %s \n",(state == GPIO_PIN_SET) ? "ON" : "OFF");
			}
		}
		if(deviceIndex != -1)
		{
			cJSON *status = cJSON_CreateObject();
			if(xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
			{
				HAL_GPIO_TogglePin(led_ports[deviceIndex], led_pins[deviceIndex]);
				HAL_GPIO_TogglePin(relay_ports[deviceIndex], relay_pins[deviceIndex]);
				state = HAL_GPIO_ReadPin(led_ports[deviceIndex], led_pins[deviceIndex]);
				deviceState = (state == GPIO_PIN_SET) ? 1 : 0;
				global_device_states[deviceIndex] = deviceState;
				cJSON_AddNumberToObject(status, devices[deviceIndex], global_device_states[deviceIndex]);
				safe_printf("TOUCH: Device %d set to %s\n", deviceIndex + 1, global_device_states[deviceIndex] ? "ON" : "OFF");
				snprintf(msg, sizeof(msg), "Device %d turned %s\n", deviceIndex + 1, global_device_states[deviceIndex] ? "ON" : "OFF");
				//HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
				xSemaphoreGive(deviceStateMutex);
			}
			updateToDisplayMenu();
			send_json_response(status);
			cJSON_Delete(status);
		}
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI2)
    {
        uint16_t nextHead = (spiHead + 1) % SPI_RING_BUFFER_SIZE;
        if(nextHead != spiTail)
        {
        	spiRingBuffer[spiHead] = spiRxByte;
        	spiHead = nextHead;
        }
        else
        {
        	safe_printf("SPI ring buffer overflow\n");
        }
        HAL_SPI_Receive_IT(&hspi2, &spiRxByte, 1);
    }
    __HAL_SPI_CLEAR_OVRFLAG(hspi);
}
