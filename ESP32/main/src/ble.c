#include "ble.h"

static const char *TAG_BLE = "BLE_APP";

static uint8_t adv_config_done = 0;
static int ble_connection_state = BLE_DISCONNECTED;
static uint16_t ble_conn_id = 0;
static uint16_t ble_gatts_if = ESP_GATT_IF_NONE;
bool ble_initialized = false;  

// Add prepared write buffer
static char *prepare_write_buffer = NULL;
static uint16_t prepare_write_len = 0;

#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)

static uint8_t adv_service_uuid128[32] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
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
};

static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,
    },
};

// Characteristic definitions
static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read                = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write               = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

// Service UUID
static const uint16_t GATTS_SERVICE_UUID_A      = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_STATUS       = 0xFF01;  // Status characteristic
static const uint16_t GATTS_CHAR_UUID_COMMAND       = 0xFF02;  // Command characteristic
static const uint16_t GATTS_CHAR_UUID_RESPONSE       = 0xFF03;  // Response characteristic

static const uint8_t heart_measurement_ccc[2]      = {0x00, 0x00};
static const uint8_t char_value[4]                 = {0x11, 0x22, 0x33, 0x44};

// Full Database Description - Used to add attributes into the database
static const esp_gatts_attr_db_t gatt_db[GATTS_NUM_HANDLE_A] =
{
    // Service Declaration
    [0] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_A), (uint8_t *)&GATTS_SERVICE_UUID_A}},

    // Status Characteristic Declaration
    [1] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read_write_notify}},

    // Status Characteristic Value
    [2] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_STATUS, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

    // Status Client Characteristic Configuration Descriptor
    [3] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},

    // Command Characteristic Declaration
    [4] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_write}},

    // Command Characteristic Value (NO AUTO_RSP to handle writes manually)
    [5] =
    {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_COMMAND, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

    // Response Characteristic Declaration
    [6] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read}},

    // Response Characteristic Value
    [7] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_RESPONSE, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},
};

int ble_status(void) {
    return ble_connection_state;
}

void ble_send_current_status(void) {
    if (ble_connection_state == BLE_CONNECTED) {
        char status_json[200];
        extern char ssid[70];
        extern int rssi_val;
        
        // Get current WiFi info
        get_ssid(ssid);
        esp_wifi_sta_get_rssi(&rssi_val);
        
        snprintf(status_json, sizeof(status_json), 
                "{\"action\": \"%s\"}", action);
        
        ESP_LOGI(TAG_BLE, "Sending BLE status: %s", status_json);
        
        esp_err_t ret = esp_ble_gatts_set_attr_value(gl_profile_tab[PROFILE_A_APP_ID].char_handle_status, 
                                    strlen(status_json), (uint8_t *)status_json);
        
        if (ret == ESP_OK) {
            // Send notification if client has enabled notifications
            esp_ble_gatts_send_indicate(ble_gatts_if, ble_conn_id, 
                                       gl_profile_tab[PROFILE_A_APP_ID].char_handle_status, 
                                       strlen(status_json), (uint8_t *)status_json, false);
            ESP_LOGI(TAG_BLE, "BLE Status sent successfully");
        } else {
            ESP_LOGE(TAG_BLE, "Failed to send BLE status: %s", esp_err_to_name(ret));
        }
    } else {
        ESP_LOGD(TAG_BLE, "BLE not connected, skipping status update");
    }
}

