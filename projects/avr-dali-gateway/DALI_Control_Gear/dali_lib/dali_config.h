#ifndef DALI_CONFIG_H
#define DALI_CONFIG_H

//_____ D E F I N I T I O N S ______________________________________________

#define FOSC		                  8000000   //Oscillator Frequency : 8MHz
#define TICK_PERIOD               1000      //in microseconds : tick every 1ms

#define DALI_VERSION_NUMBER       0x00
#define DALI_PHYSICAL_MIN_LEVEL   50
#define DALI_DEVICE_TYPE          0

#if (TICK_PERIOD > 1000)
  #if ((TICK_PERIOD%1000) != 0)
    #error TICK_PERIOD shall be a mutliple of 1000 ("dali_config.h")
  #endif
#else
  #if ((1000%TICK_PERIOD) != 0)
    #error TICK_PERIOD shall be a divider of 1000 ("dali_config.h")
  #endif
#endif

#define HAS_EEPROM

  /***  EUSART Configuration :  ***/

#define USE_DALI

#define EUSART_TX_SIZE        3
//!<0-3:5-8bits    7:9bits     8-11:13-16bits    15:17bits
#define EUSART_RX_SIZE        11
//!<0-3:5-8bits   7:9bits     8-11:13-16bits   14:16 OR 17bits  15:17bits

#define EUSART_ENABLE         1
//!<0:USART mode     1:EUSART mode
#define NB_STOP_BITS_RX       1
//!<0:1 stop bit     1:2 stop bits
#define MANCHESTER_MODE       1
//!<0:level encoding 1:manchester encoding
#define BIT_ORDER             1
//!<0:LSB first      1:MSB first

#define BAUD_RATE             1200

#define RX_COMPLETE_INTERRUPT           1
//!<0:interrupt on rx complete disabled            1:interrupt enabled
#define TX_COMPLETE_INTERRUPT           0
//!<0:interrupt on tx complete disabled            1:interrupt enabled
#define DATA_REGISTER_EMPTY_INTERRUPT   0
//!<0:interrupt on data register empty disabled    1:interrupt enabled
#define RX_ENABLE                       1
//!<0:receiver disabled                            1:receiver enabled
#define TX_ENABLE                       1
//!<0:transmitter disabled                         1:transmitter enabled

#define SYNCHRONOUS           0
//!<0:asynchronous or manchester mode  1:synchronous operation
#define PARITY_CHECK          0
//!<0:none   2:even    3:odd
#define NB_STOP_BITS_TX       1
//!<0:1 stop bit       1:2 stop bits
#define CLOCK_POLARITY        0
//!<0:for asynchronous mode    others:see datasheet

#define TX_SPEED_X2           0
//!<0:normal transmission speed 1: double speed

#endif  // DALI_CONFIG_H
