#include 	<avr/eeprom.h>

/*----------------------------------------------------------------------
 * Definitions for the DMX512 Interface (HARDWARE)
 */
 
#define DMX_PORT PORTC
#define DMX_PORT_BIT PC0
#define DMX_PORT_ENABLE PC1
#define DMX_PORT_DDR DDRC

/* 
 * If you need the transmition of all 512 channel independent from local 
 * data, phantom channels with 0x00 data can be append. 
 * If you realy need it then define this
 */
//#define SEND_PHANTOM_CHANNELS


/*----------------------------------------------------------------------
 *	Channel Data Buffer
 *
 *	Memory to hold current channel data which are transmitted
 *	via DMX interface. Default 32 channels are applied.
 *	A complete set af channel data (data anly) is a "set". Dependent from the nr
 *	of channels a number of sets can be stored in EEPROM.
 *	Default: 512 Bytes EEPROM / 32 Channel-Data (1 Byte per channel) -> 16 Sets
 *	can be stored in EEPROM (Set Nr 0 to 15).
 *	At startup set Nr 0 is used an copied to the channel buffer.
 */

// Define Nr of channels you need
// One should keep in mind that for each channel 2 bytes of RAM is occupied!
#define DMX_CHANNELCOUNT	32

// Calculate nr of sets
#define NR_OF_DMX_SETS (512 / DMX_CHANNELCOUNT)

// Channel structure, singel channel
struct channel
{
	unsigned char data;
	enum { NO , YES} changed;
};

// Channel buffer for nr of channels. Not a set, because in a set only
// channel data (1 byte per channel) is stored
static struct channel channel_buf[DMX_CHANNELCOUNT];

/*----------------------------------------------------------------------
 *	EEPROM to save sets of channel data
 */

struct channel_eeprom
{
	struct channel_set
	{
		unsigned char ch_data[DMX_CHANNELCOUNT];

	} set[NR_OF_DMX_SETS];

} channel_eeprom EEMEM;

int DMX_ActivChannelSet = 0; // Active channel data set
	
/*----------------------------------------------------------------------
 *
 *	int DMX_GetNrSets(void)
 */

int DMX_GetNrSets(void) { return NR_OF_DMX_SETS; }

/*----------------------------------------------------------------------
 *
 *	int DMX_SetChSet(int NewSetNr)
 *
 *		ChannelSet selection
 */

int DMX_SetChSet(int NewSetNr)
{
	if((NewSetNr >= 0) | (NewSetNr < NR_OF_DMX_SETS)) {
		DMX_ActivChannelSet = NewSetNr;
		return 0;
	} else
		return -1;
}

/*----------------------------------------------------------------------
 *
 *	int DMX_GetActivChSetNr(void)
 *
 *		Return activ ChannelSet number (index)
 */

int DMX_GetActivChSetNr(void) { return DMX_ActivChannelSet; }

/*----------------------------------------------------------------------
 *
 *	unsigned char DMX_GetChannelData(int channel_nr)
 *
 *		Read a single channel data from the internal buffer
 */

unsigned char DMX_GetChannelData(int channel_nr)
{ 
	return channel_buf[channel_nr].data;
}

/*----------------------------------------------------------------------
 *
 *	int DMX_GetChannelCount(void)
 *
 *		Return number of buffered channels
 */

int DMX_GetChannelCount(void) {	return DMX_CHANNELCOUNT; }

/*----------------------------------------------------------------------
 *
 *	int DMX_IsChannelChanged(int channel_nr)
 *
 *		Check if channel data has changed
 */

int DMX_IsChannelChanged(int channel_nr) { return (channel_buf[channel_nr].changed == YES); }

/*----------------------------------------------------------------------
 *
 *	void DMX_InitChannels(void)
 *	
 *		Initilize all channels to 0 and reset to NOT CHANGED
 */

void DMX_InitChannels(void)
{
 	for(register short i=0; i < DMX_CHANNELCOUNT; i++)
	{	
		channel_buf[i].data = 0; channel_buf[i].changed = NO;
	}
}

/*----------------------------------------------------------------------
 *
 *	void DMX_ClrChannels(void)
 *	
 *		Set all channels which not 0 to 0 and mark as CHANGED
 */

void DMX_ClrChannels(void)
{
 	for(register short i=0; i < DMX_CHANNELCOUNT; i++)
	{	
		if(channel_buf[i].data != 0)
			channel_buf[i].data = 0; channel_buf[i].changed = YES;
	}
}

/*----------------------------------------------------------------------
 *
 *	Set channel data and mark as CHANGED
 */

int DMX_SetChannelValue(int channel_nr,unsigned char value)
{
	if(channel_nr < DMX_CHANNELCOUNT) {
		channel_buf[channel_nr].data = value;
		channel_buf[channel_nr].changed = YES;
		return 0;
	}
	return -1;
}

/*** EEPROM Functions ***/

/*----------------------------------------------------------------------
 *
 *	Clear EEPROM Data
 */

void DMX_ClrEEPROM(void)
{
	for(int i = 0; i < NR_OF_DMX_SETS; i++)
		for(int j = 0; j < DMX_CHANNELCOUNT; j++)
			channel_eeprom.set[i].ch_data[j] = 0;
}

/*----------------------------------------------------------------------
 *
 *	Copy channel buffer to EEPROM
 */

void DMX_SaveToEEPROM()
{
	for(int j = 0; j < DMX_CHANNELCOUNT; j++) {
		eeprom_write_byte(&(channel_eeprom.set[DMX_ActivChannelSet].ch_data[j]), channel_buf[j].data);
		channel_buf[j].changed = NO;
	}
}

