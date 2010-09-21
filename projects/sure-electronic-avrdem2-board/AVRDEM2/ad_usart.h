// ad_usart.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//

#include <iom16v.h>
#include <macros.h>
#include <stdio.h>
#define OK_USART

// ----------- function prototypes
  int putchar(char);                    // custom putchar()
  int getchar(void);                    // custom getchar()
  void put_CR(void);                    // write CR/LF pair
  void put_str(char *, char);           // write string to USART
  void put_fstr(__flash char *, char);  // write flash string to USART
  void init_usart(int);                 // USART initialization

extern int _textmode;

// ------------ baud rates (12 MHz clock)
#define BRATE_24  312   // 2400   (0.2%) 
#define BRATE_48  155   // 4800   (0.2%)
#define BRATE_96   77   // 9600   (0.2%)
#define BRATE_144  51   // 14.4K  (0.2%)
#define BRATE_192  38   // 19.2K  (0.2%)
#define BRATE_288  25   // 28.8K  (0.2%)
#define BRATE_384  19   // 38.4K  (2.4%)
#define BRATE_576  12   // 57.6K  (0.2%)
