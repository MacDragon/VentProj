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
#include "systick.h"
#include "DigIO.h"
#include "PIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ITMOut.h"
#include "LiquidCrystal.h"
#include "RealTimeClock.h"
#include "TempSensor.h"
#include "BarGraph.h"

#include "LpcUart.h"

#include "SimpleMenu.h"
#include "IntegerEdit.h"
#include "DecimalEdit.h"



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

void QEI_Setup(){

//LPC_SWM->PINASSIGN[14] =

/*		7:0 USB_FRAME_TO G_O
		15:8 QEI0_PHA_I 23:16 QEI0_PHB_I 31:24 QEI0_IDX_I
		USB_FRAME_TOG function assignment. The value is the 0xFF pin number to be assigned to this function. PIO0_0 = 0,
		..., PIO1_0 = 32, ..., PIO2_11 = 75.
		QEI0_PHA function assignment. The value is the pin 0xFF number to be assigned to this function. PIO0_0 = 0, ...,
		PIO1_0 = 32, ..., PIO2_11 = 75.
		QEI0_PHB function assignment. The value is the pin 0xFF number to be assigned to this function. PIO0_0 = 0, ...,
		PIO1_0 = 32, ..., PIO2_11 = 75.
		QEI0_IDX function assignment. The value is the pin 0xFF number to be assigned to this function. PIO0_0 = 0, ...,
		PIO1_0 = 32, ..., PIO2_11 = 75.
		*/

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
		Chip_SWM_MovablePortPinAssign(SWM_QEI0_PHA_I,  PD2_Port,  PD2_Pin);
	  	Chip_SWM_MovablePortPinAssign(SWM_QEI0_PHB_I,  PD3_Port,  PD3_Pin);
		Chip_SWM_MovablePortPinAssign(SWM_QEI0_IDX_I,  PD4_Port,  PD4_Pin);

		uint32_t pinsettings = IOCON_DIGMODE_EN | IOCON_MODE_PULLUP;

		Chip_GPIO_SetPinDIRInput(LPC_GPIO, PD2_Port, PD2_Pin);
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, PD3_Port, PD3_Pin);
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, PD4_Port, PD4_Pin);

		LPC_QEI->MAXPOS=200;

//		Chip_IOCON_PinMuxSet(LPC_IOCON, PD0_Port, PD0_Pin, IOCON_DIGMODE_EN);

}



//#define LPC_QEI_PINS PINASSIGN14

/*
 * QEI0_PHA I QEI0_PHB I QEI0_IDX I
external to pin
external to pin
external to pin
any pin any pin any pin
PINASSIGN14 PINASSIGN14 PINASSIGN14
Table 121 Table 121 Table 121
Phase A (PhA) input to the Quadrature Encoder Interface.
Phase B (PhB) input to the Quadrature Encoder Interface.
Index (IDX) input to the Quadrature Encoder Interface.
 */

#define EXERCISE2

#define INPBUFLEN 80


#ifdef EXERCISE2
LpcUart *dbgu;
DecimalEdit *timev;
DecimalEdit *blank;
IntegerEdit *light;

LEDPin *ledred;
LEDPin *ledgreen;
LEDPin *ledblue;
LEDPin *lednone = NULL;

void handleleds()
{
	char buffer[BUFLEN+1] = { 0 };
	sprintf (buffer, "Time: %.1f\n\r", timev->getValue());
	dbgu->write(buffer);
	sprintf (buffer, "Blank: %.1f\n\r", blank->getValue());
	dbgu->write(buffer);
	sprintf (buffer, "Light: %d\n\r\n\r", light->getValue());
	dbgu->write(buffer);

	switch ( light->getValue() )
	{
	case 0:
		SetLED(*lednone, 0, 0); // sets to nothing showing.
		break;
	case 1:
		SetLED(*ledred, timev->getValue(), 1000 * blank->getValue());
		break;
	case 2:
		SetLED(*ledgreen, timev->getValue(), 1000 * blank->getValue());
		break;
	case 3:
		SetLED(*ledblue, timev->getValue(), 1000 * blank->getValue());
		break;
	}
}

int QEITest() {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif
	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / 1000);

	NVIC_DisableIRQ(I2C0_IRQn);

    // TODO: insert code here
    Chip_RIT_Init(LPC_RITIMER);

 //   NVIC_EnableIRQ(RITIMER_IRQn);

	SysTickSetup();

	LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none, none };
	//LpcUart dbgu(cfg);

	QEI_Setup();

	dbgu = new LpcUart(cfg);

	dbgu->write("Starting.\n\r");

	char buffer[80];
	while ( 1 ) {
		int position=LPC_QEI->POS/2;
		sprintf(buffer, "Pos: %d, Vel: %d,\n\r", position, LPC_QEI->VEL);

		dbgu->write(buffer);
		Sleep(100);
	}


	ButtonPin sw1(SW1_Port,SW1_Pin);
	ButtonPin sw2(SW2_Port,SW2_Pin);
	ButtonPin sw3(SW3_Port,SW3_Pin);

	ledred = new LEDPin(LEDRed_Port,LEDRed_Pin);
	ledgreen = new LEDPin(LEDGreen_Port,LEDGreen_Pin);
	ledblue = new LEDPin(LEDBlue_Port,LEDBlue_Pin);

	OutputPin rs(PA0_Port, PA0_Pin);
	OutputPin en(PA1_Port, PA1_Pin);

	OutputPin d4(PA2_Port, PA2_Pin);
	OutputPin d5(PA3_Port, PA3_Pin);
	OutputPin d6(PA4_Port, PA4_Pin);
	OutputPin d7(PA5_Port, PA5_Pin);

    LiquidCrystal *lcd= new LiquidCrystal(&rs, &en, &d4, &d5, &d6, &d7);

    lcd->begin(16,2);
    lcd->setCursor(0,0);

    SimpleMenu menu; /* this could also be allocated from the heap */
    timev = new DecimalEdit(lcd, std::string("Time"), 0, 200, 20 );
    blank = new DecimalEdit(lcd, std::string("Blank"), 0, 1, 0.1 );
    light = new IntegerEdit(lcd, std::string("Light"), 0, 3);
    menu.addItem(new MenuItem(timev));
    menu.addItem(new MenuItem(blank));
    menu.addItem(new MenuItem(light));
    timev->setValue(100);
    timev->setCallback(&handleleds);
    blank->setValue(0.5);
    blank->setCallback(&handleleds);
    light->setValue(1);
    light->setCallback(&handleleds);


    menu.event(MenuItem::show); // display first menu item

    uint32_t lastpressed = timebase;

    handleleds(); // set initial state of LEDs

    while ( 1 ) // run menu.
    {
    	if ( sw1.ReadOnceWithBounce() ) {
    		menu.event(MenuItem::ok); // ok
    		lastpressed = timebase;
    	}

    	if ( sw2.ReadOnceWithBounce() ) {
    		menu.event(MenuItem::up); // up
       		lastpressed = timebase;
    	}

    	if ( sw3.ReadOnceWithBounce() ) {
    		menu.event(MenuItem::down); // down
       		lastpressed = timebase;
    	}

    	if ( timebase-lastpressed > 10000 )
    	{
    		menu.event(MenuItem::back); // back
       		lastpressed = timebase; // only one level so don't need multiple events.
    	}

        Sleep(10);
    }

	while(1);

	return 0;
}

#endif

int main(void) {

	QEITest();

	while(1) Sleep(100);

}
