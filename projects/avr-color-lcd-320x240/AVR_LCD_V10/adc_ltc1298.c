/**************************************
          ADC LTC 1298 driver
**************************************/
#include "lcd_test_defs.h"             
#include "adc_ltc1298.h"

/**************************************
Function: ADC On 
**************************************/
void ADC_LTC1298_On(void)
{                                                
        //SPI clock 125Khz
        //SPI_CLK_125K;  
        
        //Zet ADC actief
        ADC_LTC1298_Power_On;
        
        return;
}
//*************************************

/**************************************
Function: ADC off
**************************************/
void ADC_LTC1298_Off(void)
{
        //Set ADC off
        ADC_LTC1298_Power_Off;
        
        return;
}
//*************************************

/**************************************
Function: ADC measure data
**************************************/
unsigned int ADC_LTC1298_Data(unsigned char Channel)
{
        unsigned char LowByte;
        unsigned int HighByte;
        unsigned int test;   
        unsigned int test2;
        
        ADC_LTC1298_On();                                       //Set ADC active
        SPI_ON;                                                 //Set SPI on
        
        //Check wich channel must be measure
        if(Channel == 0)                                        //Channel 0
                spi(ADC_LTC1298_Channel_0);                     //Zend starbit en config voor channel 0
        else                                                    //Channel 1
                spi(ADC_LTC1298_Channel_1);                     //Zend startbit en config voor channel 1
               
        //Ontvang 12bit data, MSB eerst
        HighByte = spi(0xFF);        
        LowByte = spi(0xFF);                                    //Send dummy data on the spi to create CLK    
        
        ADC_LTC1298_Off();                                      //Set ADC inactive
        SPI_OFF;                                                //Set SPI off
        
        //Schuif HighByte naar hoge byte daarna lowbyte dan 
        //correctie voor 12bit waarde, 3 naar rechts schuiven
        return(((HighByte << 8) | LowByte) >> 3);               //Return with measure data
}
//*************************************

