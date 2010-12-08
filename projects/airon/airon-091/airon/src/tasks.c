/**
 * tasks.c
 *
 * main tasks
 *
 */

#include <avr/pgmspace.h>
#include <util/atomic.h>

#include "io.h"
#include "lcd.h"
#include "rtc.h"
#include "utils.h"
#include "version.h"
#include "timing.h"
#include "tasks.h"

#define every_ticks(x) if((ticks&x)==x)

#define STATE_IDLE		0
#define STATE_CLOCK		1
#define STATE_ADC_BATT	2
#define STATE_ADC_CURR	3
#define STATE_ADC_TEMP	4
#define STATE_VERSION	5

#define STATE_MAX			STATE_VERSION

#define STATE_SETUPS		0x80
#define STATE_STP_CDAY	STATE_SETUPS
#define STATE_STP_CMON	STATE_SETUPS+1
#define STATE_STP_CYEA	STATE_SETUPS+2
#define STATE_STP_CHRS	STATE_SETUPS+3
#define STATE_STP_CMIN	STATE_SETUPS+4
#define STATE_STP_CSEC	STATE_SETUPS+5
#define STATE_STP_CEND	STATE_SETUPS+6

uint8_t state = STATE_IDLE;

extern struct time t;
extern uint32_t ticks;

void Animation(void)
{
	static uint8_t  astate=0;
	static uint32_t last_ticks=0;

	if (ticks && (!last_ticks || ticks>(last_ticks+120)))
	{
		LcdClear();

		switch (astate++)
		{
			case 0:
				LcdDisplaySplash();
			break;
			case 1:
				LcdWriteAnimated_P(PSTR("  Airone Control Center  "), ROW_TOP);
			break;
			case 2:
				LcdWriteToCenter_P(PSTR("--=[Airone]=--"), ROW_TOP);
			break;
		}
		if (astate==3) astate=0;

		last_ticks=ticks;
	}
}

void menu(char sense)
{
	state+=sense;

	if (state<0x80 && state>STATE_MAX) state=0;
	if (state==0xff) state=STATE_MAX;

	switch(state)
	{
		case STATE_IDLE:
			Animation();
		break;
		case STATE_CLOCK:
			LcdShowDateTime ();
		break;
		case STATE_ADC_BATT:
			LcdClear ();
			LcdWrite_P (PSTR("Battery :"));
			LcdShowScreenBattery ();
		break;
		case STATE_ADC_CURR:
			LcdClear ();
			LcdWrite_P (PSTR("Current :"));
			LcdShowScreenCurrent ();
		break;
		case STATE_ADC_TEMP:
			LcdClear ();
			LcdWrite_P (PSTR("Temp.   :"));
			LcdShowScreenTemperature ();
		break;
		case STATE_VERSION:
			LcdClear ();
			LcdWrite_P (PSTR(version));
		break;
		default:
		break;
	}
}

static uint32_t tick_pressed=0;
static uint16_t stp_value;
static uint8_t  pressed=0;

static const uint8_t stp[6][5] = {
/* len, line, pos, limit_l, limit_h **/
	{ 2, 0, 3,  1, 31}, // CDAY
	{ 2, 0, 6,  1, 12}, // CMON
	{ 2, 0,11, 10, 60}, // CYEA
	{ 2, 1, 4,  0, 23}, // CHRS
	{ 2, 1, 7,  0, 59}, // CMIN
	{ 2, 1,10,  0, 59}  // CSEC
};

/** copy of the time structure for atomic updates (setup) **/
static struct time tt;

static void SetupNextField(void)
{
	char		*field;
	uint8_t  index=STATE_SETUPS+state;

	/** fix last input **/
	if (stp_value)
	{
		field=i2azp(stp_value,stp[index-1][0]);
		LcdWriteAt(field, stp[index-1][1],stp[index-1][2]);
	}

	switch (state)
	{
		default: return;
		case STATE_STP_CDAY:
			tt=t;
			stp_value=tt.day;
		break;
		case STATE_STP_CMON:
			tt.day=stp_value;
			stp_value=tt.month;
		break;
		case STATE_STP_CYEA:
			tt.month=stp_value;
			stp_value=tt.year-2000;
		break;
		case STATE_STP_CHRS:
			tt.year=stp_value+2000;
			stp_value=tt.hour;
		break;
		case STATE_STP_CMIN:
			tt.hour=stp_value;
			stp_value=tt.minute;
		break;
		case STATE_STP_CSEC:
			tt.minute=stp_value;
			stp_value=tt.second;
		break;
		case STATE_STP_CEND:
			tt.second=stp_value;
			/** apply atomic **/
			ATOMIC_BLOCK(ATOMIC_FORCEON)
			{
				t=tt;
			}
			SaveTimeToRTC(t.second, t.minute, t.hour, t.day, t.month, t.year);
			stp_value=0;
			state=STATE_CLOCK;
			/** refresh menu **/
			menu(0);
		return;
	}

	/** set new input **/
	LcdSetInput(i2azp(stp_value,stp[index][0]),stp[index][1],stp[index][2]);
}

static void SetupIncField(void)
{
	uint8_t index=STATE_SETUPS+state;

	++stp_value;

	/** roll over ebd **/
	if (stp_value>stp[index][4]) stp_value=stp[index][3];

	LcdUpdateInput(i2azp(stp_value,stp[index][0]));
}

/**
 * Setup functions, 3 seconds pressed
 **/
static void CheckSetup(void)
{
	if (pressed && ticks>tick_pressed+12)
	{
		if (state==STATE_CLOCK)
		{
			state=STATE_STP_CDAY;

			SetupNextField();

			pressed=0;
		}
	}
}

void ScheduleTasks(void)
{
	static uint8_t  oldsec=0;
	static uint32_t oldtick=0;

	/** every tick **/
	if (ticks!=oldtick)
	{
		switch (state)
		{
			case STATE_CLOCK:		CheckSetup();	break;
			case STATE_IDLE:		Animation();	break;
		}
		every_ticks(0x01) LcdUpdate();

		oldtick=ticks;
	}

	/** every second **/
	if (t.second!=oldsec)
	{
		switch (state)
		{
			case STATE_ADC_BATT:	LcdUpdateBattery();	break;
			case STATE_ADC_CURR:	LcdUpdateCurrent();	break;
			case STATE_ADC_TEMP:	LcdUpdateTemp();		break;
			case STATE_CLOCK:		LcdUpdateDateTime();	break;
		}
		IOUpdate	();

		oldsec=t.second;
	}
}

int Run (void)
{
	uint8_t  key=0,old=0;

	menu(STATE_IDLE);

	RestoreTimeFromRTC(&t.second, &t.minute, &t.hour, &t.day, &t.month, &t.year);

	for (;;)
	{
		ScheduleTasks();

		key=GetKey();

		if (old!=key)
		{
			if (key==KEY_NONE)
			{
				if (state<STATE_SETUPS)
				{
					switch (pressed)
					{
						case KEY_LEFT:	 menu(-1); break;
						case KEY_RIGHT: menu(+1); break;
					}
				}
				else
				{
					switch (pressed)
					{
						case KEY_LEFT:	 SetupIncField(); break;
						case KEY_RIGHT:
							state++;
							SetupNextField();
						break;
					}
				}
				pressed=0;
			}
			else
			{
				/** save the "will be" released, and pressed time **/
				pressed=key;
				tick_pressed=ticks;
			}
			old=key;
		}
	}
	return 0;
}
