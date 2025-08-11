/*
 * rf_transmitter_app.h
 *
 *  Created on: Aug 11, 2025
 *      Author: kjeyabalan
 */

#ifndef INC_RF_TRANSMITTER_APP_H_
#define INC_RF_TRANSMITTER_APP_H_

#define RF69_FREQ   (float)433.0

typedef enum
{
	STATE_INIT = 0,
	SET_FREQ,
	SET_POW,
	SET_ENCKEY,
	TX_DATA,
	RX_DATA,
	FAIL_STATE,
	SYS_RESTART,
	SYS_REST,
	SYS_SLEEP
}Rf69_t;

void UserFsm(void);
bool RF69_TxData(void);
bool RF69_RxData(void);
bool FailIndiCation(Rf69_t FailState);
void  RF69_ModuleHandler(void);


#endif /* INC_RF_TRANSMITTER_APP_H_ */
