/*
 * gloabals.h
 *
 * Created: 2/26/2012 9:33:46 PM
 *  Author: Wedge
 */ 


#ifndef GLOABALS_H_
#define GLOABALS_H_

#define VERSION                 "v0.3082012"

/************************************************************************/
/* Handy Defines                                                        */
/************************************************************************/
#define CR                      13       //!< Carriage return
#define DEL                     127       //!< Delete Key
#define BS                      8       //!< Backspace Key
#define CUB1                    "\x1B[1D"   //!< Move Curser to the Left 1 Col
#define SAVC                    "\x1B7"     //!< Save Cursor Location
#define RESC                    "\x1B8"     //!< Restore Cursor Location
#define HOME                    "\x1B[;H"   //!< Move cursor to Top Left Corner
#define CUD1                    "\x1B[1B"   //!< Move cursor down 1 line
#define ESC                     "\x1B["      //!< vt100 Esc sequence

//#define serialLED                     LED1    // LED Used when there's USART Activity
#define serialDELIMITER         CR   // We'll use the carriage Return

// Turn on RX Echo
#define serialSERIAL_ECHO       1    

#define menuQUEUESIZE           (unsigned portBASE_TYPE)20 //10 Chars worth
// #define menuCMDSIZE     4
// // For Command Array 
// #define menuCMD         0
// #define menuACT         1
// #define menuDEV         2
// #define menuVAL         3
// 10ms Block time for ISR to exit after waking menu
#define menuBLOCKTIME           ( ( ( portTickType ) 10 ) / portTICK_RATE_MS )

#define menuPROMPT              "> "
#define menuHELP                "?"/*63*/      //!< (?) Question Mark
#define menuADC                 "A"/*65*/      //!< A
#define menuPWM                 "P"/*80*/      //!< P 
#define menuDATA                "D"/*68*/      //!< D
#define menuINFO                "I"/*73*/      //!< I
#define menuSET                 "="/*61*/      //!< (=) Equals - Chane a value
#define menuDISABLE             "-"/*45*/      //!< (-) Hyphen, or Minus - Disables Something
#define menuENABLE              "+"/*43*/      //!< (+) Plus - Enables something
#define menuTOGGLE              "!"/*33*/      //!< (!) Exclamation - Toggles a device
#define menuDISPLAY             "$"/*36*/      //!< ($) Dolar Sign - Displays Value
#define menuLWROFFSET          0x20      //!< OFFSET for Uppercase to Lowercase
#define menuCANCEL              "."/*46*/      //!< (.) Period - Cancels current menu
//#define menuRANGEDELI           58      //!< (:) Colon - Sets a range of devices
#define menuCMDDELI             " "/*32*/      //!< ( ) Space - Value Delimiter

/************************************************************************/
/* Menu Switch Defines                                                  */
/************************************************************************/
#define switchHELP                '?'/*63*/      //!< (?) Question Mark
#define switchADC                 'A'/*65*/      //!< A
#define switchPWM                 'P'/*80*/      //!< P 
#define switchDATA                'D'/*68*/      //!< D
#define switchINFO                'I'/*73*/      //!< I
#define switchSET                 '='/*61*/      //!< (=) Equals - Chane a value
#define switchDISABLE             '-'/*45*/      //!< (-) Hyphen, or Minus - Disables Something
#define switchENABLE              '+'/*43*/      //!< (+) Plus - Enables something
#define switchTOGGLE              '!'/*33*/      //!< (!) Exclamation - Toggles a device
#define switchDISPLAY             '$'/*36*/      //!< ($) Dolar Sign - Displays Value
#define switchLWROFFSET          0x20      //!< OFFSET for Uppercase to Lowercase
#define switchCANCEL              '.'/*46*/      //!< (.) Period - Cancels current menu
#define switchCMDDELI             ' '/*32*/      //!< ( ) Space - Value Delimiter


/************************************************************************/
/* Menu Items                                                           */
/************************************************************************/

#define menuDEVICES     CRLF BOLD " Available Devices:" NORMAL CRLF

// Heh, just noticed i made All duty cycles white by pure accident ;-)
#define menuPWMDEVS     CL_RED " 1: Red LED & RC Filter Duty Cycle" CRLF \
					    CL_BLUE " 2: Blue LED Duty Cycle" CRLF           \
					    CL_GREEN " 3: Green LED Duty Cycle" CRLF         \
						CL_WHITE " 4: All Duty Cycles" CRLF              \
						NORMAL " F: PWM Frequency" CRLF

#define menuPWMVALS     BOLD "Duty Cycle (0 - 254)" NORMAL CRLF \
                        CL_GRAY         "   0% (0)" CRLF \
						CL_WHITE        "  10% (25)" CRLF \
						CL_LIGHTMAGENTA "  20% (50)" CRLF \
						CL_MAGENTA      "  30% (75)" CRLF \
						CL_BLUE         "  40% (100)" CRLF \
						CL_LIGHTBLUE    "  50% (125)" CRLF \
						CL_GREEN        "  60% (150)" CRLF \
						CL_LIGHTGREEN   "  70% (175)" CRLF \
						CL_YELLOW       "  80% (200)" CRLF \
						CL_LIGHTRED     "  90% (225)" CRLF \
						CL_RED          " 100% (254)" NORMAL CRLF
									
