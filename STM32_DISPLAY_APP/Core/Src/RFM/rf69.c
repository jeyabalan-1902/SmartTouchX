/*
 * rf69.c
 *
 *  Created on: Jul 31, 2025
 *      Author: kjeyabalan
 */
#include "rf69.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "user_app.h"

#define RH_BROADCAST_ADDRESS  0xff
extern  SPI_HandleTypeDef hspi3;

// The radio OP mode to use when mode is RHModeIdle
static uint8_t _idleMode;

// The reported device type
static uint8_t _deviceType;

// The selected output power in dBm
static int8_t _power;

// The message length in _buf
static uint8_t _bufLen;

// Array of octets of teh last received message or the next to transmit message
static uint8_t _buf[RH_RF69_MAX_MESSAGE_LEN];

// True when there is a valid message in the Rx buffer
static bool _rxBufValid;

// Time in millis since the last preamble was received (and the last time the RSSI was measured)
static uint32_t	_lastPreambleTime;

// The current transport operating mode
static RHMode _mode = RHModeinitialising;

/// This node id
static uint8_t _thisAddress = RH_BROADCAST_ADDRESS;

/// Whether the transport is in promiscuous mode
static bool	_promiscuous;

/// TO header in the last received mesasge
static uint8_t _rxHeaderTo;

/// FROM header in the last received mesasge
static uint8_t _rxHeaderFrom;

/// ID header in the last received mesasge
static uint8_t _rxHeaderId;

/// FLAGS header in the last received mesasge
static uint8_t _rxHeaderFlags;

/// TO header to send in all messages
static uint8_t _txHeaderTo = RH_BROADCAST_ADDRESS;

/// FROM header to send in all messages
static uint8_t _txHeaderFrom = RH_BROADCAST_ADDRESS;

/// ID header to send in all messages
static uint8_t _txHeaderId = 0;

/// FLAGS header to send in all messages
static uint8_t _txHeaderFlags = 0;

/// The value of the last received RSSI value, in some transport specific units
static int16_t _lastRssi;

/// Count of the number of bad messages (eg bad checksum etc) received
//static uint16_t	_rxBad = 0;

/// Count of the number of successfully transmitted messaged
static uint16_t	_rxGood = 0;

/// Count of the number of bad messages (correct checksum etc) received
static uint16_t	_txGood = 0;

/// Channel activity detected
//static bool _cad;

/// Channel activity timeout in ms
//static unsigned int _cad_timeout = 0;

// These are indexed by the values of ModemConfigChoice
// Stored in flash (program) memory to save SRAM
// It is important to keep the modulation index for FSK between 0.5 and 10
// modulation index = 2 * Fdev / BR
// Note that I have not had much success with FSK with Fd > ~5
// You have to construct these by hand, using the data from the RF69 Datasheet :-(
// or use the SX1231 starter kit software (Ctl-Alt-N to use that without a connected radio)
#define CONFIG_FSK  (RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_FSK | RH_RF69_DATAMODUL_MODULATIONSHAPING_FSK_NONE)
#define CONFIG_GFSK (RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_FSK | RH_RF69_DATAMODUL_MODULATIONSHAPING_FSK_BT1_0)
#define CONFIG_OOK  (RH_RF69_DATAMODUL_DATAMODE_PACKET | RH_RF69_DATAMODUL_MODULATIONTYPE_OOK | RH_RF69_DATAMODUL_MODULATIONSHAPING_OOK_NONE)

// Choices for RH_RF69_REG_37_PACKETCONFIG1:
#define CONFIG_NOWHITE    (RH_RF69_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RH_RF69_PACKETCONFIG1_DCFREE_NONE | RH_RF69_PACKETCONFIG1_CRC_ON | RH_RF69_PACKETCONFIG1_ADDRESSFILTERING_NONE)
#define CONFIG_WHITE      (RH_RF69_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RH_RF69_PACKETCONFIG1_DCFREE_WHITENING | RH_RF69_PACKETCONFIG1_CRC_ON | RH_RF69_PACKETCONFIG1_ADDRESSFILTERING_NONE)
#define CONFIG_MANCHESTER (RH_RF69_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RH_RF69_PACKETCONFIG1_DCFREE_MANCHESTER | RH_RF69_PACKETCONFIG1_CRC_ON | RH_RF69_PACKETCONFIG1_ADDRESSFILTERING_NONE)

