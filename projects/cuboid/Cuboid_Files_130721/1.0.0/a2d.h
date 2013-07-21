/*
	A2D functions for measuring
*/

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

	//Call every timer tick
    void            a2d_tick(void);
	
	//Call to initialise module
    void            a2d_init(void);
	
	//Call from within main_fly to repeatedly monitor LED supply voltage
    void            a2d_fly(void);
    
	//Return peak LED current in mA
    double          a2d_led_i(void);
	
	//Return LED supply voltage in V
    double          a2d_led_v(void);
	
	//Return input supply voltage in V
    double          a2d_supply_v(void);
	
	//Return light level 0-1023 (10bit)
    unsigned int    a2d_light(void);

	//Return LED forward voltage drop in V
    double          a2d_led_fv(void);