#define menuADCDEVS     " 1: Temperature Sensor (Display Only)" CRLF     \
                        " 2: RC Filter (FAUX Voltage Source) (Display Only)" CRLF \
						" F: ADC Update Frequency (Display or Set)" CRLF
#define menuADCSET		" ADC Task Update Delay in ms (100ms Default)" CRLF
                       
#define menuACTIONS     CRLF BOLD " Available Actions:" NORMAL CRLF
// These are separated as some devices don't have these options
#define menuATOGG       " " menuTOGGLE " : Toggles ON/OFF Device" CRLF
#define menuADISA       " " menuDISABLE " : Disables Device" CRLF
#define menuAENA        " " menuENABLE " : Enables Device" CRLF
#define menuPWR         " " menuADISA menuAENA
#define menuASET        " " menuSET " : Sets Value" CRLF
#define menuADISP       " " menuDISPLAY " : Displays Current Value" CRLF

#define menuVALUE       CRLF BOLD " Acceptable Values:" NORMAL CRLF
                        
#define menuINVALID     CRLF BOLD CL_RED "!!Invalid Option!!" CRLF \
                        " ? - Displays Help" NORMAL CRLF
#define menuDEFAULT     menuDISPLAY
#define CMA             ", "


// Max times to loop in a function to get a correct input (prevent inf loops)
#define menuMAXTRIES            5

/************************************************************************/
/* Return Values                                                        */
/************************************************************************/
#define errNONE                 0
#define errFAILURE             -1
#define errINVALID             -2                       

/************************************************************************/
/* Display Masks                                                        */
/************************************************************************/
#define DISPLAY_ADC_TEMP        0x00000001
#define DISPLAY_ADC_RCF         0x00000002
#define DISPLAY_ADC_2           0x00000004
#define DISPLAY_ADC_3           0x00000008
#define DISPLAY_ADC_ALLCH       0x0000000F
#define DISPLAY_ADC_FREQ        0x00000010
#define DISPLAY_ADC_ALL         0X00000013

#define DISPLAY_PWM_REDLED      0x00000020
#define DISPLAY_PWM_BLUELED     0x00000040
#define DISPLAY_PWM_GREENLED    0x00000080
#define DISPLAY_PWM_ALLLEDS     0x000000E0
#define DISPLAY_PWM_FREQ        0x00000100
#define DISPLAY_PWM_ALL         0x000001E0

#define DISPLAY_OS_VERSIONS     0x00000800
#define DISPLAY_OS_CLOCKS       0x00001000
#define DISPLAY_OS_TASKS        0x00002000
#define DISPLAY_OS_ALL          0x00003800

#define DISPLAY_EVERYTHING      0xFFFFFFFF


/************************************************************************/
/* Menu Formating                                                       */
/************************************************************************/
#define MOVCRTOXY(X,Y)          ("\x1B[" X ";" Y "H")
// Moves curser forward X (# in size char)
#define CUF(X)                  ( ESC X "C" )

#define COL1                    45

/************************************************************************/
/* PWM                                                                  */
/************************************************************************/
#define PWM_CHANNEL_RED         (1 << LED_RED_PWMA)
#define PWM_CHANNEL_RED_DUTY    0
#define PWM_CHANNEL_GREEN       (1 << LED_GREEN_PWMA)
#define PWM_CHANNEL_GREEN_DUTY  2
#define PWM_CHANNEL_BLUE        (1 << LED_BLUE_PWMA) 
#define PWM_CHANNEL_BLUE_DUTY   1
#define PWM_CHANNEL_RGB         (PWM_CHANNEL_BLUE|PWM_CHANNEL_GREEN|PWM_CHANNEL_RED)
#define PWM_CLK_DIV             99
#define PWM_RBG_INIT_STATE      0     

/************************************************************************/
/* ADC                                                                  */
/************************************************************************/
#define ADC_BITS_V              0.0004458007f     // volts/bit
#define ADC_TASK_DELAY          100
#define ADC_CH0_DATA_MASK       (AVR32_ADCIFB_CH0_MASK << AVR32_ADCIFB_LCCH_OFFSET)
#define ADC_CH4_DATA_MASK       (AVR32_ADCIFB_CH4_MASK << AVR32_ADCIFB_LCCH_OFFSET)
#define ADC_SAMPLEHOLDTM        15
#define ADC_REZ                 AVR32_ADCIFB_12BIT
#define ADC_CLKPRESCALE         2
#define ADC_STARTUPTM           8
#define ADC_SLEEPMODE           AVR32_ADCIFB_SLEEP_NORM


/************************************************************************/
/* PDCA                                                                */
/************************************************************************/
#define PDCA_ADC_CHANNEL        0
#define PDCA_USART_CHANNEL      1
#define PDCA_TERMINAL_PID       AVR32_PDCA_PID_USART2_TX

/************************************************************************/
/* Timer                                                                */
/************************************************************************/
#define ADC_TIMER               (&AVR32_TC1)
#define ADC_TIMER_CH            (0)

/************************************************************************/
/* Software Reset                                                       */
/************************************************************************/
#define RESET_AVR               wdt_reset_mcu();

/************************************************************************/
/* Helpful Functions                                                    */
/************************************************************************/
char *itoa(int i);

// Takes a char and determines the number of col to move right in order
// to justify the text to a specific col
char *cpRightJust(char * data, long from, long to);


#endif /* GLOABALS_H_ */