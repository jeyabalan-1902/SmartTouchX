/*
 * gsm_app.c
 *
 *  Created on: Jul 26, 2025
 *      Author: kjeyabalan
 */
#include <APP/gsm_app.h>
#include <APP/user_app.h>
#include <MQTTSim800.h>

extern SIM800_t SIM800;

uint8_t pingreq_sent = 0;
uint32_t pingreq_time = 0;
uint32_t pingresp_timeout_ms = 5000;

void GSM_init(void)
{
	SIM800.sim.apn = "internet";
	SIM800.sim.apn_user = "";
	SIM800.sim.apn_pass = "";
	SIM800.mqttServer.host = "mqtt.onwords.in";
	SIM800.mqttServer.port = 1883;
	SIM800.mqttClient.username = "Appteam";
	SIM800.mqttClient.pass = "Appteam@321";
	SIM800.mqttClient.clientID = "TestSub";
	SIM800.mqttClient.keepAliveInterval = 60;
}

void GSM_MQTT_Task(void *pvParameters)
{
    print_task_info();
    static uint8_t sub_done = 0;

    while (1)
    {
    	SIM800_MQTT_StateMachineHandler();
        if (mqttState == MQTT_STATE_MQTT_CONNECTED || mqttState == MQTT_STATE_IDLE)
        {
            if (sub_done == 0)
            {
                MQTT_Sub("onwords/4ltc01/status");
                MQTT_Sub("onwords/4ltc01/getCurrentStatus");
                MQTT_Sub("onwords/4ltc01/currentStatus");
                sub_done = 1;
                safe_printf("MQTT Subscribed to topics\r\n");
            }
            if (SIM800.mqttReceive.newEvent)
            {
                SIM800.mqttReceive.newEvent = 0;
                unsigned char *topic = SIM800.mqttReceive.topic;
                unsigned char *payload = SIM800.mqttReceive.payload;

                safe_printf("Topic: %s\r\n", topic);
                safe_printf("Payload: %s\r\n", payload);

                MQTT_Pub("bala", (char *)payload);
            }
            if (!pingreq_sent && (millis() - lastKeepAliveTime) >= (KEEP_ALIVE_INTERVAL * 500))
            {
                uint8_t pingreq_packet[2] = {0xC0, 0x00};
                HAL_UART_Transmit(UART_SIM800, pingreq_packet, 2, 100);
                lastKeepAliveTime = millis();
                pingreq_sent = 1;
                pingreq_time = millis();
                safe_printf("MQTT PINGREQ sent\r\n");
            }
            if (pingreq_sent && (millis() - pingreq_time) > pingresp_timeout_ms)
            {
                safe_printf("MQTT PINGRESP timeout! Forcing reconnect...\r\n");
                mqttState = MQTT_STATE_RECONNECT;
                SIM800.mqttServer.connect = 0;
                pingreq_sent = 0;
            }
        }
        else
        {
            sub_done = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

uint32_t millis() {
    return HAL_GetTick();
}
