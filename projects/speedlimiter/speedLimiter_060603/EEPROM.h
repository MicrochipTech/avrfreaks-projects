/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					EEPROM.h
|
|	Author:			Rasmus Renzi,	rasmus.r@spray.se
|
|
|
--------------------------------------------------------------------------
|
|	Description:	Speedlimiter for engines with mechanical ignition
|					systems.
--------------------------------------------------------------------------*/

#ifdef EEPROM_C

	void EEPROM_write(unsigned char, unsigned char);
	unsigned char EEPROM_read(unsigned char);

#else

	extern void EEPROM_write(unsigned char, unsigned char);
	extern unsigned char EEPROM_read(unsigned char);

#endif
