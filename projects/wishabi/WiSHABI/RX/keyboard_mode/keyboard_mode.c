#include "keyboard_mode.h"

/*******************************************************************************************************************************************/

//Sends a keyboard keypress (c) along with any relevant modifiers (mod) (shift, alt, ctrl or GUI)
void Send_Keyboard_Char (unsigned char mod, unsigned char c)
{
	tx_kbd = 1;
	inputBuffer1[1] = mod;	//Condition of modifiers
	inputBuffer1[2] = c;	//Keyboard keypress to be sent
	send_packets();
	TIMER=0;
	while(TIMER<TEN_MILLISECONDS);//Minimum of 1/10 of a second between sending prevents missed characters
}
/*******************************************************************************************************************************************/

//When a button press is detected, this function can be used to wait
//until said button is released.  The parameter 'key' represents a
//keyboard character that will continuously be sent to the PC until
//the button is released.  Usually this will be KEY_NO_KEY.
void Wait_Till_Release(unsigned char button, unsigned char key)
{
	while ((Global_RX_Bytes[0] & (1<<button)) !=0)
	{
		usbPoll();
		Send_Keyboard_Char(0, key);
		Poll_And_Update_LEDs(MODE_KEYBOARD);
		send_packets();
		hid_clear();
	}
}
/*******************************************************************************************************************************************/

//This function takes the address of the character that has just
//been sent to the the PC and replaces the contents of that address
//with the next character in the cycle (depending on the tilt
//orientation).  E.g. a->b->c->2->a
void Increment_Char(unsigned char *mod, unsigned char *c)
{
	if (*mod != 0)
	{
		switch(*c)
		{
			case KEY_FORWARD_SLASH://For '?'
				*c = KEY_1;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '!'
				break;
			case KEY_1://For '!'
				*c = KEY_2;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '@'
				break;
			case KEY_2://For '@'
				*c = KEY_INVERTED_COMMA;
				*mod = (0);
				break;
			case KEY_HYPHEN://For '_'
				*c = KEY_9;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '('
				break;
			case KEY_9://For '('
				*c = KEY_0;
				//*mod = (1<<MOD_LEFT_SHIFT);//For ')'
				break;
			case KEY_0://For ')'
				*c = KEY_COLON;
				*mod = (0);
				break;
			case KEY_COLON://For ';'
				*c = KEY_7;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '&'
				break;
			case KEY_7://For '&'
				*c = KEY_FORWARD_SLASH;
				*mod = (0);
				break;
			case KEY_5://For '%'
				*c = KEY_8;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '*'
				break;
			case KEY_8://For '*'
				*c = KEY_3;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '#'
				break;
			case KEY_3://For '#'
				*c = KEY_EQUALS;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '+'
				break;
			case KEY_EQUALS://For '+'
				*c = KEY_COMMA;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '<'
				break;
			case KEY_COMMA://For '<'
				*c = KEY_EQUALS;
				*mod = (0);
				break;
			case KEY_PERIOD://For '>'
				*c = KEY_INVERTED_COMMA;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '"'
				break;
			case KEY_INVERTED_COMMA://For '"'
				*c = KEY_4;
				//*mod = (1<<MOD_LEFT_SHIFT);//For '$'
				break;
			case KEY_4://For '$'
				*c = KEY_SPACE;
				*mod = (0);
				break;
		}
	}
	else
	{
		switch(*c)
		{
			case KEY_A:
			case KEY_B:
			case KEY_D:
			case KEY_E:
			case KEY_G:
			case KEY_H:
			case KEY_J:
			case KEY_K:
			case KEY_M:
			case KEY_N:
			case KEY_P:
			case KEY_Q:
			case KEY_R:
			case KEY_T:
			case KEY_U:
			case KEY_W:
			case KEY_X:
			case KEY_Y:
			case KEY_9:
				(*c)++;
				break;
	
			case KEY_C:
				*c = (*c + 2);
			case KEY_F:
				*c = (*c + 2);
			case KEY_I:
				*c = (*c + 2);
			case KEY_L:
				*c = (*c + 2);
			case KEY_O:
				*c = (*c + 3);
			case KEY_S:
				*c = (*c + 2);
			case KEY_V:
				*c = (*c + 3);
			case KEY_Z:
				*c = (*c + 9);
				break;
	
			case KEY_2:
				*c = (*c - 2);
			case KEY_3:
				*c = (*c - 2);
			case KEY_4:
				*c = (*c - 2);
			case KEY_5:
				*c = (*c - 2);
			case KEY_6:
				*c = (*c - 2);
			case KEY_7:
				*c = (*c - 3);
			case KEY_8:
				*c = (*c - 1);
			case KEY_0:
				*c = (*c - 13);
			break;
	
			case KEY_SPACE:
				*c = KEY_1;
				break;
			case KEY_1:
				*c = KEY_PERIOD;
				break;
			case KEY_PERIOD:
				*c = KEY_COMMA;
				break;
			case KEY_COMMA:
				*c = KEY_FORWARD_SLASH;
				*mod = (1<<MOD_LEFT_SHIFT);//For '?'
				break;
			case KEY_INVERTED_COMMA:
				*c = KEY_HYPHEN;
				break;
			case KEY_HYPHEN:
				*c = KEY_HYPHEN;
				*mod = (1<<MOD_LEFT_SHIFT);//For '_'
				break;
			case KEY_COLON:
				*c = KEY_COLON;
				*mod = (1<<MOD_LEFT_SHIFT);//For ';'
				break;
			case KEY_FORWARD_SLASH:
				*c = KEY_5;
				*mod = (1<<MOD_LEFT_SHIFT);//For '%'
				break;
			case KEY_EQUALS:
				*c = KEY_PERIOD;
				*mod = (1<<MOD_LEFT_SHIFT);//For '>'
				break;
		}
	}
}
/*******************************************************************************************************************************************/