static const ModemConfig MODEM_CONFIG_TABLE[] =
{
	//	02,		   03,	 04,   05,	 06,   19,	 1a,  37
	// FSK, No Manchester, no shaping, whitening, CRC, no address filtering
	// AFC BW == RX BW == 2 x bit rate
	// Low modulation indexes of ~ 1 at slow speeds do not seem to work very well. Choose MI of 2.
	{ CONFIG_FSK,  0x3e, 0x80, 0x00, 0x52, 0xf4, 0xf4, CONFIG_WHITE}, // FSK_Rb2Fd5
	{ CONFIG_FSK,  0x34, 0x15, 0x00, 0x4f, 0xf4, 0xf4, CONFIG_WHITE}, // FSK_Rb2_4Fd4_8
	{ CONFIG_FSK,  0x1a, 0x0b, 0x00, 0x9d, 0xf4, 0xf4, CONFIG_WHITE}, // FSK_Rb4_8Fd9_6

	{ CONFIG_FSK,  0x0d, 0x05, 0x01, 0x3b, 0xf4, 0xf4, CONFIG_WHITE}, // FSK_Rb9_6Fd19_2
	{ CONFIG_FSK,  0x06, 0x83, 0x02, 0x75, 0xf3, 0xf3, CONFIG_WHITE}, // FSK_Rb19_2Fd38_4
	{ CONFIG_FSK,  0x03, 0x41, 0x04, 0xea, 0xf2, 0xf2, CONFIG_WHITE}, // FSK_Rb38_4Fd76_8

	{ CONFIG_FSK,  0x02, 0x2c, 0x07, 0xae, 0xe2, 0xe2, CONFIG_WHITE}, // FSK_Rb57_6Fd120
	{ CONFIG_FSK,  0x01, 0x00, 0x08, 0x00, 0xe1, 0xe1, CONFIG_WHITE}, // FSK_Rb125Fd125
	{ CONFIG_FSK,  0x00, 0x80, 0x10, 0x00, 0xe0, 0xe0, CONFIG_WHITE}, // FSK_Rb250Fd250
	{ CONFIG_FSK,  0x02, 0x40, 0x03, 0x33, 0x42, 0x42, CONFIG_WHITE}, // FSK_Rb55555Fd50

	//	02,		   03,	 04,   05,	 06,   19,	 1a,  37
	// GFSK (BT=1.0), No Manchester, whitening, CRC, no address filtering
	// AFC BW == RX BW == 2 x bit rate
	{ CONFIG_GFSK, 0x3e, 0x80, 0x00, 0x52, 0xf4, 0xf5, CONFIG_WHITE}, // GFSK_Rb2Fd5
	{ CONFIG_GFSK, 0x34, 0x15, 0x00, 0x4f, 0xf4, 0xf4, CONFIG_WHITE}, // GFSK_Rb2_4Fd4_8
	{ CONFIG_GFSK, 0x1a, 0x0b, 0x00, 0x9d, 0xf4, 0xf4, CONFIG_WHITE}, // GFSK_Rb4_8Fd9_6

	{ CONFIG_GFSK, 0x0d, 0x05, 0x01, 0x3b, 0xf4, 0xf4, CONFIG_WHITE}, // GFSK_Rb9_6Fd19_2
	{ CONFIG_GFSK, 0x06, 0x83, 0x02, 0x75, 0xf3, 0xf3, CONFIG_WHITE}, // GFSK_Rb19_2Fd38_4
	{ CONFIG_GFSK, 0x03, 0x41, 0x04, 0xea, 0xf2, 0xf2, CONFIG_WHITE}, // GFSK_Rb38_4Fd76_8

	{ CONFIG_GFSK, 0x02, 0x2c, 0x07, 0xae, 0xe2, 0xe2, CONFIG_WHITE}, // GFSK_Rb57_6Fd120
	{ CONFIG_GFSK, 0x01, 0x00, 0x08, 0x00, 0xe1, 0xe1, CONFIG_WHITE}, // GFSK_Rb125Fd125
	{ CONFIG_GFSK, 0x00, 0x80, 0x10, 0x00, 0xe0, 0xe0, CONFIG_WHITE}, // GFSK_Rb250Fd250
	{ CONFIG_GFSK, 0x02, 0x40, 0x03, 0x33, 0x42, 0x42, CONFIG_WHITE}, // GFSK_Rb55555Fd50

	//	02,		   03,	 04,   05,	 06,   19,	 1a,  37
	// OOK, No Manchester, no shaping, whitening, CRC, no address filtering
	// with the help of the SX1231 configuration program
	// AFC BW == RX BW
	// All OOK configs have the default:
	// Threshold Type: Peak
	// Peak Threshold Step: 0.5dB
	// Peak threshiold dec: ONce per chip
	// Fixed threshold: 6dB
	{ CONFIG_OOK,  0x7d, 0x00, 0x00, 0x10, 0x88, 0x88, CONFIG_WHITE}, // OOK_Rb1Bw1
	{ CONFIG_OOK,  0x68, 0x2b, 0x00, 0x10, 0xf1, 0xf1, CONFIG_WHITE}, // OOK_Rb1_2Bw75
	{ CONFIG_OOK,  0x34, 0x15, 0x00, 0x10, 0xf5, 0xf5, CONFIG_WHITE}, // OOK_Rb2_4Bw4_8
	{ CONFIG_OOK,  0x1a, 0x0b, 0x00, 0x10, 0xf4, 0xf4, CONFIG_WHITE}, // OOK_Rb4_8Bw9_6
	{ CONFIG_OOK,  0x0d, 0x05, 0x00, 0x10, 0xf3, 0xf3, CONFIG_WHITE}, // OOK_Rb9_6Bw19_2
	{ CONFIG_OOK,  0x06, 0x83, 0x00, 0x10, 0xf2, 0xf2, CONFIG_WHITE}, // OOK_Rb19_2Bw38_4
	{ CONFIG_OOK,  0x03, 0xe8, 0x00, 0x10, 0xe2, 0xe2, CONFIG_WHITE}, // OOK_Rb32Bw64

//	  { CONFIG_FSK,  0x68, 0x2b, 0x00, 0x52, 0x55, 0x55, CONFIG_WHITE}, // works: Rb1200 Fd 5000 bw10000, DCC 400
//	  { CONFIG_FSK,  0x0c, 0x80, 0x02, 0x8f, 0x52, 0x52, CONFIG_WHITE}, // works 10/40/80
//	  { CONFIG_FSK,  0x0c, 0x80, 0x02, 0x8f, 0x53, 0x53, CONFIG_WHITE}, // works 10/40/40

};

