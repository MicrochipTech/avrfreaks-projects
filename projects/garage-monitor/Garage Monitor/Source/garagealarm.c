#include "avr/io.h"
#include "avr/interrupt.h"
#include "avr/eeprom.h"
#include "avr/wdt.h"
#include "util/atomic.h"

#define Delay_ms(__ms) \
  if((unsigned long) (F_CPU/1000.0 * __ms) != F_CPU/1000.0 * __ms)\
        __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000.0 * __ms)+1);\
  else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000.0 * __ms))

#define DOOR_CHIME_EEPROM_LOCATION             0

//values in milliseconds
#define INPUT_HOLD_DELAY                       3000       //3s
#define INPUT_HOLD_REPEAT_DELAY                10000      //10s repeat feature not used in this program
#define DEBOUNCE_TIME                          50         //50ms

//values in tenths of seconds --> 10=one second
#define GARAGE_DOOR_LEFT_OPEN_FIRST_BEEP       3000       //first beep at 5 minutes
#define GARAGE_DOOR_LEFT_OPEN_SUBSEQUENT_BEEPS 1800       //subsequent beeps at 3 minutes
#define GARAGE_DOOR_LEFT_OPEN_BEEP_LENGTH      1000       //1s
#define GARAGE_DOOR_LEFT_OPEN_BEEPS            3          //3 beeps

//values in milliseconds
#define CHANGE_BEEP_LENGTH                     100        //100ms short beep
#define CHANGE_BEEP_OPENED_DOOR                2          //open door = 2 beeps
#define CHANGE_BEEP_CLOSED_DOOR                3          //closed door = 3 beeps

//values in tenths of seconds --> 10=one second
#define ALARM_DISABLE_PRESS_DELAY              36000      //each press equals one hour delay
#define ALARM_DISABLE_PRESS_DELAY_MAXIMUM      8*36000    //maximum delay is 8 hours no matter how many button presses

//PORT B
#define NO_OF_INPUTS                           2
#define ALARM_DISABLE_BUTTON                   0
#define GARAGE_SENSOR                          1

//PORT A
#define NO_OF_OUTPUTS                          4
#define BLUE_LED                               0
#define BEEPER                                 1
#define RED_LED                                2
#define GREEN_LED                              3

enum {OFF=0,ON,BLINKING};
enum {BEEPMODE_READY=0,BEEPMODE_REQUEST,BEEPMODE_RUNNING,BEEPMODE_CANCEL};

/*#define OFF                                    0
#define ON                                     1
#define BLINKING                               2*/

/*#define BEEPMODE_READY                         0
#define BEEPMODE_REQUEST                       1
#define BEEPMODE_RUNNING                       2
#define BEEPMODE_CANCEL                        3*/

//volatile variables
//                                                          ISR              Main Loop            Notes
volatile unsigned char  v_InputState[NO_OF_INPUTS],     //  r/w              r
                        v_InputPressed[NO_OF_INPUTS],   //  r/w              r/w                  r/m/w must be in atomic block
                        v_InputHeld[NO_OF_INPUTS],      //  r/w              r/w                  r/m/w must be in atomic block
                        v_BeepMode,                     //  r/w              r/w                  designed for comm between ISR/Main Loop
                        v_BeepsRemaining,               //  r/w              w                    access controlled by v_BeepMode
                        v_BeepDisabled,                 //  r                r/w
                        v_Leds[NO_OF_OUTPUTS];          //  r                w
volatile unsigned short v_BeepLen;                      //  r                w                    access controlled by v_BeepMode
volatile unsigned long  v_RunningTime;                  //  r/w              r                    atomic block (multiple bytes)

void clearinputs()
{
  unsigned char c1;

  //clear out pressed and held counts for both inputs
  for (c1=0;c1<NO_OF_INPUTS;c1++)
    {
      v_InputPressed[c1]=0;
      v_InputHeld[c1]=0;
    }
}

void beep(unsigned int AMilliseconds, unsigned char ABeeps)
{
  //if currently beeping, request cancel from ISR
  if (v_BeepMode!=BEEPMODE_READY)
    {
      //request cancel
      v_BeepMode=BEEPMODE_CANCEL;

      //wait for cancel
      while (v_BeepMode!=BEEPMODE_READY)
        ;
    }

  //setup beep parameters    
  v_BeepLen=AMilliseconds;
  v_BeepsRemaining=ABeeps;

  //request beeps from ISR
  v_BeepMode=BEEPMODE_REQUEST;
}

