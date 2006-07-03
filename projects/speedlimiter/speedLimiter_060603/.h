/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					speedLimiter.h
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
// -- DEFINITIONS --
#define AUTHOR_DATE				"R.Renzi...060525\r"
#define MAX_LIFE_CYCLES         10000
#define LIMIT_STEP				100
#define FIXED_LIMIT				2200
#define RPM_ABS_STOP			4500

#define LIFE_SET            	PORTC = PORTC |= 0x01;		                  		// PC0
#define LIFE_CLR            	PORTC = PORTC &= ~(0x01);		                  	// PC0

#define MENU_UP(in)				in = (0x01&(~((PINB & 0x02)>>1)))					// PB1 (Menu up)
#define MENU_DN(in)				in = (0x01&(~((PINB & 0x04)>>2)))					// PB2 (Menu down)
#define MENU_SL(in)				in = (0x01&(~((PINB & 0x40)>>6)))					// PB6 (Menu select)


#ifdef SPEEDLIMITER_C

// -- Variables

	unsigned int 		currentRPM;
	unsigned int 		currentLimit;
	unsigned char		fixedMode;


// -- Function prototypes --
	void projectInit(void);
	void timerInit(void);

	void viewRPM(void);
	void viewRPMbar(void);
	void setLimit(void);
	void about(void);
	void ignitionTime(void);

#else


#endif
