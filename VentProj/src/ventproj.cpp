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
#include <atomic>
#include "board.h"

#include "LiquidCrystal.h"
#include "SimpleMenu.h"
#include "MenuEdit.h"
#include "ModeEdit.h"
#include "I2C.h"
#include "Fan.h"
#include "PID.h"
#include "PIO.h"

#define TICKRATE_HZ (1000)

static constexpr uint32_t cancel_time = 2000; // 2000ms
static constexpr uint32_t debounce_time = 150; // 150ms. Lazy debouncing.
static constexpr uint8_t i2c_pressure_address = 0x40;
static constexpr uint8_t sensorReadCMD = 0xF1;
static std::atomic<uint32_t> counter, systicks, last_press;
static SimpleMenu* menu;

extern "C" {
void PIN_INT0_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));

	if(systicks - last_press > debounce_time) {
		last_press = systicks.load();
		if ( menu != nullptr ) menu->event(MenuItem::up);
	}
}

void PIN_INT1_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));

	if(systicks - last_press > debounce_time) {
		last_press = systicks.load();
		if ( menu != nullptr ) menu->event(MenuItem::down);
	}
}

void PIN_INT2_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));

	if(systicks - last_press > debounce_time) {
		last_press = systicks.load();
		if ( menu != nullptr ) menu->event(MenuItem::ok);

	}
}

void SysTick_Handler(void) {
	if (++systicks - last_press >= cancel_time) {
		last_press = systicks.load();
		if ( menu != nullptr ) menu->event(MenuItem::back);
	}

	if(counter > 0)
		--counter;
}
}

void Sleep(unsigned int ms) {
	counter = ms;
	while(counter > 0)
		__WFI();
}

uint32_t millis() {
	return systicks;
}

int main(void) {
	SystemCoreClockUpdate();
	Board_Init();

	int16_t pressure_diff = 0;
	uint8_t sensorData[3];

	/* Button setup - to be replaced by QEI */
	DigitalIoPin SW0(PD7_Port, PD7_Pin, true, true, true);
	DigitalIoPin SW1(PD6_Port, PD6_Pin, true, true, true);
	DigitalIoPin SW2(PD3_Port, PD3_Pin, true, true, true);

	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT); /* Enable PININT clock */
	Chip_SYSCTL_PeriphReset(RESET_PININT); /* Reset the PININT block */

	/* Configure interrupt channels for the DigitalIoPins */
	Chip_INMUX_PinIntSel(0, PD7_Port, PD7_Pin); // SW0
	Chip_INMUX_PinIntSel(1, PD6_Port, PD6_Pin); // SW1
	Chip_INMUX_PinIntSel(2, PD3_Port, PD3_Port); // SW2

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
	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);

	/* LCD setup */
	Chip_RIT_Init(LPC_RITIMER);

	LiquidCrystal lcd {
		std::make_unique<DigitalIoPin>( PA0_Port, PA0_Pin, false, true, false ),
		std::make_unique<DigitalIoPin>( PA1_Port, PA1_Pin, false, true, false ),
		std::make_unique<DigitalIoPin>( PA2_Port, PA2_Pin, false, true, false ),
		std::make_unique<DigitalIoPin>( PA3_Port, PA3_Pin, false, true, false ),
		std::make_unique<DigitalIoPin>( PA4_Port, PA4_Pin, false, true, false ),
		std::make_unique<DigitalIoPin>( PA5_Port, PA5_Pin, false, true, false )
	};

	/* Menu setup */
	menu = new SimpleMenu;

	MenuEdit mainMenu(&lcd, std::string("Pressure:"), std::string("Fan speed:"), std::string("Working..."));
	menu->addItem(&mainMenu);

	ModeEdit manualMenu(&lcd, std::string("Manual Mode"), std::string("Set Fan Speed:"), 0, 100, ModeEdit::Manual);
	menu->addItem(&manualMenu);

	ModeEdit autoMenu(&lcd, std::string("Auto Mode"), std::string("Set Pressure:"), 30, 120, ModeEdit::Automatic);
	menu->addItem(&autoMenu);

	/* Systick setup */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	/* Fan setup */
	Fan fan;

	/* I2C setup */
	I2C i2c;
	NVIC_DisableIRQ(I2C0_IRQn);

	/* PID setup */
	PID<int> pid(255, 1.85, 0);

	while(1) {
		auto fanFreq = fan.getFrequency();

		if (i2c.transaction(i2c_pressure_address, &sensorReadCMD, 1, sensorData, 3))
			pressure_diff = static_cast<int16_t>(sensorData[0] << 8 | sensorData[1]) / 240;

		switch(ModeEdit::fanMode) {
		case ModeEdit::Manual:
			if (fanFreq / 200 != manualMenu.getValue())
				fan.setFrequency(200 * manualMenu.getValue());

			/* There is a problem with these checks for now where the fanMode might change
			 * between the start of the scope and this if check. It leads to old values
			 * being written over the value just entered in the interrupt. */
			if (!autoMenu.getFocus())
				autoMenu.setValue(pressure_diff); // Keep autoMenu updated with current pressure
			break;

		case ModeEdit::Automatic:
			if (abs(pressure_diff - autoMenu.getValue()) > 1)
				fan.setFrequency( fanFreq + pid.calculate(autoMenu.getValue(), pressure_diff) );

			if (!manualMenu.getFocus())
				manualMenu.setValue(fanFreq / 200); // Keep manualMenu updated with current fan speed
			break;

		case ModeEdit::Startup:
			/* Could do something here. The system boots up in this state */
			break;
		}

		/* Update main menu with values from manual and auto menus
		 *
		 * Current and target values passed to the menu so that it
		 * can keep an error counter and update the UI when things
		 * are taking too long. */
		mainMenu.setRow1Values(pressure_diff, autoMenu.getValue());
		mainMenu.setRow2Values(fanFreq / 400, manualMenu.getValue());
		menu->event(MenuItem::show);
		Sleep(200);

	}
}