//This loop runs when button E is pressed.  It checks the tilt orientation
//and sends to the PC the corresponding keystroke.  It then waits for the
//typing timeout (or button B) before exiting.  If button E is pressed again before said
//timeout, the previously typed character will be replaced with the next
//character in the cycle (depending on the tilt orientation).
void Type_By_Sector(unsigned char sect)
{
	unsigned char c;
	unsigned char mod = 0;//Modifiers

	//Sets the first typed character depending on the transmutter unit's tilt orientation.
	if (sect == SECT_1) c=KEY_SPACE;
	else if (sect == SECT_2)c=KEY_A;
	else if (sect == SECT_3) c=KEY_D;
	else if (sect == SECT_4) c=KEY_G;
	else if (sect == SECT_5) c=KEY_J;
	else if (sect == SECT_6) c=KEY_M;
	else if (sect == SECT_7) c=KEY_P;
	else if (sect == SECT_8) c=KEY_T;
	else if (sect == SECT_9) c=KEY_W;

	usbPoll();

	Send_Keyboard_Char(0, c);
	Send_Keyboard_Char(0, KEY_NO_KEY);
	Send_Keyboard_Char((1<<MOD_LEFT_SHIFT), KEY_LEFT_ARROW);//Select the letter just typed
	Send_Keyboard_Char(0, KEY_NO_KEY);//Send a NO_KEY
	Poll_And_Update_LEDs(MODE_KEYBOARD);
	Wait_Till_Release(BUTTON_E, KEY_NO_KEY);
	unsigned int time=0;
	//Waits for timeout, button B (backspace) or button C (enter).
	while((time<TYPE_TIMEOUT) & ((Global_RX_Bytes[0] & ((1<<BUTTON_B) + (1<<BUTTON_C))) == 0))
	{
		if ((Global_RX_Bytes[0] & (1<<BUTTON_E)) != 0)
		{
			Increment_Char(&mod, &c);
			if(mod==0)
			{
				Send_Keyboard_Char(0, c);
			}
			else
			{
				Send_Keyboard_Char((1<<MOD_LEFT_SHIFT), c);
			}
			Send_Keyboard_Char((1<<MOD_LEFT_SHIFT), KEY_LEFT_ARROW);//Select the letter just typed
			Send_Keyboard_Char(0, KEY_NO_KEY);//Send a NO_KEY
			Poll_And_Update_LEDs(MODE_KEYBOARD);
			Wait_Till_Release(BUTTON_E, KEY_NO_KEY);
			time=0;
		}

		usbPoll();
		Send_Keyboard_Char(0, KEY_NO_KEY);
		Poll_And_Update_LEDs(MODE_KEYBOARD);
		time++;
	}

	//if loop exited by means of button B (backspace)
	//delete the unconfirmed character then wait for
	//button B to be released so that 'KEY_BACKSPACE'
	//isn't continuously sent.
	if ((Global_RX_Bytes[0] & (1<<BUTTON_B)) != 0)
	{
		Send_Keyboard_Char(0, KEY_BACKSPACE);
		Send_Keyboard_Char(0, KEY_NO_KEY);//Send a NO_KEY
		Wait_Till_Release(BUTTON_B, KEY_NO_KEY);
	}
	//if loop exited by means of button C (enter)
	//confirm the character early (don't wait for timeout)
	//then wait for button B to be released so that
	//'KEY_ENTER' isn't continuously sent.
	else if ((Global_RX_Bytes[0] & (1<<BUTTON_C)) != 0)
	{
		Send_Keyboard_Char(0, KEY_RIGHT_ARROW);
		Send_Keyboard_Char(0, KEY_NO_KEY);//Send a NO_KEY
		Wait_Till_Release(BUTTON_C, KEY_NO_KEY);
	}
	//Otherwise loop exited by means of a typing timeout.
	//Simulate a right-arrow keypress to prepare for the next keystroke.
	else
	{
		Send_Keyboard_Char(0, KEY_RIGHT_ARROW);
		Send_Keyboard_Char(0, KEY_NO_KEY);//Send a NO_KEY
	}
}
/*******************************************************************************************************************************************/

