
//********************************************************************************************************
// Public defines 
//********************************************************************************************************

	//in V. Used to detect a power supply fault. If LED supply voltage ever exceeds this, turn all LEDs off and reset.
    #define POWER_VLED_LIMIT    6
    
//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

	void power_init(void);
	
	void power_vled_set(unsigned char X);   //0=minimum supply voltage 255=maximum
	
	unsigned char power_vled_get(void);     //0=minimum supply voltage 255=maximum
	
	//Call to perform a power supply test
	void power_test(void);
