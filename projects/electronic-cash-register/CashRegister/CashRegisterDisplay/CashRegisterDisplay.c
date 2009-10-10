
//*****************************************************
// File Name  : cashRegisterDisplay.c
// Title      : Εκπαιδευτική ταμιακή μηχανή (Display)
// Target MCU : Atmel ATmega8 MCU at 4MHz XTAL
//*****************************************************


#include "CashRegisterDisplay.h"		// 



int main(void)
  
{
  
  DATA_DDR = 0XFF; // Port D output
  DDRB = 0XFF;          // Port B output
  DDRC = 0XFF;          // Port C output

  //u08 c=0;
	// initialize i2c function library
	i2cInit();
	// set local device address and allow response to general call
	i2cSetLocalDeviceAddr(LOCAL_ADDR, TRUE);

	// set the Slave Receive Handler function
	// (this function will run whenever a master somewhere else on the bus
	//  writes data to us as a slave)
	i2cSetSlaveReceiveHandler( i2cSlaveReceiveService );

	// set the Slave Transmit Handler function
	// (this function will run whenever a master somewhere else on the bus
	//  attempts to read data from us as a slave)
//	i2cSetSlaveTransmitHandler( i2cSlaveTransmitService );


  do
    {
    
  // Ο <localBufferLength> είναι ο συνολικός αριθμός των bytes που ελήφθησαν
  // πχ αν στείλεις τα byte 112233 , ο buffer θα έχει τον αριθμό 6
  // Η τελεία (.) υπολογίζεται κανονικά ώς byte.

  disp_number = localBufferLength ; 

  // Αντέγραψε τον συνολικό αριθμό των ληφθέντων byte μείον 1 στην <disp_number>
  // Αυτό γίνεται γιατί το 1ο byte βρίσκεται στην διεύθυνση 0 του buffer
  bufferPottition = localBufferLength -1;


//---------------------- 1o LED ---------------------------------------------
    
    if (disp_number>0)           //Εάν ο buffer δέν είναι άδειος... 
        disp_numberlay(localBuffer[bufferPottition--]); // Τότε εμφάνισε τον ASCII
		// (localBuffer) που βρίσκεται στην διεύθυνση του buffer <disp_number>
		// και μετά  μείωσέ τον αριθμό θέσης κατά 1.
    else
        disp_numberlay('0');

  PORTB |=(1<<1);   // Ενεργοποίησε το 1ο LED display (1ο απο δεξιά)
  delay_ms(1660);   // Περίμενε λίγο σε αυτή την κατάσταση...
  PORTB &= ~(1<<1); // και μετά ξανά απενεργοποίησε το 1ο LED display
  disp_number--;

//---------------------- 2o LED ---------------------------------------------
   
    if (disp_number>0)           //Εάν ο buffer δέν είναι άδειος... 
        disp_numberlay(localBuffer[bufferPottition--]);
    else
        disp_numberlay('0');

  PORTB |=(1<<2);
  delay_ms(1660);
  PORTB &= ~(1<<2);
  disp_number--;

//---------------------- 3o LED ---------------------------------------------
 
    if (disp_number>0)           //Εάν ο buffer δέν είναι άδειος... 
        disp_numberlay(localBuffer[bufferPottition--]);
    else
        disp_numberlay('0');

  PORTB |=(1<<3);
  DATA_PORT |= (_BV(Pin_Dec)); //Αναψε την τελεία
  delay_ms(1660);
  PORTB &= ~(1<<3);
  DATA_PORT &=~(_BV(Pin_Dec)); //Σβήσε την τελεία
  disp_number--;

//---------------------- 4o LED ---------------------------------------------

    if (disp_number>0)           //Εάν ο buffer δέν είναι άδειος... 
        disp_numberlay(localBuffer[bufferPottition--]);
    else
        disp_numberlay(' ');

  PORTB |=(1<<4);
  delay_ms(1660);
  PORTB &= ~(1<<4);
  disp_number--;

//---------------------- 5o LED ---------------------------------------------

    if (disp_number>0)           //Εάν ο buffer δέν είναι άδειος... 
        disp_numberlay(localBuffer[bufferPottition--]);
    else
        disp_numberlay(' ');

  PORTC |=(1<<0);
  delay_ms(1660);
  PORTC &= ~(1<<0);
  disp_number--;

//---------------------- 6o LED ---------------------------------------------

    if (disp_number>0)           //Εάν ο buffer δέν είναι άδειος... 
        disp_numberlay(localBuffer[bufferPottition--]);
    else
        disp_numberlay(' ');

  PORTC |=(1<<1);
  delay_ms(1660);
  PORTC &= ~(1<<1);
  disp_number--;

//---------------------- 7o LED ---------------------------------------------

    if (disp_number>0)           //Εάν ο buffer δέν είναι άδειος... 
        disp_numberlay(localBuffer[bufferPottition--]);
    else
        disp_numberlay(' ');

  PORTC |=(1<<2);
  delay_ms(1660);
  PORTC &= ~(1<<2);
  disp_number--;

//---------------------- 8o LED ---------------------------------------------

    if (disp_number>0)           //Εάν ο buffer δέν είναι άδειος... 
        disp_numberlay(localBuffer[bufferPottition]);
    else
        disp_numberlay(' ');

  PORTC |=(1<<3);
  delay_ms(1660);
  PORTC &= ~(1<<3);

    }
 while(1);
} 
  
//----------------------------------------------------------------


void hideZeroNumbersj (void)
{

    unsigned char i;

    for (i=0; i<5 ;i++)
    {
        if (localBuffer[i] == '0')
        {
            localBuffer[i] = ' ';
        }
        else if (localBuffer[i] > '0')            
        {
            break;
        }
    }
}


