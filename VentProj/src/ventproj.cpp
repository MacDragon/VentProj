/*
===============================================================================
 Name        : main.c
 Author      : Joshua Waugh & Visa Harvey
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#include "LiquidCrystal.h"
#include "SimpleMenu.h"
#include "ModeEdit.h"
#include "SDP650.h"
#include "board.h"
#include <atomic>
#include "Fan.h"
#include "PID.h"
#include "PIO.h"
#include "QEI.h"

static constexpr uint32_t kDebounceTime { 200 }; // 200ms. Lazy debouncing.
static constexpr uint32_t kLoopTime    { 1000 }; // 2000ms
static constexpr uint32_t kTickrateHz  { 1000 };
static std::atomic<uint32_t> counter, systicks, last_press;
static SimpleMenu* menu { nullptr };
static QEI* qei { nullptr };

extern "C" {
void PIN_INT0_IRQHandler(void) {
	if (systicks - last_press > kDebounceTime) {
		last_press = systicks.load();
		if (menu != nullptr)
			menu->event(MenuItem::ok);
	}
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
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
	if (++systicks - last_press >= kLoopTime) {
		last_press = systicks.load();
		if (menu != nullptr)
			menu->event(MenuItem::back, 0);
	}

	if (systicks % 10 == 1 && qei != nullptr) // check knob readings
	{
		auto qeichange = qei->read();
		if (qeichange != 0) {
			last_press = systicks.load();
			for (int i = 0; i < abs(qeichange) && menu != nullptr; i++)
				menu->event(MenuItem::change, qeichange);
		}
	}

	if (counter > 0)
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
	button.enableInterrupt(PIN_INT0_IRQn, Mode::Edge, Level::High);

	qei = new QEI(LpcPinMap{ PD4_Port, PD4_Pin }, LpcPinMap{ PD5_Port, PD5_Pin }, 3);

	/* LCD setup */
	Chip_RIT_Init(LPC_RITIMER);
	DigitalIoPin rs(PA0_Port, PA0_Pin, false, true, false), en(PA1_Port, PA1_Pin, false, true, false), d0(PA2_Port, PA2_Pin, false, true, false),
			d1(PA3_Port, PA3_Pin, false, true, false), d2(PA4_Port, PA4_Pin, false, true, false), d3(PA5_Port, PA5_Pin, false, true, false);
	LiquidCrystal lcd { &rs, &en, &d0, &d1, &d2, &d3 };

	/* Watchdog timer setup */
	if (Chip_WWDT_GetStatus(LPC_WWDT) & WWDT_WDMOD_WDTOF) {
		lcd.print("Error occurred.");
		Board_LED_Set(0, 1);
		Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
		while (1) {
			/* Put error handling here. */
		}
	}
	else
		lcd.print("Starting up.");

	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_WDTOSC_PD); 	/* Enable the WDT oscillator */
	auto wdtFreq = Chip_Clock_GetWDTOSCRate() / 4; 		/* The WDT divides the input frequency into it by 4 */
	Chip_WWDT_Init(LPC_WWDT); 							/* Initialize WWDT (also enables WWDT clock) */
	Chip_WWDT_SetTimeOut(LPC_WWDT, wdtFreq * 3); 		/* Set watchdog feed time constant to approximately 3s */
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET); 	/* Configure WWDT to reset on timeout */
	Chip_WWDT_Start(LPC_WWDT); 							/* Start watchdog */

	/* Fan setup */
	Fan fan;

	/* Pressure Sensor setup */
	SDP650 pressureSensor;

	/* Menu setup */
	menu = new SimpleMenu;

	ModeEdit autoEdit(lcd, 30, 120, ModeEdit::Automatic);
	autoEdit.observe(pressureSensor);
	menu->addItem(autoEdit);

	ModeEdit manualEdit(lcd, 0, 100, ModeEdit::Manual);
	manualEdit.observe(pressureSensor);
	menu->addItem(manualEdit);

	/* PID setup */
	PID<int> pid(1, 0, 0.125);

	Chip_WWDT_Feed(LPC_WWDT);

	while (1) {
		auto startTime = millis();
		auto fanFreq = fan.getFrequency();
		auto pressure_diff = pressureSensor.getPressure();

		if (fanFreq != Fan::kFanError && pressure_diff != SDP650::kI2CError) {
			switch (ModeEdit::fanMode) {
			case ModeEdit::Manual:
				if (fanFreq != manualEdit.getValue()) {
					fan.setFrequency(manualEdit.getValue());
					autoEdit.setValue(pressureSensor.getPressure());
				}
				break;

			case ModeEdit::Automatic:
				if (abs(pressure_diff - autoEdit.getValue()) > 1) {
					manualEdit.setValue(fanFreq + pid.calculate(autoEdit.getValue(), pressure_diff));
					fan.setFrequency(manualEdit.getValue());
				}
				break;
			}
		} else {
			/* Error handling here */
		}

		menu->event(MenuItem::show);

		auto timeTaken = millis() - startTime;
		if (timeTaken < kLoopTime)
			Sleep(kLoopTime - timeTaken);

		/* Feed the watchdog timer at regular 1s intervals */
		Chip_WWDT_Feed(LPC_WWDT);
	}
}
