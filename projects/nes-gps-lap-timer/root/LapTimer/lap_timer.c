/* gps lap timer */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "lap_timer.h"

/* HD44780 LCD connects via top half of PORT B and PORT D */
#define SelectLCD()    PORTD &= ~_BV(PD1)
#define DeselectLCD()  PORTD |=  _BV(PD1)

#define LCDDataReg()   PORTD |=  _BV(PD0)
#define LCDCmdReg()    PORTD &= ~_BV(PD0)

static const prog_uchar LCDBigNumPatterns [];
static const prog_char HeadingText [];

#define MAX_LAPS 100

#define MAX_SENTENCE_LENGTH 80
#define N_BUFFERS 3

#define BUFFER_SIZE ((MAX_SENTENCE_LENGTH) * (N_BUFFERS))

char SentenceBuffer[BUFFER_SIZE];
volatile int8_t VTGSentence = -1, GGASentence = -1;

volatile const prog_char *TxPtr;

struct _LapData LapTimes[MAX_LAPS], EEMEM SavedLapTimes[MAX_LAPS];
uint8_t NoLaps = 0;
uint8_t EEMEM SavedNoLaps = 0;

struct _NavData
{
	float Lat;
	float Lon;
	float Heading;
	float Time;
	float Speed;
} NavData[2], HomePos, EEMEM SavedHomePos;
uint8_t TopNav = 0;

uint8_t Tenths;
uint8_t NoHeadingFlag = 1;

float TopSpeed = 0.0, EEMEM SavedTopSpeed;
float TopSpeedTime = 0.0, EEMEM SavedTopSpeedTime;

float CumulativeDistance = 0.0, EEMEM SavedDistance;
float CumulativeStoppedTime = 0.0, EEMEM SavedStoppedTime;
float CumulativeRunTime = 0.0, EEMEM SavedRunTime;


void SaveScores (int8_t OnlyLatest)
{
	eeprom_write_byte (&SavedNoLaps, NoLaps);
	if (OnlyLatest)
	{
		eeprom_write_block (LapTimes + NoLaps - 1, SavedLapTimes + NoLaps - 1,
							sizeof(struct _LapData));
	}
	else
	{
		eeprom_write_block (LapTimes, SavedLapTimes, 100 * sizeof(struct _LapData));
		eeprom_update_block (&HomePos, &SavedHomePos, sizeof(struct _NavData));
	}
	eeprom_update_float (&SavedTopSpeed, TopSpeed);
	eeprom_update_float (&SavedTopSpeedTime, TopSpeedTime);
	eeprom_update_float (&SavedDistance, CumulativeDistance);
	eeprom_update_float (&SavedStoppedTime, CumulativeStoppedTime);
	eeprom_update_float (&SavedRunTime, CumulativeRunTime);
}

void LoadScores (void)
{
	NoLaps = eeprom_read_byte (&SavedNoLaps);
	if (NoLaps > 1 && NoLaps <= MAX_LAPS)
	{
		eeprom_read_block (LapTimes, SavedLapTimes, 100 * sizeof(struct _LapData));
		TopSpeed = eeprom_read_float (&SavedTopSpeed);
		TopSpeedTime = eeprom_read_float (&SavedTopSpeedTime);
		CumulativeDistance = eeprom_read_float (&SavedDistance);
		CumulativeStoppedTime = eeprom_read_float (&SavedStoppedTime);
		CumulativeRunTime = eeprom_read_float (&SavedRunTime);
		eeprom_read_block (&HomePos, &SavedHomePos, sizeof(struct _NavData));
	}
	else
		NoLaps = 0;
}

void ResetScores (void)
{
	NoLaps = 0;
	TopSpeed = 0.0;
	TopSpeedTime = 0.0;
	CumulativeDistance = 0.0;
	CumulativeStoppedTime = 0.0;
	CumulativeRunTime = 0.0;
}

static const prog_char ReceiverInit [] =
	"$PSRF103,00,00,01,01*25\r\n"	/* GGA updates per second */
	//"$PSRF103,00,00,00,01*24\r\n"	/* GGA off */
	"$PSRF103,01,00,00,01*25\r\n"	/* GLL off */
	"$PSRF103,02,00,00,01*26\r\n"	/* GSA off */
	"$PSRF103,03,00,00,01*27\r\n"	/* GSV off */
	"$PSRF103,04,00,00,01*20\r\n"	/* RMC off */
	"$PSRF103,05,00,01,01*20\r\n";	/* VTG updates per second */

