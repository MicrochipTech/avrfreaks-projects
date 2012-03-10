/*
 * pwmctrl.c
 *
 * Created: 3/5/2012 10:51:30 PM
 *  Author: Wedge
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <asf.h>
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"
#include "gloabals.h"
#include "pwmctrl.h"
#include "menu.h"
#include "board.h"

static volatile U8 pwmDuty[] = {0,0,0};
const char ucPWM_VALS[] = menuVALUE menuPWMVALS;
const char ucPWM_DEV[] = menuDEVICES menuPWMDEVS;
const char dutystr[] = "Duty Cycle is: ";
const char ucPWM_ACTIONS[] = menuACTIONS menuPWR menuASET menuADISP;
const char ucPWM_FREQ_VALS[] = menuVALUE
	"Equation: PWMf = PBAf / 2*(div + 1)" CRLF                       
	"Enter either 8-bit div value or desired frequency < 2.5MHz" CRLF;
const char ucPWM_FREQ_ACT[] = menuACTIONS menuASET menuADISP;
const char ucPWM_FREQ_LBL[] = "Current Frequency: ";
const char ucPWMRLEDLABEL[] = "PWM13: Red LED Duty Cycle";
const char ucPWMBLEDLABEL[] = "PWM20: Blue LED Duty Cycle";
const char ucPWMGLEDLABEL[] = "PWM17: Green LED Duty Cycle";
const char ucPWMFLABEL[] =  "PWM Operating Frequency";

/************************************************************************/
/* xHandlePWM
 * Handles the devices able to use PWM as well as the frequency
 * of the pwma module.
 * required functions: xDoPWMWork, vPWMDisplay, xHandlePWMFreq
 * returns: errNONE: 0, errFAILURE: -1, errINVALID: -2
 *                                                                      */
/************************************************************************/
// TODO: Redo this whole thing!
portBASE_TYPE lPWMHandleMenu( U8 *menuPtr )
{
	long pwmValue = 0;
	
	// Check if there's a value real fast
    if((*(menuPtr+2) > '0') && (*(menuPtr+2) <= '9'))
	{
		pwmValue = atoi((char*)(menuPtr + 2));
	}		

	switch (*(menuPtr))
	{
		case '1' :  // Red LED/RC Filter
			menuPtr++;
		    return lPWMHandleMenuActions(menuPtr, PWM_CHANNEL_RED, pwmValue);			
			break;
		case '2' :  // Blue LED
			menuPtr++;
			return lPWMHandleMenuActions(menuPtr, PWM_CHANNEL_BLUE, pwmValue);				    
			break;
		case '3' :  // Green LED
			menuPtr++;
			return lPWMHandleMenuActions(menuPtr, PWM_CHANNEL_GREEN, pwmValue);				
			break;
		case '4' :  // All PWM Channels
			menuPtr++;
			return lPWMHandleMenuActions(menuPtr, PWM_CHANNEL_RGB, pwmValue);				
			break;
		case 'F' :  // Change PWM Frequency
		case 'f' :
			menuPtr++;    
			return lPWMHandleMenuFreq(menuPtr, pwmValue);	
			break;
		// Prints List of PWM Devices
		case switchHELP :
		case 0 :
			pdca_reload_channel(PDCA_USART_CHANNEL, 
				( void* )ucPWM_DEV, sizeof(ucPWM_DEV));
			break;
		case switchCANCEL :
			break;
		default:
			return errINVALID;
			break;
	} 
	return errNONE;				
}

// TODO: DOCUMENT ME MORE, USE DOXYGEN!
/************************************************************************/
/* lPWMHandleMenuActions
 * Handles the actions for each PWM Device (excludes Frequency Change
 * Takes the pointer the input, pwm action acting on, and pwm value
 * pwm value is check prior and set to 0 if there was an error reading
 * this value is updated when required (only on a SET)
 *                                                                      */
