/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#include <cr_section_macros.h>
#include <memory>
#include <atomic>
#include "board.h"

#include "LiquidCrystal.h"
#include "IntegerEdit.h"
#include "SimpleMenu.h"
#include "LpcUart.h"
#include "I2C.h"
#include "Fan.h"
#include "PID.h"

#define TICKRATE_HZ (1000)
static constexpr uint8_t i2c_pressure_address { 0x40 };
static constexpr uint8_t sensorReadCMD { 0xF1 };
static std::atomic<uint32_t> counter, systicks, last_press;
static std::atomic<int16_t> pressure_diff;
static uint8_t sensorData[3];
static LiquidCrystal* lcd;
static SimpleMenu* menu;
static Fan* fan;
static I2C* i2c;

extern "C" {
void PIN_INT0_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	if(systicks - last_press > 200) {
		menu->event(MenuItem::up);
		last_press = systicks.load();
	}
}

void PIN_INT1_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
	if(systicks - last_press > 200) {
		menu->event(MenuItem::down);
		last_press = systicks.load();
	}
}

void PIN_INT2_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
	if(systicks - last_press > 200) {
		menu->event(MenuItem::ok);
		last_press = systicks.load();
	}
}

void SysTick_Handler(void) {
	systicks++;
	if(counter > 0) counter--;
}
}

void Sleep(int ms) {
	counter = ms;
	while(counter > 0) __WFI();
}

void delayMilliseconds(uint64_t ms) {
	uint64_t compval = ms * SystemCoreClock / 1000;
	Chip_RIT_Disable(LPC_RITIMER);
	Chip_RIT_SetCompareValue(LPC_RITIMER, compval);
	Chip_RIT_SetCounter(LPC_RITIMER, 0);
	Chip_RIT_Enable(LPC_RITIMER);
	while(!Chip_RIT_GetIntStatus(LPC_RITIMER)) {};
	Chip_RIT_Disable(LPC_RITIMER);
	Chip_RIT_ClearIntStatus(LPC_RITIMER);
}

uint32_t millis() {
	return systicks;
}

int main(void) {
	SystemCoreClockUpdate();
	Board_Init();

	/* I/O setup */
	Chip_SWM_MovablePortPinAssign(SWM_SWO_O, 1, 2);
	LpcUart dbgu{ LpcUartConfig{ LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false,
		LpcPinMap{ 0, 18 }, LpcPinMap{ 0, 13 }, LpcPinMap{-1, -1}, LpcPinMap{-1, -1} } };

	/* Button setup - to be replaced by QEI */
	DigitalIoPin sw0(0, 0, true, true, true);
	DigitalIoPin sw1(1, 3, true, true, true);
	DigitalIoPin sw2(0, 9, true, true, true);

	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT); /* Enable PININT clock */
	Chip_SYSCTL_PeriphReset(RESET_PININT); /* Reset the PININT block */

	/* Configure interrupt channels for the DigitalIoPins */
	Chip_INMUX_PinIntSel(0, 0, 0); // SW1
	Chip_INMUX_PinIntSel(1, 1, 3); // SW2
	Chip_INMUX_PinIntSel(2, 0, 9); // SW3

	/* Configure channel 0 as edge sensitive and rising edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(0));

	/* Configure channel 1 as edge sensitive and rising edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(1));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(1));

	/* Configure channel 2 as edge sensitive and rising edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(2));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(2));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_SetPriority(PIN_INT0_IRQn, 1);
	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_SetPriority(PIN_INT1_IRQn, 1);
	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	NVIC_SetPriority(PIN_INT2_IRQn, 1); // INT2 MUST be lower priority than UART irq. 0 and 1 are set to match 2.

	/* LCD setup */
	Chip_RIT_Init(LPC_RITIMER);
	DigitalIoPin A0(0, 8, false, true, false), A1(1, 6, false, true, false), A2(1, 8, false, true, false), A3(0, 5, false, true, false), A4(0, 6, false, true, false), A5(0, 7, false, true, false);
	lcd = new LiquidCrystal(&A0, &A1, &A2, &A3, &A4, &A5);
	lcd->begin(16,2);
	lcd->setCursor(0,0);

	/* Systick setup */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	/* Fan setup */
	fan = new Fan;

	/* I2C setup */
	i2c = new I2C;
	NVIC_DisableIRQ(I2C0_IRQn);

	/* Menu setup */
	menu = new SimpleMenu;

	IntegerEdit manualMode(lcd, std::string("Set fan speed:"), 0, 100);
	manualMode.setCallback([](const IntegerEdit& ie) { fan->setFrequency(static_cast<uint16_t>(200 * ie.getValue())); });
	menu->addItem(new MenuItem(&manualMode));

	IntegerEdit autoMode(lcd, std::string("Set pressure:"), 0, 120);
	autoMode.setCallback([](const IntegerEdit& ie) {
		PID pid(225, 2, 0);

		while ( i2c->transaction(i2c_pressure_address, &sensorReadCMD, 1, sensorData, 3) &&
				ie.getValue() != (pressure_diff = static_cast<int16_t>(sensorData[0] << 8 | sensorData[1]) / 240)) {

			fan->setFrequency(fan->getFrequency() + pid.calculate(ie.getValue(), pressure_diff));
			/* Incomplete UI */
			lcd->setCursor(0, 1);
			lcd->print("%3d", pressure_diff.load());
		}
		lcd->setCursor(0, 1);
		lcd->print("OK!");

		delayMilliseconds(500);
	});
	menu->addItem(new MenuItem(&autoMode));

	menu->event(MenuItem::show);

	while(1) {
		__WFI();
	}
}
