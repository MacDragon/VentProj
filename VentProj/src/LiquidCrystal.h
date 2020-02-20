#ifndef LiquidCrystal_h
#define LiquidCrystal_h


#include <cstddef>
#include <string>
#include "chip.h"
#include "DigitalIoPin.h"
#include "Imutex.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidCrystal {
public:

  LiquidCrystal(DigitalIoPin *rs,  DigitalIoPin *enable,
			    DigitalIoPin *d0, DigitalIoPin *d1, DigitalIoPin *d2, DigitalIoPin *d3);

  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();

  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t);
  virtual size_t write(uint8_t);
  void command(uint8_t);

  template<typename... Args>
  void print(std::string const &fmt, Args const &... args) {
	  print(fmt.c_str(), args...);
  }

  template<typename... Args>
  void print(char const * const fmt, Args const &... args) {
  	static char buffer[100];
  	sprintf(buffer, fmt, args...);
  	for(uint8_t i = 0; buffer[i]; ++i) {
  		write(buffer[i]);
  	}
  }

private:
  void send(uint8_t, uint8_t);
  void write4bits(uint8_t);
  void pulseEnable();

  DigitalIoPin *rs_pin; // LOW(false): command.  HIGH(true): character.
  DigitalIoPin *enable_pin; // activated by a HIGH pulse.
  DigitalIoPin *data_pins[4];

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;

  uint8_t _numlines,_currline;
};

#endif
