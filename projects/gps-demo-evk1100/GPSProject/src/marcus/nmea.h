#ifndef NMEA_H_
#define NMEA_H_

#include <avr32/io.h>
#include "compiler.h"

int nmeaProcessGPGGA(char packet[], struct currentPositionStruct *pos);
int nmeaProcessGPVTG(char packet[], struct currentPositionStruct *pos);

#endif /*NMEA_H_*/
