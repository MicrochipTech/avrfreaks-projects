
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "emmAVR.h"
#include "LCD.h"
#include "IRremote.h"
#include "TIMER.h"
#include "MidiRead.h"
#include "SDread.h"
#include "buttons.h"

// Position in Track
uint32_t tpos     = 0;
uint32_t prevpos  = 0;

// LAST EVENT READ
uint8_t runningEvent = 0;

// TEMPO (microsec/beat)
uint32_t Tempo = 500000;
double   TempoFactor = 1.0;

// Shared variables
MTHD Header;
MTRK Track;
MTEV Event;
uint32_t nextTime = 0;

// Initialise the USART for MIDI transmission
void MIDIinit(void)
{
  UCSRA = _BV(U2X);
  UCSRB = _BV(TXEN)|_BV(RXEN);
  UBRRL = 63;  // 31250 Bauds at 16MHz
  // UBRRL = 16;  // 115200 Bauds at 16MHz
  // UBRRL = 103; //   9600 Bauds at 16MHz
}


// Send a byte to the MIDI out port (Tx)
void MidiOut( uint8_t x )
{
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = x;
}


// Read a 16 bits integer
uint16_t read16(void)
{
  uint16_t v;
  v  = (SDgetc() << 8);
  v |= SDgetc() ;
  return v;
}


// Read a 32 bits integer
uint32_t read32(void)
{
  uint32_t v;
  v  = SDgetc();
  v = ( v << 8 ) | SDgetc();
  v = ( v << 8 ) | SDgetc();
  v = ( v << 8 ) | SDgetc();
  return v;
}


// Read a byte but stops if size of the track is excessed
uint8_t readTrackByte(void)
{
  uint8_t c = 0;
  if( tpos < Track.length )
  {
    c = SDgetc();
    tpos++;
  }
  return c;
}


// Read a MIDI "variable length" integer
uint32_t readVariableLength()
{
  uint32_t v = 0;
  uint8_t c;
  c = readTrackByte();
  v = c & 0x7F;
  while( c & 0x80 )
  {
    c = readTrackByte();
    v = ( v << 7 ) | ( c & 0x7F );
  }
  return v;
}


// Read "Event.nbdata" bytes in "Event.data[]" starting at "Event.data[start]"
// Event.nbdata is not limited but we will store only "maxdata" and discard extra data
uint8_t readNdata( uint8_t start )
{
  uint32_t i;
  uint8_t c;
  for( i=start; i<Event.nbdata; i++ )
  {
    c = readTrackByte();
    if( i < maxdata ) Event.data[i] = c;
  }
  return 0;
}


// Send "All Sound Off" message to MIDI out
void allSoundOff(void)
{
  for( uint8_t i=0x00; i<=0x0F; i++ )
  {
    MidiOut( 0xB0 | i );  // command: Channel Mode Message
    MidiOut( 0x78 );      // data1:   All sounds Off : 0x78=120
    MidiOut( 0x00 );      // data2:   "0"
    MidiOut( 0xB0 | i );  // command: Channel Mode Message
    MidiOut( 0x7B );      // data1:   All Notes  Off : 0x7B=123
    MidiOut( 0x00 );      // data2:   "0"
  }
}


// Read MIDI file Header Chunk
uint8_t readHeaderChunk(void)
{
  for( int i=0; i<4; i++ ) Header.chk[i] = SDgetc();
  Header.length   = read32();
  Header.format   = read16();
  Header.ntracks  = read16();
  Header.division = read16();
  Tempo = 500000.0; // Default tempo : 500000 microsec / beat
  return ( Header.chk[0]=='M' && Header.chk[1]=='T' && Header.chk[2]=='h' && Header.chk[3]=='d' && Header.length==6 ? NoError : badFileHeader );
}


// Read MIDI file Track Chunk
uint8_t readTrackChunk(void)
{
  for( int i=0; i<4; i++ ) Track.chk[i] = SDgetc();
  Track.length  = read32();
  return ( Track.chk[0]=='M' && Track.chk[1]=='T' && Track.chk[2]=='r' && Track.chk[3]=='k' ? NoError : badTrackHeader );
}


// Print the Tempo and the Tempo Factor
void printTempo(void)
{
  LCD_line_P(2,PSTR("TEMPO"));
  LCD_move(2,6);
  LCD_print32( (uint32_t)(100.0*TempoFactor+0.4), 10 );
  LCD_putc('%');
  LCD_move(2,11);
  LCD_print32( (uint32_t)(60000000.0/Tempo*TempoFactor), 10 ); 
}


