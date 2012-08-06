/*
 * os.c
 *  Created on: May 8, 2011
 *      Author: dani
 */
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "eSol.h"
#include "os.h"
#include "port.h"
#include "adc.h"
#include "pid.h"

#define K_P     46
#define K_I     0.08
#define K_D     170

//Alex
const  uint16_t ausTempPreset[6] = {150, 225, 275, 325, 375, 425};
static uint8_t  ucTempPresetIdx;
volatile tPushButtonStatus PushBtnStatus;
//end Alex

//! Parameters for regulator
struct PID_DATA pidData;

volatile uint8_t  OS_u8Cnt1ms;
volatile uint16_t OS_u16Cnt100ms;
volatile uint16_t OS_u16Cnt200ms;
volatile uint16_t OS_u16Cnt500ms;

static uint8_t  OS_u8StateCnt10ms;

static int16_t OS_i16TargetTemp;
static int16_t OS_i16TargetTempOld;
static int16_t OS_i16EncoderValueX4;
static uint16_t OS_u16CurrentTemperature;
static uint8_t  OS_u8TempPresetStateCounter = 0;

//Alex
/*up/low button status*/
static bool bbtnupstatus;
static bool bbtnlowstatus;
//end Alex

static uint8_t PreviousEncoder;
static uint8_t PreviousDirection;

static void OS_vHmiStateMachine(void);
static void OS_vTemperatureRegulator(void);

//Alex
static void OS_vUpBtnAction(void);
static void OS_vLowBtnAction(void);
//end Alex

ISR(TIMER0_COMPA_vect)
{
   unsigned int Zbuffer = 0; // setup a 16 bit temporary local buffer

   //PINA |= (1<<Spare); /* toggle Spare pin */

   asm volatile(
   // incoming stack maintenance is handled by compiler

   "mov    %A1, %2     \n\t"    // Store previous encoder in low buffer
   "in     %2,  %4     \n\t"    // Input encoder state PORTD 0b00xx0000
   "swap   %2          \n\t"    // Swap nibbles 0b76xx3210 -> 0b321076xx

   "ldi    %B1, 1      \n\t"    // Load 1 into high buffer
   "sbrc   %2,  1      \n\t"    // Skip next instruction if bit 1 is clear
   "eor    %2,  %B1    \n\t"    // Exclusive OR, ENC = 1x, if 10 -> 11, if 11 -> 10

   "sub    %A1, %2     \n\t"    // Subtract previous from 11 or 10, store in low
   "andi   %A1, 3      \n\t"    // AND with 11 - check if bits have changed
   "breq   exitpoint   \n\t"    // If equal, no change, jump to exit

   "cpi    %A1, 3      \n\t"    // Compare low buffer to 0b00000011
   "breq   decrement   \n\t"    // If equal jump to decrement

   "cpi    %A1, 1      \n\t"    // Compare low buffer to 0b00000001
   "breq   increment   \n\t"    // If equal jump to increment

   "mov    %A1, %3     \n\t"    // No branch = lost in transition
   "mov    %B1, %3     \n\t"    // Use previous direction to make up lost bit
   "lsl    %A1         \n\t"    // Logical left shift = x2 - double up for lost bits
   "asr    %B1         \n\t"    // Clear and save signed flag
   "rjmp   summation   \n\t"    // Jump to total

   "decrement:         \n\t"
   "ldi     %A1, -1    \n\t"    // Load low buffer with -1
   "ser     %B1        \n\t"    // Set high buffer for negative number = 0xFF
   "rjmp storeprevious \n\t"    // Jump over/skip increment

   "increment:         \n\t"
   "ldi     %A1, 1     \n\t"    // Load low buffer with 1
   "clr     %B1        \n\t"    // Clear high buffer

   "storeprevious:     \n\t"
   "mov     %3,  %A1   \n\t"    // Store the previous direction in low buffer

   "summation:         \n\t"
   "add     %A0, %A1   \n\t"    // Add low byte and carry to high
   "adc     %B0, %B1   \n\t"    // This becomes the 16bit variable CurrentPosition

   "exitpoint:         \n\t"    // final destination

   // Outgoing stack maintenance is handled by compiler....

   // Output
   :  "=r" (OS_i16EncoderValueX4),
      "=d" (Zbuffer),
      "=r" (PreviousEncoder),
      "=r" (PreviousDirection)

   // Input
   :  "I" (_SFR_IO_ADDR(PINA)),
      "0" (OS_i16EncoderValueX4),
      "2" (PreviousEncoder),
      "3" (PreviousDirection)

   // Clobber
   :  // nothing to clobber

   );

   /* Use 1x edge mode from existing 4x encoder routine (divide the no_of_edges by 4)*/
   OS_i16TargetTemp = OS_i16EncoderValueX4 >> 2;

   /* Enter the "temp preset" mode whenever target temp changes */
  if (OS_i16TargetTemp != OS_i16TargetTempOld)
  {
    /* Enter Temp_set_mode for 2 sec (based on 200ms container cycle time */
    OS_u8TempPresetStateCounter = 10;

    if (OS_i16TargetTemp < 50)             /* Limit the min temperature */
    {
      OS_i16TargetTemp = 50;
      OS_i16EncoderValueX4 = 50 << 2;
    }
    else if (OS_i16TargetTemp > 450)       /* Limit the max temperature */
    {
      OS_i16TargetTemp = 450;
      OS_i16EncoderValueX4 = 450 << 2;
    }

    OS_i16TargetTempOld = OS_i16TargetTemp;
  }

   /* Increase the OS "tasks" timers */
   OS_u8Cnt1ms++;
   OS_u16Cnt100ms++;
   OS_u16Cnt200ms++;
   OS_u16Cnt500ms++;
}

