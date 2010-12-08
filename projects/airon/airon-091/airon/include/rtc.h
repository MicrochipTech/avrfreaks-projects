#ifndef RTC_H
#define RTC_H

#define REG_SEC	0x00
#define REG_MIN	0x01
#define REG_HRS	0x02
#define REG_DAY	0x03
#define REG_DAT	0x04
#define REG_MON   0x05
#define REG_YEA   0x06
#define REG_CTRL  0x07

void InitRTC (void);
void rtc_write_byte(uint8_t addr, uint8_t value);
uint8_t rtc_read_byte(uint8_t addr);

void SaveTimeToRTC ( uint8_t sec,
						   uint8_t min,
						   uint8_t hour,
						   uint8_t day,
						   uint8_t month,
						   uint16_t year );

void RestoreTimeFromRTC (uint8_t *sec,
								 uint8_t *min,
								 uint8_t *hour,
								 uint8_t *day,
								 uint8_t *month,
								 uint16_t *year);

#endif //RTC_H
