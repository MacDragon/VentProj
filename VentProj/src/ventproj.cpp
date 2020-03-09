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
#include "ModeEdit.h"
#include "SDP650.h"
#include "I2C.h"
#include "Fan.h"
#include "PID.h"
#include "PIO.h"
#include "QEI.h"

static constexpr uint32_t kTickrateHz { 1000 };
static constexpr uint32_t kCancelTime { 1000 }; // 2000ms
static constexpr uint32_t kDebounceTime { 100 }; // 100ms. Lazy debouncing.
static std::atomic<uint32_t> counter, systicks, last_press;
static SimpleMenu* menu { nullptr };
static QEI* qei { nullptr };

extern "C" {
void PIN_INT0_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));

	if (systicks - last_press > kDebounceTime) {
		last_press = systicks.load();
		if (menu != nullptr)
			menu->event(MenuItem::ok);

	}
}

void QEI_IRQHandler(void){
	/*	if ( qei != nullptr ){
		int qeichange = qei->read();
		if ( qeichange != 0 ){
			last_press = systicks.load();
			for ( int i=0;i<abs(qeichange);i++)
				menu->event(MenuItem::change, qeichange);
		}
	} */
}

void SysTick_Handler(void) {
	if (++systicks - last_press >= kCancelTime) {
		last_press = systicks.load();
		if ( menu != nullptr ) menu->event(MenuItem::back, 0);
	}

	if (systicks % 10 == 1 && qei != nullptr ) // check knob readings
	{
		int qeichange = qei->read();
		if ( qeichange != 0 ){
			last_press = systicks.load();
			for (int i = 0; i < abs(qeichange); i++)
				menu->event(MenuItem::change, qeichange);
		}
	}

	if(counter > 0)
		--counter;
}
}

void Sleep(unsigned int ms) {
	counter = ms;
	while (counter > 0)
		__WFI();
}

uint32_t millis() {
	return systicks;
}

int main(void) {
	SystemCoreClockUpdate();
	Board_Init();

	/* Systick setup */
	SysTick_Config(SystemCoreClock / kTickrateHz);

	/* Input setup */
	DigitalIoPin button(PD3_Port, PD3_Pin, true, true, true);
	qei = new QEI(LpcPinMap{ PD4_Port, PD4_Pin }, LpcPinMap{ PD5_Port, PD5_Pin }, 3);

	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	Chip_INMUX_PinIntSel(0, PD3_Port, PD3_Pin);
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(0));
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT0_IRQn);

	/* LCD setup */
	Chip_RIT_Init(LPC_RITIMER);
	DigitalIoPin rs(PA0_Port, PA0_Pin, false, true, false), en(PA1_Port, PA1_Pin, false, true, false), d0(PA2_Port, PA2_Pin, false, true, false),
			d1(PA3_Port, PA3_Pin, false, true, false), d2(PA4_Port, PA4_Pin, false, true, false), d3(PA5_Port, PA5_Pin, false, true, false);
	LiquidCrystal lcd { &rs, &en, &d0, &d1, &d2, &d3 };
	lcd.print("Starting up...");

	/* Fan setup */
	Fan fan;

	/* Pressure Sensor setup */
	SDP650 pressureSensor(I2C{});

	/* Menu setup */
	menu = new SimpleMenu;
	ModeEdit autoEdit(lcd, 30, 120, ModeEdit::Automatic);
	autoEdit.observe(pressureSensor);
	menu->addItem(autoEdit);
	ModeEdit manualEdit(lcd, 0, 100, ModeEdit::Manual);
	manualEdit.observe(pressureSensor);
	menu->addItem(manualEdit);

	/* PID setup */
	PID<int> pid(255, 0, 1.35);

	while (1) {
		auto fanFreq = fan.getFrequency();
		auto pressure_diff = pressureSensor.getPressure();

		if (fanFreq != Fan::kFanError && pressure_diff != SDP650::kI2CError) {
			switch (ModeEdit::fanMode) {
			case ModeEdit::Manual:
				if (fanFreq / 200 != manualEdit.getValue())
					fan.setFrequency(200 * manualEdit.getValue());
				break;

			case ModeEdit::Automatic:
				if (abs(pressure_diff - autoEdit.getValue()) > 1) {
					fan.setFrequency(fanFreq + pid.calculate(autoEdit.getValue(), pressure_diff));
					manualEdit.setValue(fan.getFrequency());
				}
				break;
			}
		} else {
			/* Error handling here */
		}

		menu->event(MenuItem::show);

		Sleep(50);
	}
}
