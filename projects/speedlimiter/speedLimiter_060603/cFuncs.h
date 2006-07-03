/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					miscFuncs.h
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

#ifdef MISCFUNCS_C

	void aboutText(void);
	void imAlive(void);
	void checkButtons(void);
	void intToASCII(char *, unsigned int);
	void DelayFunc(unsigned int);
	void SplashText();

	unsigned char		tmpBit, upPressed, downPressed, selPressed;
	unsigned long int 	lifeCycleCnt;
	unsigned short 		blink;

#else

	extern void aboutText(void);
	extern void imAlive(void);
	extern void checkButtons(void);
	extern void intToASCII(char *, unsigned int);
	extern void DelayFunc(unsigned int);
	extern void SplashText();

	extern unsigned char		tmpBit, upPressed, downPressed, selPressed;
	extern unsigned long int 	lifeCycleCnt;
	extern unsigned short 		blink;

#endif
