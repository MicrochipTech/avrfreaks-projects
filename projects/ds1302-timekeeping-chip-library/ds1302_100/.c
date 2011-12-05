/*****************************************************************************
Title  :   DS1302 Library
Author :   SA Development
Version:   1.00
*****************************************************************************/

#include "avr/io.h"
#include "avr/pgmspace.h"

#include "ds1302.h"

#if (USE_ADELAY_LIBRARY==1)
  #include "adelay.h"
#else
  #define Delay_ns(__ns) \
    if((unsigned long) (F_CPU/1000000000.0 * __ns) != F_CPU/1000000000.0 * __ns)\
          __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000000.0 * __ns)+1);\
    else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000000.0 * __ns))
#endif

#define PIN(x) (*(&x - 2))           // Address of Data Direction Register of Port X
#define DDR(x) (*(&x - 1))           // Address of Input Register of Port X

#define SIG1 0xc9                    // Signature bytes written to the last 3 ram bytes
#define SIG2 0xf9
#define SIG3 0x31

#if (USE_TIMING==5)
  #define DS1302_TCC 1000
  #define DS1302_CWH 1000
  #define DS1302_TDC 50
  #define DS1302_CL  250
  #define DS1302_CH  250
#elif (USE_TIMING==2)
  #define DS1302_TCC 4000
  #define DS1302_CWH 4000
  #define DS1302_TDC 200
  #define DS1302_CL  1000
  #define DS1302_CH  1000
#else
  #error USE_TIMING must be 2 or 5
#endif

unsigned char ResetTime[8] PROGMEM = {0x80,0x00,0x00,0x01,0x01,0x01,0x00,0x00};

void ds1302_shiftout(unsigned char AData)
  {
    unsigned char Bits=8;

    DDR(SIO_PORT)|=_BV(SIO_PIN);           // Set SIO to output
    while (Bits)
      {
        if (AData & _BV(0))                // LSB sent first
          SIO_PORT|=_BV(SIO_PIN);
        else SIO_PORT&=~_BV(SIO_PIN);
        Delay_ns(DS1302_TDC);
        SCLK_PORT|=_BV(SCLK_PIN);          // Cycle SCLK
        Delay_ns(DS1302_CH);
        SCLK_PORT&=~_BV(SCLK_PIN);
        Delay_ns(DS1302_CL);
        AData>>=1;                         // Prepare to send next bit
        Bits--;
      }
    DDR(SIO_PORT)&=~_BV(SIO_PIN);          // Set SIO back to input
  }

unsigned char ds1302_shiftin()
  {
    unsigned char Bits=8;
    unsigned int ui1=0;

    while (Bits)
      {
        ui1>>=1;
        ui1|=(PIN(SIO_PORT) & _BV(SIO_PIN))?128:0;       // LSB received first
        SCLK_PORT|=_BV(SCLK_PIN);                        // Cycle SCLK
        Delay_ns(DS1302_CH);
        SCLK_PORT&=~_BV(SCLK_PIN);
        Delay_ns(DS1302_CL);
        Bits--;
      }
    return ui1;
  }

unsigned char ds1302_getbyte(unsigned char AAddress)
  {
    unsigned char c1;

    CE_PORT|=_BV(CE_PIN);                                // Set CE high
    Delay_ns(DS1302_TCC);
    ds1302_shiftout(AAddress | _BV(0));                  // Send command byte
    c1=ds1302_shiftin();                                 // Get result
    CE_PORT&=~_BV(CE_PIN);                               // Take CE back low
    Delay_ns(DS1302_CWH);
    return c1;
  }

void ds1302_setbyte(unsigned char AAddress, unsigned char AValue)
  {
    CE_PORT|=_BV(CE_PIN);                                // Set CE high
    Delay_ns(DS1302_TCC);
    ds1302_shiftout(AAddress & ~_BV(0));                 // Send command byte
    ds1302_shiftout(AValue);                             // Send value
    CE_PORT&=~_BV(CE_PIN);                               // Take CE back low
    Delay_ns(DS1302_CWH);
  }

unsigned char ds1302_init()
  {
    unsigned char c1,c2;

    CE_PORT&=~_BV(CE_PIN);                               // Default port configuration
    DDR(CE_PORT)|=_BV(CE_PIN);                           // CE output, low
    SCLK_PORT&=~_BV(SCLK_PIN);                           // SCLK output, low
    DDR(SCLK_PORT)|=_BV(SCLK_PIN);
    SIO_PORT&=~_BV(SIO_PIN);                             // SIO input, no pullup
    DDR(SIO_PORT)&=~_BV(SIO_PIN);

    c1=ds1302_getbyte(0x8f);                             // Test device presence (read WP byte)
    if (c1!=0x00 && c1!=0x80)                            // Result should always be 0x00 or 0x80
      return 0;
    c1^=_BV(7);                                          // Flip bit and see if we can write/reread it
    ds1302_setbyte(0x8e,c1);
    c2=ds1302_getbyte(0x8f);
    if (c1!=c2)                                          // New result should match written flipped bit
      return 0;
    if (!c2)
      ds1302_setbyte(0x8e,0x80);                         // Leave WP set

    if (ds1302_getbyte(0xf9)!=SIG1 ||                    // Look for signature
        ds1302_getbyte(0xfb)!=SIG2 ||
        ds1302_getbyte(0xfd)!=SIG3)
      ds1302_clear();                                    // Clear if no signature found
    return 1;                                            // Device is present
  }

