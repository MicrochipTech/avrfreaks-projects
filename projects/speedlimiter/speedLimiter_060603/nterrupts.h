/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					interrupts.h
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

#ifdef INTERRUPTS_C
	
	unsigned int		us50Cnt;
	unsigned char		pulseCount;
	unsigned int		tmpIgnTime;
	unsigned int		prevIgnTime;
	unsigned int		ignPeriodTime;
	unsigned int		prevIgnPeriodTime;
	unsigned char		cutNow;
	unsigned int		currentLimitTime;

	unsigned char		portTmp;

#else

	extern unsigned int			us50Cnt;
	extern unsigned char		pulseCount;
	extern unsigned int			tmpIgnTime;
	extern unsigned int			prevIgnTime;
	extern unsigned int			ignPeriodTime;
	extern unsigned int			prevIgnPeriodTime;
	extern unsigned char		cutNow;
	extern unsigned int			currentLimitTime;

#endif
