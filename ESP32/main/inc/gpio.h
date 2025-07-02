#ifndef GPIO_H
#define GPIO_H

#include "esp_app.h"
#include "mqtt.h"
#include "wifi.h"

#define STATUS_LED  4
#define RGB_LED     38
#define RESET_GPIO  19

void gpio_task_init(void);
void http_led_anim();

#endif