void ble_process_command(char* data) {
    ESP_LOGI(TAG_BLE, "Processing BLE command: %s", data);
    cJSON *json = cJSON_Parse(data);
    if (json) {

        cJSON *getCurrentStatus = cJSON_GetObjectItem(json, "request");
        if(getCurrentStatus)
        {
            uart_write_bytes(UART_PORT_NUM, data, strlen(data));
            uart_write_bytes(UART_PORT_NUM, "\n", 1);
        }
        const char *device_keys[] = {"device1", "device2", "device3", "device4"};
        int device_count = 0;
        for (int i = 0; i < 4; ++i) {
            cJSON *device_item = cJSON_GetObjectItem(json, device_keys[i]);
            if (cJSON_IsNumber(device_item)) {
                device_count++;
            }
        }

        if (device_count == 1) {
            for (int i = 0; i < 4; ++i) {
                cJSON *device_item = cJSON_GetObjectItem(json, device_keys[i]);
                if (cJSON_IsNumber(device_item)) {
                    cJSON *single_json = cJSON_CreateObject();
                    cJSON_AddNumberToObject(single_json, device_keys[i], device_item->valueint);

                    char *resp_str = cJSON_PrintUnformatted(single_json);
                    if (resp_str) {
                        uart_write_bytes(UART_PORT_NUM, resp_str, strlen(resp_str));
                        uart_write_bytes(UART_PORT_NUM, "\n", 1);
                        free(resp_str);
                    }

                    cJSON_Delete(single_json);
                    break;
                }
            }
        } else if (device_count > 1) {
            cJSON *combined_json = cJSON_CreateObject();

            for (int i = 0; i < 4; ++i) {
                cJSON *device_item = cJSON_GetObjectItem(json, device_keys[i]);
                if (cJSON_IsNumber(device_item)) {
                    cJSON_AddNumberToObject(combined_json, device_keys[i], device_item->valueint);
                }
            }

            char *resp_str = cJSON_PrintUnformatted(combined_json);
            if (resp_str) {
                uart_write_bytes(UART_PORT_NUM, resp_str, strlen(resp_str));
                uart_write_bytes(UART_PORT_NUM, "\n", 1);
                free(resp_str);
            }

            cJSON_Delete(combined_json);
        }
        cJSON_Delete(json);
    } 
}

