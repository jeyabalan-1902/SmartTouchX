/*
 * spi_rec.c
 *
 *  Created on: Jul 17, 2025
 *      Author: kjeyabalan
 */

#include "spi_rec.h"
#include "user_app.h"
#include "uart_rec.h"
#include "display_ctrl.h"

uint8_t spiRingBuffer[SPI_RING_BUFFER_SIZE];
volatile uint16_t spiHead = 0;
volatile uint16_t spiTail = 0;
uint8_t spiRxByte;

void SPI_Handler(void *param)
{
    uint8_t jsonBuffer[SPI_RING_BUFFER_SIZE];
    uint8_t index = 0;
    bool collecting = false;

    while (1)
    {
        if(spiHead != spiTail)
        {
        	uint8_t byte = spiRingBuffer[spiTail];
        	spiTail = (spiTail + 1) % SPI_RING_BUFFER_SIZE;

        	if(byte == '{')
        	{
        		collecting = true;
        		index = 0;
        		jsonBuffer[index++] = byte;
        	}
        	else if(collecting)
        	{
        		if(index < SPI_RING_BUFFER_SIZE - 1)
        		{
        			jsonBuffer[index++] = byte;
        			if(byte == '}')
        			{
        				jsonBuffer[index] = '\0';
        				printf("JSON received: %s\n", jsonBuffer);
        				process_spi_json(jsonBuffer);
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
        }
        else
        {
        	vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}


void process_spi_json(uint8_t *jsonBuffer)
{
	uint8_t deviceState;
	GPIO_PinState state;
	int deviceIndex = -1;
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
				printf("TOUCH: Device %d set to %s\n", deviceIndex + 1, global_device_states[deviceIndex] ? "ON" : "OFF");
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
        	printf("SPI ring buffer overflow\n");
        }
        HAL_SPI_Receive_IT(&hspi2, &spiRxByte, 1);
    }
    __HAL_SPI_CLEAR_OVRFLAG(hspi);
}