/************************************************************************/
portBASE_TYPE lPWMHandleMenuActions( U8 *menuPtr, portBASE_TYPE CH, portBASE_TYPE pwmValue )
{
	long chtoduty = 0;
	int x;

	switch(CH)
	{
		case PWM_CHANNEL_RED:   chtoduty = 0;
		    break;
		case PWM_CHANNEL_BLUE:  chtoduty = 1;
		    break;
		case PWM_CHANNEL_GREEN: chtoduty = 2;
		    break;
		case PWM_CHANNEL_RGB :  chtoduty = 0;
		    break;
		default:
		    return errFAILURE;
	}

	switch (*menuPtr)
	{
		case switchSET :
			// Missing Value, use disable cmd if wish to turn off LED 
			if (!pwmValue)    
			{
 				pdca_reload_channel(PDCA_USART_CHANNEL, 
					( void* )ucPWM_VALS, sizeof(ucPWM_VALS));
 				return errINVALID;
			}
			if (CH == PWM_CHANNEL_RGB) // All LEDs
			{
				// change all duty cycles
				for (x = 0; x < 3; x++)
				{
					pwmDuty[x] = pwmValue & 0x0FE;
				}
			} 
			else  // Set only one channel's duty cycle
			{
				// Duty register is only 8 bits
				pwmDuty[chtoduty] = pwmValue & AVR32_PWMA_DUTY0_MASK; 
			} 
			SET:
 			pwma_set_channels_value(&AVR32_PWMA, CH, pwmDuty[chtoduty]);
			break;
		case switchDISABLE :
		    // Just set to 0, duty is saved when setting value
			pwma_set_channels_value(&AVR32_PWMA, CH, 0);
 			break;
		case switchENABLE :
			goto SET;  // Just using the code above since it does what we want 
 			break;
		case switchDISPLAY :
			pdca_reload_channel(PDCA_USART_CHANNEL, 
			    ( void* )dutystr, sizeof(dutystr));
			// Prints the duty cycle
			vSerialPutString( itoa(pwmDuty[chtoduty]) );
			vSerialPutString(CRLF);
			break;
		case switchHELP :
		case 0 :
			//Print Action Help
			pdca_reload_channel(PDCA_USART_CHANNEL, 
			    ( void* )ucPWM_ACTIONS, sizeof(ucPWM_ACTIONS));
		    break;
		case switchCANCEL :
			break;
		default: 
			return errINVALID; 
			break;   	
	}
	return errNONE;			
}

/************************************************************************/
/* xHandlePWMFreq
 * Can only display and set frequency
 *                                                                      */
/************************************************************************/
portBASE_TYPE lPWMHandleMenuFreq( U8 *menuPtr, portBASE_TYPE pwmValue )
{
	volatile unsigned long x;

	    switch (*menuPtr)
	    {
	    case switchDISPLAY :
	        //vSerialPutString("Current Frequency: ");
			pdca_reload_channel(PDCA_USART_CHANNEL, 
			        ( void* )ucPWM_FREQ_LBL, sizeof(ucPWM_FREQ_LBL));
		    // PWMf = PBAf/ 2*(div+1) Per documentation (13.6.24)
	        x = sysclk_get_pba_hz()/(2*(AVR32_SCIF.gcctrl[AVR32_PM_GCLK_GCLK3] >> 
			        AVR32_SCIF_GCCTRL_DIV_OFFSET)+1);
		    vSerialPutString(itoa(x));
		    vSerialPutString( "Hz" CRLF );
		    break; 
	    case switchSET :
	        if (pwmValue == switchHELP)  // frequency div can be 0.
		    {
 			    pdca_reload_channel(PDCA_USART_CHANNEL, 
 				    ( void* )ucPWM_FREQ_VALS, sizeof(ucPWM_FREQ_VALS));
				return errNONE;   
		    }
			//4910Hz - 2.4MHz with pba config 
	        // Turn off GCLK3
			if(pwmValue > 254)
			{
				// Figure the div based on a frequency
				// (Pbaf/pwmf*2)-1 = div
				 pwmValue = (sysclk_get_pba_hz()/(2*pwmValue))-1;
				 if(pwmValue > 254 || pwmValue < 1)
				 {
					 return errINVALID;
				 }
			}
            // Disable GCLK3 first
	        AVR32_SCIF.gcctrl[AVR32_PM_GCLK_GCLK3] &= !AVR32_SCIF_GCCTRL_CEN_MASK;  
		    // Set new value then re-enable  
            AVR32_SCIF.gcctrl[AVR32_PM_GCLK_GCLK3] = 
			    (pwmValue << AVR32_SCIF_GCCTRL_DIV_OFFSET) | 
		        AVR32_SCIF_GCCTRL_DIVEN_MASK | AVR32_SCIF_GCCTRL_CEN_MASK | 
				(SCIF_GCCTRL_PBACLOCK << AVR32_SCIF_GCCTRL_OSCSEL_OFFSET);				
		    break;
		case switchHELP :
		case 0 :
		    pdca_reload_channel(PDCA_USART_CHANNEL, 
				( void* )ucPWM_FREQ_ACT, sizeof(ucPWM_FREQ_ACT));	
			break;
		case switchCANCEL :
			break;
		default:
		    return errINVALID;
		    break;
	    }
		return errNONE;
}

