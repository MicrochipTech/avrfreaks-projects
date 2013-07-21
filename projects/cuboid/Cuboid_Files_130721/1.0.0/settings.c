/*
	Settings module
*/

#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

	struct ee_struct settings_ram;
	struct ee_struct settings_ee EEMEM;
	struct ee_struct settings_p PROGMEM = 
	{
		0xFF,	//must be first byte in structure
		0xFF,	//test

		2,	//preferred sequence at night
		0,	//preferred sequence during day
		
		TRUE,	//mode auto
		0,		//preferred sequence at power up
		
		200,	//threshold night
		500,	//threshold day
		
		//sequence_settings
		{
			{255, TRUE, 0},		//SEQ0 brightness, rotation, pattern if rotation disabled
			{255, TRUE, 0},
			{140, TRUE, 0},
			{140, TRUE, 0}
		}
	};

//********************************************************************************************************
// Private variables
//********************************************************************************************************

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

//********************************************************************************************************
// Public functions
//********************************************************************************************************

//default all settings and save
void settings_default(void)
{
	char test;
	test=settings_ram.test;
	memcpy_P(&settings_ram, &settings_p, sizeof(struct ee_struct));
	settings_ram.test=test;
	settings_save();
	settings_ram.version=EEPROM_LAYOUT_VERSION;	//correct version only AFTER a successful default
	settings_save();
}

//save settings to eeprom, only writes modified bytes to save time
void settings_save(void)
{
	unsigned char* ram_ptr;
	unsigned char* ee_ptr;
	int size=0;

	ee_ptr = (unsigned char*)&settings_ee;
	ram_ptr = (unsigned char*)&settings_ram;
	while(size < sizeof(struct ee_struct))
	{
		if(eeprom_read_byte(ee_ptr) != *ram_ptr)
			eeprom_write_byte(ee_ptr, *ram_ptr);
		size++;
		ee_ptr++;
		ram_ptr++;
	};
}

//load settings, default settings if none saved
void settings_init(void)
{
	eeprom_read_block(&settings_ram, &settings_ee, sizeof(struct ee_struct));
	if(settings_ram.version != EEPROM_LAYOUT_VERSION)
		settings_default();
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************
