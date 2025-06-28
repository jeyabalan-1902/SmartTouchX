#include "http_server.h"

static const char *TAG = "HTTP";

extern char *product_id;

esp_err_t wifi_handler(httpd_req_t *req)
{
    char content[100];
    int ret = httpd_req_recv(req, content, sizeof(content));
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    content[ret] = '\0';
    ESP_LOGI("HTTP", "Received JSON: %s", content);

    // Parse JSON and extract "ssid" and "password"
    cJSON *json = cJSON_Parse(content);
    if (json == NULL) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    const cJSON *ssid = cJSON_GetObjectItem(json, "ssid");
    const cJSON *password = cJSON_GetObjectItem(json, "password");

    if (ssid && password && cJSON_IsString(ssid) && cJSON_IsString(password)) {
        store_ssid_password(ssid->valuestring, password->valuestring);
        cJSON_Delete(json);
        httpd_resp_sendstr(req, "Credentials saved. Restarting...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
    } else {
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
    }

    return ESP_OK;
}


void start_http_server(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8182;

    httpd_handle_t server = NULL;
    httpd_start(&server, &config);

    httpd_uri_t wifi_uri = {
        .uri = "/",
        .method = HTTP_POST,
        .handler = wifi_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &wifi_uri);

    ESP_LOGI(TAG, "HTTP Server started at 192.168.4.1:8182");
}

void start_softap(){
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "",
            .ssid_len = 0,
            .channel = 1,
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        }
    };

    snprintf((char *)ap_config.ap.ssid, sizeof(ap_config.ap.ssid), "onwords-%s", product_id);

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "SoftAP started with SSID: %s", ap_config.ap.ssid);
}