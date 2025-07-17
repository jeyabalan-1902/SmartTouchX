#define F_CPU 8000000UL
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <atmel_start.h>
#include "touch.h"
#include "cJSON.h"

#define SPI_SS_PIN PB4 // SS pin connected to PB4
#define SPI_MOSI_PIN PB5 // MOSI pin connected to PB5
#define SPI_MISO_PIN PB6 // MISO pin connected to PB6
#define SPI_SCK_PIN PB7 // SCK pin connected to PB7

// Function declarations
void touchpin1();
void touchpin2();
void touchpin3();
void touchpin4();
void loop();
void SPI_MasterTransmitString(const char* str);
void SPI_MasterTransmit(uint8_t data);
void SPI_MasterInit();
void timer1_init() ;
uint8_t calculateCRC(uint8_t *data, size_t length);
void sendDeviceJson(const char* deviceName);

// Capacitive touch related variables
extern volatile uint8_t measurement_done_touch;
// Declare previous key status and touch counter variables for all touch pins
uint8_t key_status_1, previous_key_status_1, touch_counter_1;
uint8_t key_status_2, previous_key_status_2, touch_counter_2;
uint8_t key_status_3, previous_key_status_3, touch_counter_3;
uint8_t key_status_4, previous_key_status_4, touch_counter_4;
volatile uint16_t second_counter = 0;
volatile uint16_t second_counter_2 = 0;

int main(void)
{
	atmel_start_init();
	cpu_irq_enable();
	SPI_MasterInit();
	sei();
	while (1)
	{
		loop();
	}
	return 0;
}

void timer1_init() {
	TCCR1B |= (1 << WGM12);
	OCR1A = 65535; 
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << CS12) | (1 << CS10);
}

void SPI_MasterInit() {
	DDRB |= (1 << SPI_SS_PIN) | (1 << SPI_MOSI_PIN) | (1 << SPI_SCK_PIN);
	SPCR0 = (1 << SPE) | (1 << MSTR) | (1 << SPR0); 
	PORTB |= (1 << SPI_SS_PIN);
}

void SPI_MasterTransmit(uint8_t data) {
	PORTB &= ~(1 << SPI_SS_PIN);
	SPDR0 = data;
	while (!(SPSR0 & (1 << SPIF))) {
	}
	PORTB |= (1 << SPI_SS_PIN);
}

void sendDeviceJson(const char* deviceName) {
	cJSON *root = cJSON_CreateObject();
	if (root == NULL) return;

	cJSON_AddStringToObject(root, "device", deviceName);

	char *jsonStr = cJSON_PrintUnformatted(root);
	if (jsonStr != NULL) {
		SPI_MasterTransmitString(jsonStr);
		SPI_MasterTransmit('\n'); 
		cJSON_free(jsonStr);               
	}
	cJSON_Delete(root); 
}



void loop() {
	touch_process();
	if (measurement_done_touch == 1) {
		measurement_done_touch = 0;
		touchpin1();
		touchpin2();
		touchpin3();
		touchpin4();

		if (second_counter >= 200 && second_counter_2 >= 200) {
			sendDeviceJson("D1");
			second_counter = 0;
			second_counter_2 = 0;
			touch_counter_1 = 0;
			touch_counter_2 = 0;
		}
	}
}

void touchpin1() {
	previous_key_status_1 = key_status_1;
	key_status_1 = get_sensor_state(0) & 0x80;
	if (key_status_1 && !previous_key_status_1) {
		second_counter = 0;
		touch_counter_1 = 1;
		if (touch_counter_1 == 1) {
			sendDeviceJson("L1");
			second_counter = 0;
			touch_counter_1 = 0;
		}
	}
	else if (key_status_1) {
		second_counter++;
	}
}

void touchpin2() {
	previous_key_status_2 = key_status_2;
	key_status_2 = get_sensor_state(1) & 0x80;
	if (key_status_2 && !previous_key_status_2) {
		second_counter_2 = 0;
		touch_counter_2 = 1;
		if (touch_counter_2 == 1) {
			sendDeviceJson("L2");
			second_counter_2 = 0;
			touch_counter_2 = 0;
		}
	}
	else if (key_status_2) {
		second_counter_2++;
	}
}

void touchpin3() {
	previous_key_status_3 = key_status_3;
	key_status_3= get_sensor_state(2) & 0x80;
	if (key_status_3 && !previous_key_status_3) {
		touch_counter_3++;
		if (touch_counter_3 == 1) {
			sendDeviceJson("L3");
		}
		else if (touch_counter_3 == 2) {
			sendDeviceJson("L3");
			touch_counter_3 = 0;
		}
    }
}

void touchpin4() {
	previous_key_status_4 = key_status_4;
	key_status_4 = get_sensor_state(3) & 0x80;
    if (key_status_4 && !previous_key_status_4) {
	    touch_counter_4++;
	    if (touch_counter_4 == 1) {
		    sendDeviceJson("L4");
	    }
	    else if (touch_counter_4 == 2) {
		    sendDeviceJson("L4");
		    touch_counter_4 = 0;
	    }
    }
}

void SPI_MasterTransmitString(const char* str) {
	for (int i = 0; str[i] != '\0'; i++) {
		SPI_MasterTransmit(str[i]);
		_delay_ms(1);
	}
}
