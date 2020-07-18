
 
//Proj_5E:  A stop watch application
 
 /*
 sw2 selects 100ms clock, sw1 a 10ms clock
 Press sw1 to pause SW and save time
 Press sw2 the read back saved times

 While pressing sw2 press sw1 to restore SW
 Press sw3 to re-initialise SW
 Works from a 5V supply independently of the PC
 */



#include "Proj_5E_header_file.h"



int main (void){

	setup_UNO_extra;

	setup_and_enable_PCI;
	disable_pci_on_sw1_and_sw2;                               		//Only PCI on sw3 is required
	sei();


	{char digit_num=0; for (int m = 0; m < 8; m++)					//initialise display by
		{I2C_Tx_any_segment('d', digit_num++);}}					//illuminating segment "d" on each digit


		while(switch_1_up && switch_2_up);							//wait for a keypress
		I2C_Tx_any_segment_clear_all();								//clear display
		if(switch_2_down){I2C_Tx_Clock_command(one100ms_mode);}		//sw_1 selects 100ms clock
		if(switch_1_down){I2C_Tx_Clock_command(ten_ms_mode);}		//sw_3 selects 10ms clock


		while(switch_1_down || switch_2_down);						//wait for switch release

		while(1){													//Infinite loop, requires sw3 interrupt to exit
			while(switch_1_up && switch_2_up);						//wait for a keypress

			if(switch_1_down)										//Press sw1 to
			{I2C_Tx_Clock_command(store_time);						//save time and pause clock display
				while (switch_1_down);								//wait for key release
			I2C_Tx_Clock_command(display_current_time);}			//clock display continues as normal


			if(switch_2_down)										//Press sw2 to
			{I2C_Tx_Clock_command(display_stored_times); 			//read stored times at 200ms intervals
			Timer_T0_10mS_delay_x_m(20);}


			if(switch_1_down && switch_2_down)                       //press sw1 while sw2 is still held down to
			{I2C_Tx_Clock_command(display_current_time);             //continue clock display
			while(switch_1_down || switch_2_down);}}}                //wait until both switches have been released
			
  



/*********************************************************************************************************/


ISR(PCINT0_vect) {if(switch_3_up){return;}
I2C_Tx_Clock_command(AT_exit_stop_watch);
SW_reset;}