// Arduino compatible macros
#define LOW  0
#define HIGH 1


uint8_t spiRead(uint8_t reg)
{
	uint8_t val;
	uint8_t TxDt;

	TxDt = (reg & ~RH_RF69_SPI_WRITE_MASK);
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, &TxDt,sizeof(TxDt), 1000);                   // Send the address with the write mask off
	HAL_SPI_Receive(&hspi3, &val, sizeof(val), 1000);                     // The written value is ignored, reg value is read
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_SET);
	return val;
}

uint8_t spiWrite(uint8_t reg, uint8_t val)
{
	uint8_t TxDt;
	uint8_t status = 0;

	TxDt = (reg | RH_RF69_SPI_WRITE_MASK);
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi3, &TxDt, &status, sizeof(TxDt), 1000); // Send the address with the write mask on
	HAL_SPI_Transmit(&hspi3, &val, sizeof(val), 1000);                   // New value follows
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_SET);
	return status;
}

uint8_t spiBurstRead(uint8_t reg, uint8_t* dest, uint8_t len)
{
	uint8_t TxDt;
	uint8_t dummyData =0;
	uint8_t rxDt =0;
	uint8_t status = 0;
	TxDt = (reg & ~RH_RF69_SPI_WRITE_MASK);
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi3, &TxDt, &status, sizeof(TxDt), 1000); // Send the start address with the write mask off
	while (len--)
	{
		HAL_SPI_TransmitReceive(&hspi3, &dummyData, &rxDt, sizeof(rxDt), 1000);
		*dest++ = rxDt;
	}
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_SET);
	return status;
}