void ds1302_reset_time()
  {
    unsigned char c1;
    
    ds1302_setbyte(0x8e,0x00);                           // Disable WP
    ds1302_setbyte(0x90,TCS_REGISTER_DEFAULT);           // Set TCS to default
    CE_PORT|=_BV(CE_PIN);                                // Set CE high
    Delay_ns(DS1302_TCC);
    ds1302_shiftout(0xbe);                               // Burst write of time sets all registers in sync with each other
    for (c1=0;c1<8;c1++)
      ds1302_shiftout(pgm_read_word(ResetTime+c1));
    CE_PORT&=~_BV(CE_PIN);                               // Take CE back low
    Delay_ns(DS1302_CWH);
    ds1302_setbyte(0x8e,0x80);                           // Reenable WP
  }

void ds1302_clear()
  {
    unsigned char c1;

    ds1302_reset_time();
    ds1302_setbyte(0x8e,0x00);                           // Disable WP
    CE_PORT|=_BV(CE_PIN);                                // Set CE high
    Delay_ns(DS1302_TCC);
    ds1302_shiftout(0xfe);                               // Burst write RAM
    for (c1=0;c1<28;c1++)                                // Set RAM to zeros
      ds1302_shiftout(0);
    ds1302_shiftout(SIG1);                               // Write signature to RAM
    ds1302_shiftout(SIG2);
    ds1302_shiftout(SIG3);
    CE_PORT&=~_BV(CE_PIN);                               // Take CE back low
    Delay_ns(DS1302_CWH);
    ds1302_setbyte(0x8e,0x80);                           // Reenable WP
  }

unsigned char ds1302_get_time(struct ymdhms_type *ATime)
  {
    unsigned char c1,s1[8];

    CE_PORT|=_BV(CE_PIN);                                // Set CE high
    Delay_ns(DS1302_TCC);
    ds1302_shiftout(0xbf);                               // Burst read of time gets all registers in sync with each other
    for (c1=0;c1<8;c1++)
      s1[c1]=ds1302_shiftin();
    CE_PORT&=~_BV(CE_PIN);                               // Take CE back low
    Delay_ns(DS1302_CWH);
    if (s1[0] & _BV(7))                                  // If CH is set then clock is not set, return 0
      return 0;
                                                         // Convert from BCD
    ATime->second=((s1[0] & (_BV(4)|_BV(5)|_BV(6)))>>4)*10         +(s1[0] & (_BV(0)|_BV(1)|_BV(2)|_BV(3)));
    ATime->minute=((s1[1] & (_BV(4)|_BV(5)|_BV(6)))>>4)*10         +(s1[1] & (_BV(0)|_BV(1)|_BV(2)|_BV(3)));
    ATime->hour  =((s1[2] & (_BV(4)|_BV(5)))>>4)*10                +(s1[2] & (_BV(0)|_BV(1)|_BV(2)|_BV(3)));
    ATime->day   =((s1[3] & (_BV(4)|_BV(5)))>>4)*10                +(s1[3] & (_BV(0)|_BV(1)|_BV(2)|_BV(3)));
    ATime->month =((s1[4] & (_BV(4)))>>4)*10                       +(s1[4] & (_BV(0)|_BV(1)|_BV(2)|_BV(3)));
    ATime->year  =((s1[6] & (_BV(4)|_BV(5)|_BV(6)|_BV(7)))>>4)*10  +(s1[6] & (_BV(0)|_BV(1)|_BV(2)|_BV(3)));
    return 1;
  }

void ds1302_set_time(struct ymdhms_type *ATime)
  {
    unsigned char c1,s1[8];
                                                         // Convert to BCD
    s1[0]=((ATime->second/10)<<4)+ATime->second%10;
    s1[1]=((ATime->minute/10)<<4)+ATime->minute%10;
    s1[2]=((ATime->hour/10)<<4)  +ATime->hour%10;
    s1[3]=((ATime->day/10)<<4)   +ATime->day%10;
    s1[4]=((ATime->month/10)<<4) +ATime->month%10;
    s1[6]=((ATime->year/10)<<4)  +ATime->year%10;
    s1[7]=0;

    ds1302_setbyte(0x8e,0x00);                           // Disable WP
    CE_PORT|=_BV(CE_PIN);                                // Set CE high
    Delay_ns(DS1302_TCC);
    ds1302_shiftout(0xbe);                               // Burst write of time sets all registers in sync with each other
    for (c1=0;c1<8;c1++)
      ds1302_shiftout(s1[c1]);
    CE_PORT&=~_BV(CE_PIN);                               // Take CE back low
    Delay_ns(DS1302_CWH);
    ds1302_setbyte(0x8e,0x80);                           // Reenable WP
  }

unsigned char ds1302_get_ram(unsigned char AAddress)
  {
    if (AAddress<28)
      return ds1302_getbyte(0xc1+2*AAddress);
    else return 0;
  }

void ds1302_set_ram(unsigned char AAddress, unsigned char AValue)
  {
    if (AAddress<28)
      {
        ds1302_setbyte(0x8e,0x00);                           // Disable WP
        ds1302_setbyte(0xc0+2*AAddress,AValue);              // Write
        ds1302_setbyte(0x8e,0x80);                           // Reenable WP
      }
  }
  
