/*
 * MQTTSim800.h
 *
 *  Created on: July 25, 2025
 *      Author: kjeyabalan
 *
 */


#include "main.h"
#include <string.h>
#include <stdio.h>


extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
// === CONFIG ===
#define UART_SIM800 &huart4
#define FREERTOS    1
#define CMD_DELAY   2000
// ==============

typedef struct {
    char *apn;
    char *apn_user;
    char *apn_pass;
} sim_t;

typedef struct {
    char *host;
    uint16_t port;
    uint8_t connect;
} mqttServer_t;

typedef struct {
    char *username;
    char *pass;
    char *clientID;
    unsigned short keepAliveInterval;
} mqttClient_t;

typedef struct {
    uint8_t newEvent;
    unsigned char dup;
    int qos;
    unsigned char retained;
    unsigned short msgId;
    unsigned char payload[256];
    int payloadLen;
    unsigned char topic[128];
    int topicLen;
} mqttReceive_t;

typedef struct {
    sim_t sim;
    mqttServer_t mqttServer;
    mqttClient_t mqttClient;
    mqttReceive_t mqttReceive;
} SIM800_t;


typedef enum{
	MQTT_STATE_INIT = 0,
	MQTT_STATE_CGATT_ATTACH,
	MQTT_STATE_APN_CONFIG,
	MQTT_STATE_CIICR_BRINGUP,
	MQTT_STATE_GET_IP,
	MQTT_STATE_TCP_CONNECT,
	MQTT_STATE_WAIT_TCP_RESPONSE,
	MQTT_STATE_MQTT_CONNECT,
	MQTT_STATE_MQTT_CONNECTED,
	MQTT_STATE_IDLE,
	MQTT_STATE_RECONNECT
}mqtt_state_t;

extern mqtt_state_t mqttState;
extern int mqttError;

extern uint8_t rx_data;

void SIM800_MQTT_StateMachineHandler(void);
void Sim800_RxCallBack(void);

void clearRxBuffer(void);

void clearMqttBuffer(void);

int SIM800_SendCommand(char *command, char *reply, uint16_t delay);

int MQTT_Init(void);

void MQTT_Connect(void);

void MQTT_Pub(char *topic, char *payload);

void MQTT_PubUint8(char *topic, uint8_t data);

void MQTT_PubUint16(char *topic, uint16_t data);

void MQTT_PubUint32(char *topic, uint32_t data);

void MQTT_PubFloat(char *topic, float payload);

void MQTT_PubDouble(char *topic, double data);

void MQTT_PingReq(void);

void MQTT_Sub(char *topic);

void MQTT_Receive(unsigned char *buf);
