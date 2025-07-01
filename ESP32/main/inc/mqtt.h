#ifndef MQTT_H
#define MQTT_H

#include "esp_app.h"
#include "mqtt_config.h"

void mqtt_app_start(char *product_id);
void send_mqtt_status(const char* topic, const char* status, const char* message);
void mqtt_topics(void);
void soft_reset();
void hard_reset();

extern esp_mqtt_client_handle_t mqtt_client;
extern int mqtt_status;
extern char current_status[50];
extern char get_current_status[50];
extern char soft_Reset[50];
extern char get_status[50];
extern char hard_Reset[50];
extern char firmware_esp[50];
extern char firmware_stm[50];
extern char update_status[50];
extern char pid[50];
extern char credentials[50];

#endif