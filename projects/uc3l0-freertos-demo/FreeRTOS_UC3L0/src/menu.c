/********************************************************************
	created:	2012/03/04
	created:	4:3:2012   2:44
	filename: 	FreeRTOS_UC3L0\src\menu.c
	file path:	FreeRTOS_UC3L0\src
	file base:	menu
	file ext:	c
	author:		Dustin Sanders
	
	purpose:	Task for the USART Terminal for board control
*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <asf.h>
#include "FreeRTOS.h"
#include "task.h"
#include "menu.h"
#include "serial.h"
#include "gloabals.h"
#include "adc.h"
#include "pwmctrl.h"


/************************************************************************/
/* Main Menu Variables                                                                     */
/************************************************************************/

// Used for current menu item
//static volatile U8 menuItem = 0; 
                 
// Main Menu Task Handle
xTaskHandle menuTermHandle;
//xTaskHandle menuDataHandle;

xComPortHandle xPort = NULL; //useless, but keeps code generic

/************************************************************************/
/* Menu Help Text                                                       */
/************************************************************************/
// TODO: Clean up this menu!
const char ucDISPLAYHELP[] =  CLEARSCR BOLD UNDERLINE 
    " Main Functions \t\t Actions" NORMAL CRLF
	" " menuHELP " : This Menu\t\t\t" menuAENA
	" " menuPWM  " : PWM Control\t\t" menuADISA
	" " menuADC  " : ADC Control\t\t" menuADISP
	" " menuDATA " : Display System Data\t" menuASET
	" Command Structure: <cmd><dev><action>(value)" CRLF;

const char ucTIMEOUT[] = BOLD CL_RED "Check your keyboard!" NORMAL CRLF;

const char ucINVALID[] = menuINVALID; 

const char ucFWVERSION[] = "Firmware Version"; 
const char ucFRTOSVER[] = "FreeRTOS Kernel Version";
const char ucCPUSPD[] = "Main Clock Speed";
const char ucPBASPD[] = "PBA Clock Speed";
const char ucADCSPD[] = "ADCIFB Clock Speed";
const char ucDATAHELP[] = UNDERLINE BOLD " Data Display Help" NORMAL CRLF 
	    "s then p/a/i and #" CRLF;


/************************************************************************/
/* Menu  Functions                                                      */
/************************************************************************/

portBASE_TYPE lMenuStartTerminal( unsigned portBASE_TYPE uxPriority )
{
	long lmenuErr = 0;
      // Initialize USART Hardware 
    xSerialPortInitMinimal((unsigned long)57600, menuQUEUESIZE);
	
    // Sets up PDCA for USART
	pdca_channel_options_t pdca_opt = 
    {
	.addr               = (void *)ucDISPLAYHELP,  
	.pid                = PDCA_TERMINAL_PID, 
	.size               = sizeof(ucDISPLAYHELP),
	.r_addr             = NULL,				
	.r_size             = 0, 
	.transfer_size      = PDCA_TRANSFER_SIZE_BYTE,
    };
	
	pdca_init_channel(PDCA_USART_CHANNEL, &pdca_opt);
	  
	#if configDBG
	if((lmenuErr = xTaskCreate(vMenuHandleTerminal, (signed char *) "Menu",
	    configMINIMAL_STACK_SIZE, NULL, uxPriority, 
		&menuTermHandle )) != pdPASS)
    {
	    portDBG_TRACE( CL_RED BOLD 
		    "!!!--Termainl Task Failed to Init--!!!" NORMAL);	
	}			
	portDBG_TRACE( "...Terminal Ready." );
	#else
	lmenuErr = xTaskCreate(vMenuHandleTerminal, (signed char *) "Menu",
	    configMINIMAL_STACK_SIZE, NULL, uxPriority, 
		&menuTermHandle );
	#endif
	 
	return lmenuErr;
}

