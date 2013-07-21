/*
	Current sequences are:
	
	Sequence 0	Day - All patterns intended for daytime
	Sequence 1	Day - Favorite patterns from Seq 0
	Sequence 2	Night  - All patterns intended for night (static, slow fading)
	Sequence 3	Night  - A small selection of patterns from Sequence 0
	
	To modify sequences:
	
		Under private variables:
			Modify sequence_N_times, including the size of the sequence.
			Modify sequence_info[] with DAY/NIGHT mode and sequence size
			
		At end of this file:
			Modify static void sequence_N_show(); dont forget case breaks!
	
*/

#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define TEXTOUT_P(arg1)			uart1_tx_string_P(arg1)
	#define TEXTOUT(arg1)			uart1_tx_string(arg1)

	#define TEXTOUT_CHAR(arg1)		uart1_tx_char(arg1)
	#define TEXTOUT_INT(arg1)		uart1_tx_string(itoa(arg1, text, 10))
	#define TEXTOUT_FLOAT(arg1)		do { sprintf(text, "%5.2f", arg1); uart1_tx_string(text); } while(0)

	#define MODE_SELECT_TIMEOUT	3

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	struct sequence_info_struct
	{
		//SEQUNCE_MODE_DAY/NIGHT
		const char		mode;
		
		//Number of patterns in this sequence 
		const unsigned char	size;
	};

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static char 	seq_quit = FALSE;
	
	static char		button_skip;
	static char		button_rotate;
	static char		button_select;
	
	static char		mode_auto;	//if true, use light level to select preferred day/night sequence

	static char		forced_measure=TRUE;	//force a light measurement on reset or after changing to auto mode

	static unsigned char	sequence_index=0;

	static char		text[10];

	static unsigned int	time;


	//pattern times for sequences
	static unsigned int sequence_0_times[13] =
	{
		(unsigned int)(20/TIMER_TICK),
		(unsigned int)(15/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(20/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(20/TIMER_TICK),
		(unsigned int)(20/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(1/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK)
	};

	static unsigned int sequence_1_times[7] =
	{
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK)
	};
	
	static unsigned int sequence_2_times[6] =
	{
		(unsigned int)(120/TIMER_TICK),
		(unsigned int)(120/TIMER_TICK),
		(unsigned int)(120/TIMER_TICK),
		(unsigned int)(120/TIMER_TICK),
		(unsigned int)(120/TIMER_TICK),
		(unsigned int)(120/TIMER_TICK)
	};
	
	static unsigned int sequence_3_times[3] =
	{
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK),
		(unsigned int)(30/TIMER_TICK)
	};

	//sequence info
	struct sequence_info_struct sequence_info[NUMBER_OF_SEQUENCES] =
	{
		{ SEQUENCE_MODE_DAY, 13 },		// sequence 0 DAY/NIGHT , number of patterns in sequence
		{ SEQUENCE_MODE_DAY, 7 },		// sequence 1 DAY/NIGHT , number of patterns in sequence
		{ SEQUENCE_MODE_NIGHT, 6 },	// sequence 2 DAY/NIGHT , number of patterns in sequence
		{ SEQUENCE_MODE_NIGHT, 3 }		// sequence 3 DAY/NIGHT , number of patterns in sequence
	};

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void show_hold(void);
	static void show_rotate(void);

	static void menu_show(void);

	//function to display a pattern for sequence_day
	static void sequence_0_show(unsigned char index);
	static void sequence_1_show(unsigned char index);
	static void sequence_2_show(unsigned char index);
	static void sequence_3_show(unsigned char index);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

void sequence_tick(void)
{
	if(time)
		time--;
}

void sequence_init(void)
{
	sequence_index = settings_ram.sequence;
	mode_auto = settings_ram.mode_auto;
}

//stop display and exit
void sequence_exit(void)
{
	seq_quit=TRUE;
	pattern_break();
}

void sequence_skip(void)
{
	button_skip=TRUE;
	pattern_break();
}

void sequence_rotate_en(void)
{
	button_rotate=TRUE;
	pattern_break();
}

void sequence_select_press(void)
{
	seq_quit=TRUE;
	pattern_break();
	button_select=TRUE;
}

void sequence_save_brightness(void)
{
	settings_ram.sequence_settings[sequence_index].brightness = power_vled_get();
	settings_save();	
}

void sequence_menu(void)
{
	unsigned char x;

	if(button_select)
	{
		TEXTOUT_P(PSTR("\r\nSequence menu\r\n"));
		
		led_framerate(100);
		led_plane_intensity_all(255);	
		power_vled_set(255);
		
		do
		{
			button_select = FALSE;
			time = (int)(MODE_SELECT_TIMEOUT/TIMER_TICK);
			
			led_blank();
			if(mode_auto)
			{
				text_set_P(PSTR("A"));
				x=0;
				while(!text_isfinished())
					text_draw_y(x++, 1, 0);	
			}
			else
				menu_show();
			
			while(!button_select && time)
				main_fly();
			
			if(button_select)
			{
				if(mode_auto)
					mode_auto=FALSE;
				else
				{
					sequence_index++;
					if(sequence_index == NUMBER_OF_SEQUENCES)
					{
						sequence_index=0;
						mode_auto=TRUE;
					};
				};
				
				if(mode_auto)
				{
					TEXTOUT_P(PSTR("Auto, sequence "));
					TEXTOUT_INT(settings_ram.sequence_auto_day);
					TEXTOUT_P(PSTR("during day, sequence "));
					TEXTOUT_INT(settings_ram.sequence_auto_night);
					TEXTOUT_P(PSTR("during night\r\n"));
				}
				else
				{
					TEXTOUT_P(PSTR("Sequence "));
					TEXTOUT_INT(sequence_index);
					if(sequence_info[sequence_index].mode == SEQUENCE_MODE_DAY)
						TEXTOUT_P(PSTR(" (day)\r\n"));
					else
						TEXTOUT_P(PSTR(" (night)\r\n"));
				};
			};			
		}while(time);
		if(mode_auto == FALSE)
		{
			TEXTOUT_P(PSTR("Preferred "));
			if(sequence_info[sequence_index].mode == SEQUENCE_MODE_DAY)
			{
				TEXTOUT_P(PSTR("day"));
				settings_ram.sequence_auto_day = sequence_index;
			}
			else
			{
				TEXTOUT_P(PSTR("night"));
				settings_ram.sequence_auto_night = sequence_index;
			};
			TEXTOUT_P(PSTR(" sequence = "));
			TEXTOUT_INT(sequence_index);
			TEXTOUT_P(PSTR("\r\n"));
			settings_ram.sequence = sequence_index;
			settings_ram.mode_auto = FALSE;
		}
		else
		{
			settings_ram.mode_auto = TRUE;
			forced_measure=TRUE;
		};
		settings_save();
	};
}


/*
 *Sequence psudo code is:
 *
 *	Light sensing enabled?

	Yes
		Measure light
		Do we still want THIS sequence?
		No
			Finished

	Finished?
	No
		(Determine time to display pattern)
		Rotation enabled?
		Yes
			Lookup pattern duration
		No
			Light sensing enabled?
			Yes
				30sec
			No
				Forever

		Display pattern X

	Exit requested?
	Yes
		Finished
	No
		Skip pressed?
		Yes
			Rotation enabled?
				Disable rotation
			No
				Advance X
				Skip next light sense (to speed up button response)

		Rotate_en pressed?
		Yes
			Rotation enabled?
			No
				Enable rotation

		Rotation enabled?
		Yes
			Advance X

	Loop while not finished
 */

void sequence_run(void)
{
	unsigned char pattern_index=0;
	char finished=FALSE;
	char skip_measure=FALSE;

	unsigned int tempint;

	seq_quit=FALSE;
	
	if(forced_measure && mode_auto);
	else
	{
		TEXTOUT_P(PSTR("\r\nRunning sequence "));
		TEXTOUT_INT(sequence_index);
		TEXTOUT_P(PSTR("\r\nMode "));
		if(sequence_info[sequence_index].mode == SEQUENCE_MODE_DAY)
			TEXTOUT_P(PSTR("DAY\r\n"));
		else
			TEXTOUT_P(PSTR("NIGHT\r\n"));
			
		TEXTOUT_INT(sequence_info[sequence_index].size);
		TEXTOUT_P(PSTR(" patterns in sequence\r\n"));
		
		TEXTOUT_P(PSTR("Brightness "));
		TEXTOUT_INT(settings_ram.sequence_settings[sequence_index].brightness);
		TEXTOUT_P(PSTR("(/255)\r\n"));
		
		TEXTOUT_P(PSTR("Rotation "));
		if(settings_ram.sequence_settings[sequence_index].rotation)
			TEXTOUT_P(PSTR("enabled\r\n"));
		else
		{
			pattern_index = settings_ram.sequence_settings[sequence_index].preferred;
			TEXTOUT_P(PSTR("disabled\r\nPattern "));
			TEXTOUT_INT(pattern_index);
			TEXTOUT_P(PSTR("\r\n"));
		};
	};

	power_vled_set(settings_ram.sequence_settings[sequence_index].brightness);

	do
	{
		//Light sensing enabled?
		if(mode_auto && !skip_measure)
		{
			//Do we still want DAY sequence?
			tempint = a2d_light();
			TEXTOUT_P(PSTR("Light = "));
			TEXTOUT_INT(tempint);
			
			if(forced_measure)
			{
				forced_measure=FALSE;
				finished=TRUE;
				TEXTOUT_P(PSTR("\r\nSelecting "));
				if(tempint < (settings_ram.threshold_night + settings_ram.threshold_day)/2)
				{
					sequence_index = settings_ram.sequence_auto_night;
					TEXTOUT_P(PSTR("night "));
				}
				else
				{
					sequence_index = settings_ram.sequence_auto_day;
					TEXTOUT_P(PSTR("day "));
				};
				TEXTOUT_P(PSTR(" sequence "));
				TEXTOUT_INT(sequence_index);
			}
			else
			{
				if(sequence_info[sequence_index].mode == SEQUENCE_MODE_DAY)
				{
					if(tempint < settings_ram.threshold_night)
					{
						sequence_index = settings_ram.sequence_auto_night;
						finished=TRUE;
						TEXTOUT_P(PSTR("\r\nSelecting night sequence "));
						TEXTOUT_INT(sequence_index);
					}
					else
						TEXTOUT_P(PSTR("(still day)"));
				}
				else
				{
					if(tempint > settings_ram.threshold_day)
					{
						sequence_index = settings_ram.sequence_auto_day;
						finished=TRUE;
						TEXTOUT_P(PSTR("\r\nSelecting day sequence "));
						TEXTOUT_INT(sequence_index);
					}
					else
						TEXTOUT_P(PSTR("(still night)"));
				};
			};
			TEXTOUT_P(PSTR("\r\n"));
			
			//if we changed the current sequence index, display it 
			if(finished)
			{
				led_framerate(100);
				led_plane_intensity_all(255);	
				power_vled_set(255);
				menu_show();
				time = (unsigned int)(0.5/TIMER_TICK);
				while(time)
					main_fly();
			};
		};
		skip_measure=FALSE;
		
		if(!finished)
		{
			//Determine time to display pattern
			//Rotation enabled?
			if(settings_ram.sequence_settings[sequence_index].rotation)
			{
				//Lookup pattern duration
				switch(sequence_index)
				{
					case 0:	pattern_duration_set(sequence_0_times[pattern_index]); break;
					case 1:	pattern_duration_set(sequence_1_times[pattern_index]); break;
					case 2:	pattern_duration_set(sequence_2_times[pattern_index]); break;
					case 3:	pattern_duration_set(sequence_3_times[pattern_index]); break;
				};
			}
			else
			//No rotation
			{
				//Light sensing enabled?
				if(mode_auto)
					//Yes, expire after 30sec to measure light
					pattern_duration_set(30/TIMER_TICK);
				else
					//No, run until pattern_break() called
					pattern_duration_set(0);
			};
			
			//Display pattern
			TEXTOUT_P(PSTR("Pattern "));
			TEXTOUT_INT(pattern_index);
			TEXTOUT_P(PSTR("\r\n"));
			switch(sequence_index)
			{
				case 0: sequence_0_show(pattern_index); break;
				case 1: sequence_1_show(pattern_index); break;
				case 2: sequence_2_show(pattern_index); break;
				case 3: sequence_3_show(pattern_index); break;										
			};
			
		};
		
		//Exit requested?
		if(seq_quit)
			finished=TRUE;
		else
		{
			//Skip pressed?
			if(button_skip)
			{
				button_skip=FALSE;
				//Rotation enabled?
				if(settings_ram.sequence_settings[sequence_index].rotation)
				{
					//Yes, disable it, display hold
					settings_ram.sequence_settings[sequence_index].rotation = FALSE;
					settings_ram.sequence_settings[sequence_index].preferred = pattern_index;
					TEXTOUT_P(PSTR("Rotation disabled\r\nPreferred pattern = "));
					TEXTOUT_INT(pattern_index);
					TEXTOUT_P(PSTR("\r\n"));
					show_hold();
				}
				else
				{
					//No, skip to next pattern
					pattern_index++;
					if(pattern_index == sequence_info[sequence_index].size)
						pattern_index=0;					
					
					settings_ram.sequence_settings[sequence_index].preferred = pattern_index;
					
					TEXTOUT_P(PSTR("Preferred pattern = "));
					TEXTOUT_INT(pattern_index);
					TEXTOUT_P(PSTR("\r\n"));
					
					//skip next light measurement to speed up button response
					skip_measure = TRUE;
				};
				settings_save();
			};
			
			//Rotate enable pressed? (skip button held for 1sec)
			if(button_rotate)
			{
				button_rotate=FALSE;
				if(!settings_ram.sequence_settings[sequence_index].rotation)
				{
					TEXTOUT_P(PSTR("Rotation enabled\r\n"));
					settings_ram.sequence_settings[sequence_index].rotation = TRUE;
					show_rotate();
				};
				settings_save();
			};
			
			//Rotate patterns?
			if(settings_ram.sequence_settings[sequence_index].rotation)
			{
				pattern_index++;
				if(pattern_index == sequence_info[sequence_index].size)
					pattern_index=0;
			};
		};
		
	}while(!finished);
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

//Display STOP symbol
static void show_hold(void)
{
	led_framerate(100);
	led_blank();
	led_plane_intensity_all(255);

	led_line(2, 0, 0, 5, 0, 0, TRUE);
	led_line(2, 7, 0, 5, 7, 0, TRUE);
	led_line(7, 2, 0, 7, 5, 0, TRUE);
	led_line(0, 2, 0, 0, 5, 0, TRUE);	
	
	led_waitframe(50);
};

//Display ROTATION symbol
static void show_rotate(void)
{
	unsigned char x = 0, j=0;

	led_framerate(60);
	led_plane_intensity_all(255);

	for(j=0; j!=1; j++)
	{
		for(x=0; x!=8; x++)
		{
			led_blank();
			led_line(x, 0, 0, 7-x, 7, 0, TRUE);
			led_waitframe(1);
		};
		for(x=0; x!=8; x++)
		{
			led_blank();
			led_line(7, x, 0, 0, 7-x, 0, TRUE);
			led_waitframe(1);
		};
	};
};

static void menu_show(void)
{
	unsigned char x;

	text[0] = sequence_index+'0';
	text[1] = 0;
	text_set(text);
	x=0;
	while(!text_isfinished())
		text_draw_y(x++, 1, 0);
	
	if(sequence_info[sequence_index].mode == SEQUENCE_MODE_DAY)
		text_set_P(PSTR("D"));
	else
		text_set_P(PSTR("N"));
	
	x=7;
	while(!text_isfinished())
		text_draw_y(0, 1, x--);	
}

// Function to display a pattern from sequence
// Don't forget to break; each case!!
static void sequence_0_show(unsigned char index)
{
	//Display pattern
	switch(index)
	{
		case 0: pattern_waveup( 255, 1, 32, 200);break;
		case 1: pattern_diamond_scroll();break;
		case 2: pattern_popcorn();break;
		case 3: pattern_rotate1();break;
		case 4: pattern_rain1();break;
		case 5: pattern_screw();break;
		case 6: pattern_cube_stretch();break;
		case 7: pattern_fountain();break;
		case 8: pattern_spinface();break;
		case 9: pattern_flapsheet();break;
		case 10: pattern_text1_P(PSTR("Cuboid        "));break;
		case 11: pattern_noise_scroll();break;
		case 12: pattern_fireworks();break;
	};
}

//function to display a pattern from sequence
static void sequence_1_show(unsigned char index)
{
	//Display pattern
	switch(index)
	{
		case 0: pattern_popcorn();break;
		case 1: pattern_rotate1();break;
		case 2: pattern_rain1();break;
		case 3: pattern_screw();break;
		case 4: pattern_cube_stretch();break;
		case 5: pattern_flapsheet();break;
		case 6: pattern_noise_scroll();break;
	};
}

//function to display a pattern from sequence
static void sequence_2_show(unsigned char index)
{
	//Display pattern
	switch(index)
	{
		case 0:	pattern_wirecube(); break;
		case 1:	pattern_wirecubediag(); break;
		case 2: pattern_tesseract();break;
		case 3:	pattern_cubewalls(); break;
		case 4:	pattern_diamond(); break;
		case 5: pattern_waveup( 255, 10, 64, 200);break;
	};
};

//function to display a pattern from sequence
static void sequence_3_show(unsigned char index)
{
	//Display pattern
	switch(index)
	{
		case 0:	pattern_screw(); break;
		case 1:	pattern_flapsheet(); break;
		case 2:	pattern_rain1(); break;
	};
};
