//**************************************************************************
//*        Definitions for SPARK-LIGHT main.c
//**************************************************************************

// Enable the bit definitions
#ifndef ENABLE_BIT_DEFINITIONS
#define ENABLE_BIT_DEFINITIONS
#endif

#include <inavr.h>
#include <ioavr.h>

#define FOSC 4800000 // Internal RC

// PINS PORTB
#define   pPWM      0       // PWM output
#define   pBTN      1       // Button
#define   pADC      2       // Analog input

// Constants
#define   MSLEEP 0
#define   MPOW_1 1
#define   MPOW_2 2
#define   MPOW_3 3
#define   MPOW_4 4
#define   MBLINK 5
#define   MLAST  MBLINK
// ADC values (ADC = I*0.5*1024/1.1)
#define   ADC_M1  140   // 300 mA
#define   ADC_M2  93    // 200 mA
#define   ADC_M3  47    // 100 mA
#define   ADC_M4  23    // 50 mA
#define   ADC_M5  93    // 200 mA blink

#define   BlinkTimeout 2500  // about 0.64 sec in simulator

