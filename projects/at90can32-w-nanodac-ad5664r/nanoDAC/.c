#include ".\compiler.h"
#include <avr/io.h>
#include "math.h"
#include <delay.h>

//========================================================================
//  A Simple Testing program for AVR interface to ADI/nanoDAC/AD5664R
//========================================================================

// Original designed fro AT90CAN32 
// SYNC is PG1
// SCLK is PB1
// SDI  is PB3
// above 2 pins are SPI interface.
// try to use it.(after Software SPI)
// however, maybe Software SPI is enough.

#define sync_low()   PORTG &= ~(1<<PG1)
#define sync_high()  PORTG |= 1<<PG1
#define sclk_low()   PORTB &= ~(1<<PB1)
#define sclk_high()  PORTB |= 1<<PB1
#define sdi_low()    PORTB &= ~(1<<PB3)
#define sdi_high()	 PORTB |= 1<<PB3

void nanoDAC_Write(U8 nanoCmd, U8 nanoAdr, U16 data)
{	U8 i, B23_B16, DB8 ;

    sync_high(); // from idle mode to working mode, for power-saving design
    //; do 15 ns at least, YOUR AVR MCU, 16MHz=62.5ns., do nothing is okay for nanoDAC
	B23_B16 = 0+ nanoCmd + nanoAdr;

	sclk_high();
    sync_low();  //! starting write sequence

    for (i=23;i!=15;i--) 
	{
	  sclk_high();
	  if (0==(B23_B16 & 0x80)) sdi_low();
	  else sdi_high();
	  B23_B16<<=1;  // left shift
	  sclk_low();	// strobe in
	}

    DB8 = ((data &0xFF00) >>8);
    for (i=15;i!=7;i--) 
	{
	  sclk_high();
	  if (0==(DB8 & 0x80)) sdi_low();
	  else sdi_high();
	  DB8<<=1;  // left shift
	  sclk_low();	// strobe in
	}


    DB8 = (data &0x00FF);
    for (i=7;i!=0xFF;i--) 
	{
	  sclk_high();
	  if (0==(DB8 & 0x80)) sdi_low();
	  else sdi_high();
	  DB8<<=1;  // left shift
	  sclk_low();	// strobe in
	}

	sync_high();
	// do 15ns at least.
	sclk_high();
    sync_low();
}

#define nanoDAC_Command_WriteTo  0x00
#define nanoDAC_Command_Update	 0x08
#define nanoDAC_Command_input    0x10
#define nanoDAC_Command_WrUpate  0x18
#define nanoDAC_Command_PWRDown  0x20
#define nanoDAC_Command_Reset    0x28
#define nanoDAC_Command_LDAC     0x30
#define nanoDAC_Command_RefON    0x38
#define POR_Reset()     nanoDAC_Write(nanoDAC_Command_Reset, 0, 0x0001)
// issue the Software RESET command
#define VREF_Init()     nanoDAC_Write(nanoDAC_Command_RefON, 0, 0x0001)
// turn-ON internal VRef

// Hardware manual wired. So DAC out is a little different.
#define DAC_AOUT1	1
#define DAC_AOUT2   0


// =============================== nanoDAC TEST BLOCK ================================================

void VREF_Test(void)
{
   nanoDAC_Write(nanoDAC_Command_RefON, 0, 0x0001); // turn-ON internal VRef
   _delay_ms(1000);	// wait 1 second
   _delay_ms(1000);	// wait 1 second
   nanoDAC_Write(nanoDAC_Command_RefON, 0, 0x0000); // turn-OFF internal VRef
   _delay_ms(1000);	// wait 1 second
   _delay_ms(1000);	// wait 1 second
}

//#ifdef  DAC_TEST_BLOCK
#define ARRAY_SIZE  360
U16   bb[ARRAY_SIZE];
// DO NOT exceed the largest SRAM for your MCU, or you will see FREAK things.

void DACTest_init(void)
{
	double  aa,step;
	int i;
    step= 2*M_PI/ARRAY_SIZE;
	for (aa=0.0, i=0;  i!=ARRAY_SIZE; i++) 
	{
	  bb[i]= 0x8000+sin(aa)*32760;
	  aa=aa+step;
	}
}
void DAC_SineTest1(void)
{
	int i,j;
   //machine_state=2;	// Any indicator?

	for (j=0, i=0;  i!=65535; i++) 
	  {
	  nanoDAC_Write(nanoDAC_Command_WrUpate, DAC_AOUT1, bb[j] );
	  j++; if (ARRAY_SIZE==j) j=0;
	  }
}

void DAC_SineTest2(void)
{
	double  aa,step;
	int i;
    step= M_PI/80;
	for (aa=0.0, i=0;  i!=65535; i++) 
	{
	  nanoDAC_Write(nanoDAC_Command_WrUpate, DAC_AOUT1, 0x8000+sin(aa)*32760);
	  aa=aa+step;
	}
   //machine_state=2;	// Any indicator?
}

//#endif
// =============================== nanoDAC TEST BLOCK ================================================

int main (void)
{	
	int i;

// SYNC is PG1
// SCLK is PB1
// SDI  is PB3
    DDRG |= 1<<PG1; // PG1 is 
	DDRB |= (1<<PB3) + (1<<PB1);
	POR_Reset();
	VREF_Init();

	DACTest_init(); // initialize the SINE table, for faster data movement...300HZ@300points.

while(1) { 
	DAC_SineTest1();
	/*  ----- A ultra-simple-stupid testing ----------------
	VREF_Test(); 
	sync_low();
	sync_high();
	sclk_high();
	sclk_low();
	sdi_high();
	sdi_low();
	*/
	}
}