/*----------------------------------------------------------------
          FUNCTIONS TO disp_numberLAY VALUES 
-----------------------------------------------------------------*/  
  void disp_numberlay(int a)
  {

  switch(a)
  {

  case 0x30:  
			DATA_PORT&=~_BV(Pin_g);
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f));
			break;
			
  case 0x31:			
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			DATA_PORT|=(_BV(Pin_b)|_BV(Pin_c));
			break;
			
  case 0x32:  
			DATA_PORT&=~(_BV(Pin_c)|_BV(Pin_f));
			DATA_PORT|=(_BV(Pin_b)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_a)|_BV(Pin_g));
			break;
	
  case 0x33:  
			DATA_PORT&=~(_BV(Pin_e)|_BV(Pin_f));
			DATA_PORT|=(_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_g));
			break;
			
  case 0x34:  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_d)|_BV(Pin_e));
			DATA_PORT|=(_BV(Pin_c)|_BV(Pin_b)|_BV(Pin_f)|_BV(Pin_g));
			break;
			
  case 0x35:  
			DATA_PORT&=~(_BV(Pin_b)|_BV(Pin_e));
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_f)|_BV(Pin_g));
			break;
			
  case 0x36:  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b));
			DATA_PORT|=(_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_f)|_BV(Pin_g)|_BV(Pin_e));
			break;
			
  case 0x37:  
			DATA_PORT&=~(_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_c)|_BV(Pin_b));
			break;
			
  case 0x38:  
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_c)|_BV(Pin_b)|_BV(Pin_f)|_BV(Pin_g));
			break;
		
  case 0x39:  
			DATA_PORT&=~(_BV(Pin_d)|_BV(Pin_e));
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_c)|_BV(Pin_b)|_BV(Pin_f)|_BV(Pin_g));
			break;

  case '-':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f));
			DATA_PORT|=(_BV(Pin_g));
			break;
  case ' ':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g)|_BV(Pin_Dec));
			break;

  case 'x': 
            localBuffer[0] = '0';
            localBuffer[1] = '0';
            localBuffer[2] = '0';
            localBufferLength = 3;
			break;

  case 'A':  
			DATA_PORT&=~(_BV(Pin_d));
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			break;
  case 'b':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b));
			DATA_PORT|=(_BV(Pin_c)|_BV(Pin_f)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_g));
			break;
  case 'c':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_f)|_BV(Pin_b)|_BV(Pin_c));
			DATA_PORT|=(_BV(Pin_e)|_BV(Pin_d)|_BV(Pin_g));
			break;
  case 'd':             
			DATA_PORT&=~(_BV(Pin_f)|_BV(Pin_a));
			DATA_PORT|=(_BV(Pin_e)|_BV(Pin_c)|_BV(Pin_b)|_BV(Pin_d)|_BV(Pin_g));
			break;
  case 'E':  
			DATA_PORT&=~(_BV(Pin_c)|_BV(Pin_b));
			DATA_PORT|=(_BV(Pin_f)|_BV(Pin_a)|_BV(Pin_e)|_BV(Pin_d)|_BV(Pin_g));
			break;
  case 'F':  
			DATA_PORT&=~(_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d));
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			break;
  case 'H':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_d));
			DATA_PORT|=(_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			break;

  case 'i':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_f)|_BV(Pin_g));
			DATA_PORT|=(_BV(Pin_e));
			break;

  case 'J':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_f)|_BV(Pin_g));
            DATA_PORT|=(_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_e));
			break;
  case 'L':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_g));
			DATA_PORT|=(_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f));
			break;
  case 'n':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_d)|_BV(Pin_f));
			DATA_PORT|=(_BV(Pin_c)|_BV(Pin_e)|_BV(Pin_g));
			break;

  case 'o':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_f));
			DATA_PORT|=(_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_g));
			break;

  case 'P':  
			DATA_PORT&=~(_BV(Pin_c)|_BV(Pin_d));
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			break;

  case 'r':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_f));
			DATA_PORT|=(_BV(Pin_e)|_BV(Pin_g));
			break;

  case 'S':  
			DATA_PORT&=~(_BV(Pin_b)|_BV(Pin_e));
			DATA_PORT|=(_BV(Pin_a)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_f)|_BV(Pin_g));
			break;

  case 't':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_b)|_BV(Pin_c));
			DATA_PORT|=(_BV(Pin_d)|_BV(Pin_e)|_BV(Pin_f)|_BV(Pin_g));
			break;
  case 'y':  
			DATA_PORT&=~(_BV(Pin_a)|_BV(Pin_e));
			DATA_PORT|=(_BV(Pin_b)|_BV(Pin_c)|_BV(Pin_d)|_BV(Pin_f)|_BV(Pin_g));
			break;

  default:  
            disp_numberlay(localBuffer[bufferPottition--]);//Αν δεν είναι έγκυρος χαρακτήρας...
            // μείωσε τον buffer κατά μία θέση και διάβασε τον επόμενο χαρακτήρα του buffer.
			break;
					
	}
    }
/*----------------------------------------------------------------
           ------------ slave operations ----------------
-----------------------------------------------------------------*/

void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData)
{

	unsigned char i;
    
	// this function will run when a master somewhere else on the bus
	// addresses us and wishes to write data to us


	// copy the received data to a local buffer
	for(i=0; i<receiveDataLength; i++)
	{
		localBuffer[i] = *receiveData++;
	}

	localBufferLength = receiveDataLength;

}


/*----------------------------------------------------------------
--------FUNCTION TO GENERATE DELAY IN MILLISECONDS----------------
-----------------------------------------------------------------*/

void delay_ms(u32 count) // Καθηστέρηση 2.5 ms
{
    while(count--);
}


