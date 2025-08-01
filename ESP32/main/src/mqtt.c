#include "mqtt.h"
#include "update_esp.h"

static const char *TAG = "MQTT_HANDLER";

esp_mqtt_client_handle_t mqtt_client;
int mqtt_status = 0;

char current_status[50];
char get_current_status[50];
char soft_Reset[50];
char get_status[50];
char hard_Reset[50];
char firmware_esp[50];
char firmware_stm[50];
char update_status[50];
char pid[50];
char credentials[50];
time_t now;


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_SERVER_CONNECTED");
        mqtt_status = 1;
        stop_ble_fallback();
        //esp_mqtt_client_subscribe(client, current_status, 1);
        esp_mqtt_client_subscribe(client, get_current_status, 1);
        esp_mqtt_client_subscribe(client, get_status, 1);
        esp_mqtt_client_subscribe(client, soft_Reset, 1);
        esp_mqtt_client_subscribe(client, firmware_stm, 1);
        esp_mqtt_client_subscribe(client, firmware_esp, 1);
        send_mqtt_status(current_status, "Connected", "Device connected with Mqtt Broker");
        break;
    case MQTT_EVENT_DISCONNECTED:
        mqtt_status = 0;
        start_ble_fallback(); 
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);
        
        if(strncmp(firmware_stm, event->topic, event->topic_len) == 0 && strlen(firmware_stm) == event->topic_len)
        {
            char *json_string = malloc(event->data_len + 1);
            if (json_string) {
                memcpy(json_string, event->data, event->data_len);
                json_string[event->data_len] = '\0';
                
                cJSON *json = cJSON_Parse(json_string);
                if (json) {
                    cJSON *firmware_sts = cJSON_GetObjectItem(json, "firmware_sts");
                    cJSON *firmwareUrl = cJSON_GetObjectItem(json, "firmwareUrl");
                    
                    if (cJSON_IsNumber(firmware_sts) && cJSON_IsString(firmwareUrl)) {
                        if (firmware_sts->valueint == 1) {
                            ESP_LOGI(TAG, "Firmware update requested: %s", firmwareUrl->valuestring);
                            strncpy(firmware_url, firmwareUrl->valuestring, sizeof(firmware_url) - 1);
                            firmware_update_requested = true;
                            ESP_LOGI(TAG, "firmware update request : %d", firmware_update_requested);
                        }
                    }
                    cJSON_Delete(json);
                }
                free(json_string);
            }
        }
        else if (strncmp(firmware_esp, event->topic, event->topic_len) == 0 && strlen(firmware_esp) == event->topic_len)
        {
            cJSON *json = cJSON_Parse(event->data);
            if (cJSON_HasObjectItem(json,"firmwareUrl") && cJSON_HasObjectItem(json,"firmware_sts"))
            {
                ESP_LOGI(TAG, "Topic match success!");
                cJSON *ota_url_json = cJSON_GetObjectItem(json, "firmwareUrl");
                cJSON *ota_sts_json = cJSON_GetObjectItem(json, "firmware_sts");
                char *ota_url = ota_url_json -> valuestring ;
                int ota_sts = ota_sts_json -> valueint ;
                if (ota_sts == 1)
                {
                    ota_init(ota_url);
                    ota_status_led = 1;
                }
            }
        }
        else if(strncmp(get_current_status, event->topic, event->topic_len) == 0)
        {
            char *json_string = malloc(event->data_len + 1);
            if(json_string)
            {
                memcpy(json_string, event->data, event->data_len);
                json_string[event->data_len] = '\0';
                uart_write_bytes(UART_PORT_NUM, json_string, strlen(json_string));
                uart_write_bytes(UART_PORT_NUM, "\n", 1);
                memset(json_string, 0, sizeof(json_string));
            }
        }
        else if (strncmp(get_status, event->topic, event->topic_len) == 0)
        {
            char *json_string = malloc(event->data_len + 1);
            if (json_string) {
                memcpy(json_string, event->data, event->data_len);
                json_string[event->data_len] = '\0';

                cJSON *json = cJSON_Parse(json_string);
                if (json) {
                    const char *device_keys[] = {"device1", "device2", "device3", "device4"};
                    int device_count = 0;
                    for (int i = 0; i < 4; ++i) {
                        cJSON *device_item = cJSON_GetObjectItem(json, device_keys[i]);
                        if (cJSON_IsNumber(device_item)) {
                            device_count++;
                        }
                    }

                    cJSON *wifi_led = cJSON_GetObjectItem(json, "wifi_led");
                    if(cJSON_IsNumber(wifi_led))
                    {
                        cJSON *displayControl = cJSON_CreateObject();
                        cJSON_AddNumberToObject(displayControl, "backlit", wifi_led->valueint);
                        char *resp_str = cJSON_PrintUnformatted(displayControl);
                        if (resp_str) {
                            uart_write_bytes(UART_PORT_NUM, resp_str, strlen(resp_str));
                            uart_write_bytes(UART_PORT_NUM, "\n", 1);
                            free(resp_str);
                        }

                        cJSON_Delete(displayControl);
                        break;
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
                free(json_string);
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void send_mqtt_status(const char* topic, const char* status, const char* message) {
    if (!mqtt_status) return;
    
    cJSON *json = cJSON_CreateObject();
    cJSON *status_item = cJSON_CreateString(status);
    cJSON *message_item = cJSON_CreateString(message);
    
    cJSON_AddItemToObject(json, "status", status_item);
    cJSON_AddItemToObject(json, "message", message_item);
    
    char *json_string = cJSON_Print(json);
    esp_mqtt_client_publish(mqtt_client, topic, json_string, 0, 1, 0);
    
    free(json_string);
    cJSON_Delete(json);
}

void mqtt_topics(void)
{
    sprintf(current_status,"onwords/%s/currentStatus",product_id);
    sprintf(get_status,"onwords/%s/status",product_id);
    sprintf(get_current_status,"onwords/%s/getCurrentStatus",product_id);
    sprintf(soft_Reset,"onwords/%s/softReset",product_id);
    sprintf(hard_Reset,"onwords/hardReset");
    sprintf(firmware_esp,"onwords/%s/firmwareEsp",product_id);
    sprintf(firmware_stm,"onwords/%s/firmwareStm",product_id);
    sprintf(update_status,"onwords/%s/otaStatus",product_id);
    sprintf(pid,"onwords/%s/pid",product_id); 
    sprintf(credentials, "onwords/%s/credentials", product_id);
}

void mqtt_app_start(char *product_id) {
    mqtt_topics();
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_URL,
        .credentials.client_id = product_id,
    };
    mqtt_cfg.session.keepalive = 30;
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void soft_reset()
{
    char *rest_response= malloc(50);
    ESP_LOGI(TAG," soft-reset ");
    sprintf(rest_response, "{\"status\": %s}","true"); 
    ESP_LOGI(TAG,"soft resets %s",rest_response);
    int msg_id = esp_mqtt_client_publish(mqtt_client, soft_Reset, rest_response , 0, 1, 0);
    ESP_LOGI(TAG, "soft reset publish successful, msg_id=%d", msg_id);
    free(rest_response);
}

void hard_reset()
{
    char *rest_response= malloc(50);
    ESP_LOGI(TAG," hard-reset ");
    sprintf(rest_response, "{\"id\":\"%s\"}", product_id);
    ESP_LOGI(TAG,"hard reset %s",rest_response);
    int msg_id = esp_mqtt_client_publish(mqtt_client, hard_Reset, rest_response , 0, 1, 0);
    ESP_LOGI(TAG, "hard reset publish successful, msg_id=%d", msg_id);
    free(rest_response);
}