uint8_t spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len)
{
	uint8_t TxDt;
	uint8_t status = 0;
	uint8_t TxDt1;
	TxDt = (reg | RH_RF69_SPI_WRITE_MASK);
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi3, &TxDt, &status, sizeof(TxDt), 1000);  // Send the start address with the write mask on
	while (len--)
	{
		TxDt1 = *src++;
		HAL_SPI_Transmit(&hspi3, &TxDt1, sizeof(TxDt1), 1000);            // New value follows
	}
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_SET);
	return status;
}

void setIdleMode(uint8_t idleMode)
{
	_idleMode = idleMode;
}

bool RF69_init()
{

	HAL_GPIO_WritePin(RF_RST_GPIO_Port,RF_RST_Pin,GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(RF_RST_GPIO_Port,RF_RST_Pin,GPIO_PIN_RESET);
	HAL_Delay(100);

	_idleMode = RH_RF69_OPMODE_MODE_STDBY;

	// Get the device type and check it
	_deviceType = spiRead(RH_RF69_REG_10_VERSION);
	safe_printf("_deviceType=%x\n", _deviceType);

	if (_deviceType != 0x24)
		return false;

	setModeIdle();

	// Configure important RH_RF69 registers
	// Here we set up the standard packet format for use by the RH_RF69 library:
	// 4 bytes preamble
	// 2 SYNC words 2d, d4
	// 2 CRC CCITT octets computed on the header, length and data (this in the modem config data)
	// 0 to 60 bytes data
	// RSSI Threshold -114dBm
	// We dont use the RH_RF69s address filtering: instead we prepend our own headers to the beginning
	// of the RH_RF69 payload
	spiWrite(RH_RF69_REG_3C_FIFOTHRESH, RH_RF69_FIFOTHRESH_TXSTARTCONDITION_NOTEMPTY | 0x0f); // thresh 15 is default
	// RSSITHRESH is default
	//	  spiWrite(RH_RF69_REG_29_RSSITHRESH, 220); // -110 dbM
	// SYNCCONFIG is default. SyncSize is set later by setSyncWords()
	//	  spiWrite(RH_RF69_REG_2E_SYNCCONFIG, RH_RF69_SYNCCONFIG_SYNCON); // auto, tolerance 0
	// PAYLOADLENGTH is default
	//	  spiWrite(RH_RF69_REG_38_PAYLOADLENGTH, RH_RF69_FIFO_SIZE); // max size only for RX
	// PACKETCONFIG 2 is default
	spiWrite(RH_RF69_REG_6F_TESTDAGC, RH_RF69_TESTDAGC_CONTINUOUSDAGC_IMPROVED_LOWBETAOFF);
	// If high power boost set previously, disable it
	spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
	spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);

	// The following can be changed later by the user if necessary.
	// Set up default configuration
	uint8_t syncwords[] = { 0x2d, 0xd4 };

	setSyncWords(syncwords, sizeof(syncwords)); // Same as RF22's
	// Reasonably fast and reliable default speed and modulation
	setModemConfig(GFSK_Rb250Fd250);

	// 3 would be sufficient, but this is the same as RF22's
	setPreambleLength(4);
	// An innocuous ISM frequency, same as RF22's
	setFrequency(434.0);
	// No encryption
	setEncryptionKey(NULL);
	// +13dBm, same as power-on default
	setTxPower(13, RH_RF69_DEFAULT_HIGHPOWER);

	spiWrite(RH_RF69_REG_25_DIOMAPPING1, RH_RF69_DIOMAPPING1_DIO0MAPPING_00 );

	return true;
}