ISR(ANALOG_COMP_vect)
{

  /* Disable the interrupt to avoid false re-triggers when duty<5% */
  ACSR &= ~(1 << ACIE);

  /* On AIN0 zero cross detection toggle restart the PWM counter */
  TCNT1 = ICR1 - 1;

  /* Load the timeout counter to measure 5ms */
  OS_u8StateCnt10ms = 5;

}

void OS_vInit(void)
{
//Alex
   uint16_t usEepromStartTemp = 0;
//end Alex

   pid_Init(K_P * SCALING_FACTOR, K_I * SCALING_FACTOR , K_D * SCALING_FACTOR , &pidData);

   TCCR0A = (1<<WGM01);         /* CTC: mode 2 */
   TCCR0B = (1<<CS01);          /* Prescaler: CLK/8 */
   OCR0A  = OS_100us_RUN_TIMER;  /* 1MHz/100 = 100us */

   ACSR = (1<<ACIE);                 /* Enable ACOMP interrupt on output toggle */
   DIDR1 = (1<<AIN1D) | (1<<AIN0D);  /* Disable digital inputs on AIN1/0 */

   /* Fast PWM mode 14 */
   TCCR1A = (1<<COM1A1) | (1<<WGM11);                          /* Clear OC1A on Compare Match */
   TCCR1B = (1<<CS11) | (1<<WGM13) | (1<<WGM12);               /* Prescaler CLK/64 */

   ICR1  = 10500;      /* make it >10ms as half of the 50Hz mains frequency */
   OCR1A = 100;      /* Start duty cycle */

//Alex
   /*read the current temperature set from eeprom at initialization and set it as starting value*/
   /* Adapt the current encoder pulse counter to the 4x edges logic */
   usEepromStartTemp = eeprom_read_word((uint16_t*)03);
   if ((usEepromStartTemp >= 50) || (usEepromStartTemp <= 450))
   {
     OS_i16EncoderValueX4 = usEepromStartTemp << 2;
   }
   else
   {
     OS_i16EncoderValueX4 = 180 << 2;
   }

   ucTempPresetIdx  = 0;
   PushBtnStatus.bUpPushButtonPressed = false;
   PushBtnStatus.bLowPushButtonPressed = false;
//end Alex
}