//Main Keyboard Mode loop
void Keyboard_Mode (void)
{
	unsigned char exit = 0;

	//Wait until mode change button is released.
	Wait_Till_Release(BUTTON_D, KEY_NO_KEY);

	usbPoll();

	//Main mouse mode loop.
	while (exit != 1)
	{
		//Increment Global_Timeout.  This is reset to 0 whenever a packet is received by the USART receive interrupt subroutine
		Global_Timeout++;

		//If timeout value is reached, change RGB LED (mode indicator) to red to indicate loss of signal then wait until signal returns.
		while (Global_Timeout >= MAX_TIMEOUT)
		{
			usbPoll();
			Send_Keyboard_Char(0, KEY_NO_KEY);
			Poll_And_Update_LEDs(MODE_NO_SIGNAL);
			send_packets();
			hid_clear();
			TIMER=0;
			while(TIMER<TEN_MILLISECONDS);
		}

//For debugging
//USART_Transmit(Global_RX_Bytes[2]);

		usbPoll();
		Send_Keyboard_Char(0, KEY_NO_KEY);
		Poll_And_Update_LEDs(MODE_KEYBOARD);
		send_packets();
		hid_clear();
		
		//Check to see if Caps Lock (Button A) is pressed.  If so, toggle caps lock then wait until it is released.
		if ((Global_RX_Bytes[0] & (1<<BUTTON_A)) !=0)
		{
			TIMER=0;
			while(TIMER<TEN_MILLISECONDS);//Wait 10ms then check for button A again (ensure it's actually pressed)
			if ((Global_RX_Bytes[0] & (1<<BUTTON_A)) !=0)
			{
				//MAnually send the keystroke instead of using function "Send_Keyboard_Char"
				//because the 10ms delay in said function sometimes causes a fault.
				tx_kbd = 1;
				inputBuffer1[1] = 0;	//Condition of modifiers
				inputBuffer1[2] = KEY_CAPS_LOCK;	//Keyboard keypress to be sent
				send_packets();
				Wait_Till_Release(BUTTON_A, KEY_NO_KEY);
			}
		}

		//Check to see if Backspace (Button B) is pressed.  If so, continue sending backspace until button is released.
		if ((Global_RX_Bytes[0] & (1<<BUTTON_B)) !=0)
		{
			TIMER=0;
			while(TIMER<TEN_MILLISECONDS);//Wait 10ms then check for button B again (ensure it's actually pressed)
			if ((Global_RX_Bytes[0] & (1<<BUTTON_B)) !=0)
			{
				Wait_Till_Release(BUTTON_B, KEY_BACKSPACE);
			}
		}

		//Check to see if Enter (Button C) is pressed.  If so, continue sending Enter until button is released.
		if ((Global_RX_Bytes[0] & (1<<BUTTON_C)) !=0)
		{
			TIMER=0;
			while(TIMER<TEN_MILLISECONDS);//Wait 10ms then check for button C again (ensure it's actually pressed)
			if ((Global_RX_Bytes[0] & (1<<BUTTON_C)) !=0)
			{
				Wait_Till_Release(BUTTON_C, KEY_ENTER);
			}
		}

		//Check to see if type button (Button E) is pressed.  If so, type by sector.
		if ((Global_RX_Bytes[0] & (1<<BUTTON_E)) !=0)
		{
			TIMER=0;
			while(TIMER<TEN_MILLISECONDS);//Wait 10ms then check for button E again (ensure it's actually pressed)
			if ((Global_RX_Bytes[0] & (1<<BUTTON_E)) !=0)
			{
				Type_By_Sector(Poll_Sector());
			}
		}

		//Check to see if the mode change button is pressed.  If so, wait about a second then check again.
		//If it is still pressed, exit keyboard mode.
		if ((Global_RX_Bytes[0] & (1<<BUTTON_D)) !=0)
		{
			unsigned char i=0;
			while( (i < MODE_CHANGE_ITERATIONS) & ((Global_RX_Bytes[0] & (1<<BUTTON_D)) != 0) )//Wait 1/10 of a second and check again to ensure it wasn't a fluke.
			{
				usbPoll();
				Send_Keyboard_Char(0, KEY_NO_KEY);
				Poll_And_Update_LEDs(MODE_KEYBOARD);
				send_packets();
				hid_clear();
				i++;
			}

			if (i >= MODE_CHANGE_ITERATIONS)
			{
				Global_Mode = MODE_MOUSE;
				exit = 1;
			}
		}
	}

	//When exiting mouse mode loop (mode button pressed) send to pc
	//a no_key to clear last values sent.
	Send_Keyboard_Char(0, KEY_NO_KEY);
	Poll_And_Update_LEDs(MODE_KEYBOARD);
}

