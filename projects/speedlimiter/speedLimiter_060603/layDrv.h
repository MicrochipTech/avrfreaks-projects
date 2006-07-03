/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					displayDrv.h
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

#define DISPL_EN_SET			PORTC |= 0x02;										// PC1
#define DISPL_EN_CLR			PORTC &= ~(0x02);									// PC1
#define DISPL_RW_SET			PORTC |= 0x04;										// PC2
#define DISPL_RW_CLR			PORTC &= ~(0x04);									// PC2
#define DISPL_RS_SET			PORTC |= 0x08										// PC3
#define DISPL_RS_CLR			PORTC &= ~(0x08);									// PC3	
#define DISPL_DATA(in)          PORTD = (in & 0xFF)                       			// PORTD

#ifdef DISPLAYDRV_C

	void InitDisplay(void);
	void ClearDisplay(void);
	void SetToLn2(void);
	void PutCharAtPosLn1(int, int);
	void PutCharAtPosLn2(int, int);
	void PutChar (int);
	void PrintString(unsigned char*)



#else

	extern void InitDisplay(void);
	extern void ClearDisplay(void);
	extern void SetToLn2(void);
	void PutCharAtPosLn1(int, int);
	void PutCharAtPosLn2(int, int);
	extern void PutChar (int);
	void PrintString(unsigned char*);



#endif