void OS_vStartFreeRunTimer(void)
{
  TIMSK0 = (1<<OCIE0A);
}

void OS_vCyclicTask1ms(void)
{

 // PINA |= (1<<Spare); /* toggle Spare pin */

  gvDisplayRefreshFct();

  if (OS_u8StateCnt10ms > 0)
  {
    OS_u8StateCnt10ms--;
  }
  else
  {
    /* Re-enable AC interrupt and clear any pending interrupt */
    ACSR = (1 << ACIE) | (1 << ACI);
  }

 // PINA |= (1<<Spare); /* toggle Spare pin */
}

void OS_vCyclicTask100ms(void)
{
//Alex
   /*Read the status of the input selection button*/
   bbtnupstatus = gvReadUpperButtonPressed();
   if (bbtnupstatus != false)
   {
      PushBtnStatus.bUpPushButtonPressed = true;
   }

   bbtnlowstatus = gvReadLowerButtonPressed();
   if (bbtnlowstatus != false)
   {
      PushBtnStatus.bLowPushButtonPressed = true;
   }
//end Alex
}

void OS_vTemperatureRegulator(void)
{
  /* Modulate the heater duty cycle;
   * Current_temp gets 8x more because of ADC oversampling&decimation method
   */
  OCR1A = pid_Controller(OS_i16TargetTemp * 8, OS_u16CurrentTemperature,&pidData);
}
void OS_vCyclicTask200ms(void)
{
//Alex
   OS_vUpBtnAction();
   OS_vLowBtnAction();
//end Alex

   OS_vHmiStateMachine();
   OS_vTemperatureRegulator();
}

void OS_vCyclicTask500ms(void)
{

}

void OS_vBackgroundTask(void)
{
  /* Read ADC only when heater is not engaged, to avoid noisy readings */
  if ((PIND & (1<< PIND5)) == 0)
  {
    OS_u16CurrentTemperature = ADC_u16ConvertTempToLsb(ADC_vGetMovingAvg());
  }
}

static void OS_vHmiStateMachine(void)
{
   static uint8_t u8Blink = 0;
   uint16_t       u16CurrentTemp;

   if (OS_u8TempPresetStateCounter > 0)
   {
      u8Blink ^= 1;                             /* Toggle bit */
      itobcd(OS_i16TargetTemp, 0, 0, u8Blink);  /* Write value with blinking dot */
      OS_u8TempPresetStateCounter--;
   }
   else /* Exit from Temp_set_mode */
   {
//Alex
      u16CurrentTemp = OS_u16CurrentTemperature / 8;
      if (u16CurrentTemp <= 40)
      {
         /*if current temperature is lower or equal to 40 degrees, write Lo on display*/
         gvWriteLowDisplay();
      }
      else if (u16CurrentTemp > 500)
      {
         /*if current temperature is higher than 500 degrees, then write Err on display*/
         gvWriteErrorDisplay();
      }
      else
      {
         /* Current_temp gets 8/ lower because of ADC over sampling & decimation method */
         itobcd(OS_u16CurrentTemperature / 8, 0, 0, 0); /* Write value with no dots */
      }
//end Alex
   }
}

//Alex
static void OS_vUpBtnAction()
{
   if (PushBtnStatus.bUpPushButtonPressed != false)
   {
      eeprom_write_word ((uint16_t*) 03, OS_i16TargetTemp);
      PushBtnStatus.bUpPushButtonPressed = false;
   }
   else
   {
      /*do nothing*/
   }
}

static void OS_vLowBtnAction()
{
   uint16_t temp;

   if (PushBtnStatus.bLowPushButtonPressed != false)
   {
      temp = ausTempPreset[ucTempPresetIdx];

      ucTempPresetIdx ++;
      if (ucTempPresetIdx > 5)
      {
         ucTempPresetIdx = 0;
      }

      OS_i16EncoderValueX4 = temp << 2;
      PushBtnStatus.bLowPushButtonPressed = false;
   }
}
//end Alex