int main()
{
  unsigned char DoorChime,LastState,ThisState;
  unsigned long GarageOpenTime=0,AlarmDisabledTime=0,AlarmDelayTime,ul1;

  //disable wdt
  wdt_reset();
  MCUSR=0;
  WDTCSR|=_BV(WDCE) | _BV(WDE);
  WDTCSR=0;

  //enable pullups on input pins and pins we are not using (PB2 PA4 PA5 PA6 PA7)
  PORTB=_BV(GARAGE_SENSOR) | _BV(ALARM_DISABLE_BUTTON) | _BV(2); //PB2
  PORTA=_BV(4) | _BV(5) | _BV(6) | _BV(7); //PA4 PA5 PA6 PA7

  //enable output pins
  DDRA=_BV(GREEN_LED) | _BV(RED_LED) | _BV(BLUE_LED) | _BV(BEEPER);

  //setup timer and debounced isr
  OCR0A=124; 			    			                      //Set Timer Compare Register to 1ms - 8000000/64/125 = 1000hz
  TIMSK0|=_BV(OCIE0A);       	    	              //Enable CTC Interrupt
  sei();							                            //Enable Global Interrupts
  TCCR0A|=_BV(WGM01);                             //Enable CTC Mode and Clk/64
  TCCR0B|=_BV(CS01) | _BV(CS00);

  //get status beep setting from EEPROM (no need for my eeprom library)
  DoorChime=eeprom_read_byte(DOOR_CHIME_EEPROM_LOCATION)?1:0;

  //delay long enough to allow inputstates to be set properly
  Delay_ms(DEBOUNCE_TIME+10);

  //is alarm disable button pressed at startup?  this will allow enable/disable of the change beep feature
  if (v_InputState[ALARM_DISABLE_BUTTON])
    {
      //red and green on, blue shows change beep status
      v_Leds[GREEN_LED]=ON;
      v_Leds[RED_LED]=ON;
      v_Leds[BLUE_LED]=DoorChime;

      //wait for user to release button
      while(v_InputState[ALARM_DISABLE_BUTTON])
        ;
      clearinputs();

      //loop until they hold the button to exit
      while(!v_InputHeld[ALARM_DISABLE_BUTTON])
        {
          if (v_InputPressed[ALARM_DISABLE_BUTTON])
            {
              ATOMIC_BLOCK(ATOMIC_FORCEON)
                {
                  v_InputPressed[ALARM_DISABLE_BUTTON]--;
                }

              //toggle change beep
              DoorChime=!DoorChime;

              //indicate current value in blue led
              v_Leds[BLUE_LED]=DoorChime;

              //beep once when enabled
              if (DoorChime)
                beep(CHANGE_BEEP_LENGTH,1);

              //write setting to eeprom immediately in case they power cycle and don't hold the button to exit
              eeprom_write_byte(DOOR_CHIME_EEPROM_LOCATION,DoorChime);
            }
        }

      //clear leds
      //I could have used a loop here, but it comes to the same bytes and this way eliminates a local variable on the stack
      v_Leds[GREEN_LED]=OFF;
      v_Leds[RED_LED]=OFF;
      v_Leds[BLUE_LED]=OFF;
    }

	//clear inputs
  clearinputs();

  //before main loop we set garage door status (open, closed)
  LastState=ThisState=v_InputState[GARAGE_SENSOR];
  if (!ThisState)
    {
      ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
          GarageOpenTime=v_RunningTime;
        }
    }
  else GarageOpenTime=0;  
  AlarmDelayTime=GARAGE_DOOR_LEFT_OPEN_FIRST_BEEP;

  //enable watchdog timer 15ms
  wdt_enable(WDTO_120MS);

  //main loop
  for(;;)
    {
      wdt_reset();
      
      //get runningtime in ul1
      ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
          ul1=v_RunningTime;
        }

      ThisState=v_InputState[GARAGE_SENSOR];

      //state change
      if (ThisState!=LastState)
        {
          //if change beep is enabled
          if (DoorChime)
            {
              if (ThisState)
                beep(CHANGE_BEEP_LENGTH,CHANGE_BEEP_OPENED_DOOR);
              else beep(CHANGE_BEEP_LENGTH,CHANGE_BEEP_CLOSED_DOOR);
            }

          //if garage door has been opened, keep a timestamp and set default delay time  
          if (!ThisState)
            {
              GarageOpenTime=ul1;
              AlarmDelayTime=GARAGE_DOOR_LEFT_OPEN_FIRST_BEEP;
            }

          LastState=ThisState;
        }

      //has garage door been opened too long?  
      if (!ThisState && ul1-GarageOpenTime>=AlarmDelayTime)
        {
          //alarm
          beep(GARAGE_DOOR_LEFT_OPEN_BEEP_LENGTH,GARAGE_DOOR_LEFT_OPEN_BEEPS);

          //reset garageopentime
          GarageOpenTime=ul1;

          //set subsequent delay time
          AlarmDelayTime=GARAGE_DOOR_LEFT_OPEN_SUBSEQUENT_BEEPS;
        }

      //update leds
      if (ThisState)
        {
          //door closed, green led on
          v_Leds[GREEN_LED]=ON;
          v_Leds[RED_LED]=OFF;
        }
      else
        {
          //door opened, red led blinking
          v_Leds[GREEN_LED]=OFF;
          v_Leds[RED_LED]=BLINKING;
        }

      //enable alarm disable feature
      if (v_InputPressed[ALARM_DISABLE_BUTTON])
        {
          ATOMIC_BLOCK(ATOMIC_FORCEON)
            {
              v_InputPressed[ALARM_DISABLE_BUTTON]--;
            }

          //disable beep output
          v_BeepDisabled=ON;

          //blink alarm disable led
          v_Leds[BLUE_LED]=BLINKING;

          //each press puts alarmdisabledtime ALARM_DISABLE_PRESS_DELAY ahead until a maximum of ALARM_DISABLE_PRESS_DELAY_MAXIMUM
          if (!AlarmDisabledTime)
            AlarmDisabledTime=ul1;
          AlarmDisabledTime+=ALARM_DISABLE_PRESS_DELAY;
          if (AlarmDisabledTime-ul1>ALARM_DISABLE_PRESS_DELAY_MAXIMUM)
            AlarmDisabledTime=ul1+ALARM_DISABLE_PRESS_DELAY_MAXIMUM;
        }

      //cancel alarm disable feature
      if ((v_BeepDisabled && ul1>=AlarmDisabledTime) || (v_InputHeld[ALARM_DISABLE_BUTTON]))
        {
          //reset held count
          v_InputHeld[ALARM_DISABLE_BUTTON]=0;

          //reenable beep output
          v_BeepDisabled=OFF;

          //turn out alarm disable led
          v_Leds[BLUE_LED]=OFF;

          //reset alarm disabled time
          AlarmDisabledTime=0;
        }
    }
}

