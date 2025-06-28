
#include "nvs.h"

static const char *TAG = "NVS";

extern char *product_id;
extern char *product_type;

void init_nvs()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void get_pid()
{
    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open("storage", NVS_READONLY, &nvs_handle);
    size_t required_size = 0;
    ret = nvs_get_str(nvs_handle, "product_id", NULL, &required_size);
    if (ret == ESP_OK)
    {
        product_id = malloc(required_size);
        ret = nvs_get_str(nvs_handle, "product_id", product_id, &required_size);
        if (ret != ESP_OK)
        {
            free(product_id);
            product_id = strdup("0000");
            ESP_LOGW(TAG, "Failed to get PID, default used");
        }
    }
    else
    {
        product_id = strdup("0000");
        ESP_LOGW(TAG, "PID not found, default used");
    }
    nvs_close(nvs_handle);
}

void get_product_type()
{
    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open("storage", NVS_READONLY, &nvs_handle);
    size_t required_size = 0;
    ret = nvs_get_str(nvs_handle, "product_type", NULL, &required_size);
    if (ret == ESP_OK)
    {
        product_type = malloc(required_size);
        ret = nvs_get_str(nvs_handle, "product_type", product_type, &required_size);
        if (ret != ESP_OK)
        {
            free(product_type);
            product_type = strdup("NULL");
            ESP_LOGW(TAG, "Failed to get Product type");
        }
    }
    else
    {
        product_type = strdup("NULL");
        ESP_LOGW(TAG, "Product type not found, default used");
    }
    nvs_close(nvs_handle);
}

bool is_wifi_credentials_available()
{
    nvs_handle_t handle;
    char ssid[70] = {0};
    char password[70] = {0};
    size_t ssid_len = sizeof(ssid);
    size_t pass_len = sizeof(password);

    if (nvs_open("storage", NVS_READONLY, &handle) == ESP_OK)
    {
        esp_err_t err1 = nvs_get_str(handle, "ssid", ssid, &ssid_len);
        esp_err_t err2 = nvs_get_str(handle, "password", password, &pass_len);
        ESP_LOGI(TAG, "WiFi credentials found. SSID: %s, connecting......", ssid);
        nvs_close(handle);
        return (err1 == ESP_OK && err2 == ESP_OK);
    }
    return false;
}

void get_ssid(char* ssid){
    nvs_handle data_handler;
    size_t length=70;
    nvs_open("storage", NVS_READONLY, &data_handler);
    nvs_get_str(data_handler,"ssid",ssid,&length);
    nvs_commit(data_handler);
    nvs_close(data_handler);
    ESP_LOGI(TAG, "From nvs ssid is %s ",ssid);
}

void get_password(char* password){
    nvs_handle data_handler;
    size_t length=70;
    nvs_open("storage", NVS_READONLY, &data_handler);
    nvs_get_str(data_handler,"password",password,&length);
    nvs_commit(data_handler);
    nvs_close(data_handler);
    ESP_LOGI(TAG, "From nvs password is %s",password);
}

void store_ssid_password(const char *ssid, const char *password)
{
    nvs_handle handle;
    nvs_open("storage", NVS_READWRITE, &handle);
    nvs_set_str(handle, "ssid", ssid);
    nvs_set_str(handle, "password", password);
    nvs_commit(handle);
    nvs_close(handle);
    ESP_LOGI(TAG, "Stored SSID: %s and Password.", ssid);
}

void store_product_id(const char* new_pid) {
    nvs_handle_t nvs_handle;
    esp_err_t ret;

    ret = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Failed to open NVS for writing");
        return;
    }

    ret = nvs_set_str(nvs_handle, "product_id", new_pid);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Failed to store PID");
    } else {
        ESP_LOGI("NVS", "Stored new PID: %s", new_pid);
    }

    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
}

void store_product_type(const char* product)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret;

    ret = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE("NVS", "failed to open nvs for writing" );
        return;
    }
    ret = nvs_set_str(nvs_handle, "product_type", product);
    if(ret != ESP_OK)
    {
        ESP_LOGE("NVS", "Failed to store Product type");
    }
    else
    {
        ESP_LOGI("NVS", "Stored Product Type: %s", product); 
    }
    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
}

void nvs_erase()
{
    nvs_handle handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Failed to open NVS: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_erase_key(handle, "ssid");
    if (err == ESP_OK) {
        ESP_LOGI("NVS", "SSID erased successfully.");
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW("NVS", "SSID not found.");
    } else {
        ESP_LOGE("NVS", "Failed to erase SSID: %s", esp_err_to_name(err));
    }

    err = nvs_erase_key(handle, "password");
    if (err == ESP_OK) {
        ESP_LOGI("NVS", "Password erased successfully.");
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW("NVS", "Password not found.");
    } else {
        ESP_LOGE("NVS", "Failed to erase password: %s", esp_err_to_name(err));
    }

    nvs_commit(handle);
    nvs_close(handle);

    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();
}