void LCDByte (uint8_t Data)
{
	PORTB = Data & 0xF0;
	SelectLCD();
	DeselectLCD();
	PORTB = Data << 4;
	SelectLCD();
	DeselectLCD();
	_delay_us (40);
}

void LCDCommand (uint8_t Command)
{
	LCDCmdReg();
	LCDByte (Command);
	if (Command && (Command < 0x04))
		_delay_us (1600);
}

void LCDData (uint8_t Data)
{
	LCDDataReg();
	LCDByte (Data);
}

volatile uint16_t Debug = 0;

char PrintBuffer[64];

static const prog_char PatternVTG [] = "VTG";
static const prog_char PatternGGA [] = "GGA";
ISR(USART1_RX_vect)
{
	static uint8_t Index = 0, Start = N_BUFFERS;
	static uint16_t TimerOffset = 0;
	//static char *Buffer = NULL;
	char c;
	
	/*if (SentenceFIFO.Head != SentenceFIFO.Tail - 1)
	{
		if ((SentenceFIFO.Buffer [SentenceFIFO.Head++] = UDR1) == '\r')
			SentenceFIFO.NewSentence++;
	}*/

	c = UDR1;
	//Debug++;
	if (c == '$')
	{
		TimerOffset = TCNT1;
		for (Start = 0; Start < N_BUFFERS; Start++)
		{
			if ((Start != VTGSentence) && (Start != GGASentence))
			{
				Index = (uint8_t)Start * 80;
				break;
			}
		}
	}
	if (Start < N_BUFFERS)
	{
		if (c == '\r')
		{
			SentenceBuffer[Index] = 0;
			if ((VTGSentence < 0) &&
				!memcmp_P (SentenceBuffer + Start * 80 + 3, PatternVTG, 3))
			{
				VTGSentence = Start;
			}
			else if ((GGASentence < 0) &&
				!memcmp_P (SentenceBuffer + Start * 80 + 3, PatternGGA, 3))
			{
				/* timestamp for position fix needs to be referenced */
				TCNT1 = -3125 + TimerOffset;
				Tenths = 0;
				TIMSK1 |= _BV(TOIE1);
				GGASentence = Start;
			}
			Start = N_BUFFERS;
		}
		else
			SentenceBuffer[Index++] = c;
	}
}

ISR(USART1_TX_vect)
{
	char c;

	c = pgm_read_byte (TxPtr);
	if (c)
	{
		UDR1 = c;
		TxPtr++;
		Debug++;
	}
}

ISR(TIMER1_OVF_vect)
{
	TCNT1 = -3125;	/* count for 1/10th second */
	Tenths++;
}

void LCDClear (void)
{
	LCDCommand (0x01);
}

uint8_t LCDAddress (uint8_t X, uint8_t Y)
{
	switch (Y)
	{
	default:
	case 0:
		return (0x80 | X);
	case 1:
		return (0xc0 | X);
	case 2:
		return (0x90 | X);
	case 3:
		return (0xd0 | X);
	}
}

void CursorOn (void)
{
	LCDCommand (0x0f);
}

void CursorOff (void)
{
	LCDCommand (0x0c);
}

void GPSSendString_P (const prog_char *Ptr)
{
	UDR1 = pgm_read_byte (Ptr);
	TxPtr = Ptr + 1;
}

void LCDHex (uint8_t x)
{
	x >>= 4;
	if (x < 10)
		LCDData (x + '0');
	else
		LCDData (x - 10 + 'a');
}

void LCDString_P (const prog_char *String)
{
	char c;
	
	c = pgm_read_byte (String++);
	do
	{
		if (c == '\0')
			break;
		LCDData ((uint8_t)c);
		c = pgm_read_byte (String++);
	} while (1);
}

void LCDString (char *String)
{
	char c;
	
	c = *(String++);
	do
	{
		if (c == '\0')
			break;
		LCDData ((uint8_t)c);
		c = *(String++);
	} while (1);
}