// Low level function reads the FIFO and checks the address
// Caution: since we put our headers in what the RH_RF69 considers to be the payload, if encryption is enabled
// we have to suffer the cost of decryption before we can determine whether the address is acceptable.
// Performance issue?
void readFifo()
{
	uint8_t Data;
	uint8_t payloadlen ;

	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, 0);
	Data = RH_RF69_REG_00_FIFO;
	HAL_SPI_Transmit(&hspi3, &Data, sizeof(Data), 100) ;			// Send the start address with the write mask off
	HAL_SPI_Receive(&hspi3, &payloadlen, sizeof(payloadlen), 100);	// First byte is payload len (counting the headers)
	safe_printf("PayloadLen:%d\n",payloadlen);
	if (payloadlen <= RH_RF69_MAX_ENCRYPTABLE_PAYLOAD_LEN && payloadlen >= RH_RF69_HEADER_LEN)
	{
		HAL_SPI_Receive(&hspi3, &_rxHeaderTo, sizeof(_rxHeaderTo), 100);
		printf("_rxHeaderTo:%d\n",_rxHeaderTo);
		// Check addressing
		if (_promiscuous ||	_rxHeaderTo == _thisAddress ||_rxHeaderTo == RH_BROADCAST_ADDRESS)
		{
			// Get the rest of the headers
			HAL_SPI_Receive(&hspi3, &_rxHeaderFrom, sizeof(_rxHeaderFrom), 1000);
			safe_printf("_rxHeaderFrom:%d\n",_rxHeaderFrom);
			HAL_SPI_Receive(&hspi3, &_rxHeaderId, sizeof(_rxHeaderId), 1000);
			safe_printf("_rxHeaderId:%d\n",_rxHeaderId);
			HAL_SPI_Receive(&hspi3, &_rxHeaderFlags, sizeof(_rxHeaderFlags), 1000);
			safe_printf("_rxHeaderFlags:%d\n",_rxHeaderFlags);

			// And now the real payload
			for (_bufLen = 0; _bufLen < (payloadlen - RH_RF69_HEADER_LEN); _bufLen++)
			{
				HAL_SPI_Receive(&hspi3, &_buf[_bufLen], 1, 1000);
			}
				_rxGood++;
				_rxBufValid = true;
		}
	}
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, 1);
	// Any junk remaining in the FIFO will be cleared next time we go to receive mode.
}

int8_t temperatureRead()
{
	// Caution: must be ins standby.
	//	  setModeIdle();
	spiWrite(RH_RF69_REG_4E_TEMP1, RH_RF69_TEMP1_TEMPMEASSTART); // Start the measurement
	while (spiRead(RH_RF69_REG_4E_TEMP1) & RH_RF69_TEMP1_TEMPMEASRUNNING)
		;                                                        // Wait for the measurement to complete
	return 166 - spiRead(RH_RF69_REG_4F_TEMP2);                  // Very approximate, based on observation
}

bool setFrequency(float centre)
{
	uint32_t frf = (uint32_t)((centre * 1000000.0) / RH_RF69_FSTEP);
	spiWrite(RH_RF69_REG_07_FRFMSB, (frf >> 16) & 0xff);
	spiWrite(RH_RF69_REG_08_FRFMID, (frf >> 8) & 0xff);
	spiWrite(RH_RF69_REG_09_FRFLSB, frf & 0xff);

	return true;
}