portBASE_TYPE lMenuHandleData( U8 *menuPtr )
{
	static unsigned long ulDataPram = 0;
	unsigned long x;

	switch (*menuPtr)
	{   
        // Prints all sensor data 
		case  0:
			    
			vPWMHandleDisplay(DISPLAY_EVERYTHING);
			vADCHandleDisplay(DISPLAY_EVERYTHING);
			return errNONE;
			break;
		//Selective display
		case 's':
		case 'S':
			menuPtr++;
			if (*menuPtr) // there's a something there 
			{
				for (x = 0; x < menuQUEUESIZE-3; x++)
				{  
                    switch (*(menuPtr++))
					{
						case switchPWM:
						case (switchPWM | switchLWROFFSET):
						    switch(*(menuPtr++))
							{
								case '1':
									ulDataPram |= DISPLAY_PWM_REDLED;
									break;
								case '2':
									ulDataPram |= DISPLAY_PWM_GREENLED;
									break;
								case '3':
									ulDataPram |= DISPLAY_PWM_BLUELED;
									break;
								case '4':
									ulDataPram |= DISPLAY_PWM_ALLLEDS;
								case 'a':
								case 'A':
									ulDataPram |= DISPLAY_PWM_ALL;
									break;
								case 'f':
								case 'F':
									ulDataPram |= DISPLAY_PWM_FREQ;
									break;
								default:
									goto DATAINVALID;
									break;									    
							}
							break;
						case switchADC:
						case (switchADC | switchLWROFFSET):
							switch (*(menuPtr++))
							{
							    case '1':
									ulDataPram |= DISPLAY_ADC_TEMP;
							    	break;
								case '2':
									ulDataPram |= DISPLAY_ADC_RCF;
								case 'a':
								case 'A':
									ulDataPram |= DISPLAY_ADC_ALL;
									break;
								case 'f':
								case 'F':
									ulDataPram |= DISPLAY_ADC_FREQ;
									break;
						        default:
									goto DATAINVALID;
									break;
							}
						case '0':
							goto DATARUN;
						case switchHELP:
							goto DATAHELP;
					}
				}
			}
			else
			{
                return errINVALID;
			}
			DATARUN:
			vADCHandleDisplay(ulDataPram);
			vPWMHandleDisplay(ulDataPram);
			return errNONE;
			break;
		case 'i':
		case 'I':
			// System Versions (need to add more AVR specific versions)
			// System Clocks
			// FreeRTOS Task List
			vMenuHandleOSDisplay(DISPLAY_EVERYTHING);
			return errNONE;
			break;
		case 'a':
		case 'A': 
			// Auto Data Task
			// asks for data to be included, parses an input such as
			// a1a2p1p2p3i means adc 1 and 2, pwm 1 2 3 and info 
			// (which is everything), or you could just say e for 
			// everything!
			// maybe allow for a mask? m00101100 enables the devices 
			// that masks? add that to future maybe.
			break;  
		case switchHELP:
		DATAHELP:
			pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucDATAHELP,
				sizeof(ucDATAHELP));
			break;
		default:
		DATAINVALID:     
			return errINVALID;
			break;
	}
	return errNONE;
}

void vMenuHandleDataTask( void *pvParameters )
{
	
	//Task goes here
}

void vMenuHandleTerminal( void *pvParameters )
{
	static U8 menuInput[menuQUEUESIZE];
	U8 *menuInputPtr = menuInput; //point to the begining of menuInput
	signed long menuERR = 0;
	portTickType menuBlockTime = menuBLOCKTIME;
	// Temp value for any loops and such
	long i; 
	
	// clear Input buffer
	for ( i=0; i < menuQUEUESIZE; i++ ) { menuInput[i] = 0; }
	
	( void ) pvParameters;
	
	//Get PDC Going
	pdca_enable(PDCA_USART_CHANNEL);
	
	for( ;; )
	{ 
		//START:
		/*Suspend until ISR wakes it upon receiving serialDELIMITER*/
		vTaskSuspend( NULL );

		/************************************************************************/
		/* Get bytes from queue 
		 * While Queue is not empty, keep filling menuInput
		 * menuInput == Queue size                                              */
		/************************************************************************/
		// TODO: Prevent overflow
		while ( xSerialGetChar( xPort, menuInputPtr++, menuBlockTime ) );
		// Reset pointer
		menuInputPtr = menuInput;

		/************************************************************************/
		/* Generic format
		* <command><device><action>[value]; 
		*       where value is optional for some actions                                                     */
		/************************************************************************/
	
		switch (*menuInputPtr)
		{
			// CR was hit w/o char input
		    case 0:
				break;
			case switchHELP :      /* Print Help Menu */
			    pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucDISPLAYHELP,
				    sizeof(ucDISPLAYHELP));
			    break;
		    case switchADC:
		    case (switchADC | switchLWROFFSET) :
                menuERR = lADCHandleMenu(++menuInputPtr);
			    break;
		    case switchPWM :
		    case (switchPWM | switchLWROFFSET) :
			    menuERR = lPWMHandleMenu(++menuInputPtr);
			    break;    
		    case switchDATA :
			case (switchDATA | switchLWROFFSET) : 
			    menuERR = lMenuHandleData(++menuInputPtr);
				break;
			default:   /* Bad Input, display error and reprompt */
				pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucINVALID,
				    sizeof(ucINVALID));
				break;
		}  
		// Where there errors? 
		switch (menuERR)
		{
			case errFAILURE:
			    // Dunno if there'll be a FAILURE but keep the option open
			    menuERR = 0;
				break;
			case errINVALID:
			    pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucINVALID,
				    sizeof(ucINVALID));
				menuERR = 0;
				break;
			case errNONE:
			default:
			    //no err, no big
			    break;
		}
		vSerialPutString( "/" menuPROMPT );
		
		// Clear menuInput for next wakeup
		for ( i = 0; i < menuQUEUESIZE; i++ ) {  menuInput[i] = 0;  }
		//Reset menuInputPtr
		menuInputPtr = menuInput;
	}			
}

