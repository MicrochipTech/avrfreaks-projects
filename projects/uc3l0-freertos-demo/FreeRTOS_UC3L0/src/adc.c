/********************************************************************
	created:	2012/03/05
	created:	5:3:2012   18:35
	filename: 	FreeRTOS_UC3L0\src\adc.c
	file path:	FreeRTOS_UC3L0\src
	file base:	adc
	file ext:	c
	author:		Dustin Sanders
	
	purpose:	ADC Data Handling Task
*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <asf.h>
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"
#include "serial.h"
#include "gloabals.h"
#include "menu.h"

#define BUFF_MAX 40
static volatile U32 adc_buff[BUFF_MAX];
static volatile U16 adc_ch4_avg;
static volatile U16 adc_ch0_avg;
static volatile U16 adc_ch4_avg_pt;
static volatile U16 adc_ch0_avg_pt;
static volatile U16 ustaskcnt = 0;
static volatile portTickType xDelay = ADC_TASK_DELAY / portTICK_RATE_MS;
static volatile xTaskHandle xADCTask;

const char ucADC_SET[] = menuADCSET;
const char ucADC_DEV[] = menuDEVICES menuADCDEVS;
const char ucADC4LABEL[] = "ADC4: NTC Temperature Sensor";
const char ucADC0LABEL[] = "ADC0: RC Filter";
const char ucADCFLABEL[] = "ADC Task Update Delay";

portBASE_TYPE lADCInit( void )
{
	long mainADCERR = 0;
    
	adcifb_opt_t adc_opt = 
    {
    .resolution             = ADC_REZ,
    .shtim                  = ADC_SAMPLEHOLDTM, // Sample and Hold Time, based on Z_adcin and Z_src
    .ratio_clkadcifb_clkadc = ADC_CLKPRESCALE,  // Clock prescaler
    .startup                = ADC_STARTUPTM,  // ADC Startup Time (in ADC clocks)
    .sleep_mode_enable      = ADC_SLEEPMODE,   // No sleep for the wicked
    };
	
	//Assign GPIO Pins
    mainADCERR |= gpio_enable_module_pin(AVR32_ADCIFB_AD_4_PIN, 
	                    AVR32_ADCIFB_AD_4_FUNCTION);
	mainADCERR |= gpio_enable_module_pin(RC_FILTER_ADC_PIN,
	                    RC_FILTER_ADC_FUNCTION);
	
	// Setup Clock    
    sysclk_enable_peripheral_clock(&AVR32_ADCIFB);
    
	// Configure ADC
	mainADCERR |= adcifb_configure(&AVR32_ADCIFB, &adc_opt);
    
	mainADCERR |= adcifb_configure_trigger(&AVR32_ADCIFB, 
	                    AVR32_ADCIFB_TRGR_TRGMOD_PT, 0x0fff);
	
	//Enable Channels
	adcifb_channels_enable(&AVR32_ADCIFB, 
	                        AVR32_ADCIFB_CH4_MASK | AVR32_ADCIFB_CH0_MASK);

	// Will be non-zero upon Error
	return mainADCERR;
}

portBASE_TYPE lADC_PDCAInit( void )
{
	// Sets up PDCA for ADC transfer using a ring buffer
	// TODO: Setup defines for these parameters
	pdca_channel_options_t pdca_adc_opt = 
    {
	.addr               = (void *)adc_buff,  
	.pid                = AVR32_PDCA_PID_ADC_RX, 
	.size               = sizeof(adc_buff)/sizeof(U32),
	.r_addr             = NULL,				
	.r_size             = 0, 
	.transfer_size      = PDCA_TRANSFER_SIZE_WORD,
    };
	
	//Init PDC Channels
	pdca_init_channel(PDCA_ADC_CHANNEL, &pdca_adc_opt);
	
	//Enable Ring Buffer
	//pdca_get_handler(PDCA_ADC_CHANNEL)->mr |= AVR32_PDCA_MR_RING_MASK;
	
    // Turn on PDCA Channels	
	pdca_enable(PDCA_ADC_CHANNEL);
	
	return errNONE;
}

portBASE_TYPE lADCStartHandler( unsigned portBASE_TYPE uxPriority )
{
	portBASE_TYPE lADCErr = 0;
	// TODO: Add #if #else for configDBG
	if(lADCInit()) { portDBG_TRACE( "ADC FAILED" ); }
	else{ portDBG_TRACE( "...ADC Channels Ready." ); }
		
	if(lADC_PDCAInit()) { portDBG_TRACE( "ADC's PDCA FAILED" ); }
	else{ portDBG_TRACE( "...ADC's PDCA Channels Ready." ); }
		
	#if configDBG
	if((lADCErr = xTaskCreate(vADCHandler, (signed char *) "ADC",
	    configMINIMAL_STACK_SIZE, NULL, uxPriority, xADCTask )) != pdPASS)
    {
	    portDBG_TRACE( "ADC Task Failed");	
	}			
	portDBG_TRACE( "...ADC Task Ready." );
	#else
	lADCErr = xTaskCreate(vADCHandler, (signed char *) "ADC",
	    configMINIMAL_STACK_SIZE, NULL, uxPriority, xADCTask );
	#endif 
	return lADCErr;
}

void vADCHandler( void *pvParameters )
{
	// Temp loop var
	portBASE_TYPE i;
	unsigned long c0 = 0, c4 = 0;
	unsigned long tch0, tch4;
	

	// Clear buffers for initial use
    for (i = 0; i < BUFF_MAX; i++)
    {
		adc_buff[i] = 0;
    }
	adc_ch0_avg = 0;
	adc_ch4_avg = 0;
		 
	//Wait a bit before start'n
	vTaskDelay( 2000 / portTICK_RATE_MS );
	 
	//Start Task Loop
	for(;;)
	{
		
		//vTaskSuspend( void );
		vTaskDelay( xDelay );

		// Save last avg, we'll use this later.
		adc_ch0_avg_pt = adc_ch0_avg;
		adc_ch4_avg_pt = adc_ch4_avg;
		// Reset these guys
		tch0 = 0;
		tch4 = 0;
		c0 = 0; 
		c4 = 0; 
		
		// Separate channel values and ADD
		for (i = 0; i < BUFF_MAX; i++)
		{
		    switch( (adc_buff[i] & AVR32_ADCIFB_LCCH_MASK) >> 
			                                    AVR32_ADCIFB_LCCH_OFFSET )
			{
				case AVR32_ADCIFB_CH0:
				    tch0 += (adc_buff[i] & 0x0FFF);
					c0++;
					break;
				case AVR32_ADCIFB_CH4:
				    tch4 += (adc_buff[i] & 0x0FFF); 
					c4++;
				    break;
				default:
				    ustaskcnt = 0; // something terribly wrong happened
			}
		}
		
		// Lets avoid div by 0
		if (c0)
		{    
            adc_ch0_avg = (U16)(tch0 / c0);
		}
		if (c4)
		{   
            adc_ch4_avg = (U16)(tch4 / c4);
		}
		
		ustaskcnt++;
		
		pdca_reload_channel(PDCA_ADC_CHANNEL, (void*)adc_buff,
		    sizeof(adc_buff)/sizeof(U32));
			
	}    	
}

portBASE_TYPE lIsADCHandlerRunning( void )
{
	// taskcnt should increment each time the task is run
	//  if not, then its not running, gets cleared each check.
	if (!ustaskcnt)
	{
		return errFAILURE;
	}
	return ustaskcnt = errNONE;
}

void vADCTaskDelay( portBASE_TYPE uxDelayMS )
{
	xDelay = uxDelayMS / portTICK_RATE_MS;
}

portBASE_TYPE lADCHandleMenu( U8 *menuPtr )
{
	volatile float l;
	char strbuff[10];
    
	portDBG_TRACE( "I'm in ADC Menu" );
		switch (*menuPtr)
		{   
            // NTC Temperature Sensor ADC4 Display Only
		    case '1':  
			    l = (((float)adc_ch4_avg * ADC_BITS_V)); 
			    vSerialPutString( gcvtf(l, 4, strbuff) );
				vSerialPutString( " V" CRLF );
				return errNONE;
		    	break;
			// RC Filter ADC0 Display Only
		    case '2':   
			    l = (((float)adc_ch0_avg * ADC_BITS_V)); 
			    vSerialPutString( gcvtf(l, 4, strbuff) );
				vSerialPutString( " V" CRLF );
				return errNONE;
				break;
			// Change ADC Update Frequency (how often it calcs avg)
			// Value should be in ms
			case 'F':
			case 'f':
			// F BEGIN
			    menuPtr++;
				switch (*(menuPtr))
			    {
			        case switchSET:
					menuPtr++;				            
					if ((*(menuPtr) > '0') && (*(menuPtr) <= '9'))
				    {
						l = atoi((char*)menuPtr);
						xDelay = l;
						return errNONE;
				    }					
					// Crap					
					    return errINVALID;  
			    	    break;
				    case switchDISPLAY:
					    vSerialPutString( "ADC Delay is ");
					    vSerialPutLong((unsigned long)xDelay, -1);
					    vSerialPutString("ms");
					    return errNONE;
					    break;
				    case switchHELP:
				    case 0:
					    pdca_reload_channel(PDCA_USART_CHANNEL,
						    ( void* )ucADC_SET, sizeof(ucADC_SET));
					    break;
				    default:
					    return errINVALID;
					    break;					    	
			    }
			    break;
			case 0:
			case switchHELP:
			    pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucADC_DEV,
				    sizeof(ucADC_DEV));
				break;
		    default:
			    return errINVALID;
				break;
		}
		return errNONE;
}

/************************************************************************/
/* ADCHandleDisplay
 * Simply prints formated ADC Values to Terminal
 * For use with the Display option                          
 *                                                                      */