void LCDBigNum (uint8_t n, uint8_t a)
{
	char i, j, v;
	volatile const prog_uchar *Pattern;
	static uint8_t Prev = 0xee;
	
	if (a & 0x80)
	{
		/* redraw the characters if the address is valid */
		LCDCommand (a);
		LCDData (4);
		LCDData (6);
		LCDData (0);
		LCDData (2);
		LCDCommand (a + 0x40);
		LCDData (5);
		LCDData (7);
		LCDData (1);
		LCDData (3);
	}
	
	/* redefine the characters */
	LCDCommand (0x40);
	v = n & 0x0f;
	for (i = 0; i < 2; i++)
	{
		if (v != (Prev & 0x0f))
		{
			if (v > 9)
			{
				/* blank character */
				for (j = 0; j < 32; j++)
					LCDData (0);
			}
			else
			{
				Pattern = LCDBigNumPatterns + (32 * v);
				/* poke pattern data */
				for (j = 0; j < 32; j++)
					LCDData (pgm_read_byte (Pattern++));
			}
		}
		else
		{
			/* skip redefining the character if it hasn't changed */
			LCDCommand (0x40 + 32);
		}
		v = n >> 4;		
		Prev >>= 4;
	}
	Prev = n;
}

const char *NthWord (const char *Ptr, int n)
{
	for (; n > 0; n--)
	{
		do
		{
			if (*Ptr == '\0')
				return Ptr;
		} while (*(Ptr++) != ',');
	}
	return Ptr;
}

float WordToTime (const char *Ptr)
{
	float Time;
	uint32_t Tmp, Mag;
	
	if (Ptr[0] == ',' || Ptr[0] == '\0')
		return 0.0;
	
	Tmp = ((Ptr[0] - '0') * 36000) +
	      ((Ptr[1] - '0') * 3600) +
	      ((Ptr[2] - '0') * 600) +
	      ((Ptr[3] - '0') * 60) +
	      ((Ptr[4] - '0') * 10) +
	      ((Ptr[5] - '0') * 1);
	Time = (float)Tmp;
	Ptr += 6;
	if (*Ptr == '.')
	{
		Tmp = 0;
		Mag = 1;
		while (*(++Ptr) != ',')
		{
			Tmp *= 10;
			Tmp += *Ptr - '0';
			Mag *= 10;
		}
		Time += (float)Tmp / (float)Mag;
	}
	return Time;
}

float WordToRadians (const char *Ptr)
{
	float Angle;
	int Tmp, Mag;
	
	if (Ptr[0] == ',' || Ptr[0] == '\0')
		return 0.0;
	
	Ptr = strchr (Ptr, '.');
	if (!Ptr) Ptr = strchr (Ptr, ',');
	Tmp = ((Ptr[-4] - '0') * 600) +
	      ((Ptr[-3] - '0') * 60) +
	      ((Ptr[-2] - '0') * 10) +
	      ((Ptr[-1] - '0') * 1);
	if (isdigit (Ptr[-5]))
		Tmp += (Ptr [-5] - '0') * 6000;
	Angle = (float)Tmp;
	if (*Ptr == '.')
	{
		Tmp = 0;
		Mag = 1;
		while (*(++Ptr) != ',')
		{
			Tmp *= 10;
			Tmp += *Ptr - '0';
			Mag *= 10;
		}
		Angle += (float)Tmp / (float)Mag;
	}
	if (Ptr[1] == 'S' || Ptr[1] == 'W')
		Angle = -Angle;
	/* convert from minutes to radians */
	Angle *= .0002908882087;
	return Angle;
}

float WordToFloat (const char *Ptr)
{
	float Val;
	int Tmp, Mag;

	Tmp = 0;
	while (isdigit (Ptr[0]))
	{
		Tmp *= 10;
		Tmp += Ptr[0] - '0';
		Ptr++;
	}
	Val = (float)Tmp;
	if (Ptr[0] == '.')
	{
		Tmp = 0;
		Mag = 1;
		while (isdigit ((++Ptr)[0]))
		{
			Tmp *= 10;
			Tmp += Ptr[0] - '0';
			Mag *= 10;
		}
		Val += (float)Tmp / (float)Mag;
	}
	return Val;
}

