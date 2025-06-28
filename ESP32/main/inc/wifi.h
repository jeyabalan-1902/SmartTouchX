#ifndef WIFI_H
#define WIFI_H

#include "esp_app.h"
#include "nvs.h"

#define EXAMPLE_ESP_STA_WIFI_SSID   "Rnd"
#define EXAMPLE_ESP_STA_WIFI_PASS   "1234567890"
#define EXAMPLE_ESP_MAXIMUM_RETRY  100

#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define EXAMPLE_H2E_IDENTIFIER ""
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK


extern char dhcp[50];
extern char ssid[70];
extern char password[70];
extern int rssi_val;
extern int wifi_state;

void wifi_connection_init(void);

#endif