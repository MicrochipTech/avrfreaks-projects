//Created by: Sagun Man Singh
//Taking full advantage of May Day, 2010 (Maoist's Indefinite Strike) to do hardcore coding!
//Freescale MMA7260Q 3-Axis Accelerometer ported to ATmega32 microcontroller
//Started on: Thursday, May 6, 2010
//Update: LCD interface at PortC [Started: Friday, May 7, 2010; 7:00 PM]

#include <mega32.h>
#include <stdio.h>
#include <stdlib.h>

#asm(".equ __lcd_port=0x15")
#include <lcd.h> 

#define LCDwidth 16 
char lcd_buffer[17];
char adc_itoa[7];

unsigned short oldADCpin, ADCpin; 
unsigned int ADCtemp;
unsigned int ADCarray[3];
unsigned int xyzOrigin[3];
char xyzSpeed[3];

unsigned char xyzsampledflag;
unsigned char avgOrgflag;
unsigned char result;

                              
void initialize(void);
void displayOrientation(char val, unsigned char axis); 

void main(void) { 
 
        initialize();
        
        for (;;)  {     // main loop
                if ((ADCSRA & (1<<6)) == 0) {	// If ADC conversion has finished
                        ADCtemp = ADCH;
        
                        if (++ADCpin > 2) {      //if all x,y,z values are sampled, return to the first x value
                                ADCpin = 0;
                                xyzsampledflag = 1;
                        }
                                
                        ADMUX = (1<<5)|(5 - ADCpin); 
                        ADCSRA |= (1<<6);	// Start new ADC conversion
                        ADCarray[oldADCpin] =  ADCtemp;
                	oldADCpin = ADCpin;
                        if(xyzsampledflag == 1) {       //Print only after sampling of all 3 values x,y,z are complete and put in array
                	        if(avgOrgflag < 3) {
                                        xyzOrigin[0] += ADCarray[0];
                                        xyzOrigin[1] += ADCarray[1];               
                                        xyzOrigin[2] += ADCarray[2];
                                        avgOrgflag++;
                                }
                                else if(avgOrgflag == 3) {
                                        xyzOrigin[0] /= 3;
                                        xyzOrigin[1] /= 3;
                                        xyzOrigin[2] /= 3;
                                        printf("\r\nOrigin [%d,%d,%d]",xyzOrigin[0],xyzOrigin[1],xyzOrigin[2]);
                                        avgOrgflag++;
                                }
                                else {
                        	        //
                        	        result =  ADCarray[0];
                                        if(result > xyzOrigin[0]) {
                                	        xyzSpeed[0] = result - xyzOrigin[0] ;
                                        }
                                        else {
                                	        xyzSpeed[0] = xyzOrigin[0] - result ;
                                		xyzSpeed[0] |= 0x80; // set to negative number
                                        }
                                        //printf("\nX=%d",xyzSpeed[0]);
                                        //
                                        result =  ADCarray[1]; // read Y
                                        if(result > xyzOrigin[1]) {
                                		xyzSpeed[1] = result - xyzOrigin[1];
                                        }
                                        
                                        else{
                                		xyzSpeed[1] = xyzOrigin[1] - result;
                                		xyzSpeed[1] |= 0x80;
                                        }
                                        //printf("\nY=%d",xyzSpeed[1]);
                                        //
                                        result =  ADCarray[2]; // read Z
                                        if(result > xyzOrigin[2]){
                                		xyzSpeed[2] = result - xyzOrigin[2];
                                        }
                                        
                                        else{
                                		xyzSpeed[2] = xyzOrigin[2] - result;
                                		xyzSpeed[2] |= 0x80;
                                        }
                                        //printf("\nZ=%d",xyzSpeed[2]);
                                        printf("\tSpeed [%d,%d,%d]",xyzSpeed[0],xyzSpeed[1],xyzSpeed[2]);
                                }
                                
                                printf("\r\n[%d,%d,%d]",ADCarray[0],ADCarray[1],ADCarray[2]);
                	        xyzsampledflag = 0;
                	        
                	        displayOrientation(xyzSpeed[0], 0);
                	        displayOrientation(xyzSpeed[1], 1); 
                	           
                	}
                }

        }
}

void initialize() {              
        UCSRB = 0x18 ;    // UART to setup TX and Rx
        UBRRL = 103 ;     // Baud Rate for mega32.
         
        ADCpin = 0;  // X, Y, Z:  0, 1, 2
        oldADCpin = 0;
        xyzsampledflag = 0;
        avgOrgflag = 0;
        xyzOrigin[0] = 0;
        xyzOrigin[1] = 0;
        xyzOrigin[2] = 0;
        
        xyzSpeed[0] = 0;
        xyzSpeed[1] = 0;
        xyzSpeed[2] = 0;
        
        
        //ADC pin 3-5
        ADMUX = (1<<5)|5;		// Channel 5 only
        ADCSR = 0b11000111;
        
        lcd_init(LCDwidth);       //initialize the display
        lcd_clear();
        lcd_gotoxy(2,0); 
        lcd_putsf("MMA7260 TEST");
	printf("\nInitialized\n");
}

void displayOrientation(char val, unsigned char axis) {          //Axis: x=0, y=1, z=2

	if( !(val & 0x80) && val ) {    // if positive
		// print on the right
		if( val > 10 ) {         //PORTB = 0x0C;
		        if(axis == 1) {
        		        lcd_gotoxy(0,1); 
                                lcd_putsf("FRONT");
                        }
                        if(axis == 0) {
                                lcd_gotoxy(0,1); 
                                lcd_putsf("LEFT ");
                        } 
                }
                /* 		
		else {          //PORTB = 0x08;
	                lcd_gotoxy(0,1); 
                        lcd_putsf("STOP ");
                }
                */        
	}
	else if(val) {            // if negative
		val = val & 0x7F;
		// print on le left
		if( val > 10 ) {   //PORTB = 0x30;
			if(axis == 1) {
        		        lcd_gotoxy(0,1); 
                                lcd_putsf("BACK ");
                        }
                        if(axis == 0) {
                                lcd_gotoxy(0,1); 
                                lcd_putsf("RIGHT");
                        } 
                }
		else {          //PORTB = 0x10;
	                lcd_gotoxy(0,1); 
                        lcd_putsf("STOP ");
                }
                 
	}
	//else PORTB = 0;            // if zeros
	
	else { 
                lcd_gotoxy(0,1); 
                lcd_putsf("STOP ");
        } 
}