void Init (void)
{
	/* select external clock */
	CLKSEL1 |= 0x0f; /* low power crystal osc 8-16MHz */
	CLKSEL0 |= _BV(EXTE);
	while (!(CLKSTA & _BV(EXTON)));
	CLKSEL0 |= _BV(CLKS);
	CLKSEL0 &= ~(_BV(RCE));
	CLKPR = _BV(CLKPCE);
	CLKPR = 0; /* prescale cSentenceInBuffer by 1 */

	/* set up serial port */
	UBRR1 = 12; /* 38k baud @ 8MHz */
	UCSR1A = 0;
	UCSR1B = _BV(RXEN1) | _BV(TXEN1) | _BV(TXCIE1); /* Rx, Tx interrupt enable */
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); /* n-8-1 */

	/* set up timer 1 (31250 counts per second) */
	TCCR1B = _BV(CS12);

	/* intialise display */
	DeselectLCD ();
	
	DDRD |= _BV(PD1) | _BV(PD0);
	DDRB |= _BV(PB4) | _BV(PB5) | _BV(PB6) | _BV(PB7);
	
	_delay_ms (20); /* Wait 20ms for LCD to power up */
	
	/* get into 4 bit mode */
	LCDCmdReg();
	PORTB = 0x3 << 4;
	SelectLCD();
	DeselectLCD();
	_delay_ms (5);
	SelectLCD();
	DeselectLCD();
	_delay_us (200);
	SelectLCD();
	DeselectLCD();
	_delay_us (200);
	PORTB = 0x2 << 4;
	SelectLCD();
	DeselectLCD();
	_delay_ms (5);
	
	LCDCommand (0x28);
	LCDClear ();
	CursorOff ();
	
	/* pull ups for the two buttons */
	PORTD |= _BV(PD5);
	PORTD |= _BV(PD7);
	
	sei();
}


void InitUI (void)
{
	uint8_t i;
	
	LCDString_P (PSTR("Data for "));
	LoadScores ();
	sprintf_P (PrintBuffer, PSTR("%2d"), (NoLaps && NoLaps <= MAX_LAPS)? NoLaps - 1: 0);
	LCDString (PrintBuffer);
	LCDCommand (LCDAddress (0, 1));
	LCDString_P (PSTR("laps retrieved."));
	
	/* wait for a byte to appear over the uart */
	while (!(UCSR1A & _BV(RXC1)))
		ServiceUSB ();
	
	/* enable receiver ISR */
	UCSR1B |= _BV(RXCIE1);
	
	/* transmit our config string, enabling VTG messages */
	GPSSendString_P (ReceiverInit);
	
	if (NoLaps > 1 && NoLaps <= MAX_LAPS)
	{
		/* give the user a few seconds to clear the lap times */
		LCDCommand (LCDAddress (0, 2));
		LCDString_P (PSTR("BLACK:"));	
		LCDCommand (LCDAddress (1, 3));
		LCDString_P (PSTR("clear memory-->"));
		
		for (i = 0; i < 500; i++)
		{
			if (!(PIND & _BV(PD5)))
			{
				/* black button pressed */
				
				while (!(PIND & _BV(PD5)));
				
				ResetScores ();
				SaveScores (0);
				break;
			}
			_delay_ms (10);
		}
	}
	StopUSB ();
}

void ParseNavData (void)
{
	const char *Ptr;
	struct _NavData *PtrNav;
	
	TopNav ^= 1;
	PtrNav = NavData + TopNav;
	
	/*
	 * Parse VTG sentence for heading
	 */
	Ptr = SentenceBuffer + VTGSentence * 80;
	PtrNav->Heading = WordToFloat (NthWord (Ptr, 1));
	
	/*
	 * Parse VTG sentence for speed
	 */		
	PtrNav->Speed = WordToFloat (NthWord (Ptr, 7));
	
	/*
	 * Parse GGA sentence for position & time
	 */
	Ptr = SentenceBuffer + GGASentence * 80;
	PtrNav->Lat  = WordToRadians (NthWord (Ptr, 2));
	PtrNav->Lon  = WordToRadians (NthWord (Ptr, 4));
	PtrNav->Time = WordToTime (NthWord (Ptr, 1));
	
	if (PtrNav->Speed > TopSpeed)
	{
		TopSpeed = PtrNav->Speed;
		TopSpeedTime = PtrNav->Time;
	}
}

