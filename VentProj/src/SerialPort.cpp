/*
 * SerialPort.cpp
 *
 *  Created on: 10.2.2016
 *      Author: krl
 */
#include "SerialPort.h"


/* Not sure why uart is allocated on the heap */
SerialPort::SerialPort() :
uart{ new LpcUart { LpcUartConfig { LPC_USART1, 9600, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_2, true,
	LpcPinMap{ 1, 9 }, LpcPinMap{ 1, 10 }, LpcPinMap{ 0, 29 }, LpcPinMap{ -1, -1 } } }
} { /* Empty */ }

int SerialPort::available() {
	return uart->peek();
}

void SerialPort::begin(int speed) {
	uart->speed(speed);
}

int SerialPort::read() {
	char byte;
	if (uart->read(byte) > 0)
		return (byte);
	return -1;
}

int SerialPort::write(const char* buf, int len) {
	return uart->write(buf, len);
}

int SerialPort::print(int val, int format) {
	// here only to maintain compatibility with Arduino interface
	(void) val;
	(void) format;
	return (0);
}

void SerialPort::flush() {
	while (!uart->txempty())
		__WFI();
}