/*----------------------------------------------------------------------
 *
 *	Copy EEPROM to channel buffer
 */

void DMX_ReadFromEEPROM()
{
	for(int j = 0; j < DMX_CHANNELCOUNT; j++) {
		channel_buf[j].data = eeprom_read_byte(&(channel_eeprom.set[DMX_ActivChannelSet].ch_data[j]));
		channel_buf[j].changed = NO;
	}
}

/*** DMX Output Handler ***/

/*----------------------------------------------------------------------
 *
 *	Initialize DMX Interface (HW und SW)
 */

void DMX_Init()
{
	// Set DMX-Port as Output
	DMX_PORT_DDR |= _BV(DMX_PORT_BIT);
	DMX_PORT_DDR |= _BV(DMX_PORT_ENABLE);
	// Default auf MARK setzen
	DMX_PORT = (DMX_PORT | _BV(DMX_PORT_BIT));
	// Treiber freischalten
	DMX_PORT = (DMX_PORT | _BV(DMX_PORT_ENABLE));

	// Init Channel-Buffer
	DMX_InitChannels();
}

/*----------------------------------------------------------------------
 *	Send DMX-Frame
 *  
 *	Figures in () correspond to the numbering used in DIN 56930-2 
 *	A frane (3) is composed of
 *	- (4) Startbit t4 = 4 us
 *	- (5)-(6) 8 Data bits pased by the argument "frame_data", bit order  
 *	          LSB ... HSB. t5-t6 = 8 * 4us = 32us
 *	- (7) 2 Stopbit t7 = 2 * 4us = 8us
 *
 *	t3 = 4us + 32 us + 8us = 44us
 *
 *	At frame end the DMV output is hold "1" (MARK)
 *	Optional a wait time <wait_after_us> in us can be added.
 */

void DMX_Send_Frame(unsigned char frame_data, unsigned short wait_after_us)
{
	register unsigned char filter;

	// Start frame (3) t3 = 44us composed of
	// Startbit, logical "0" (SPACE), t1 = 4 us

	DMX_PORT = (DMX_PORT & ~_BV(DMX_PORT_BIT));

	asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP"); 
	asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP"); // asm ("NOP"); 

	for( register short j=0; j < 6; j++) {
 		asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP"); }

	// Transmition of 8 Bit data started with LSB (5) and end with MSB (6)

	// Bit filter on LSB
	filter = 0x01;

	// Repeat for each Bit
	for( register short i = 0; i < 8; i++)
	{
		// If bit set
		if(frame_data & filter) {
			asm ("NOP"); asm ("NOP"); asm ("NOP"); // asm ("NOP");
			DMX_PORT = (DMX_PORT | _BV(DMX_PORT_BIT)); // DMX output High
		} else {
			asm ("NOP"); asm ("NOP");
			DMX_PORT = (DMX_PORT & ~_BV(DMX_PORT_BIT)); // DMX output Low
		}

		// Shift filter to next bit
		filter = filter << 1;

		// Adjust bit time to 4 us
		asm ("NOP"); asm ("NOP");  asm ("NOP"); // asm ("NOP"); asm ("NOP");
		for( register short j=0; j < 6; j++) {
 				asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP"); }
	}

	// Adjust time
	asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP");

	// (7) Stopbit, logical "1" (MARK), t1 = 2x4 us

	DMX_PORT = (DMX_PORT | _BV(DMX_PORT_BIT));

	// Adjust time
	asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP");

	for( register short j=0; j < 15; j++) {
 			asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP");	}

	// End of frame time (3) t3 = 44us

	// Wait <wait_after_us> if requested
	if(wait_after_us)
		for( register short j = 0; j < (wait_after_us << 1); j++) {
			asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP"); } ;

}

/*----------------------------------------------------------------------
 *	Send DMX-Package for 512 channel max
 *	
 *	repeat: Repeat transmition <repeat> times
 *
 *	At the end the output is set to logical "0" (SPACE) what is
 *	equivalent to DMX-RESET
 */

void DMX_Transmit(int repeat)
{ 
	for(int i = 0; i < repeat; i++)
	{
		// (1) Hold DMX-Reset, logical "0" (SPACE), t1 >= 88us

		DMX_PORT = (DMX_PORT & ~_BV(DMX_PORT_BIT));

		for( register short j=0; j < 176; j++) // Original 176 for 88 us
		{
 			asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP");
		}
	
		// (2) Time between RESET ans Startcode, logical "1" (MARK)
		// t2 = 8us - 1s

		DMX_PORT = (DMX_PORT | _BV(DMX_PORT_BIT));

		for( register short j=0; j < 150; j++) // Original 15 für 8 us
		{
 			asm ("NOP"); asm ("NOP"); asm ("NOP"); asm ("NOP");
		}

		// Send channel		
		for( register short j=0; j < DMX_CHANNELCOUNT; j++) // Aktive Kanäle senden
		{
			DMX_Send_Frame(DMX_GetChannelData(j),0); // Ch0 - DMX_CHANNELCOUNT
		}			

#ifdef SEND_PHANTOM_CHANNELS
		// If all 512 channel needed send phantom channels with no real data		
		for( register short j=0; j < 512-DMX_CHANNELCOUNT; j++)
		{
			DMX_Send_Frame(0x00,0); // bis Channel 512
		}
#endif			

		// Go to Reset, logical "0" (SPACE)
		DMX_PORT = (DMX_PORT & ~_BV(DMX_PORT_BIT));
	}
}
