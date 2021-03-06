#include "Brujula.h"

Brujula::Brujula():TwoWire()
{
	TwoWire::begin();
}

float Brujula::lecturaf()
{
	return((float)Brujula::lectura()/10);
}

int Brujula::lectura()
{
	byte highByte;
	byte lowByte;
	TwoWire::beginTransmission(0x60);
	TwoWire::send(2);
	TwoWire::endTransmission();
	TwoWire::requestFrom(0x60, 2);
	while(TwoWire::available() < 2);
	highByte = TwoWire::receive();
	lowByte = TwoWire::receive();
	return(((highByte<<8)+lowByte));
}

int Brujula::version()
{
	int version;
	TwoWire::beginTransmission(0x60);
	TwoWire::send(0);
	TwoWire::endTransmission();
	TwoWire::requestFrom(0x60,1);
	while(TwoWire::available()<1);
	version = TwoWire::receive();
	return(version);
}