// Read MIDI file Track Event
uint8_t readTrackEvent(void)
{
  uint8_t c;
  uint32_t ms;
  uint32_t IRreceived;
  uint32_t time, buttonDelay=0;
  // Read time
  Event.wait = readVariableLength();
  // Read track event
  Event.event = readTrackByte();
  if( Event.event == 0xFF )
  {
    // Meta Event
    // read Meta event type
    Event.mtype = readTrackByte();
    // read data length
    Event.nbdata = readVariableLength();
    // read data
    readNdata(0);
    if( Event.mtype == MF_Meta_Tempo ) // Tempo
    {
      Tempo = Event.data[0] * 65536 + Event.data[1] * 256 + Event.data[2];
      printTempo(); 
   }
  }
  else if( Event.event == 0XF0 || Event.event == 0xF7 )
  {
    // SysEx Event
    Event.nbdata = 0;
    do
    {
      // read one byte
      c = readTrackByte();
      if( Event.nbdata < maxdata ) Event.data[Event.nbdata++] = c;
    } while( c != 0xF7 && tpos < Track.length );
  }
  else if( Event.event & 0x80 )
  {
    // Midi Event
    runningEvent = Event.event;
    // calculate the number of data bytes
    Event.nbdata = ( (Event.event & 0xE0) == 0xC0 ? 1 : 2 );
    // Read data bytes
    readNdata(0);
  }
  else
  {
    // Running Event
    // transfer first byte from event to data
    Event.data[0] = Event.event;
    // recall last event value
    Event.event = runningEvent;
    // calculate the number of data bytes
    Event.nbdata = ( (runningEvent & 0xE0) == 0xC0 ? 1 : 2 );
    // Read data bytes (starting from the second one since the first byte is alread in data)
    readNdata(1);
  }
  // Calculate next time on which data shall be played
  ms = ( Event.wait * (uint32_t)(Tempo / TempoFactor) ) / Header.division / 1000;
  nextTime += ms;
  // Output to MIDI device
  if(  Event.event != 0xFF )
  {
    while( millis < nextTime ) // Wait...
		{
      IRreceived = IRcode();
      time = millis;
			if( (button0 && time > buttonDelay) || IRreceived==LG_Up   ) 
      {  // Faster
        TempoFactor += 0.05;      // Faster
        buttonDelay = time + 100;
        LCD_line_P(1,PSTR("PLUS VITE..."));
        printTempo(); 
      }
			if( (button1 && time > buttonDelay) || IRreceived==LG_Down ) 
      { // Slower
        TempoFactor -= 0.05;      // Slower
        buttonDelay = time + 100;
        LCD_line_P(1,PSTR("MOINS VITE..."));
        printTempo(); 
      }
			if( IRreceived==LG_Enter ) 
      {  // Reset tempo factor
        TempoFactor = 1.0;      // Reset tempo factor
        buttonDelay = time + 100;
        LCD_line_P(1,PSTR("TEMPO NORMAL..."));
        printTempo(); 
      }
			if( button2 || IRreceived==LG_Stop ) 
      {
        allSoundOff();
        return userStop;   // Stop
      }
			if( IRreceived==LG_Pause ) 
      {
        LCD_line_P(1,PSTR("PAUSE..."));
        while( IRcode()==0 ) _delay_ms(100);
        LCD_line_P(1,PSTR("LECTURE..."));
      }
		}
    MidiOut( Event.event );
    for( uint32_t i=0; i<Event.nbdata && i<maxdata; i++ ) MidiOut( Event.data[i] );
  }
  return NoError;
}

// Read MIDI file (main part)
uint8_t readMidi(void)
{
  uint16_t i;
  uint8_t err;
  // Setup MIDI device
  MIDIinit();
  LCD_line_P(3,PSTR("  \2        \5     \6"));
	// Rewind file pointer
	SDstartFile();
  // Read File Header Chunk
  err = readHeaderChunk();
  // Read succesive Tracks
  for( i=1; i<=Header.ntracks && !err; i++ )
  {
    // Read Track Header Chunk
    err = readTrackChunk();
    // Read succesive Events
    resetTimer();
		nextTime = 0;
    for( tpos=0; tpos < Track.length && !err; ) 
		{
			err = readTrackEvent();
		}
  }
  if( err && err != userStop )
  {
    LCD_move(1,0);
    LCD_puts("MIDI Error: ");
    LCD_putc( '0' + err );
    _delay_ms(5000);
  }
  // End of MIDI file
  return err;
}
