#ifndef NVS_H
#define NVS_H

#include "esp_app.h"

void get_pid();
void get_product_type();
bool is_wifi_credentials_available();
void get_password(char* password);
void get_ssid(char* ssid);
void store_ssid_password(const char *ssid, const char *password);
void nvs_erase();
void init_nvs();
void store_product_id(const char* new_pid);
void store_product_type(const char* product);

#endif