ISR(TIM0_COMPA_vect) //ISR for CTC Mode executed every 1ms
  {
    static unsigned int RunningTimeM=0,BeepPosition;
    static unsigned long RunningTimeS=0;
    static unsigned char InputDebounceCount[NO_OF_INPUTS]={0,0},
                         InputLast[NO_OF_INPUTS]={0,0},
                         InputSkipRelease[NO_OF_INPUTS]={0,0};
    static unsigned int InputStateAge[NO_OF_INPUTS]={0,0};
    unsigned char c1,c2,c3;

    //runningtime is the tenths of seconds since startup, useful for comparing time between events
    RunningTimeM++;
    if (RunningTimeM>=1000)
      {
        RunningTimeS++;
        RunningTimeM-=1000;
      }
    v_RunningTime=RunningTimeS*10+RunningTimeM/100;

    //debounce inputs, also handle InputPressed and InputHeld
    c3=PINB;
    for (c1=0;c1<NO_OF_INPUTS;c1++)
      {
        c2=((c3 & _BV(c1))?0:1);

        if (InputLast[c1]==c2)
          InputDebounceCount[c1]++;
        else
          {
            InputLast[c1]=c2;
            InputDebounceCount[c1]=1;
          }
        if (v_InputState[c1]!=InputLast[c1] && InputDebounceCount[c1]>=DEBOUNCE_TIME)
          {
            v_InputState[c1]=InputLast[c1];
            InputStateAge[c1]=0;

            if (!v_InputState[c1])
              {
                if (!InputSkipRelease[c1] && v_InputPressed[c1]<255)
                  v_InputPressed[c1]++;
                InputSkipRelease[c1]=0;
              }
          }
        else InputStateAge[c1]++;

        if (v_InputState[c1] && ((!InputSkipRelease[c1] && InputStateAge[c1]>=INPUT_HOLD_DELAY)
            || (InputSkipRelease[c1] && InputStateAge[c1]>=INPUT_HOLD_REPEAT_DELAY)))
          {
            InputSkipRelease[c1]=1;
            InputStateAge[c1]=0;
            if (v_InputHeld[c1]<255)
              v_InputHeld[c1]++;
          }
      }

    //handle beeper
    switch(v_BeepMode)
      {
        case BEEPMODE_REQUEST : v_BeepMode=BEEPMODE_RUNNING;
                                nextbeep:
                                BeepPosition=0;
                                if (!v_BeepDisabled)
                                  PORTA|=_BV(BEEPER);
                                break;
        case BEEPMODE_RUNNING : BeepPosition++;
                                if (BeepPosition==v_BeepLen)
                                  PORTA&=~_BV(BEEPER);
                                else
                                if (BeepPosition==v_BeepLen*2)
                                  {
                                    if (--v_BeepsRemaining)
                                      goto nextbeep;
                                    v_BeepMode=BEEPMODE_READY;
                                  }
                                break;
        case BEEPMODE_CANCEL  : PORTA&=~_BV(BEEPER);
                                v_BeepMode=BEEPMODE_READY;
                                break;
      }

    //handle leds
    for (c1=0;c1<NO_OF_OUTPUTS;c1++)
      {
        //skip beeper
        if (c1==BEEPER)
          continue;

        if (v_Leds[c1]==ON || (v_Leds[c1]==BLINKING && v_RunningTime%10<5))
          PORTA|=_BV(c1);
        else PORTA&=~_BV(c1);
      }
  }

