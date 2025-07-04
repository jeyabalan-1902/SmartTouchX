#ifndef ESP_APP_H
#define ESP_APP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_http_server.h"
#include "esp_mac.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include <sys/socket.h>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

extern bool firmware_update_requested;
extern char firmware_url[256];
extern char ip[20];
extern char action[20];
extern int16_t pair_status;
extern uint8_t device;
extern uint16_t mqtt_count;
extern uint16_t connection_count;
extern int ota_status_led;
extern char *product_id;
extern char *product_type;

extern void start_ble_fallback(void);
extern void stop_ble_fallback(void);


#endif