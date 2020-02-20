/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include <atomic>
#include "DigitalIoPin.h"
#include "Dice.h"
// TODO: insert other definitions and declarations here
#define TICKRATE_HZ1 (1000)
#define DELAY (10)
static volatile std::atomic_int counter;
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

void SysTick_Handler(void)
{
	if(counter > 0) counter--;
}

void Sleep(int ms)
{
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}

int main(void) {

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#endif

	// TODO: insert code here
	unsigned int count[3];
	int counter = 1;
	uint32_t sysTickRate;

	SystemCoreClockUpdate();
	Board_Init();

	Chip_Clock_SetSysTickClockDiv(1);

	sysTickRate = Chip_Clock_GetSysTickClockRate();

	SysTick_Config(sysTickRate / TICKRATE_HZ1);

	// Exercise 1/2
	//DigitalIoPin SW1(0, 17, true, true, true), SW2(1, 11, true, true, true), SW3(1, 9, true, true, true);
	// Exercise 3
	DigitalIoPin PB1(1, 0, true, true, true); // Gambler button.
	DigitalIoPin test(0, 8, true, true, true);
	Dice dice;


	while (1) {
		/*if(SW1.read()) {
			for(unsigned int i = 0; i < 3; ++i){
				Board_LED_Toggle(i);
				Sleep(1000);
				Board_LED_Toggle(i);
			}
		}*/

		// Exercise 2
		if(SW1.read()) {
			Board_LED_Set(0, true);
			count[0] = 0;
		} else if (Board_LED_Test(0) && (count[0] += DELAY) >= 1000) {
			Board_LED_Set(0, false);
			count[0] = 0;
		}

		if(SW2.read()) {
			Board_LED_Set(1, true);
			count[1] = 0;
		} else if (Board_LED_Test(1) && (count[1] += DELAY) >= 1000) {
			Board_LED_Set(1, false);
			count[1] = 0;
		}

		if(SW3.read()) {
			Board_LED_Set(2, true);
			count[2] = 0;
		} else if (Board_LED_Test(2) && (count[2] += DELAY) >= 1000) {
			Board_LED_Set(2, false);
			count[2] = 0;
		}

		Sleep(DELAY);

		// Exercise 3

		/*while (test.read()) {
			dice.Show(7);
			counter = 0;
		}

		while (PB1.read()) {
			dice.Show(0);
			if(++counter > 6)
				counter = 1;
		}

		dice.Show(counter); // */

	}

	return 0;
}
