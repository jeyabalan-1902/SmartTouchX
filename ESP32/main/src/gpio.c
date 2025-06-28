#include "gpio.h"

static const char *TAG_IO = "GPIO";

TaskHandle_t gpio_task_Handle = NULL;

int count_reset;


void http_led_anim()
{
    gpio_set_level(STATUS_LED, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(STATUS_LED, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(STATUS_LED, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(STATUS_LED, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(STATUS_LED, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(STATUS_LED, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void wifi_disconnect_led_anim()
{
    gpio_set_level(STATUS_LED, 1);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    gpio_set_level(STATUS_LED, 0);
    vTaskDelay(200 / portTICK_PERIOD_MS);
}

void mqtt_disconnect_led_anim()
{
    gpio_set_level(STATUS_LED, 1);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(STATUS_LED, 0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void wifi_mqtt_paired_Led_anim()
{
    if(!ota_status_led)
    {
        gpio_set_level(STATUS_LED, 1);
    }
    else 
    {
        gpio_set_level(STATUS_LED, 1);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_set_level(STATUS_LED, 0);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void led_anim()
{
    if(pair_status)
    {
        if (wifi_state)
        {
            if(mqtt_status)
            {
                wifi_mqtt_paired_Led_anim();
            }
            else
            {
                mqtt_disconnect_led_anim();
            }
        }
        else
        {
            wifi_disconnect_led_anim();
        }
    }
}

void gpio_task(void *pvParameters)
{
    while(1)
    {
        led_anim();
        int reset_pin = gpio_get_level(RESET_GPIO);
        if (!reset_pin)
            ESP_LOGI(TAG_IO, "reset GPIO state %d and count_reset %d",reset_pin,count_reset);
        if (reset_pin == 0 )
        {
            count_reset++;
        }
        else
        {
            count_reset = 0;
        }
        if (count_reset >= 500)
        {
            hard_reset();
            vTaskDelay(500 / portTICK_PERIOD_MS);
            nvs_erase();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void gpio_task_init(void)
{
    ESP_LOGI(TAG_IO, "GPIO Initialization!");
    gpio_set_direction(RESET_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction(STATUS_LED, GPIO_MODE_OUTPUT);
    xTaskCreatePinnedToCore( gpio_task, "switchlogic", 4*1024, NULL , 3, &gpio_task_Handle, 1 );
}