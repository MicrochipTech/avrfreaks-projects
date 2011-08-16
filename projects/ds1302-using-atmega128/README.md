# DS1302 using with Atmega128

Uploaded by YahyaAVR on 2011-08-16 09:34:00 (rating 0 out of 5)

## Summary

DS1302 full function support library


All pins can be modified in DS1302.h


////////////////////////////////////////////////////////////////////////  

/// DS1302.C ///  

/// Driver for Real Time Clock ///  

///  

/// rtc\_init() Call after power up ///  

/// rtc\_set\_datetime() Set the date/time ///  

/// rtc\_get\_date() Get the date ///  

/// rtc\_get\_time() Get the time ///  

/// rtc\_write\_nvr(address,data) Write to NVR ///  

/// data = rtc\_read\_nvr(address) Read from NVR  

///  

/// get\_bcd(data) Convert data to BCD  

///  

/// rm\_bcd(data) Convert data to int  

///  

/// Yahya T. 15082011 TR ///  

////////////////////////////////////////////////////////////////////////


#define RTCCLKH PORTB |= 0x02 // PINB1 - SCLK  

#define RTCCLKL PORTB &= 0xFD //  

#define RTCIOH PORTB |= 0x04 // PINB2 - I/O (for DATA OUT)  

#define RTCIOL PORTB &= 0xFB  

#define RTCRSTH PORTB |= 0x01 // PINB0 - RST  

#define RTCRSTL PORTB &= 0xFE  

#define RTCIOIN (PINB & 0x04) == 0x04 // PINB2 (for DATA IN)


uint8\_t Day=0,Mth=0,Year=0,Dow=0,Hr=0,Min=0,Sec=0;


void write\_ds1302\_byte(unsigned char cmd)  

{  

 unsigned char i;


 DDRB = 0xFF;


 for(i=0;i<=7;i++)  

 {  

 if (cmd & 0x01) {RTCIOH;} else {RTCIOL;}  

 cmd >>= 1;  

 RTCCLKH; \_delay\_us(2);  

 RTCCLKL; \_delay\_us(2);  

 }  

}  

void write\_ds1302(unsigned char cmd, unsigned char data)  

{  

 RTCRSTH;  

 write\_ds1302\_byte(cmd);  

 write\_ds1302\_byte(data);  

 RTCRSTL;  

}  

uint8\_t read\_ds1302(unsigned char cmd)  

{  

 uint8\_t i,data=0;


 RTCRSTH;  

 write\_ds1302\_byte(cmd);


 DDRB &= 0xFB;


 for(i=0;i<=7;i++)  

 {  

 data >>= 1;  

 if (RTCIOIN) {data |= 0x80;}


 RTCCLKH; \_delay\_us(2);  

 RTCCLKL; \_delay\_us(2);  

 }  

 DDRB |= 0x04;


 RTCRSTL;  

 return(data);  

}  

void rtc\_init()  

{  

 uint8\_t x;


 write\_ds1302(0x8e,0);  

 write\_ds1302(0x90,0xa4);  

 x = read\_ds1302(0x81);  

 if((x & 0x80)!=0)  

 {  

 write\_ds1302(0x80,0);  

 }  

}  

uint8\_t get\_bcd(uint8\_t data)  

{  

 uint8\_t nibh;  

 uint8\_t nibl;


 nibh=data/10;  

 nibl=data-(nibh*10);


 return((nibh<<4)|nibl);  

}  

uint8\_t rm\_bcd(uint8\_t data)  

{  

 uint8\_t i;


 i=data;  

 data=(i>>4)*10;  

 i <<= 4;  

 data=data+(i>>4);


 return data;  

}  

void rtc\_set\_datetime()  

{  

 write\_ds1302(0x86,get\_bcd(Day));  

 write\_ds1302(0x88,get\_bcd(Mth));  

 write\_ds1302(0x8c,get\_bcd(Year));  

 write\_ds1302(0x8a,get\_bcd(Dow));  

 write\_ds1302(0x84,get\_bcd(Hr));  

 write\_ds1302(0x82,get\_bcd(Min));  

 write\_ds1302(0x80,get\_bcd(0));  

}  

void rtc\_get\_date()  

{  

 Day = rm\_bcd(read\_ds1302(0x87));  

 Mth = rm\_bcd(read\_ds1302(0x89));  

 Year = rm\_bcd(read\_ds1302(0x8d));  

 Dow = rm\_bcd(read\_ds1302(0x8b));  

}  

void rtc\_get\_time()  

{  

 Hr = rm\_bcd(read\_ds1302(0x85));  

 Min = rm\_bcd(read\_ds1302(0x83));  

 Sec = rm\_bcd(read\_ds1302(0x81));  

}  

void rtc\_write\_nvr(unsigned char address, uint8\_t data)  

{  

 write\_ds1302(address|0xc0,data);  

}  

uint8\_t rtc\_read\_nvr(unsigned char address)  

{  

 return(read\_ds1302(address|0xc1));  

}

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
