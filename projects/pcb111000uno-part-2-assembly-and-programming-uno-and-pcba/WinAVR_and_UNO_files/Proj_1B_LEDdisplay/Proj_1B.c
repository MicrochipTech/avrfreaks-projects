
/* 
Upload commentary file for details
*/


#include "Proj_1B_header_file.h"



int main (void)

{
unsigned int PORT_1=1;								//PORT_1 defines the display and initialises it
char  n;										
char overflow=0;									//When overflow is set the display scrolls back to the start

setup_UNO_extra;									//Set up UNO hardware


config_sw1_and_sw2_for_PCI;							//Configures sw1 and sw2 for interrupts

sei();												//Enable interrupts
n = Char_from_EEPROM(0)%15;							//n increments from 0 to 14

for(int m = 1; m <= n; m++)						//Initialise the display
	{I2C_Tx_2_integers(PORT_1, ~PORT_1);
	Timer_T0_10mS_delay_x_m(6);
	PORT_1 |= (PORT_1 << 1);}


while(1){											//Rotate the display
	I2C_Tx_2_integers(PORT_1, ~PORT_1);
	Timer_T0_10mS_delay_x_m(6);

	PORT_1 = PORT_1 << 1;							//Shift the display left									
	
	if(overflow)PORT_1 |= 1;						//If overflow is set illuminate the most right hand segment.
	
	if (PORT_1 & 0x8000) overflow=1;				//Set overflow when most left hand segment is illuminated
	else overflow=0;								//otherwise clear it
		}											//Note: 0x8000 = binary 1000000000000000
}													//End of main routine													



/***************************************************************************************************/
ISR(PCINT2_vect) 									//ISR called when switches 1 or 2 are pressed

{if(switch_2_down)
	{I2C_Tx_any_segment_clear_all();
	while(switch_2_down);							//Wait for user to release switch
	SW_reset_with_interrupt;}						//Trigger a WDT rest with interrupt

if(switch_1_down)
	{while(switch_1_down); return;}}				//Pause display



ISR (WDT_vect)										//ISR that runs immediately before the WDT reset 
{Char_to_EEPROM(0, (Char_from_EEPROM(0) + 1));}	//Write to EEPROM Note: zero is the EEPROM address	

