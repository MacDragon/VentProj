#include "DigIO.h"
#include "systick.h"


DigitalIoPin::DigitalIoPin(int port, int pin, bool input, bool pullup, bool invert) : port(port), pin(pin), input(input), invert(invert)
{
	uint32_t pinsettings = IOCON_DIGMODE_EN;
	if ( input ) {
		if ( pullup ) pinsettings = pinsettings | IOCON_MODE_PULLUP;
		if ( invert ) pinsettings = pinsettings | IOCON_INV_EN;
		Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, pinsettings);
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
		Sleep(1); // allow input to settle
	} else
	{
		pinsettings = pinsettings | IOCON_MODE_INACT;
		Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, pinsettings);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
		state = false;
		write(false);
	}
};

DigitalIoPin::~DigitalIoPin() {};

bool DigitalIoPin::read() {
	if ( input ) {
		return Chip_GPIO_GetPinState(LPC_GPIO, port,pin);
	} else return ( invert ) ? !Chip_GPIO_GetPinState(LPC_GPIO, port,pin)
		: Chip_GPIO_GetPinState(LPC_GPIO, port,pin);
		//return state; // state = !Chip_GPIO_GetPinState(LPC_GPIO, ledports[LEDNumber], ledpins[LEDNumber]); used in board library
};

void DigitalIoPin::write(bool in) {
	bool value = in;
	if (!input){
		state = value;
		if ( invert ) value = !value;
		Chip_GPIO_SetPinState(LPC_GPIO, port, pin, value);
	}
};

void DigitalIoPin::toggle() {
	if (!input){
		state = !state;
		write(state);
	}

}


ButtonPin::ButtonPin(int port, int pin, bool invert ) :
	DigitalIoPin( port,  pin, true, true, invert), press(0), pressed(false), release(0), returnstate(false), lastcall() { };


uint32_t ButtonPin::ReadWithBounce() {

	if ( timebase-lastcall < 10 ) return returnstate; // don't check status unless it's been at least 9ms since last check;
	lastcall = timebase;

	if(read()) {
		press++;
		release = 0;
	}
	else {
		release++;
		press = 0;
	}

	if ( press > 3 ) {
		timepressed = timebase;
		pressed = true;
		returnstate = true;
	};

	if ( release > 3 )
	{
		returnstate = false;
	}

	if ( returnstate ) {
		uint32_t returnval = timebase - timepressed;
		if ( returnval == 0 ) returnval = 1;
		return returnval;
	}
	else return 0; // return the current internal button state
}

bool ButtonPin::ReadOnceWithBounce() {
	if ( !pressed ) return ReadWithBounce(); else if ( !ReadWithBounce() ) ResetPress();
	return 0;
}

void ButtonPin::ResetPress() { pressed = false; returnstate = false; };

bool ButtonPin::Released() { if ( !read() && pressed ) { ResetPress(); return true; } else return false; };

OutputPin::OutputPin(int port, int pin, bool invert) :
	DigitalIoPin( port,  pin, false, false, invert ), offms(0) { };

void OutputPin::setcounter(uint32_t time) {
	if ( !read() ) write(true);
	starttime = timebase;
	offms = time;
};

void OutputPin::checkcounter() {
	if ( timebase - starttime > offms && read() ) write(false);
};


LEDPin::LEDPin(int port, int pin, bool invert) :
	OutputPin( port,  pin, invert ) { };