/************************************************************************/

void vADCHandleDisplay( unsigned long dev )
{
    char strbuff[10];
	float f;

	static char ucADCSTR[150];
	
	ucADCSTR[0] = '\0';
	strcat(ucADCSTR, CRLF); // Put a space at the beginning to separate data
	
    if( dev & (DISPLAY_ADC_RCF | DISPLAY_ADC_ALLCH | DISPLAY_ADC_ALL ))
	{	
	    f = (float)adc_ch0_avg * ADC_BITS_V;
	    strcat(ucADCSTR, ucADC0LABEL);
		strcat( ucADCSTR, 
		    cpRightJust( gcvtf(f, 4, strbuff), strlen(ucADC0LABEL), COL1));
	    strcat(ucADCSTR, " V" CRLF);
	}
	if( dev & ( DISPLAY_ADC_TEMP | DISPLAY_ADC_ALLCH | DISPLAY_ADC_ALL ))
	{	    
		f = (float)adc_ch4_avg * ADC_BITS_V;
	    strcat(ucADCSTR, ucADC4LABEL);
		strcat( ucADCSTR, 
		    cpRightJust( gcvtf(f, 4, strbuff), strlen(ucADC4LABEL), COL1)); 
	    strcat(ucADCSTR, " V" CRLF);
	}
	if ( dev & ( DISPLAY_ADC_FREQ | DISPLAY_ADC_ALLCH | DISPLAY_ADC_ALL ) )
	{
		strcat(ucADCSTR, ucADCFLABEL);
		strcat( ucADCSTR, 
		    cpRightJust(  itoa( (int)xDelay ), strlen(ucADCFLABEL), COL1));
	    strcat(ucADCSTR, " ms" CRLF);
	}
	
	strcat(ucADCSTR, "\0"); //Make sure there's a terminating char at the end
			
	//Wew, now we send it via PDCA
	// Wait for pdca reload channel to become available 
	while (!(AVR32_PDCA.channel[PDCA_USART_CHANNEL].sr 
		& AVR32_PDCA_ISR_RCZ_MASK));
	pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucADCSTR,
	    sizeof(ucADCSTR));  
}