#ifndef BLE_H
#define BLE_H

#include "esp_app.h"
#include "gpio.h"
#include "mqtt.h"
#include "nvs.h"
#include "wifi.h"
#include "uart_config.h"

#define GATTS_NUM_HANDLE_A          8
#define TEST_MANUFACTURER_DATA_LEN  17

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE        1024

// BLE Application Profile
#define PROFILE_NUM                 1
#define PROFILE_A_APP_ID            0
#define INVALID_HANDLE              0

// BLE Connection States
#define BLE_DISCONNECTED            0
#define BLE_CONNECTED               1

typedef struct {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle_status;
    uint16_t char_handle_command;
    uint16_t char_handle_response;
    uint16_t descr_handle;
    esp_bt_uuid_t char_uuid_status;
    esp_bt_uuid_t char_uuid_command;
    esp_bt_uuid_t char_uuid_response;
    esp_bt_uuid_t descr_uuid;
} gatts_profile_inst;

// Function declarations
void ble_init(void);
void ble_deinit(void);
int ble_status(void);
void ble_send_current_status(void);
void ble_process_command(char* data);
void ble_send_response(const char* response);

#endif