#include "mouse_mode.h"

//Sends mouse commands to PC including button conditions (buttons) plus motion in x (delta_x) and y (delta_y) directions.
void Send_Mouse_Command (unsigned char buttons, int delta_x, int delta_y)
{
	tx_mouse = 1;
	inputBuffer2[1] = buttons;	//Condition of modifiers
	inputBuffer2[2] = delta_x;	//Keyboard keypress to be sent
	inputBuffer2[3] = delta_y;	//Keyboard keypress to be sent
	send_packets();
	TIMER=0;
	while(TIMER<TEN_MILLISECONDS);//Minimum of 1/10 of a second between sending prevents missed characters
}
/*******************************************************************************************************************************************/

//Check Buttons A, B & C for mouse button control
unsigned char Get_Mouse_Buttons (void)
{
		unsigned char mouse_buttons = 0;
		//Check button A (Centre Click)
		if ((Global_RX_Bytes[0] & (1<<BUTTON_A)) !=0)
		{
			mouse_buttons |= (1<<MOUSE_BUTTON_CENTER);
		}
		//Check button B (Left Click)
		if ((Global_RX_Bytes[0] & (1<<BUTTON_B)) !=0)
		{
			mouse_buttons |= (1<<MOUSE_BUTTON_LEFT);
		}
		//Check button C (Right Click)
		if ((Global_RX_Bytes[0] & (1<<BUTTON_C)) !=0)
		{
			mouse_buttons |= (1<<MOUSE_BUTTON_RIGHT);
		}
		return(mouse_buttons);
}
/*******************************************************************************************************************************************/

//Determine mouse motion value along the PC's X axis.
signed char Get_Delta_X (unsigned char sector, unsigned char magnitude)
{
	if ( (sector == SECT_1) | (sector == SECT_4) | (sector == SECT_7) )
	{
		return(0-magnitude);
	}
	else if ( (sector == SECT_3) | (sector == SECT_6) | (sector == SECT_9) )
	{
		return(magnitude);
	}
	else
	{
		return(0);
	}
}
/*******************************************************************************************************************************************/

//Determine mouse motion value along the PC's Y axis.
signed char Get_Delta_Y (unsigned char sector, unsigned char magnitude)
{
	if ( (sector == SECT_1) | (sector == SECT_2) | (sector == SECT_3) )
	{
		return(0-magnitude);
	}
	else if ( (sector == SECT_7) | (sector == SECT_8) | (sector == SECT_9) )
	{
		return(magnitude);
	}
	else
	{
		return(0);
	}
}
/*******************************************************************************************************************************************/

//Main Mouse Mode loop
void Mouse_Mode (void)
{
	signed char delta_x, delta_y;
	unsigned char delta_magnitude_counter = 0;
	unsigned char mouse_buttons;
	unsigned char exit = 0;

	//Wait until mode change button is released.
	while ((Global_RX_Bytes[0] & (1<<BUTTON_D)) !=0)
	{
		usbPoll();
		Send_Mouse_Command (0, 0, 0);
		Poll_And_Update_LEDs(MODE_MOUSE);
		send_packets();
		hid_clear();
	}

	//Main mouse mode loop.
	while (exit != 1)
	{
		//Increment Global_Timeout.  This is reset to 0 whenever a packet is received by the USART receive interrupt subroutine
		Global_Timeout++;

		//If timeout value is reached, change RGB LED (mode indicator) to red to indicate loss of signal then wait until signal returns.
		while (Global_Timeout >= MAX_TIMEOUT)
		{
			usbPoll();
			Send_Mouse_Command (0, 0, 0);
			Poll_And_Update_LEDs(MODE_NO_SIGNAL);
			send_packets();
			hid_clear();
			TIMER=0;
			while(TIMER<TEN_MILLISECONDS);
		}

//For debugging
//USART_Transmit(Global_RX_Bytes[2]);

		usbPoll();//usbPoll must be executed at least every 50ms.

		//If button E pressed, mouse motion is enabled so set
		//values for mouse buttons and mouse motion.
		if ( (Global_RX_Bytes[0] & (1<<BUTTON_E)) != 0)
		{
			usbPoll();
			
			mouse_buttons = Get_Mouse_Buttons();
			
			unsigned char sector = Poll_Sector();
			if (delta_magnitude_counter<DELTA_MAGNITUDE_COUNTER_SPEED_1)
			{
				delta_magnitude_counter++;
				delta_x = Get_Delta_X(sector, DELTA_MAGNITUDE_SPEED_1);
				delta_y = Get_Delta_Y(sector, DELTA_MAGNITUDE_SPEED_1);
			}
			else if (delta_magnitude_counter<DELTA_MAGNITUDE_COUNTER_SPEED_2)
			{
				delta_magnitude_counter++;
				delta_x = Get_Delta_X(sector, DELTA_MAGNITUDE_SPEED_2);
				delta_y = Get_Delta_Y(sector, DELTA_MAGNITUDE_SPEED_2);
			}
			else
			{
				delta_magnitude_counter = DELTA_MAGNITUDE_COUNTER_SPEED_2;
				delta_x = Get_Delta_X(sector, DELTA_MAGNITUDE_SPEED_3);
				delta_y = Get_Delta_Y(sector, DELTA_MAGNITUDE_SPEED_3);
			}
		}

		//Else if button E NOT pressed, mouse motion is disabled so
		//set mouse buttons and zeros for mouse motion.
		else if ( (Global_RX_Bytes[0] & (1<<BUTTON_E)) == 0)
		{
			usbPoll();
			
			mouse_buttons = Get_Mouse_Buttons();

			delta_magnitude_counter = 0;
			delta_x = 0;
			delta_y = 0;
		}

		//Send the mouse button and motion values to the PC.
		Send_Mouse_Command (mouse_buttons, delta_x, delta_y);
		Poll_And_Update_LEDs(MODE_MOUSE);

		//Check to see if the mode change button is pressed.  If so, wait about a second then check again.
		//If it is still pressed, exit mouse mode.
		if ((Global_RX_Bytes[0] & (1<<BUTTON_D)) !=0)
		{
			unsigned char i=0;
			while( (i < MODE_CHANGE_ITERATIONS) & ((Global_RX_Bytes[0] & (1<<BUTTON_D)) != 0) )//Wait 1/10 of a second and check again to ensure it wasn't a fluke.
			{
				usbPoll();
				Send_Mouse_Command(0, 0, 0);
				Poll_And_Update_LEDs(MODE_MOUSE);
				send_packets();
				hid_clear();
				i++;
			}

			if (i >= MODE_CHANGE_ITERATIONS)
			{
				Global_Mode = MODE_KEYBOARD;
				exit = 1;
			}
		}

		send_packets();
		hid_clear();
	}

	//When exiting mouse mode loop (mode button pressed) send to pc
	//no mouse buttons and no mouse motion to clear last values sent.
	Send_Mouse_Command (0, 0, 0);
	Poll_And_Update_LEDs(MODE_MOUSE);
}
