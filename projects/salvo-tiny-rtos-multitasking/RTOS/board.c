/*****************************************************************
//ICC-AVR application builder : 2/15/2013 11:46:31 AM
// Target : M16
// Crystal: 14.7456Mhz
// NOTE: WHILE WE STRIVE TO GENERATE CORRECT CODE FOR ALL SUPPORTED DEVICES,
//   THE APPBUILDER MAY GENERATE INCORRECT CODE AT TIMES. IF YOU FIND ANY
//   ISSUES. PLEASE EMAIL support@imagecraft.com. THANK YOU
******************************************************************/
#include "board.h"

//Globals


//*************************************************
// cpu sleeps here when no tasks are active
//*************************************************
void OSIdlingHook(void)
{
    SLEEP();
}

//****************************************
//Port initialization
//****************************************
void port_init(void)
{
//                   7 6 5 4 3 2 1 0
    PORTB = 0x00;// |N|N|N|N|N|N|N|N| Active Pullups
    DDRB  = 0xFF;// |O|O|O|O|O|O|O|O|
    PORTC = 0x00;// |N|N|N|N|N|N|N|N|
    DDRC  = 0x00;// |I|I|I|I|I|I|I|I|
    PORTD = 0x00;// |N|N|N|N|N|N|N|N|
    DDRD  = 0x00;// |I|I|I|I|I|I|I|I|
}

//TIMER2 initialize - prescale:1024
// WGM: Normal
// desired value: 100Hz
// actual value: 100.699Hz (0.7%)
void timer2_init(void)
{
 TCCR2 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT2 = 0x71; //setup
 OCR2  = 0x8F;
 TCCR2 = 0x07; //start
}

#pragma interrupt_handler timer2_ovf_isr:iv_TIM2_OVF
void timer2_ovf_isr(void)
{
 TCNT2 = 0x71; //reload counter value
/************************************************************
Active ISRs.
Timer1: Interrupt happens at predefined system tick rate,
          calls OSTimer(). Since this ISR calls a Salvo
          service, it (and all other ISRs that call Salvo
          services) must be disabled by Salvo's interrupt
          hooks.
************************************************************/
    OSTimer();
}

//*************************************************
// init all devices
//*************************************************
//call this routine to initialize all peripherals
void panel_init(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts
 port_init();
 timer2_init();

 MCUCR = 0x40; //enable idle sleep mode
 GICR  = 0x00;
 TIMSK = 0x40; //timer interrupt sources
 //SEI(); //re-enable interrupts
 //all peripherals are now initialized
}


