#ifndef SYSTEM_H
#define SYSTEM_H

#define FALSE 0
#define TRUE  1
#define NULL  0

// Defined for convenience when porting from MSP430
#define BIT0  (1<<0)
#define BIT1  (1<<1)
#define BIT2  (1<<2)
#define BIT3  (1<<3)
#define BIT4  (1<<4)
#define BIT5  (1<<5)
#define BIT6  (1<<6)
#define BIT7  (1<<7)

// Each tick approximately 16 ms with WDT (1 sec = 64 ticks)
#define WDTCLK_FREQ    131072 // Set to match WDT osc frequency
#define TICK_DIVISOR   2048   // Set to match WDT divisor in System.c
#define TICKS_PER_SEC  (WDTCLK_FREQ / TICK_DIVISOR)

// Port and I/O pin definitions
#define LED_OUT PORTB
#define LED1    BIT0
#define LED2    BIT1
#define LED3    BIT2

#define PB_IN   PINB
#define PB_PIE  PCMSK0
#define PSHBTN1 BIT3
#define PSHBTN2 BIT4
#define PSHBTN3 BIT5

void System_InitializeHW(void);

#endif
