/*
 * 	DMX-Interface Funktions
 *
 *	Written by G. Lehnerts
 *	Version 1.0
 *	Date: Dec 2009
 *
 */

/*----------------------------------------------------------------------
 *
 *	int DMX_GetNrSets(void)
 */

int DMX_GetNrSets(void);

/*----------------------------------------------------------------------
 *	ChannelSet selection
 */

int DMX_SetChSet(int NewSetNr);

int DMX_GetActivChSetNr(void);

/*----------------------------------------------------------------------
 *
 *	Read a single channels data from the internal buffer
 */

unsigned char DMX_GetChannelData(int channel_nr);

/*----------------------------------------------------------------------
 *
 *	Return nummber of buffered channels
 */

int DMX_GetChannelCount(void);

/*----------------------------------------------------------------------
 *
 *	Check if channel data has changed
 */

int DMX_IsChannelChanged(int channel_nr);

/*----------------------------------------------------------------------
 *
 *	Initilize all channels to 0 and reset to NOT CHANGED
 */

void DMX_InitChannals(void);

/*----------------------------------------------------------------------
 *
 *	void DMX_ClrChannels(void)
 *	
 *		Set all channels which not 0 to 0 and set to CHANGED
 */

void DMX_ClrChannels(void);

/*----------------------------------------------------------------------
 *
 *	Write channel data, mark as CHANGED
 */

int DMX_SetChannelValue(int channel_nr,unsigned char value); 

/*----------------------------------------------------------------------
 *
 *	Init DMX Interface (HW and SW)
 */

void DMX_Init(); 

/*----------------------------------------------------------------------
 *
 *	Send DMX-Datapackage for 512 DMX channel 
 *
 *	This is the main DMX Output Handler!
 *	
 *	repeat: nummber of transmition repeats
 *
 */

void DMX_Transmit(int repeat);


/*** EEPROM Functions ***/

/*----------------------------------------------------------------------
 *
 *	Clear EEPROM Data
 */

void DMX_ClrEEPROM(void); 

/*----------------------------------------------------------------------
 *
 *	Copy channel buffer to EEPROM
 */

void DMX_SaveToEEPROM(void); 

/*----------------------------------------------------------------------
 *
 *	Copy EEPROM to channel buffer
 */

void DMX_ReadFromEEPROM(void); 


