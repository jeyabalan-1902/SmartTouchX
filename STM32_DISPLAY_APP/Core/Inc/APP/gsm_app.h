/*
 * gsm_app.h
 *
 *  Created on: Jul 26, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_APP_GSM_APP_H_
#define INC_APP_GSM_APP_H_

#include <stdint.h>

uint32_t millis();
void GSM_init(void);
void GSM_MQTT_Task(void *pvParameters);

#endif /* INC_APP_GSM_APP_H_ */
