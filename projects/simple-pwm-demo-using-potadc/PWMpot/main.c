/*
 * Compiler ImageCraft C for Mega8(16)
 * Read poteniometer connected to ADC0 and set PWM duty cycle and
 * send ADC value to serial USART
 */
#define F_CPU 1000000UL
#include <iccioavr.h> //<avr/io.h>  gcc io header
#include <stdint.h>
#include <stdio.h>
//#include <delay.h>  //not used

#include "init.h"

// Macro definitions
//#define SetBit(port,bit)  (port |= (1<<bit))   //set bit in port
//#define ClrBit(port,bit)  (port &= ~(1<<bit))  //clear bit in port

/*****************************************************************************
*
*   Function name : ADC_read
*
*   Returns :       uint16_t ADC (10 bit value)
*
*   Parameters :    none
*
*   Purpose :       Do one Analog to Digital Conversion
*
*****************************************************************************/
uint16_t ADC_read(void)
{
    ADCSRA |= (1<<ADSC);        // do single conversion
    while(ADCSRA & (1<<ADSC)) {};   // wait for conversion done, ADSC flag active
    return ADC;
}

/*****************************************************************************
*
*   Function name : ADC_ref
*
*   Returns :       none (with reference set)
*
*   Parameters :    Reference value (0-3, 0=internal AREF off, 1=AVCC, 2=Reserved, 3=internal 2.56v)
*
*   Purpose :       Sets new ADC reference
*
*****************************************************************************/
void ADC_ref(uint8_t ref)
{
    ADMUX &= 0xC0;         //clear ADC reference
    ref = ref & 0x03;      //limit ref to valid values(0-3)
    ADMUX |= (ref<<REFS0); //set new reference
    ADC_read(); //do dummy read affter reference change
}

/*****************************************************************************
*
*   Function name : ADC_read_ch
*
*   Returns :       uint16_t ADC (10 bit value) Average of 8 readings
*
*   Parameters :    uint8_t ADC MUX channel (0-15)
*
*   Purpose :       Do a Analog to Digital Conversion with noise reduction(averaging)
*
*****************************************************************************/
uint16_t ADC_read_ch(uint8_t ch)
{
#define ADCAVG 8  //set number of times to average (1-16)
    uint8_t i;
    uint16_t ADC_var = 0;

    /* select (ch)annel input */
    ADMUX &= 0xf0;        //clear mux channel;
    ADMUX |= (ch & 0x0f); //select new mux channel

    for (i=0; i<ADCAVG; i++)
    {
        ADC_var += ADC_read();
    }

    return ADC_var/ADCAVG;
}

/* This putchar works with the M8 UART. It will work with other devices
 * if their UART registers use the same names.
 * For ICC this function links clibs(printf) to USART
 */
int putchar(unsigned char c)
{
    /* if char is LF, send CR first */
    if (c == '\n')
        putchar('\r');
    /* Wait for empty transmit buffer */
    while ( !(UCSRA & (1<<UDRE)) )
        ;
    UDR = c;
    return c;
}

int main(void)
{
    uint16_t pot_value;  // holds ADC value of poteniometer

    init_devices(); // init ports, timer, usart, and ADC
    ADC_ref(1);     // use internal AVCC

    do
    {
        pot_value = ADC_read_ch(0);    //read ADC(0) voltage from poteniometer UNO A0
        OCR1A = pot_value;             //set PWM value (Dim an LED!) PORTB1 UNO D9
        printf("ADC value = %u \n", pot_value); //send ADC data to terminal UNO Serial Monitor
        //insert delay here if desired (100ms)
    }
    while(1); //forever loop

    return 0;
}
