

//********************************************************************************************************
// Public defines
//********************************************************************************************************

	//if you change the eeprom structure, increment this to allow the firmware to default the eeprom
	#define EEPROM_LAYOUT_VERSION	2

	//non-volatile settings for each sequence
	struct sequence_settings_struct
	{
		unsigned char brightness;
		unsigned char rotation;		//should sequence rotate through patterns?
		unsigned char preferred;	//pattern shown if rotation is disabled
	};

	//settings layout, instances exist in ram for easy access, flash for defaulting, and e^2
	struct ee_struct
	{
		char 			version;				// must be first
		char 			test;					// not defaulted, =0 if unit has been tested
		
		unsigned char	sequence_auto_night;	// preferred sequence at night
		unsigned char	sequence_auto_day;		// preferred sequence during day
		
		char			mode_auto;				// If true, sequence will detect day/night with loght sensor
		unsigned char	sequence;				// Currently selected sequence
		
		unsigned int 	threshold_night;
		unsigned int 	threshold_day;
		
		struct sequence_settings_struct	sequence_settings[NUMBER_OF_SEQUENCES];
	};

//********************************************************************************************************
// Public variables
//********************************************************************************************************

	//Access members from above structure in settings_ram, call settings_save() to write changes to eeprom
	extern struct ee_struct settings_ram;

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

	//Call to write default settings from flash to eeprom and ram
	void settings_default(void);
	
	//Call to save any changes to eeprom
	void settings_save(void);
	
	//Initialise settings module, loads settings from eeprom, or defaults if device is new
	void settings_init(void);
