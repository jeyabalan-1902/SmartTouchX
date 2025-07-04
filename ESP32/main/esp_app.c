#include "esp_app.h"
#include "flash_cmd.h"
#include "wifi.h"
#include "http.h"
#include "mqtt.h"
#include "nvs.h"
#include "http_server.h"
#include "gpio.h"
#include "ble.h"


bool firmware_update_requested = false;
char firmware_url[256];
char ip[20];
char action[20]="  ";
int16_t pair_status;
uint8_t device;
uint16_t mqtt_count;
uint16_t connection_count;
int ota_status_led = 0;
char *product_id = NULL;
char *product_type = NULL;
volatile bool uart_exit = false;
int ble_enabled = 0;

extern TaskHandle_t uartTaskHandle;
extern bool ble_initialized; 
static const char *TAG = "ESP_APP";
static const char *TAG_CON = "RECONNECT";

static esp_err_t flash_stm32_firmware(void) {
    ESP_LOGI(TAG, "Starting STM32 firmware update process");
    
    if (download_firmware(firmware_url) != ESP_OK) {
        ESP_LOGE(TAG, "Firmware download failed");
        return ESP_FAIL;
    }
    
    esp_err_t result = flash_downloaded_firmware();
    
    if (download_buffer) {
        free(download_buffer);
        download_buffer = NULL;
    }  
    return result;
}

void start_ble_fallback(void) {
    if (!ble_enabled) {
        ESP_LOGI(TAG, "WiFi disconnected, enabling BLE advertising...");
        ble_init();
        ble_enabled = 1;
        ESP_LOGI(TAG, "Ble_enabled: %d", ble_enabled);
    }
}

void stop_ble_fallback(void) {
    if (ble_enabled) {
        ESP_LOGI(TAG, "WiFi connected, stopping BLE fallback...");
        ble_deinit();
        ble_enabled = 0;
        ESP_LOGI(TAG, "Ble_enabled: %d", ble_enabled);
    }
}

static void firmware_update_task(void *pvParameters) 
{
    while (1) {
        if (firmware_update_requested) { 
            firmware_update_requested = false;
            ESP_LOGI(TAG, "Processing firmware update request");

            uart_task_delete();
            
            uart_reinit();

            if (flash_stm32_firmware() == ESP_OK) {
                ESP_LOGI(TAG, "Firmware update completed successfully");
                ota_status_led = 0;
                send_mqtt_status(update_status, "Success", "STM32 firmware updated successfully");
                uart_exit = false;
                xTaskCreatePinnedToCore(uart_rx_task, "UART reception Task", 8192, NULL, 6, &uartTaskHandle, 1);
            } else {
                ESP_LOGE(TAG, "Firmware update failed");
                ota_status_led = 0;
                send_mqtt_status(update_status, "Failed", "STM32 firmware update failed");
                uart_exit = false;
                xTaskCreatePinnedToCore(uart_rx_task, "UART reception Task", 8192, NULL, 6, &uartTaskHandle, 1);
            }
        }       
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


static void connection_check(void *pvParameters)
{
    while(1)
    {
        if( wifi_state == 0 )
        {
            if (mqtt_status == 0)
            {
                mqtt_count++;
                //ESP_LOGI(TAG_CON,"Error wifi. wifi reconnects in %d",mqtt_count-30);
            }
            connection_count++;
            //ESP_LOGI(TAG_CON,"Error mqtt. mqtt reconnects in %d",connection_count-30);
        }
        else
        {
            connection_count=0;
        }
        if (mqtt_count >= 30 )
        {
            mqtt_count = 0;
            esp_mqtt_client_reconnect(mqtt_client);
        }
        if( connection_count >= 30 )
        {
            connection_count=0;
            esp_wifi_connect();
            esp_mqtt_client_reconnect(mqtt_client);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    esp_log_level_set("*", ESP_LOG_DEBUG); 
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(TAG, "STM32 Bootloader with MQTT starting...");

    init_nvs();
    gpio_task_init();
    get_pid();
    uart_init();
    ESP_LOGI(TAG, "UART initialized");

    xTaskCreatePinnedToCore(firmware_update_task, "firmware_update", 8192, NULL, 3, NULL, 1); 
    
    xTaskCreatePinnedToCore(uart_rx_task,"UART reception Task", 8192, NULL, 5, &uartTaskHandle, 1);
    
    if (is_wifi_credentials_available())
    {
        wifi_connection_init();
        pair_status = 1;
        if(wifi_state)
        {
            mqtt_app_start(product_id);
        }
        else 
        {
            start_ble_fallback();
        }      
        xTaskCreatePinnedToCore(connection_check, "connection_check", 4*1024, NULL , 6, NULL, 1);
    }
    else
    {
        ESP_LOGI(TAG, "WiFi credentials not found, starting SoftAP + HTTP Server...");
        pair_status = 0;
        http_led_anim();
        start_softap();
        start_http_server();
    } 
}
