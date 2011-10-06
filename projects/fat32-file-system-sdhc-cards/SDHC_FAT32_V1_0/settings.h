
//if you change the eeprom structure, increment this to allow the firmware to default the eeprom
#define EEPROM_LAYOUT_VERSION	1

	struct ee_struct
	{
		char version;	//must be first
		char test;		//not defaulted, =0 if unit has been tested
		
		unsigned char setting1;
		unsigned char setting2;
	};

	extern struct ee_struct settings_ram;
	
void settings_default(void);
void settings_save(void);
void settings_load(void);
