/*
 * user_app.c
 *
 *  Created on: Jun 27, 2025
 *      Author: kjeyabalan
 */

#include "gsm_app.h"
#include "user_app.h"
#include "display_spi_app.h"
#include "onwords_logo.h"
#include "esp32_uart_app.h"
#include "MQTTSim800.h"
#include "rfm69_app.h"
#include "touch_spi_app.h"
#include "RTC_app.h"

// -------------------- Global Resources --------------------
SemaphoreHandle_t deviceStateMutex;
SemaphoreHandle_t uartMutex;
QueueHandle_t btnEventQueue;
BaseType_t status;

SIM800_t SIM800;

const char *devices[DEVICE_COUNT] = {"device1", "device2", "device3", "device4"};

const GPIO_TypeDef* led_ports[DEVICE_COUNT]   = {TOUCH_LED1_GPIO_Port, TOUCH_LED2_GPIO_Port, TOUCH_LED3_GPIO_Port, TOUCH_LED4_GPIO_Port};
const GPIO_TypeDef* relay_ports[DEVICE_COUNT] = {L_RELAY_1_GPIO_Port, L_RELAY_2_GPIO_Port, L_RELAY_3_GPIO_Port, L_RELAY_4_GPIO_Port};

const uint16_t led_pins[DEVICE_COUNT]   = {TOUCH_LED1_Pin, TOUCH_LED2_Pin, TOUCH_LED3_Pin, TOUCH_LED4_Pin};
const uint16_t relay_pins[DEVICE_COUNT] = {L_RELAY_1_Pin, L_RELAY_2_Pin, L_RELAY_3_Pin, L_RELAY_4_Pin};

volatile int global_device_states[DEVICE_COUNT] = {0, 0, 0, 0};
uint32_t lastKeepAliveTime = 0;

// ==========================================================
//                INIT MODULES (Modular Approach)
// ==========================================================

/**
 * @brief Initialize display and menu system
 */

static void init_st7735(void)
{
	ST7735_Init(0);
	ST7735_SetRotation(1);
	fillScreen(BLACK);
	showStartupLogoAndMenu();
}

static void init_display_app(void)
{
    initializeMenu();
}

/**
 * @brief Initialize synchronization primitives
 */
static void init_os_primitives(void)
{
    deviceStateMutex = xSemaphoreCreateMutex();
    configASSERT(deviceStateMutex != NULL);

    uartMutex = xSemaphoreCreateMutex();
    configASSERT(uartMutex != NULL);

    btnEventQueue = xQueueCreate(10, sizeof(ButtonEvent_t));
    configASSERT(btnEventQueue != NULL);
}

/**
 * @brief Initialize device states
 */
static void init_device_states(void)
{
    for (int i = 0; i < DEVICE_COUNT; i++) {
        global_device_states[i] = 0;
        device_states[i] = 0;
    }
}

/**
 * @brief Initialize communication peripherals
 */
static void init_interrupts(void)
{
    HAL_SPI_Receive_IT(&hspi2, &spiRxByte, 1);
    HAL_UART_Receive_IT(&huart4, &rx_data, 1);
    HAL_UART_Receive_IT(&huart3, &uartRxByte, 1);
}

/**
 * @brief Create FreeRTOS tasks
 */
static void init_tasks(void)
{
    status = xTaskCreate(SPI_Handler, "SPIHandler", 512, NULL, 2, NULL);
    configASSERT(status == pdPASS);

    status = xTaskCreate(RFM_Task, "RFM69Handler", 256, NULL, 2, NULL);
    configASSERT(status == pdPASS);

    status = xTaskCreate(UART_Handler, "UARTHandler", 1024, NULL, 2, NULL);
    configASSERT(status == pdPASS);

    status = xTaskCreate(Display_Handler, "DisplayHandler", 1024, NULL, 2, NULL);
	configASSERT(status == pdPASS);

	status = xTaskCreate(RTC_Task, "RTC_TaskHandler", 256, NULL, 2, NULL);
	configASSERT(status == pdPASS);

    // Optional GSM MQTT Task
    // status = xTaskCreate(GSM_MQTT_Task, "GSM_MQTT_Task", 1024, NULL, 7, NULL);
    // configASSERT(status == pdPASS);

}

/**
 * @brief Start FreeRTOS scheduler
 */
static void start_scheduler(void)
{
    vTaskStartScheduler();
}

// ==========================================================
//                PUBLIC FUNCTIONS
// ==========================================================

void user_app_init(void)
{
	printf("********* SmartTouchX ********\n");
	init_st7735();
	init_os_primitives();
	init_interrupts();
	init_device_states();
    init_display_app();
    RTC_init();
    GSM_init();
    init_tasks();
    start_scheduler();
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








