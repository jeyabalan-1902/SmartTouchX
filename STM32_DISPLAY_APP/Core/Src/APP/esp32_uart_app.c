/*
 * uart_rec.c
 *
 *  Created on: Jul 17, 2025
 *      Author: kjeyabalan
 */

#include "user_app.h"
#include "display_spi_app.h"
#include "esp32_uart_app.h"
#include "MQTTSim800.h"

uint8_t uartRingBuffer[UART_RING_BUFFER_SIZE];
volatile uint16_t uartHead = 0;
volatile uint16_t uartTail = 0;
volatile uint8_t uartRxByte;

typedef enum
{
	UART_IDLE,
	UART_COLLECT_JSON,
	UART_PROCESS_JSON
}UartState_t;

static UartState_t uartState = UART_IDLE;

void UART_Handler(void *param)
{
    uint8_t jsonBuffer[JSON_BUFFER_SIZE];
    uint8_t index = 0;
    while (1)
    {
        if (uartHead != uartTail)
        {
            uint8_t byte = uartRingBuffer[uartTail];
            uartTail = (uartTail + 1) % UART_RING_BUFFER_SIZE;

            switch(uartState)
            {
				case UART_IDLE:
					if (byte == '{')
					{
						index = 0;
						jsonBuffer[index++] = byte;
						uartState = UART_COLLECT_JSON;
					}
					else if(byte == BOOT_CMD)
					{
						NVIC_SystemReset();
					}
					break;

				case UART_COLLECT_JSON:
					if (index < JSON_BUFFER_SIZE - 1)
					{
						jsonBuffer[index++] = byte;

						if (byte == '}')
						{
							jsonBuffer[index] = '\0';
						    uartState = UART_PROCESS_JSON;
						}
					}
					else
					{
						safe_printf("UART JSON buffer overflow, resetting...\n");
						index = 0;
						uartState = UART_IDLE;
					}
					break;

				case UART_PROCESS_JSON:
					process_json(jsonBuffer);
					index = 0;
					uartState = UART_IDLE;
					break;
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}


void send_json_response(cJSON *jsonObj)
{
    char *response = cJSON_PrintUnformatted(jsonObj);
    if (response)
    {
    	//safe_printf("Device State JSON: %s\n", response);
        HAL_UART_Transmit(&huart3, (uint8_t *)response, strlen(response), HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart3, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
        free(response);
    }
}

void handle_get_status_request(void)
{
    cJSON *resp = cJSON_CreateObject();
    if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        for (int i = 0; i < DEVICE_COUNT; i++)
        {
            cJSON_AddNumberToObject(resp, devices[i], global_device_states[i]);
        }
        xSemaphoreGive(deviceStateMutex);
    }
    send_json_response(resp);
    cJSON_Delete(resp);
}

void handle_device_control(cJSON *json)
{
    cJSON *resp = cJSON_CreateObject();
    cJSON *Display = cJSON_GetObjectItem(json, "backlit");

    if(Display)
	{
		if (cJSON_IsNumber(Display))
		{
			HAL_GPIO_TogglePin(DISP_BACKLIT_GPIO_Port, DISP_BACKLIT_Pin);
			GPIO_PinState state = HAL_GPIO_ReadPin(DISP_BACKLIT_GPIO_Port, DISP_BACKLIT_Pin);
		    safe_printf("ESP32: Display backlit is %s", (state == GPIO_PIN_SET) ? "ON": "OFF");
		}
	}


	if (xSemaphoreTake(deviceStateMutex, pdMS_TO_TICKS(100)) == pdTRUE)
	{
		for (int i = 0; i < DEVICE_COUNT; i++)
		{
			cJSON *Device = cJSON_GetObjectItem(json, devices[i]);
			if (cJSON_IsNumber(Device))
			{
				HAL_GPIO_WritePin(led_ports[i], led_pins[i], Device->valueint ? GPIO_PIN_SET : GPIO_PIN_RESET);
				HAL_GPIO_WritePin(relay_ports[i], relay_pins[i], Device->valueint ? GPIO_PIN_SET : GPIO_PIN_RESET);
				GPIO_PinState state = HAL_GPIO_ReadPin(led_ports[i], led_pins[i]);
				global_device_states[i] = (state == GPIO_PIN_SET) ? 1 : 0;
				cJSON_AddNumberToObject(resp, devices[i], global_device_states[i]);
				safe_printf("ESP32: Device %d set to %s\n", i + 1, global_device_states[i] ? "ON" : "OFF");
			}
		}
		xSemaphoreGive(deviceStateMutex);
	}
	send_json_response(resp);
	cJSON_Delete(resp);
}

void process_json(uint8_t *jsonBuffer)
{
    cJSON *json = cJSON_Parse((char *)jsonBuffer);
    if (!json)
    {
        safe_printf("Invalid JSON!\n");
        return;
    }

    cJSON *request = cJSON_GetObjectItem(json, "request");
    if (cJSON_IsString(request) && strcmp(request->valuestring, "getCurrentStatus") == 0)
    {
        handle_get_status_request();
    }
    else
    {
        handle_device_control(json);
    }
    cJSON_Delete(json);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        uint16_t nextHead = (uartHead + 1) % UART_RING_BUFFER_SIZE;
        if (nextHead != uartTail)
        {
            uartRingBuffer[uartHead] = uartRxByte;
            uartHead = nextHead;
        } else {
            safe_printf("UART ring buffer overflow!\n");
        }
        HAL_UART_Receive_IT(&huart3, &uartRxByte, 1);
    }

    else if(huart == UART_SIM800)
    {
    	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		Sim800_RxCallBack();
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
