/*
 * user_app.c
 *
 *  Created on: Jun 27, 2025
 *      Author: kjeyabalan
 */

#include <APP/display_ctrl.h>
#include <APP/gsm_app.h>
#include <APP/spi_rec.h>
#include <APP/uart_rec.h>
#include <APP/user_app.h>
#include <MQTTSim800.h>
#include <rfm69_app.h>
#include <onwords_logo.h>


SemaphoreHandle_t deviceStateMutex;
BaseType_t status;
SemaphoreHandle_t uartMutex;

SIM800_t SIM800;
const char *devices[DEVICE_COUNT] = {"device1", "device2", "device3", "device4"};
GPIO_TypeDef* led_ports[DEVICE_COUNT] = {TOUCH_LED1_GPIO_Port, TOUCH_LED2_GPIO_Port, TOUCH_LED3_GPIO_Port, TOUCH_LED4_GPIO_Port};
GPIO_TypeDef* relay_ports[DEVICE_COUNT] = {L_RELAY_1_GPIO_Port, L_RELAY_2_GPIO_Port, L_RELAY_3_GPIO_Port, L_RELAY_4_GPIO_Port};
uint16_t led_pins[DEVICE_COUNT] = {TOUCH_LED1_Pin, TOUCH_LED2_Pin, TOUCH_LED3_Pin, TOUCH_LED4_Pin};
uint16_t relay_pins[DEVICE_COUNT] = {L_RELAY_1_Pin, L_RELAY_2_Pin, L_RELAY_3_Pin, L_RELAY_4_Pin};
volatile int global_device_states[4] = {0, 0, 0, 0};
uint32_t lastKeepAliveTime = 0;


void setup_freeRTOS(void)
{
	HAL_SPI_Receive_IT(&hspi2, &spiRxByte, 1);
	HAL_UART_Receive_IT(&huart4, &rx_data, 1);
	HAL_UART_Receive_IT(&huart3, &uartRxByte, 1);

	status = xTaskCreate(SPI_Handler, "SPIHandler", 512, NULL, 5, NULL);
	configASSERT(status == pdPASS);

	status = xTaskCreate(RFM_Task, "RFM69Handler", 256, NULL, 6, NULL);
	configASSERT(status == pdPASS);

	status = xTaskCreate(UART_Handler, "UARTHandler", 1024, NULL, 4, NULL);
	configASSERT(status == pdPASS);

	status = xTaskCreate(GSM_MQTT_Task, "GSM_MQTT_Task", 1024, NULL, 7, NULL);
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
	uartMutex = xSemaphoreCreateMutex();
	initializeMenu();
	GSM_init();
	setup_freeRTOS();
}


void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    safe_printf("Stack overflow in task: %s\n", pcTaskName);
    while(1);
}

void print_task_info(void)
{
    char taskList[512];
    safe_printf("Task Info:\nName               State  Prio   StackNum     ID\n");
    vTaskList(taskList);
    safe_printf("%s\n", taskList);
}


void safe_printf(const char *fmt, ...)
{
    if (uartMutex != NULL && xSemaphoreTake(uartMutex, pdMS_TO_TICKS(200)) == pdTRUE)
    {
        char buffer[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
        xSemaphoreGive(uartMutex);
    }
}