int8_t rssiRead()
{
	return -((int8_t)(spiRead(RH_RF69_REG_24_RSSIVALUE) >> 1));
}

void setOpMode(uint8_t mode)
{
	uint8_t opmode = spiRead(RH_RF69_REG_01_OPMODE);
	opmode &= ~RH_RF69_OPMODE_MODE;
	opmode |= (mode & RH_RF69_OPMODE_MODE);
	spiWrite(RH_RF69_REG_01_OPMODE, opmode);
	safe_printf("setOpMode=%x\n", opmode);

	// Wait for mode to change.
	while (!(spiRead(RH_RF69_REG_27_IRQFLAGS1) & RH_RF69_IRQFLAGS1_MODEREADY))
		;

	// Verify new mode
	uint8_t _opmode = spiRead(RH_RF69_REG_01_OPMODE);
	if (opmode != _opmode)
	{
		safe_printf("setOpMode fail. %x %x\n", opmode, _opmode);
	}
}

void setModeIdle()
{
	if (_mode != RHModeIdle)
	{
		if (_power >= 18)
		{
			// If high power boost, return power amp to receive mode
			spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
			spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);
		}
		setOpMode(_idleMode);
		_mode = RHModeIdle;
	}
}

bool setSleep()
{
	if (_mode != RHModeSleep)
	{
		spiWrite(RH_RF69_REG_01_OPMODE, RH_RF69_OPMODE_MODE_SLEEP);
		_mode = RHModeSleep;
	}
	return true;
}

void setModeRx()
{
	if (_mode != RHModeRx)
	{
		if (_power >= 18)
		{
			spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
			spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);
		}
		spiWrite(RH_RF69_REG_25_DIOMAPPING1, RH_RF69_DIOMAPPING1_DIO0MAPPING_01);
		setOpMode(RH_RF69_OPMODE_MODE_RX);
		_mode = RHModeRx;
	}
}

void setModeTx()
{
	if (_mode != RHModeTx)
	{
		if (_power >= 18)
		{
			// Set high power boost mode
			// Note that OCP defaults to ON so no need to change that.
			spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_BOOST);
			spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_BOOST);
		}
		spiWrite(RH_RF69_REG_25_DIOMAPPING1, RH_RF69_DIOMAPPING1_DIO0MAPPING_00); // Set interrupt line 0 PacketSent
		setOpMode(RH_RF69_OPMODE_MODE_TX); // Clears FIFO
		_mode = RHModeTx;
	}
}

void setTxPower(int8_t power, bool ishighpowermodule)
{
  _power = power;
  uint8_t palevel;

  if (ishighpowermodule)
  {
	if (_power < -2)
	  _power = -2; //RFM69HW only works down to -2.
	if (_power <= 13)
	{
	  // -2dBm to +13dBm
	  //Need PA1 exclusivelly on RFM69HW
	  palevel = RH_RF69_PALEVEL_PA1ON | ((_power + 18) &
	  RH_RF69_PALEVEL_OUTPUTPOWER);
	}
	else if (_power >= 18)
	{
	  // +18dBm to +20dBm
	  // Need PA1+PA2
	  // Also need PA boost settings change when tx is turned on and off, see setModeTx()
	  palevel = RH_RF69_PALEVEL_PA1ON
	| RH_RF69_PALEVEL_PA2ON
	| ((_power + 11) & RH_RF69_PALEVEL_OUTPUTPOWER);
	}
	else
	{
	  // +14dBm to +17dBm
	  // Need PA1+PA2
	  palevel = RH_RF69_PALEVEL_PA1ON
	| RH_RF69_PALEVEL_PA2ON
	| ((_power + 14) & RH_RF69_PALEVEL_OUTPUTPOWER);
	}
  }
  else
  {
	if (_power < -18) _power = -18;
	if (_power > 13) _power = 13; //limit for RFM69W
	palevel = RH_RF69_PALEVEL_PA0ON
	  | ((_power + 18) & RH_RF69_PALEVEL_OUTPUTPOWER);
  }
  spiWrite(RH_RF69_REG_11_PALEVEL, palevel);
}

