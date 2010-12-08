/**
 * rtc.c
 *
 * dallas DS1307 i2c driver
 */

#include <avr/io.h>
#include <compat/twi.h>
#include <util/atomic.h>

#include "rtc.h"
#include "utils.h"
#include "timing.h"
#include "common.h"

#define SLAVE_DS1307		0xD0

#define __start() TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN)
#define __stop()  TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO)
#define __fire()  TWCR=(1<<TWINT)|(1<<TWEN)
#define __wait_sent()  while(!(TWCR&(1<<TWINT)))

void InitRTC (void)
{
	int i;

	/**
	 *                    CPU_CLOCK
	 * SCL FREQ = --------------------------
	 *				  16+2(TWBR)*PRESCALER_VALUE
	 *
	 *
	 *   DS1307 operate at 100Khz only
	 */

	/* prescaler to 0 (1), TWBR to 32 */
	TWSR = 0x00;
	TWBR = 0x20;

	/* CH to 0 to run !! */
	i=rtc_read_byte(REG_SEC);

	// new ds1307 ? oscillator disabled ?
	if (i&0x80)
		rtc_write_byte(REG_SEC,(0));

	/* start square wave 32768Khz */
	rtc_write_byte(REG_CTRL, 0x13);
}

void rtc_write_byte(uint8_t addr, uint8_t value)
{
	__start();
	__wait_sent();

	TWDR = SLAVE_DS1307;
	__fire();
	__wait_sent();

	TWDR = addr;
	__fire();
	__wait_sent();

	TWDR = value;
	__fire();
	__wait_sent();

	__stop();
}

uint8_t rtc_read_byte(uint8_t addr)
{
	uint8_t rval=0;

	__start();
	__wait_sent();

	TWDR = SLAVE_DS1307;
	__fire();
	__wait_sent();

	TWDR = addr;
	__fire();
	__wait_sent();

	__start();
	__wait_sent();

	TWDR = SLAVE_DS1307+0x01;
	__fire();
	__wait_sent();

	__fire();
	__wait_sent();

	switch (TW_STATUS)  {
		case TW_MR_DATA_NACK:
			// FALLTHROUGH
		case TW_MR_DATA_ACK:
			rval=TWDR;
		break;
		default:
			rval=-1;
		break;
	}

	__stop();

	return rval;
}

void SaveTimeToRTC ( uint8_t sec,
						   uint8_t min,
						   uint8_t hour,
						   uint8_t day,
						   uint8_t month,
						   uint16_t year )
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		rtc_write_byte(REG_SEC, dec2bcd(sec&0x7f)); /* CH bit to 0 ! */
		rtc_write_byte(REG_MIN, dec2bcd(min));
		rtc_write_byte(REG_HRS, dec2bcd(hour));
		rtc_write_byte(REG_DAT, dec2bcd(day));
		rtc_write_byte(REG_MON, dec2bcd(month));
		rtc_write_byte(REG_YEA, dec2bcd(year-2000));
	}
}

void RestoreTimeFromRTC ( uint8_t *sec,
								  uint8_t *min,
								  uint8_t *hour,
								  uint8_t *day,
								  uint8_t *month,
								  uint16_t *year )
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		*sec		= bcd2dec(rtc_read_byte(REG_SEC));
		*min		= bcd2dec(rtc_read_byte(REG_MIN));
		*hour		= bcd2dec(rtc_read_byte(REG_HRS));
		*day		= bcd2dec(rtc_read_byte(REG_DAT));
		*month	= bcd2dec(rtc_read_byte(REG_MON));
		*year		= bcd2dec(rtc_read_byte(REG_YEA))+2000;
	}
}



