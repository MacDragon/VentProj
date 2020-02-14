/*
 * TempSensor.h
 *
 *  Created on: 27 Jan 2020
 *      Author: visa
 */

#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include "i2c_common_15xx.h"

#include "systick.h"

/* I2C clock is set to 1.8MHz */
#define I2C_CLK_DIVIDER         (40)
/* 100KHz I2C bit-rate */
#define I2C_BITRATE         (50000) // ~half way in operating range.
/* Standard I2C mode */
#define I2C_MODE    (0)

/** 7-bit I2C addresses of Temperature Sensor */
#define I2C_TEMP_ADDR_7BIT (0x48) // (0x48) // 1001 101b

#define TEMPREADREG ( 0x00 )
#define CONFIGREG ( 0x01 )
#define STANDBY ( 0x80 )
#define OPERATIONAL ( 0x00 )

#define WAKETIME	250

class TempSensor {
public:
	enum TempState { sStartup, sShutdown, sSleep, sError, sWaking, sOperational, eCriticalError, sBadTemp
		};
	TempSensor();
	virtual ~TempSensor();
	TempState Sleep();
	TempState Wake();
	TempState Ready();
	TempState Read( int &temperature); // returns temperature
	TempState ReadWithWakeBlocking( int &temperature );
private:
	uint32_t waketime;
	uint32_t lastcommand;
	TempState state;
	/* I2CM transfer record */
	I2CM_XFER_T  i2cmXferRec;
	void Init_I2C_PinMux();
	void setupI2CMaster();
    void SetupXferRecAndExecute(uint8_t devAddr,
									   uint8_t *txBuffPtr,
									   uint16_t txSize,
									   uint8_t *rxBuffPtr,
									   uint16_t rxSize);
};

#endif /* TEMPSENSOR_H_ */