// Sets registers from a canned modem configuration structure
void setModemRegisters(const ModemConfig* config)
{
	spiBurstWrite(RH_RF69_REG_02_DATAMODUL,		&config->reg_02, 5);
	spiBurstWrite(RH_RF69_REG_19_RXBW,			&config->reg_19, 2);
	spiWrite(RH_RF69_REG_37_PACKETCONFIG1,		 config->reg_37);

}

// Set one of the canned FSK Modem configs
// Returns true if its a valid choice
bool setModemConfig(ModemConfigChoice index)
{
	if (index > (signed int)(sizeof(MODEM_CONFIG_TABLE) / sizeof(ModemConfig)))
		return false;

	ModemConfig cfg;
	memcpy(&cfg, &MODEM_CONFIG_TABLE[index], sizeof(ModemConfig));
	setModemRegisters(&cfg);

	return true;
}

void setPreambleLength(uint16_t bytes)
{
	spiWrite(RH_RF69_REG_2C_PREAMBLEMSB, bytes >> 8);
	spiWrite(RH_RF69_REG_2D_PREAMBLELSB, bytes & 0xff);
}

void setSyncWords(const uint8_t* syncWords, uint8_t len)
{
	uint8_t syncconfig = spiRead(RH_RF69_REG_2E_SYNCCONFIG);

	if (syncWords && len && len <= 4)
	{
		spiBurstWrite(RH_RF69_REG_2F_SYNCVALUE1, syncWords, len);
		syncconfig |= RH_RF69_SYNCCONFIG_SYNCON;
	}
	else
	{
		syncconfig &= ~RH_RF69_SYNCCONFIG_SYNCON;
	}
		syncconfig &= ~RH_RF69_SYNCCONFIG_SYNCSIZE;
		syncconfig |= (len-1) << 3;
		safe_printf("syncconfig=%x\n", syncconfig);
		spiWrite(RH_RF69_REG_2E_SYNCCONFIG, syncconfig);

}

void setEncryptionKey(uint8_t* key)
{
	if (key)
	{
		spiBurstWrite(RH_RF69_REG_3E_AESKEY1, key, 16);
		spiWrite(RH_RF69_REG_3D_PACKETCONFIG2, spiRead(RH_RF69_REG_3D_PACKETCONFIG2) | RH_RF69_PACKETCONFIG2_AESON);
	}
	else
	{
		spiWrite(RH_RF69_REG_3D_PACKETCONFIG2, spiRead(RH_RF69_REG_3D_PACKETCONFIG2) & ~RH_RF69_PACKETCONFIG2_AESON);
	}
}

bool available()
{
	// Get the interrupt cause
	uint8_t irqflags2 = spiRead(RH_RF69_REG_28_IRQFLAGS2);
//		printf("available irqflags2=%x\n", irqflags2);
	// Must look for PAYLOADREADY, not CRCOK, since only PAYLOADREADY occurs _after_ AES decryption
	// has been done
	if (irqflags2 & RH_RF69_IRQFLAGS2_PAYLOADREADY) {
		// A complete message has been received with good CRC
		_lastRssi = -((int8_t)(spiRead(RH_RF69_REG_24_RSSIVALUE) >> 1));
		_lastPreambleTime = HAL_GetTick();

		setModeIdle();
		// Save it in our buffer
		readFifo();
		safe_printf("PAYLOADREADY\n");
	}
	setModeRx(); // Make sure we are receiving
	return _rxBufValid;
}

// Blocks until a valid message is received or timeout expires
// Return true if there is a message available
// Works correctly even on millis() rollover
bool waitAvailableTimeout(uint16_t timeout)
{
	unsigned long starttime = HAL_GetTick();

	while(((HAL_GetTick()) - starttime) < timeout)
		{
			if (available())
			{
			   return true;
			}
		}
	return false;
}


