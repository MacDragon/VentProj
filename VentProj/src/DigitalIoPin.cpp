/*
 * DigitalIoPin.cpp
 *
 *  Created on: 10 Mar 2020
 *      Author: Joshua
 */
#include "DigitalIoPin.h"

bool DigitalIoPin::interrupt_enabled { false };

DigitalIoPin::DigitalIoPin(int const port, int const pin, bool const input, bool const pullup, bool const invert) : port { port }, pin { pin }, input { input }, invert { invert } {
	LPC_IOCON->PIO[port][pin] = (1U + pullup) << 3 | 1U << 7 | invert << 6;
	LPC_GPIO->DIR[port] = input ? LPC_GPIO->DIR[port] & ~(1UL << pin) : LPC_GPIO->DIR[port] | 1UL << pin;
}

bool DigitalIoPin::read() const {
	return static_cast<bool>(LPC_GPIO->B[port][pin]);
}

void DigitalIoPin::write(bool const value) const {
	LPC_GPIO->B[port][pin] = invert ? !value : value;
}

void DigitalIoPin::toggle() const {
	LPC_GPIO->B[port][pin] = invert ? read() : !read();
}

void DigitalIoPin::enableInterrupt(IRQn_Type IRQn, Mode mode, Level level) {
	if (IRQn < IRQn_min || IRQn > IRQn_max || NVIC_GetActive(IRQn))
		return;

	if (!interrupt_enabled) {
		Chip_PININT_Init(LPC_GPIO_PIN_INT);
		interrupt_enabled = true;
	}

	Chip_INMUX_PinIntSel(IRQn - IRQn_min, port, pin);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(IRQn - IRQn_min));
	if (mode == Mode::Edge)
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(IRQn - IRQn_min));
	else
		Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(IRQn - IRQn_min));

	if (level == Level::High)
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(IRQn - IRQn_min));
	else
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(IRQn - IRQn_min));
	NVIC_ClearPendingIRQ(IRQn);
	NVIC_EnableIRQ(IRQn);
}