void ble_send_response(const char* response) {
    if (ble_connection_state == BLE_CONNECTED && response != NULL) {
        ESP_LOGI(TAG_BLE, "Updating response characteristic with: %s", response);
        
        esp_err_t ret = esp_ble_gatts_set_attr_value(gl_profile_tab[PROFILE_A_APP_ID].char_handle_response, 
                                    strlen(response), (uint8_t *)response);
        
        if (ret == ESP_OK) {
            ESP_LOGI(TAG_BLE, "BLE Response updated successfully");
        } else {
            ESP_LOGE(TAG_BLE, "Failed to update BLE response: %s", esp_err_to_name(ret));
        }
    } else {
        ESP_LOGW(TAG_BLE, "Cannot send BLE response - not connected or null response");
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~adv_config_flag);
            if (adv_config_done == 0) {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~scan_rsp_config_flag);
            if (adv_config_done == 0) {
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG_BLE, "Advertising start failed");
            } else {
                ESP_LOGI(TAG_BLE, "Advertising start successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG_BLE, "Advertising stop failed");
            } else {
                ESP_LOGI(TAG_BLE, "Stop adv successfully");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(TAG_BLE, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
            break;
        default:
            break;
    }
}

static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
        case ESP_GATTS_REG_EVT:
            ESP_LOGI(TAG_BLE, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
            gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
            gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
            gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
            gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_A;

            esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(product_id);
            if (set_dev_name_ret) {
                ESP_LOGE(TAG_BLE, "set device name failed, error code = %x", set_dev_name_ret);
            }

            esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
            if (ret) {
                ESP_LOGE(TAG_BLE, "config adv data failed, error code = %x", ret);
            }
            adv_config_done |= adv_config_flag;

            ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
            if (ret) {
                ESP_LOGE(TAG_BLE, "config scan response data failed, error code = %x", ret);
            }
            adv_config_done |= scan_rsp_config_flag;

            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, GATTS_NUM_HANDLE_A, 0);
            break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(TAG_BLE, "ESP_GATTS_READ_EVT");
            break;
        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(TAG_BLE, "WRITE_EVT: handle=%" PRIu16 ", conn_id=%" PRIu16 ", trans_id=%" PRIu32 ", len=%" PRIu16 ", is_prep=%d",
                    param->write.handle,
                    param->write.conn_id,
                    param->write.trans_id,
                    param->write.len,
                    param->write.is_prep);
            
            if (!param->write.is_prep) {
                // Regular write
                ESP_LOG_BUFFER_HEX(TAG_BLE, param->write.value, param->write.len);
                
                // Check if this is the command characteristic by comparing with stored handle
                bool is_command_char = false;
                bool is_status_ccc = false;
                
                // Command characteristic is at index 5 in our attribute table
                if (param->write.handle == gl_profile_tab[PROFILE_A_APP_ID].char_handle_command) {
                    is_command_char = true;
                }
                // Status characteristic CCC descriptor is at index 3
                else if (param->write.handle == (gl_profile_tab[PROFILE_A_APP_ID].char_handle_status + 1)) {
                    is_status_ccc = true;
                }
                
                if (is_command_char && param->write.len > 0) {
                    char *json_data = malloc(param->write.len + 1);
                    if (json_data) {
                        memcpy(json_data, param->write.value, param->write.len);
                        json_data[param->write.len] = '\0';
                        ESP_LOGI(TAG_BLE, "Processing JSON command: %s", json_data);
                        ble_process_command(json_data);
                        free(json_data);
                    }
                } else if (is_status_ccc) {
                    uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                    if (descr_value == 0x0001) {
                        ESP_LOGI(TAG_BLE, "Status notifications enabled");
                    } else if (descr_value == 0x0000) {
                        ESP_LOGI(TAG_BLE, "Status notifications disabled");
                    }
                } else {
                    ESP_LOGW(TAG_BLE, "Write to unknown handle: %d", param->write.handle);
                }
                
                // Send response for regular writes
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
                
            } else {
                // Prepared write - store data for later execution
                ESP_LOGI(TAG_BLE, "PREP_WRITE_EVT: handle=%d, offset=%d, len=%d", 
                         param->write.handle, param->write.offset, param->write.len);
                
                // Allocate or reallocate buffer
                if (prepare_write_buffer == NULL) {
                    prepare_write_buffer = malloc(PREPARE_BUF_MAX_SIZE);
                    prepare_write_len = 0;
                }
                
                if (prepare_write_buffer && (param->write.offset + param->write.len <= PREPARE_BUF_MAX_SIZE)) {
                    memcpy(prepare_write_buffer + param->write.offset, param->write.value, param->write.len);
                    if (param->write.offset + param->write.len > prepare_write_len) {
                        prepare_write_len = param->write.offset + param->write.len;
                    }
                    
                    // Send response for prepared write
                    esp_gatt_rsp_t rsp;
                    memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
                    rsp.attr_value.handle = param->write.handle;
                    rsp.attr_value.len = param->write.len;
                    rsp.attr_value.offset = param->write.offset;
                    memcpy(rsp.attr_value.value, param->write.value, param->write.len);
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, &rsp);
                } else {
                    ESP_LOGE(TAG_BLE, "Prepared write buffer overflow");
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_INVALID_ATTR_LEN, NULL);
                }
            }
            break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            ESP_LOGI(TAG_BLE, "ESP_GATTS_EXEC_WRITE_EVT: exec_write_flag=%d", param->exec_write.exec_write_flag);
            
            if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC) {
                // Execute the prepared write
                if (prepare_write_buffer && prepare_write_len > 0) {
                    prepare_write_buffer[prepare_write_len] = '\0';
                    ESP_LOGI(TAG_BLE, "Executing prepared write: %s", prepare_write_buffer);
                    ble_process_command(prepare_write_buffer);
                }
            }
            
            // Clean up prepared write buffer
            if (prepare_write_buffer) {
                free(prepare_write_buffer);
                prepare_write_buffer = NULL;
                prepare_write_len = 0;
            }
            
            esp_ble_gatts_send_response(gatts_if, param->exec_write.conn_id, param->exec_write.trans_id, ESP_GATT_OK, NULL);
            break;
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(TAG_BLE, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(TAG_BLE, "ESP_GATTS_CONF_EVT, status = %d", param->conf.status);
            break;
        case ESP_GATTS_START_EVT:
            ESP_LOGI(TAG_BLE, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(TAG_BLE, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            ESP_LOG_BUFFER_HEX(TAG_BLE, param->connect.remote_bda, 6);
            esp_ble_conn_update_params_t conn_params = {0};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            conn_params.latency = 0;
            conn_params.max_int = 0x20;
            conn_params.min_int = 0x10;
            conn_params.timeout = 400;
            
            ble_connection_state = BLE_CONNECTED;
            ble_conn_id = param->connect.conn_id;
            ble_gatts_if = gatts_if;
            
            esp_ble_gap_update_conn_params(&conn_params);
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG_BLE, "ESP_GATTS_DISCONNECT_EVT, reason = %d", param->disconnect.reason);
            ble_connection_state = BLE_DISCONNECTED;
            
            // Clean up prepared write buffer on disconnect
            if (prepare_write_buffer) {
                free(prepare_write_buffer);
                prepare_write_buffer = NULL;
                prepare_write_len = 0;
            }
            
            esp_ble_gap_start_advertising(&adv_params);
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            if (param->add_attr_tab.status != ESP_GATT_OK) {
                ESP_LOGE(TAG_BLE, "create attribute table failed, error code = %x", param->add_attr_tab.status);
            } else if (param->add_attr_tab.num_handle != GATTS_NUM_HANDLE_A) {
                ESP_LOGE(TAG_BLE, "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to GATTS_NUM_HANDLE_A(%d)", param->add_attr_tab.num_handle, GATTS_NUM_HANDLE_A);
            } else {
                ESP_LOGI(TAG_BLE, "create attribute table successfully, the number handle = %d", param->add_attr_tab.num_handle);
                
                // Store characteristic handles with proper indexing
                gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->add_attr_tab.handles[0];
                gl_profile_tab[PROFILE_A_APP_ID].char_handle_status = param->add_attr_tab.handles[2];   // Status characteristic value
                gl_profile_tab[PROFILE_A_APP_ID].char_handle_command = param->add_attr_tab.handles[5];  // Command characteristic value
                gl_profile_tab[PROFILE_A_APP_ID].char_handle_response = param->add_attr_tab.handles[7]; // Response characteristic value
                
                ESP_LOGI(TAG_BLE, "Service handle: %d", gl_profile_tab[PROFILE_A_APP_ID].service_handle);
                ESP_LOGI(TAG_BLE, "Status char handle: %d", gl_profile_tab[PROFILE_A_APP_ID].char_handle_status);
                ESP_LOGI(TAG_BLE, "Command char handle: %d", gl_profile_tab[PROFILE_A_APP_ID].char_handle_command);
                ESP_LOGI(TAG_BLE, "Response char handle: %d", gl_profile_tab[PROFILE_A_APP_ID].char_handle_response);
                
                esp_ble_gatts_start_service(gl_profile_tab[PROFILE_A_APP_ID].service_handle);
            }
            break;
        default:
            break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGI(TAG_BLE, "Reg app failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }
    }

    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

void ble_init(void) {
    if (ble_initialized) {
        ESP_LOGW(TAG_BLE, "BLE already initialized");
        return;
    }
    
    esp_err_t ret;
    if (!esp_bt_controller_get_status()) {
        ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG_BLE, "Release classic BT memory failed: %s", esp_err_to_name(ret));
        }

        esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
        ret = esp_bt_controller_init(&bt_cfg);
        if (ret) {
            ESP_LOGE(TAG_BLE, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
            return;
        }
    }

    // Check if BT controller is enabled
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_INITED) {
        ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
        if (ret) {
            ESP_LOGE(TAG_BLE, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
            return;
        }
    }

    // Check if Bluedroid is initialized
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
        ret = esp_bluedroid_init();
        if (ret) {
            ESP_LOGE(TAG_BLE, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
            return;
        }
    }

    // Check if Bluedroid is enabled
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_INITIALIZED) {
        ret = esp_bluedroid_enable();
        if (ret) {
            ESP_LOGE(TAG_BLE, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
            return;
        }
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(TAG_BLE, "gatts register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG_BLE, "gap register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (ret) {
        ESP_LOGE(TAG_BLE, "gatts app register error, error code = %x", ret);
        return;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret) {
        ESP_LOGE(TAG_BLE, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    ble_initialized = true;
    ESP_LOGI(TAG_BLE, "BLE initialized successfully");
}

void ble_deinit(void) {
    // Check if BLE is already deinitialized
    if (!ble_initialized) {
        ESP_LOGW(TAG_BLE, "BLE already deinitialized");
        return;
    }
    
    // Stop advertising first
    esp_ble_gap_stop_advertising();
    
    // Clean up prepared write buffer
    if (prepare_write_buffer) {
        free(prepare_write_buffer);
        prepare_write_buffer = NULL;
        prepare_write_len = 0;
    }
    
    // Disable and deinit Bluedroid only if enabled
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED) {
        esp_bluedroid_disable();
    }
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_INITIALIZED) {
        esp_bluedroid_deinit();
    }
    
    // Disable and deinit BT controller only if enabled
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) {
        esp_bt_controller_disable();
    }
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_INITED) {
        esp_bt_controller_deinit();
    }
    
    ble_connection_state = BLE_DISCONNECTED;
    ble_initialized = false;
    ESP_LOGI(TAG_BLE, "BLE deinitialized");
}