portBASE_TYPE lMenuTerminalRunning( void )
{
	// Implemenet sometime.
	return 0;
}

void vMenuHandleOSDisplay( unsigned long dev )
{
	static int8_t ucRTOSTASKS[200];
	static char ucOSSTUFF[200];
	
	// so strcat behaves
	ucRTOSTASKS[0] = '\0';   
	ucOSSTUFF[0] = '\0';
	
	strcat( ucOSSTUFF, CRLF );
    portDBG_TRACE( "Hello from Info Display");
	// Print Versions
	if (dev & DISPLAY_OS_VERSIONS)
	{
		strcat( ucOSSTUFF, ucFWVERSION );
		strcat( ucOSSTUFF, 
		    cpRightJust(VERSION, strlen(ucFWVERSION), COL1) );
		strcat( ucOSSTUFF, CRLF);
		strcat( ucOSSTUFF, ucFRTOSVER );
		strcat( ucOSSTUFF, 
		    cpRightJust(tskKERNEL_VERSION_NUMBER, strlen(ucFRTOSVER), COL1) );
		strcat( ucOSSTUFF, CRLF);
	}
	// Print Clock Speeds
	if (dev & DISPLAY_OS_CLOCKS)
	{
	    strcat( ucOSSTUFF, ucCPUSPD );	
		strcat( ucOSSTUFF, 
		    cpRightJust( itoa(sysclk_get_cpu_hz() ), 
			    strlen(ucCPUSPD), COL1) );    
		strcat( ucOSSTUFF, " Hz" CRLF);
		strcat( ucOSSTUFF, ucPBASPD );	
		strcat( ucOSSTUFF, 
		    cpRightJust( itoa( sysclk_get_pba_hz() ), 
			    strlen( ucPBASPD ), COL1) );    
		strcat( ucOSSTUFF, " Hz" CRLF);
		strcat( ucOSSTUFF, ucADCSPD );	
		strcat( ucOSSTUFF, 
		    cpRightJust( itoa( sysclk_get_peripheral_bus_hz(&AVR32_ADCIFB) ), 
			    strlen(ucADCSPD), COL1) );    
		strcat( ucOSSTUFF, " Hz" CRLF);
	}
	if (dev & (DISPLAY_OS_CLOCKS | DISPLAY_OS_VERSIONS ) )
	{
		// Wait for pdca reload channel to become available 
		while (!(AVR32_PDCA.channel[PDCA_USART_CHANNEL].sr 
		    & AVR32_PDCA_ISR_RCZ_MASK));
	    pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucOSSTUFF,
	        sizeof(ucOSSTUFF));			      
	}
	// Print FreeRTOS Info
	if (dev & DISPLAY_OS_TASKS)
	{
		vTaskList(ucRTOSTASKS);
		// Wait for pdca reload channel to become available 
		while (!(AVR32_PDCA.channel[PDCA_USART_CHANNEL].sr 
		    & AVR32_PDCA_ISR_RCZ_MASK));
	    pdca_reload_channel(PDCA_USART_CHANNEL, ( void* )ucRTOSTASKS,
	        sizeof(ucRTOSTASKS));
	}	    
}

void vMenuHadleHistory( U8 *menuPtr )
{
	// Command history that uses xQueues
	// Implement later
}


