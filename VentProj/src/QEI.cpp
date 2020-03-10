/*
 * QEI.cpp
 *
 *  Created on: 21.2.2016
 *      Author: krl
 *      Based on example provided by NXP Semiconductors. See copyright notice below.
 */


#include "QEI.h"
#include <stdlib.h>

/**
 * @brief Quadrature Encoder Interface register block structure
 */
typedef struct {/*!< QEI Structure          */
__O  uint32_t  CON;/*!< Control register       */
__I  uint32_t  STAT;/*!< Encoder status register */
__IO uint32_t  CONF;/*!< Configuration register */
__I  uint32_t  POS;/*!< Position register      */
__IO uint32_t  MAXPOS;/*!< Maximum position register */
__IO uint32_t  CMPOS0;/*!< position compare register 0 */
__IO uint32_t  CMPOS1;/*!< position compare register 1 */
__IO uint32_t  CMPOS2;/*!< position compare register 2 */
__I  uint32_t  INXCNT;/*!< Index count register   */
__IO uint32_t  INXCMP0;/*!< Index compare register 0 */
__IO uint32_t  LOAD;/*!< Velocity timer reload register */
__I  uint32_t  TIME;/*!< Velocity timer register */
__I  uint32_t  VEL;/*!< Velocity counter register */
__I  uint32_t  CAP;/*!< Velocity capture register */
__IO uint32_t  VELCOMP;/*!< Velocity compare register */
__IO uint32_t  FILTERPHA;/*!< Digital filter register on input phase A (QEI_A) */
__IO uint32_t  FILTERPHB;/*!< Digital filter register on input phase B (QEI_B) */
__IO uint32_t  FILTERINX;/*!< Digital filter register on input index (QEI_IDX) */
__IO uint32_t  WINDOW;/*!< Index acceptance window register */
__IO uint32_t  INXCMP1;/*!< Index compare register 1 */
__IO uint32_t  INXCMP2;/*!< Index compare register 2 */
__I  uint32_t  RESERVED0[0x3E1];//933 int gap;[/color]
__O  uint32_t  IEC;/*!< Interrupt enable clear register */
__O  uint32_t  IES;/*!< Interrupt enable set register */
__I  uint32_t  INTSTAT;/*!< Interrupt status register */
__I  uint32_t  IE;/*!< Interrupt enable register */
__O  uint32_t  CLR;/*!< Interrupt status clear register */
__O  uint32_t  SET;/*!< Interrupt status set register */
} LPC_QEI_T;

#define LPC_QEI ((LPC_QEI_T*) LPC_QEI_BASE)

QEI::QEI( LpcPinMap A, LpcPinMap B, uint16_t maxvel) : ignore(false), maxvel(maxvel), lastdir(1)
{

/*
	Use the SYSAHBCLKCTRL1 register (Table 51) to enable the clock to the QEI interface.
	Clear the peripheral reset for the entire comparator block using the PRESETCTRL1 register (Table 36).
	The QEI creates one interrupt connected to slot #44 in the NVIC.
	Use the switch matrix to assign the QEI inputs to pins. See Table 296.
*/

// 	Use the SYSAHBCLKCTRL1 register (Table 51) to enable the clock to the QEI interface.
		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_QEI);
//	Clear the peripheral reset for the entire comparator block using the PRESETCTRL1 register (Table 36).
		Chip_SYSCTL_PeriphReset(RESET_QEI0);

//	Use the switch matrix to assign the QEI inputs to pins. See Table 296.
		Chip_SWM_MovablePortPinAssign(SWM_QEI0_PHA_I,  A.port,  A.pin);
	  	Chip_SWM_MovablePortPinAssign(SWM_QEI0_PHB_I,  B.port,  B.pin);
//		Chip_SWM_MovablePortPinAssign(SWM_QEI0_IDX_I,  IDX.port,  IDX.pin); // we're not using index signal.

		Chip_GPIO_SetPinDIRInput(LPC_GPIO, A.port, A.pin);
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, B.port, B.pin);
//		Chip_GPIO_SetPinDIRInput(LPC_GPIO, IDX.port, IDX.pin); // not using index signal.

		Chip_IOCON_PinMuxSet(LPC_IOCON, A.port, A.pin, IOCON_DIGMODE_EN | IOCON_MODE_PULLUP);
		Chip_IOCON_PinMuxSet(LPC_IOCON, B.port, B.pin, IOCON_DIGMODE_EN | IOCON_MODE_PULLUP);
// 		Chip_IOCON_PinMuxSet(LPC_IOCON, PD4_Port, PD4_Pin, IOCON_DIGMODE_EN | IOCON_MODE_PULLUP);

		// max size of int16, so that can cast as a signed short int and have counter clockwise values show as negative
		LPC_QEI->MAXPOS = UINT16_MAX;

//		LPC_QEI->IE = 0b1;
		LPC_QEI->IES = 1 << 5;


		//		NVIC_ClearPendingIRQ(QEI_IRQn);

		//		NVIC_SetPriority((IRQn_Type) QEI_IRQn, 2);
//		NVIC_EnableIRQ((IRQn_Type) QEI_IRQn);

		/* we're using in 2x mode not 4x now, 4x accuracy only makes harder to read out knob. */
//		LPC_QEI->CONF |= (1<<2);

		// add some level of filtering to prevent mistriggers, low enough value that humanly can't move knob fast enough to misread.
		LPC_QEI->FILTERPHA = 100;
		LPC_QEI->FILTERPHB = 100;
//		LPC_QEI->FILTERINX=100; // Not using index
		LPC_QEI->LOAD = SystemCoreClock / 10; // use 100ms period for velocity filtering, shorter results in too changeable a value.
// reset velocity counter
		LPC_QEI->CON = 1 << 2;
}

int QEI::read() {
	 // get the current position counter, cast it to a signed value so that counter clockwise will read as negative.
	int16_t posread = (int16_t)LPC_QEI->POS;
	int posupd = 0;

	// get movement direction.
	uint8_t dir = LPC_QEI->STAT;

	// get current velocity reading, convert scale to be useful as multiplier and ensure it is reads at least one
	uint16_t vel = (LPC_QEI->CAP / 6) + 1;

	// ignore every other input as two inputs per notch on input.

	// behaviour may need tweaking, intended to stop spurious readings on slow precision knob turning for single unit adjustments.

	if ( abs(posread) > 0 && !ignore ){ //
		if ( vel > maxvel ) vel = maxvel; // cap maximum velocity to prevent unexpectedly large swings.

		if ( lastdir != dir ) vel = 1; // if switched directions, assume minimum velocity to eliminate spurious jumps on change.

		posupd = posread * vel; // calculate how much to move position by.

		if ( posupd == 0 ) posupd = 1 * (dir ? -1 : 1 ); // an input was detected, ensure at least one value movement regardless of calculation.
		ignore = true; // ignore next reading.
		lastdir = dir; // store current direction so change can be detected.
	} else if ( abs(posread) > 0 && ignore ){
		ignore = false;
	}

	// reset position to 0, so that next read will be relative to new position.
	LPC_QEI->CON = 1;
    LPC_QEI->CLR = 0xFFFF;

	return posupd; // return how much change occurred since last call
}