float Haversine (struct _NavData *A, struct _NavData *B)
{
	float dLat, dLon, a;

	dLat = (B->Lat - A->Lat) / 2.0;
	dLon = (B->Lon - A->Lon) / 2.0;

	a = sin (dLat) * sin (dLat) +
		sin (dLon) * sin (dLon) * cos(B->Lat) * cos(A->Lat);

	return 2.0 * atan2(sqrt(a), sqrt(1.0 - a)) * 6371009.0;
}

void LCDCurrentTime (void)
{
	uint8_t i = 0;
	if (GGASentence >= 0)
	{
		const char *Ptr;
		Ptr = NthWord (SentenceBuffer + GGASentence * 80, 1);
		for (; i < 6 && isdigit (*Ptr); i++)
			LCDData (*(Ptr++));
	}
	else
	{
		for (; i < 6; i++)
			LCDData ('?');
	}
}


void BeginMode (void)
{
	float Heading = 0.0;

	LCDClear ();

	LCDCommand (LCDAddress (0, 0));
	LCDString_P (PSTR("time:           "));
	LCDCommand (LCDAddress (0, 3));
	LCDString_P (PSTR("  ** NO LOCK ** "));
	
	do
	{
		if (VTGSentence >= 0 && GGASentence >= 0)
		{
			LCDCommand (LCDAddress (10, 0));
			LCDCurrentTime ();

			ParseNavData ();
			VTGSentence = -1;
			GGASentence = -1;			
		}

	} while (NavData[TopNav].Heading == 0);
	
	LCDCommand (LCDAddress (0, 3));
	LCDString_P (PSTR("RED: set line-->"));
	
	while (1)
	{
		if (VTGSentence >= 0 && GGASentence >= 0)
		{
			LCDCommand (LCDAddress (10, 0));
			LCDCurrentTime ();

			ParseNavData ();
				
			if (NavData[TopNav].Speed > 2.0)
			{
				LCDCommand (LCDAddress (0, 1));
				LCDString_P (PSTR("speed:       kph"));
				LCDCommand (LCDAddress (9, 1));
				sprintf_P (PrintBuffer, PSTR("% 2.0f"), (double)NavData[TopNav].Speed);
				LCDString (PrintBuffer);
				Heading = NavData[TopNav].Heading;
			}
			else
			{
				LCDCommand (LCDAddress (0, 1));
				LCDString_P (PSTR("heading:        "));
				LCDCommand (LCDAddress (13, 1));
				//sprintf_P (PrintBuffer, PSTR("% 3.0f"), (double)Heading);
				LCDString_P (HeadingText + (((int)Heading + 13) * 2 / 45) * 4);
			}
		
			VTGSentence = -1;
			GGASentence = -1;
		}
	
		if (!(PIND & _BV(PD7)))
		{
			/* red button pressed */
			HomePos = NavData[TopNav];

			/* if we are not moving, heading thru start point still
			 * needs to be worked out */
			NoHeadingFlag = !(NavData[TopNav].Speed > 2.0);
			
			if (NoLaps == 0)
			{
				LapTimes[0].Time = NavData[TopNav].Time + ((float)Tenths / 10);
				NoLaps++;
			}
			break;
		}
	}
}

void FormatElapsed (float D)
{
	int T;
	
	D += 0.05;
	if (D < -1.0) D += (24.0 * 60.0 * 60.0);
	T = (int)D;
	sprintf_P (PrintBuffer, PSTR("%2d:%02d.%d"), T / 60, T % 60, (int)((D - (float)T) * (float)10.0));
}

void LCDLatestLapTimes (char Flag)
{
	int8_t i;

	if (1)//Flag)
	{
		for (i = -1; i > -4; i--)
		{
			if ((NoLaps + i) <= 0)
				break;
			
			FormatElapsed (LapTimes[NoLaps + i].Time - LapTimes[NoLaps + i - 1].Time);
			LCDCommand (LCDAddress (0, 3 + i));
			LCDString (PrintBuffer);
		}
	}
	
	FormatElapsed (NavData[TopNav].Time + ((float)Tenths / 10) - LapTimes[NoLaps - 1].Time);
	LCDCommand (LCDAddress (0, 3)); 
	LCDString (PrintBuffer);
}