portBASE_TYPE lPWMInit( void )
{
    long mainPWMERR = 0;
	// GPIO pin/function map for the RGB LEDs.
    mainPWMERR |= gpio_enable_module_pin(LED_RED_PWMA,   LED_PWMA_FUNCTION);
    mainPWMERR |= gpio_enable_module_pin(LED_GREEN_PWMA, LED_PWMA_FUNCTION);
    mainPWMERR |= gpio_enable_module_pin(LED_BLUE_PWMA,  LED_PWMA_FUNCTION);
    
    const scif_gclk_opt_t genclk3_opt = {
        .clock_source = SCIF_GCCTRL_PBACLOCK,
        .divider      = PWM_CLK_DIV,
        .diven        = true,
    };

    // Start generic clock 3 for the PWM outputs.
    mainPWMERR |= scif_start_gclk(AVR32_PM_GCLK_GCLK3, &genclk3_opt);
    
    // Enable RGB LED PWM.
    sysclk_enable_peripheral_clock(&AVR32_PWMA);
    pwma_config_and_enable(&AVR32_PWMA, PWM_CHANNEL_RED | PWM_CHANNEL_BLUE
            | PWM_CHANNEL_GREEN, 255, PWM_RBG_INIT_STATE);
			
	return mainPWMERR;
}

void vPWMHandleDisplay( unsigned long dev )
{
	unsigned long x;
	static char ucPWMSTR[250];
	
	ucPWMSTR[0] = '\0';  // Make sure strcat behaves
	strcat( ucPWMSTR, CRLF); // Space at the beginning to separate data
	
	if ( dev & ( DISPLAY_PWM_REDLED | DISPLAY_PWM_ALLLEDS | DISPLAY_PWM_ALL ) )
	{
		strcat( ucPWMSTR, ucPWMRLEDLABEL );
		// Justify data value to COL1
		strcat( ucPWMSTR, 
		    cpRightJust(itoa( pwmDuty[PWM_CHANNEL_RED_DUTY] ), 
			strlen(ucPWMRLEDLABEL), COL1));
	    strcat( ucPWMSTR, "/255" CRLF );		
	}
	if ( dev & ( DISPLAY_PWM_GREENLED | DISPLAY_PWM_ALLLEDS | DISPLAY_PWM_ALL ) )
	{
		strcat( ucPWMSTR, ucPWMGLEDLABEL );
	    strcat( ucPWMSTR, 
		    cpRightJust(itoa( pwmDuty[PWM_CHANNEL_GREEN_DUTY] ), 
			strlen(ucPWMGLEDLABEL), COL1)); 
	    strcat( ucPWMSTR, "/255" CRLF );		
	}
	if ( dev & ( DISPLAY_PWM_BLUELED | DISPLAY_PWM_ALLLEDS | DISPLAY_PWM_ALL ) )
	{
		strcat( ucPWMSTR, ucPWMBLEDLABEL );
	    strcat( ucPWMSTR, 
		    cpRightJust(itoa( pwmDuty[PWM_CHANNEL_BLUE_DUTY] ), 
			strlen(ucPWMBLEDLABEL), COL1));
	    strcat( ucPWMSTR, "/255" CRLF );		
	}
	if ( dev & ( DISPLAY_PWM_FREQ | DISPLAY_PWM_ALLLEDS | DISPLAY_PWM_ALL ) )
	{
		x = sysclk_get_pba_hz()/(2*(AVR32_SCIF.gcctrl[AVR32_PM_GCLK_GCLK3] >> 
			        AVR32_SCIF_GCCTRL_DIV_OFFSET))+1;
		strcat( ucPWMSTR, ucPWMFLABEL );
	    strcat( ucPWMSTR, 
		    cpRightJust(itoa( x ), strlen(ucPWMFLABEL), COL1));
		//strcat( ucPWMSTR, itoa( x ) ); 
	    strcat( ucPWMSTR, "Hz" CRLF );		
	}
	
	//Make sure there's a terminating char at the end
	strcat( ucPWMSTR, "\x0" ); 
	//Wew, now we send it via PDCA
	// Wait for pdca reload channel to become available 
	while (!(AVR32_PDCA.channel[PDCA_USART_CHANNEL].sr 
	    & AVR32_PDCA_ISR_RCZ_MASK));
	pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucPWMSTR,
	    sizeof(ucPWMSTR));	
}
