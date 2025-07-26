/*
 * gsm_app.c
 *
 *  Created on: Jul 26, 2025
 *      Author: kjeyabalan
 */
#include "user_app.h"
#include "MQTTSim800.h"
#include "gsm_app.h"

extern SIM800_t SIM800;

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
	uint8_t sub = 0;
	while(1)
	{
	   if (SIM800.mqttServer.connect == 0)
	   {
			   MQTT_Init();
			   sub = 0;
	   }
	   if (SIM800.mqttServer.connect == 1)
	   {
		   if(sub == 0){
			   MQTT_Sub("onwords/4ltc01/status");
			   MQTT_Sub("onwords/4ltc01/getCurrentStatus");
			   MQTT_Sub("onwords/4ltc01/currentStatus");
			   sub = 1;
		   }


		   if(SIM800.mqttReceive.newEvent)
		   {
			   unsigned char *topic = SIM800.mqttReceive.topic;
				unsigned char *payload = SIM800.mqttReceive.payload;
				SIM800.mqttReceive.newEvent = 0;
				safe_printf("Topic: %s\n\r", topic);
				safe_printf("Payload: %s\n\r",payload);
				MQTT_Pub("bala", payload);
		   }
		   if ((millis() - lastKeepAliveTime) >= (KEEP_ALIVE_INTERVAL * 500))
			{
			   uint8_t pingreq_packet[2] = {0xC0, 0x00};
			   HAL_UART_Transmit(UART_SIM800, pingreq_packet, 2, 100);
			   lastKeepAliveTime = millis();
			}
	   }
	   vTaskDelay(100);
	}
}

uint32_t millis() {
    return HAL_GetTick();
}
