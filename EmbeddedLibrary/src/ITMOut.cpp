/*
 * ITM.cpp
 *
 *  Created on: 17 Jan 2020
 *      Author: visa
 */

#include "ITMOut.h"
#include "ITM_write.h"
#include <stdlib.h>

ITMOut::ITMOut() {
	// TODO Auto-generated constructor stub
	ITM_init();
}

void ITMOut::print( const char *output)
{
	ITM_write( output );
}
void ITMOut::print( const std::string &output )
{
	ITM_write( output.c_str() );
}

void ITMOut::print( const int output )
{
	snprintf ( buffer, BUFLEN, "%d", output );
	ITM_write(buffer);
}

ITMOut::~ITMOut() {
	// TODO Auto-generated destructor stub
}

