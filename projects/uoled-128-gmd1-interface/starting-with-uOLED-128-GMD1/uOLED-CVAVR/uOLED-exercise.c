/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.9 Standard
Automatic Program Generator
© Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project :
Version :
Date    : 4/9/2008
Author  : Walt Sacuta
Company : Arctan Systems
Comments:


Chip type           : ATmega16L
Program type        : Application
Clock frequency     : 3.686000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega16.h>

// Standard Input/Output functions
#include <stdio.h>
#include <delay.h>
#include <stdlib.h>
#include "uoled.h"




// This program uses the STK500 evaluation board to control the uOLED-128-GMD1
//  display made by 4D Systems (www.4dsystems.com.au).
// The STK500 has an AVR chip (ATmega16L) connected in socket A3.  The USART on the ATmega16L
//  is used to control the uOLED display.  The USART operating speed is 57600 baud.
// NOTE: the STK500 is a 5VDC system, and the uOLED is a 3V device, so precautions must be taken
//  to avoid damaging the display by excess voltage.  Details of the interface are described in the
//  uOLED datasheet.  A schematic of the interface is








void main(void)
    {
    // Screen Saver for uOLED...
    u8 xC,yC;               // current position of circle center.
    u8 xBcur,yBcur;         // block to enclose circle (current start position).
    u8 xBnew,yBnew;         // block to enclose circle (new start position).

    ////u8 xSign;
    int xVel,yVel;          // x and y velocities.
    int xInc,yInc;          // x and y velocity increments.

    initRegisters();
    delay_ms(STARTUP_DELAY); // wait before issuing commands...
    baudRateSync();
    xC = START_X; yC = START_Y;

    // algorithm: velocity is a fractional amount (percent of a pixel).
    // when velocity magnitude is above 100, one pixel of movement is indicated.
    // the total magnitude of x and y velocities is 200 (2 pixels of movment per step on average).
    xInc = X_VEL;
    yInc = Y_VEL;
    xVel = xInc;
    yVel = yInc;

    clearDisplay();

    // bouncing circle around display...
    drawCircle(xC,yC,CIRC_RADIUS,FULL_BLUE);

    while (TRUE)
        {
        // move circle around.

        // calculate block start coordinates...
        xBcur = xC-(CIRC_RADIUS+1);
        yBcur = yC-(CIRC_RADIUS+1);

        // calculate the new position based on the x and y motion directions.
        // shifts can be one or two pixels in either direction, or no shift.
        if(xVel >= (2*PIXEL_DIV) )      { xBnew=xBcur-2; xVel -= (2*PIXEL_DIV); xC-=2; }
        else if (xVel >= PIXEL_DIV)     { xBnew=xBcur-1; xVel -= PIXEL_DIV;     xC-=1; }
        else if (xVel <= -(PIXEL_DIV))  { xBnew=xBcur+1; xVel += PIXEL_DIV;     xC +=1;}
        else if (xVel <= -(2*PIXEL_DIV)){ xBnew=xBcur+2; xVel += (2*PIXEL_DIV); xC +=2;}
        else {xBnew=xBcur;}

        if(yVel >= (2*PIXEL_DIV) )      { yBnew=yBcur-2; yVel -= (2*PIXEL_DIV); yC -=2;}
        else if (yVel >= PIXEL_DIV)     { yBnew=yBcur-1; yVel -= PIXEL_DIV;     yC -=1;}
        else if (yVel <= -(PIXEL_DIV))  { yBnew=yBcur+1; yVel += PIXEL_DIV;     yC +=1;}
        else if (yVel <= -(2*PIXEL_DIV)){ yBnew=yBcur+2; yVel += (2*PIXEL_DIV); yC +=2;}
        else {yBnew=yBcur;}


        shiftBlock(xBcur,yBcur,xBnew,yBnew,BOX_WID,BOX_LEN);
        delay_ms(LOOP_DELAY);

        xVel += xInc;
        yVel += yInc;

        if(xVel < 0)  // if either x limit is reached, re-set the velocity increments.
            {
            if(xC >= (DISP_MAX-START_X)){xInc = -X_VEL; xVel = xInc; drawRandomColourCircle(xC,yC);}
            }
        else
            {
            if(xC <= START_X){xInc = X_VEL; xVel = xInc; drawRandomColourCircle(xC,yC);}
            }

        if(yVel < 0) // if either y limit is reached, re-set the velocity increments.
            {
            if(yC >= (DISP_MAX-START_Y)){yInc = -Y_VEL; yVel = yInc; drawRandomColourCircle(xC,yC);}
            }
        else
            {
            if(yC <= START_Y){yInc = Y_VEL; yVel = yInc; drawRandomColourCircle(xC,yC);}
            }
        }
    }







