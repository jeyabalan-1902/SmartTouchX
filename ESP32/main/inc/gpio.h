#ifndef GPIO_H
#define GPIO_H

#include "esp_app.h"
#include "mqtt.h"
#include "wifi.h"

#define STATUS_LED  4
#define RESET_GPIO  32

void gpio_task_init(void);
void http_led_anim();

#endif