bool recv1(uint8_t* buf, uint8_t* len)
{
	if (!available())
	return false;

	if (buf && len)
	{
	if (*len > _bufLen)
		*len = _bufLen;
	memcpy(buf, _buf, *len);
	}
	_rxBufValid = false; // Got the most recent message
//	  printBuffer("recv:", buf, *len);
	return true;
}

bool send1(const uint8_t* data, uint8_t len)
{
	uint8_t TxDt;

	if (len > RH_RF69_MAX_MESSAGE_LEN)
	return false;

	setModeIdle();                                     // Prevent RX while filling the fifo

	safe_printf( "_txHeaderTo    =%d\n", _txHeaderTo);
	safe_printf( "_txHeaderFrom  =%d\n", _txHeaderFrom);
	safe_printf( "_txHeaderId    =%d\n", _txHeaderId);
	safe_printf( "_txHeaderFlags =%d\n", _txHeaderFlags);

	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_RESET);
	TxDt = (RH_RF69_REG_00_FIFO | RH_RF69_SPI_WRITE_MASK);
	HAL_SPI_Transmit(&hspi3, &TxDt, sizeof(TxDt), 1000);
	TxDt = (len + RH_RF69_HEADER_LEN);                 // Include length of headers
	HAL_SPI_Transmit(&hspi3, &TxDt, sizeof(TxDt), 1000);

	HAL_SPI_Transmit(&hspi3, &_txHeaderTo, 	 sizeof(_txHeaderTo),   1000);
	HAL_SPI_Transmit(&hspi3, &_txHeaderFrom, sizeof(_txHeaderFrom), 1000);
	HAL_SPI_Transmit(&hspi3, &_txHeaderId, 	 sizeof(_txHeaderId),   1000);
	HAL_SPI_Transmit(&hspi3, &_txHeaderFlags,sizeof(_txHeaderFlags),1000);

	uint8_t SendData;
	while (len--)
	{
		SendData = *data++;
		HAL_SPI_Transmit(&hspi3, &SendData, sizeof(SendData), 1000);
	}
	HAL_GPIO_WritePin(RF_CS_GPIO_Port, RF_CS_Pin, GPIO_PIN_SET);

	setModeTx();                                       // Start the transmitter

	return true;
}

bool waitPacketSent()
{
	while (1)
	{
		    // Get the interrupt cause
		uint8_t irqflags2 = spiRead(RH_RF69_REG_28_IRQFLAGS2);
		    //printf("waitPacketSent irqflags2=%x\n", irqflags2);
		if (irqflags2 & RH_RF69_IRQFLAGS2_PACKETSENT)
		{
	       // A transmitter message has been fully sent
		   // Clears FIFO
			setModeIdle();
			_txGood++;
			safe_printf("PACKETSENT\n");
			break;
		}
	}
	return true;
}


uint8_t maxMessageLength()
{
	return RH_RF69_MAX_MESSAGE_LEN;
}

bool printRegister(uint8_t reg)
{
	safe_printf("%x %x\n", reg, spiRead(reg));
	return true;
}

bool printRegisters()
{
	uint8_t i;
	for (i = 0; i < 0x50; i++)
	printRegister(i);
	// Non-contiguous registers
	printRegister(RH_RF69_REG_58_TESTLNA);
	printRegister(RH_RF69_REG_6F_TESTDAGC);
	printRegister(RH_RF69_REG_71_TESTAFC);

	return true;
}

uint8_t headerTo()
{
	return _rxHeaderTo;
}

uint8_t headerFrom()
{
	return _rxHeaderFrom;
}

uint8_t headerId()
{
	return _rxHeaderId;
}

uint8_t headerFlags()
{
	return _rxHeaderFlags;
}

int16_t lastRssi()
{
	return _lastRssi;
}