void drawRandomColourCircle(u8 xC,u8 yC)
    {
    int randv;

    randv = rand();     // random value between 0 and 32767, divide it in three...

    if( (randv >= 0) && (randv <= 10922) )
        {
        drawCircle(xC,yC,CIRC_RADIUS,FULL_BLUE);
        }
    else if( (randv >= 10923) && (randv <= 21845) )
        {
        drawCircle(xC,yC,CIRC_RADIUS,FULL_GREEN);
        }
    else
        {
        drawCircle(xC,yC,CIRC_RADIUS,FULL_RED);
        }
    }





void shiftBlock(u8 xstart,u8 ystart,u8 xnew,u8 ynew,u8 width,u8 height)
    {
    putchar(CM_SHIFT_BLOCK);      // copy-shift command
    putchar(xstart & 0x7F);
    putchar(ystart & 0x7F);
    putchar(xnew & 0x7F);
    putchar(ynew & 0x7F);
    putchar(width);
    putchar(height);
    delay_ms(COMMAND_DELAY);
    }



void drawCircle(u8 xstart,u8 ystart,u8 radius,u16 colour)
    {
    putchar(CM_DRAW_CIRCLE);
    putchar(xstart & 0x7F);
    putchar(ystart & 0x7F);
    putchar(radius);
    putchar( (u8) ((colour>>8) & 0xFF) );
    putchar( (u8) (colour & 0xFF) );
    delay_ms(COMMAND_DELAY);
    }


void clearDisplay(void)
    {
    putchar(CM_CLEAR_DISP);
    delay_ms(COMMAND_DELAY);
    }


void baudRateSync(void)
    {
    putchar(CM_BAUD_SYNC);
    delay_ms(BAUD_SYNC_DELAY);
    }




void initRegisters(void)
{

    // Input/Output Ports initialization
    // Port A initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
    PORTA=0x00;
    DDRA=0x00;

    // Port B initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
    PORTB=0x00;
    DDRB=0x00;

    // Port C initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
    PORTC=0x00;
    DDRC=0x00;

    // Port D initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
    PORTD=0x00;
    DDRD=0x00;

    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: Timer 0 Stopped
    // Mode: Normal top=FFh
    // OC0 output: Disconnected
    TCCR0=0x00;
    TCNT0=0x00;
    OCR0=0x00;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: Timer 1 Stopped
    // Mode: Normal top=FFFFh
    // OC1A output: Discon.
    // OC1B output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer 1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    TCCR1A=0x00;
    TCCR1B=0x00;
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;

    // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: Timer 2 Stopped
    // Mode: Normal top=FFh
    // OC2 output: Disconnected
    ASSR=0x00;
    TCCR2=0x00;
    TCNT2=0x00;
    OCR2=0x00;

    // External Interrupt(s) initialization
    // INT0: Off
    // INT1: Off
    // INT2: Off
    MCUCR=0x00;
    MCUCSR=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=0x00;

    // USART initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART Receiver: On
    // USART Transmitter: On
    // USART Mode: Asynchronous
    // USART Baud Rate: 57600
    UCSRA=0x00;
    UCSRB=0x18;
    UCSRC=0x86;
    UBRRH=0x00;
    UBRRL=0x03;

    // Analog Comparator initialization
    // Analog Comparator: Off
    // Analog Comparator Input Capture by Timer/Counter 1: Off
    ACSR=0x80;
    SFIOR=0x00;

}















