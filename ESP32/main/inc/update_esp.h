#ifndef UPDATE_ESP_H
#define UPDATE_ESP_H

#include "esp_app.h"
#include "mqtt.h"

void ota_init(char *ota_firmware_url);

#endif