void RunMode (void)
{
	float SegDistance, StartTime, D, PrevHomeD;
	char GettingCloser = 0;

	LCDClear ();
	LCDCommand (LCDAddress (13, 0));
	LCDString_P (PSTR("kph"));
	LCDBigNum (0xf0, LCDAddress (9, 0));
	LCDCommand (LCDAddress (13, 1));
	
	SegDistance = Haversine (&NavData[TopNav ^ 1], &NavData[TopNav]);
	StartTime = NavData[TopNav].Time;
	
	PrevHomeD = Haversine (&HomePos, &NavData[TopNav]);
	
	while (1)
	{
		if (VTGSentence >= 0 && GGASentence >= 0)
		{
			float L;
			
			ParseNavData ();
			L = Haversine (&NavData[TopNav ^ 1], &NavData[TopNav]);
			SegDistance += L;
			
			LCDCommand (LCDAddress (9, 2));
			sprintf_P (PrintBuffer, PSTR("%4.1fkm"), (double)(SegDistance / (float)1000.0));
			LCDString (PrintBuffer);
			
			sprintf_P (PrintBuffer, PSTR("%5.1f"), (double)NavData[TopNav].Speed);
			LCDCommand (LCDAddress (8, 0));
			LCDData (PrintBuffer[0]);
			LCDCommand (LCDAddress (13, 1));
			LCDData (PrintBuffer[3]);
			LCDData (PrintBuffer[4]);
			LCDData (pgm_read_byte (PSTR("|\xa5-/|\xa5-/|") + 
				(((int)NavData[TopNav].Heading + 22) / 45)));
			if (isdigit (PrintBuffer[1]))
				LCDBigNum (((PrintBuffer[1] - '0') << 4) | (PrintBuffer[2] - '0'), 0);
			else
				LCDBigNum ((0xf0 | (PrintBuffer[2] - '0')), 0);
			
			D = Haversine (&HomePos, &NavData[TopNav]);
			if (NoHeadingFlag)
			{
				/* don't yet know what direction we pass the
				 * start finish line */	
				if (D < PrevHomeD)
				{
					PrevHomeD = D;
				}
				else
				{
					HomePos.Heading = NavData[TopNav ^ 1].Heading;
					NoHeadingFlag = 0;
				}
			}
			else
			{
				float DH;
				
				GettingCloser = (GettingCloser << 1) | ((D < PrevHomeD)? 1: 0);

				DH = NavData[TopNav].Heading - HomePos.Heading;
				if (DH > 180.0)
					DH -= 360.0;

				if (((GettingCloser & 0x03) == 0x02) &&
					(PrevHomeD < 20.0) && (DH > -30.0 && DH < 30.0) &&
					((NavData[TopNav].Time - LapTimes[NoLaps].Time) > 10.0))
				{
					/* got within 20 metres of the start point, and
					 * heading in the right direction. Call this a lap */
										
					LapTimes[NoLaps++].Time = NavData[TopNav].Time +
						(PrevHomeD * PrevHomeD - D * D - L * L) / (2.0 * L * L);
					
					LCDLatestLapTimes (1);
					
					SaveScores (NoLaps > 2);
				}
				PrevHomeD = D;
			}
			
			if (NavData[TopNav].Speed < 2)
			{
				/* stationary */
				
				CumulativeDistance += SegDistance;
				CumulativeRunTime += NavData[TopNav].Time +
					((float)Tenths / 10) - StartTime;
				break;
			}

			VTGSentence = -1;
			GGASentence = -1;
		}

		LCDLatestLapTimes (0);
				
		FormatElapsed (NavData[TopNav].Time + ((float)Tenths / 10) - StartTime);
		LCDCommand (LCDAddress (9, 3));
		LCDString (PrintBuffer);
	}
}

