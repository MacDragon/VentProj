/*
 * TempSensor.cpp
 *
 *  Created on: 27 Jan 2020
 *      Author: visa
 */

#include "TempSensor.h"


/* Initializes pin muxing for I2C interface - note that SystemInit() may
   already setup your pin muxing at system startup */
void TempSensor::Init_I2C_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_DIGMODE_EN | I2C_MODE);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_DIGMODE_EN | I2C_MODE);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);
}

/* Setup I2C handle and parameters */
void TempSensor::setupI2CMaster()
{
	/* Enable I2C clock and reset I2C peripheral - the boot ROM does not
	   do this */
	Chip_I2C_Init(LPC_I2C0);

	/* Setup clock rate for I2C */
	Chip_I2C_SetClockDiv(LPC_I2C0, I2C_CLK_DIVIDER);

	/* Setup I2CM transfer rate */
	Chip_I2CM_SetBusSpeed(LPC_I2C0, I2C_BITRATE);

	/* Enable Master Mode */
	Chip_I2CM_Enable(LPC_I2C0);
}

void TempSensor::SetupXferRecAndExecute(uint8_t devAddr,
								   uint8_t *txBuffPtr,
								   uint16_t txSize,
								   uint8_t *rxBuffPtr,
								   uint16_t rxSize)
{
	/* Setup I2C transfer record */
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;

	Chip_I2CM_XferBlocking(LPC_I2C0, &i2cmXferRec);

	::Sleep(1);

}

TempSensor::TempSensor() {
	/* Disable the interrupt for the I2C */
	NVIC_DisableIRQ(I2C0_IRQn);

	/* Setup I2C pin muxing */
	Init_I2C_PinMux();

	/* Allocate I2C handle, setup I2C rate, and initialize I2C
	   clocking */
	setupI2CMaster();

	state = Ready(); // ensure device is operational;
}



TempSensor::TempState TempSensor::Sleep() // doesn't matter if already awake, can still execute.
{
	uint8_t command[2] = { 0 };
	uint8_t status = 0;

	command[0] = CONFIGREG;
	command[1] = STANDBY;

	SetupXferRecAndExecute(I2C_TEMP_ADDR_7BIT,command, 2, &status, 1); // request the actual temperature

	if (i2cmXferRec.status == I2CM_STATUS_OK) { // check if we successfully received temp
	//	itm.print( "Sensor put into standby mode.\n" );
		state = sSleep;
	} else {
		state = sError;
	//	itm.print( "Error putting sensor into standby.\n" );
	}

	return state;
}


TempSensor::TempState TempSensor::Wake() // doesn't matter if already awake, can still execute.
{
	uint8_t command[2] = { 0 };
	uint8_t status = 0;

	if ( state == sWaking && timebase-waketime < WAKETIME ) return state; // device already waking, don't do anything.
	else {
		if ( state == sWaking )
		{
			return Ready(); // past wake counter, check and return current status instead.
		}
	}

	command[0] = CONFIGREG;
	command[1] = OPERATIONAL;

	SetupXferRecAndExecute(I2C_TEMP_ADDR_7BIT,command, 2, &status, 1); // send config command operational

	::Sleep(1);

	if (i2cmXferRec.status == I2CM_STATUS_OK) { // check if we successfully received temp
		state = sWaking;
		waketime = timebase; // set time wake command was sent, so as not to block.
		//itm.print( "Sensor Wake command sent\n" );
	} else {
		state = sError;
		//itm.print( "Error sending Wake command\n" );
	}

	return state;
}


TempSensor::TempState TempSensor::Read( int &temperature)
{
	uint8_t command[2] = { 0 };
	uint8_t receive;
	temperature = 0;

	if ( state != sOperational ) return state;

//	if ( state == sWaking && timebase-waketime < WAKETIME ) return state;

	command[0] = TEMPREADREG;
	SetupXferRecAndExecute(I2C_TEMP_ADDR_7BIT,command, 1, &receive, 1); // request the actual temperature

	if (i2cmXferRec.status == I2CM_STATUS_OK) { // check if we successfully received temp
		if ( receive > 127 ) temperature = receive-256; else temperature = receive;

		if ( temperature > -65 && temperature < 127 ) {
			state = sOperational;

		} else
		{
			temperature = 0;
			state = sBadTemp;
		}
	} else {
		state = sError;
	}
	return state;
}

TempSensor::TempState TempSensor::ReadWithWakeBlocking( int &temperature ){
	state = Ready();
	if ( state == sSleep ) {
		do {
			state = Wake();
			::Sleep ( 10 );
		} while  ( state == sWaking ); // wait for wake;
	}
	::Sleep(1);

	if ( state == sOperational ) state = Read(temperature);
	return state;
}

TempSensor::TempState TempSensor::Ready()
{
	uint8_t command[2] = { 0 };
	uint8_t receive;

	command[0] = CONFIGREG;
	SetupXferRecAndExecute(I2C_TEMP_ADDR_7BIT,command, 1, &receive, 1); // check device status

	if (i2cmXferRec.status == I2CM_STATUS_OK) { // status information was received.


		if ((receive & 0x40) != 0) { // check if bit 6 is set, status ready to read temp - will this ever be 1 if in standby?
			state = sOperational;
		}

		if ((receive & 0x80) != 0) {
			state = sSleep;
		}
	} else state = sError;
	return state;
}

TempSensor::~TempSensor() {
	// TODO Auto-generated destructor stub
}

