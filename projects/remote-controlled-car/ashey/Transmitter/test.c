//Checking for the Timer to set the IR pulse 
unsigned char high,Low;
void SetIR(unsigned char width , unsigned char low, unsigned char OCR_value)
{
    OCR2 = OCR_value;
    high = width ;
	Low  = width+low;
    while(TCCR2!=0);    
} 
INTERRUPT (SIG_OUTPUT_COMPARE2)
{ 
   unsigned static char cycles = 0 ;
   if(cycles < high )
   {
   
        //keep toggling 
        if (cycles%2)        
			PORTB = PORTB & ~(_BV(IR));
		else
			PORTB = PORTB | _BV(IR);
	  
   }
   // making it zero after 50 cycles 
   else if(cycles == high)
        PORTB&=~(_BV(IR));
   //waiting till the cycles become 150
   else if(cycles == Low)
   {
        // make it low 
        cycles = 0 ;
		TCCR2  = 0 ;
		high   = 0 ;
		Low    = 0 ;
   }
   cycles ++  ;
}
// This Timer starts the delay 
void delay(unsigned char value , unsigned char low , unsigned char OCR_value)
{   
    OCR1A  = OCR_value;
    high   = value;
	Low    = low + value ;
    while(TCCR1B!=0);

}
INTERRUPT(SIG_OUTPUT_COMPARE1A)
{
    unsigned  static  char cycles = 0; 
    if (cycles < high )
    {
        if (cycles%2)        
			PORTB = PORTB & ~(_BV(IR));
		else
			PORTB = PORTB & ~(_BV(IR));
	}
    else if(cycles == high)   
		PORTB&=~(_BV(IR));
    if(cycles == Low )
	 {
	   cycles = 0 ;
	   TCCR1B = 0 ;
	   Low    = 0 ;
	   high   = 0 ;
	 }
    cycles ++;   
}
