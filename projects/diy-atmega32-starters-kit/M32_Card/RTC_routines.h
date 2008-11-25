

#define		SECONDS			rtc_register[0]
#define		MINUTES			rtc_register[1]
#define		HOURS			rtc_register[2]
#define		DAY    			rtc_register[3]
#define		DATE   			rtc_register[4]
#define		MONTH  			rtc_register[5]
#define		YEAR   			rtc_register[6]


void RTC_setStartAddress(void);
void RTC_read(void);
void RTC_write(void);
void RTC_getTime(void);
void RTC_getDate(void);
void RTC_displayTime(void);
void RTC_displayDate(void);
void RTC_displayDay(void);
void RTC_updateRegisters(void);
unsigned char RTC_writeTime(void);
unsigned char RTC_writeDate(void);
void RTC_updateTime(void);
void RTC_updateDate(void);

	