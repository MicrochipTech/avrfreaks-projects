//***************************************************************************
//
//  File........: ADC.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 3.3.1; avr-libc 1.0
//
//  Description.: AVR Butterfly ADC routines
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//
//***************************************************************************

#include <avr/io.h>
#include "button.h"

#include "main.h"
#include "ADC.h"
#include "BCD.h"
#include "LCD_functions.h"
#include "timer0.h"


/*****************************************************************************
*
*   Function name : ADC_init
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Initialize the ADC with the selected ADC-channel
*
*****************************************************************************/
void ADC_init(char input)
{
  
    ADMUX = input;    // external AREF and ADCx
    
    ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);    // set ADC prescaler to , 1MHz / 8 = 125kHz    

    input = ADC_read();        // dummy 
}


/*****************************************************************************
*
*   Function name : ADC_read
*
*   Returns :       int ADC
*
*   Parameters :    None
*
*   Purpose :       Do a Analog to Digital Conversion
*
*****************************************************************************/
int ADC_read(void)
{
    char i;
    int ADC_temp;
    // mt int ADC = 0 ;
    int ADCr = 0;
    
    // To save power, the voltage over the LDR and the NTC is turned off when not used
    // This is done by controlling the voltage from a I/O-pin (PORTF3)
    sbiBF(PORTF, PF3); // mt sbi(PORTF, PORTF3);     // Enable the VCP (VC-peripheral)
    sbiBF(DDRF, DDF3); // sbi(DDRF, PORTF3);        

    sbiBF(ADCSRA, ADEN);     // Enable the ADC

    //do a dummy readout first
    ADCSRA |= (1<<ADSC);        // do single conversion
    while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
        
    for(i=0;i<8;i++)            // do the ADC conversion 8 times for better accuracy 
    {
        ADCSRA |= (1<<ADSC);        // do single conversion
        while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
        
        ADC_temp = ADCL;            // read out ADCL register
        ADC_temp += (ADCH << 8);    // read out ADCH register        

        ADCr += ADC_temp;      // accumulate result (8 samples) for later averaging
    }

    ADCr = ADCr >> 3;     // average the 8 samples
        
    //cbiBF(PORTF,PF3); // mt cbi(PORTF, PORTF3);     // disable the VCP
    //cbiBF(DDRF,DDF3); // mt cbi(DDRF, PORTF3);
    
    cbiBF(ADCSRA, ADEN);      // disable the ADC

    return ADCr;
}


/*****************************************************************************
*
*   Function name : ADC_periphery
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calculates the Temperature/Voltage/Ligth from the ADC_read
*                   and puts it out on the LCD.
*
*****************************************************************************/
void ADC_periphery(void)
{
    int ADCresult = 0;
    char Temp;
    char TL;
    char TH;

    char VoltageHB;
    
    ADCresult = ADC_read();         // Find the ADC value
     
    if( ADMUX == LIGHT_SENSOR )
    {
        // The relation between ADC-value and lux is yet to be found, 
        // for now the ADC-value is presented on the LCD
        
        VoltageHB = CHAR2BCD2(ADCH);    // Convert from char to bin

        Temp = ADCL;                
    
        TL = (Temp & 0x0F) + '0';       
        if(TL > '9')        // if the hex-value is over 9, add 7 in order to go 
            TL += 7;        // jump to the character in the ASCII-table
                
        TH = (Temp >> 4) + '0';
        if(TH > '9')        // if the hex-value is over 9, add 7 in order to go 
            TH += 7;        // jump to the character in the ASCII-table
            
        LCD_putc(0, 'A');
        LCD_putc(1, 'D');
        LCD_putc(2, 'C');
        LCD_putc(3, (ADCH + 0x30));
        LCD_putc(4, TH);
        LCD_putc(5, TL);
        LCD_putc(6, '\0');


    }
}


/*****************************************************************************
*
*   Function name : LightFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable light measurements
*
*****************************************************************************/
char LightFunc(char input)
{
    static char enter = 1;
    
    if (enter)
    {
    
        enter = 0;
        
        ADC_init(LIGHT_SENSOR);     // Init the ADC
        
        // Enable auto-run of the ADC_perphery every 10ms 
        // (it will actually be more than 10ms cause of the SLEEP)  
        Timer0_RegisterCallbackFunction(ADC_periphery);        
    }
    else
        LCD_UpdateRequired(TRUE, 0); 

    if (input == KEY_PREV)
    {
        // Disable the auto-run of the ADC_periphery      
        Timer0_RemoveCallbackFunction(ADC_periphery);
        
        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
    
        return ST_LIGHT;
    }
    else
        return ST_LIGHT_FUNC;    
}