void StoppedMode (void)
{
	struct _NavData StopPos;
	float D;
	unsigned T;
	
	StopPos = NavData[TopNav];
	StopPos.Time += ((float)Tenths / 10);
	
	LCDClear ();
	
	LCDLatestLapTimes (1);
	LCDCommand (LCDAddress (9, 0));
	sprintf_P (PrintBuffer, PSTR("% 2d laps"), NoLaps - 1);
	LCDString (PrintBuffer);
	//LCDString_P (PSTR("top"));
	LCDCommand (LCDAddress (9, 1));
	sprintf_P (PrintBuffer, PSTR("%4.1fkph"), (double)(TopSpeed));
	LCDString (PrintBuffer);
	LCDCommand (LCDAddress (9, 2));
	sprintf_P (PrintBuffer, PSTR("%4.1fkm"), (double)(CumulativeDistance / 1000));
	LCDString (PrintBuffer);
	
	SaveScores (NoLaps > 2);
	
	while (1)
	{
		if (VTGSentence >= 0 && GGASentence >= 0)
		{
			ParseNavData ();
			
			if (Haversine (&StopPos, &NavData[TopNav]) > 3)
			{
				/* moving again */
				CumulativeStoppedTime += NavData[TopNav].Time +
					((float)Tenths / 10) - StopPos.Time;
				break;
			} 
			
			VTGSentence = -1;
			GGASentence = -1;			
		}		
	
		
		D = NavData[TopNav].Time + ((float)Tenths / 10) -
			StopPos.Time;
		if (D < 0.0) D += (24.0 * 60.0 * 60.0);
		T = (unsigned)D;
		sprintf_P (PrintBuffer, PSTR("%2d:%02d.%d"), T / 60, T % 60,
			(int)((D - (float)T) * (float)10.0));
			//Tenths);
		LCDCommand (LCDAddress (9, 3));
		LCDString (PrintBuffer);
	}
}

int main (void)
{	
	Init ();
	InitUSB ();
	
	InitUI ();

	BeginMode ();	
	while (1)
	{
		RunMode ();
		StoppedMode ();
	}
	
	return 0;
}

static const prog_char HeadingText [] =
	"  N\0NNE\0 NE\0ENE\0  E\0ESE\0 SE\0SSE\0"
	"  S\0SSW\0 SW\0WSW\0  W\0WNW\0 NW\0NNW\0  N";

static const prog_uchar LCDBigNumPatterns [] =
{
	/* 0 */
	0x07, 0x0f, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0f, 0x07,
	0x1c, 0x1e, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x1c,
	/* 1 */
	0x00, 0x01, 0x03, 0x06, 0x0c, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	/* 2 */
	0x07, 0x0f, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00,
	0x07, 0x0f, 0x18, 0x18, 0x18, 0x18, 0x1f, 0x1f,
	0x1c, 0x1e, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x1c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f,
	/* 3 */
	0x07, 0x0f, 0x18, 0x18, 0x18, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x0f, 0x07,
	0x1c, 0x1e, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x1e,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x1c,
	/* 4 */
	0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x02, 0x06,
	0x0c, 0x18, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00,
	0x1c, 0x1c, 0x1c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
	0x0c, 0x0c, 0x1f, 0x1f, 0x0c, 0x0c, 0x0c, 0x0c,
	/* 5 */
	0x1f, 0x1f, 0x18, 0x18, 0x18, 0x1b, 0x1f, 0x1c,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x1f, 0x07,
	0x1e, 0x1e, 0x00, 0x00, 0x00, 0x18, 0x1e, 0x06,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x1c,
	/* 6 */
	0x07, 0x0f, 0x18, 0x18, 0x18, 0x18, 0x1b, 0x1f,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0f, 0x07,
	0x1c, 0x1e, 0x03, 0x03, 0x00, 0x00, 0x18, 0x1e,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x1c,
	/* 7 */
	0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x06, 0x06,
	0x1f, 0x1f, 0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c,
	0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* 8 */
	0x07, 0x0f, 0x18, 0x18, 0x18, 0x18, 0x0c, 0x0f,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0f, 0x07,
	0x1c, 0x1e, 0x03, 0x03, 0x03, 0x03, 0x06, 0x1e,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x1c,
	/* 9 */
	0x07, 0x0f, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x0f, 0x03, 0x00, 0x00, 0x18, 0x18, 0x0f, 0x07,
	0x1c, 0x1e, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x1f, 0x1b, 0x03, 0x03, 0x03, 0x03, 0x1e, 0x1c
};

