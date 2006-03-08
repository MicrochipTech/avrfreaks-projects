#include <eeprom.h>
#include "defines.h"

/* Handel the buzzer playing tones */

/* The buzzer is able to command notes with half periods that are multiples
   of 8 uS.  At the higher freqencies it is hard to generate notes that are
   an honest half step apart.  I used a Lisp program to search for notes in
   a useful range that have the smallest differences between the frequency
   I desire and the one I am able to command.  This is it:
 */
 
const unsigned char notes[] = {//
  0,     //    [0] This codes for no sound from the buzzer
  117,   // D  [1] Error = 0.4%,  117 => 532 hz  (between C# and D)
  111,	 // D# [2]-.1
  105,	 // E  [3]
  99,	 // F  [4]
  93,	 // F# [5]  0
  88,	 // G  [6]
  83,	 // G# [7]  []
  78,	 // A  [8]
  74,	 // A# [9]  0
  70,	 // B  [10]
  66,	 // hC  [11]   -10
  62,	 // hC# [12]
  59,	 // hD  [13]
  55,	 // hD# [14]
  52,	 // hE  [15]
  49,	 // hF  [16]
  47,	 // hF# [17]
  44,	 // hG  [18]  0
  42,	 // hG# [19] 
  39,	 // hA  [20]
  37,	 // hA# [21]  0    -20
  35,	 // hB  [22]
  33,	 // hhC  [24]
  31,	 // hhC# [24]
  29,	 // hhD  [25]
  28,	 // hhD# [26]
  26,	 // hhE  [27]
  25,	 // hhF  [28]
  23,	 // hhF# [29]
  22,	 // hhG  [30]
  21, 	 // hhG# [31]  -1.1%, 3009 Hz
  20	 // hhA  Don't use this last one (too much error)
  } ;
  
// Play one of the defined notes
void tone(unsigned char note_index) {
  buzzer(notes[note_index]);
  }

// The onboard tune stored in flash
const unsigned char bi_tune[] = {// BuiltIn_tune // was arr[]
  20,  // Number of notes in song
  note(1,1),
  note(1,2),
  note(1,3),
  note(2,11),
  note(1,3),
  note(2,11),
  note(1,3),
  note(5,11),
  note(1,0),	// Rest here...
  note(1,11),
  note(1,13),
  note(1,14),
  note(1,15),
  note(1,11),
  note(1,13),
  note(2,15),
  note(1,10),
  note(2,13),
  note(5,11),
  note(3,0)
  };

const unsigned char diff_tune[] = {	// Differential Drive conf startup tune
  5,  		   // 5 notes in the song
  note(1,6),
  note(1,10),
  note(1,6),
  note(1,13),
  note(2,0)
  };
  
const unsigned char steer_tune[] = { // Drive/Steer config startup tune
  5,
  note(1,13),
  note(1,10),
  note(1,13),
  note(1,6),
  note(2,0)
  };
  
const unsigned char low_bat_tune[] = { // Low 9v battery tune  
  3,
  note(3,1),
  note(3,7),
  note(3,0)
  };

// Associate names with headings
#define bkw (  0 >> 3)
#define arb ( 32 >> 3)
#define slf ( 64 >> 3)
#define alf ( 96 >> 3)
#define fwd (128 >> 3)
#define arf (160 >> 3)
#define srh (192 >> 3)
#define alb (224 >> 3)

// Step = [ 3 bits of distance, 5 bits of heading]
// If distance = 0, then heading is interpreted as how long to wait
//   in units of 1/4 seconds, e.g. 16 => wait 4 seconds
const unsigned char bi_dance[] = {// BuiltIn Dance
  4,		   		// Very simple dance!
  step(3,alf), 		// Arc forward to the left
  step(0,3),		// A graceful pause
  step(3,arf),		// Arc forward to the right
  step(0,3)			// Another graceful pause
  };

// Get the next note of a song or dance from EEPROM or FLASH  
unsigned char get_step(unsigned char type_index, unsigned char item_index) {
  unsigned char nt;
  switch(type_index) {
    // Case 0-4 called only by sing function
    case 0: nt = bi_tune[item_index];			 // Built in song in flash
		    break;
	case 1: nt = EEPROMread(item_index + EE_SONG_OS);// Built in song in EEPROM
	        break;		
	case 2: nt = diff_tune[item_index];	 // Play the Diff Drive config tune
	        break;
	case 3: nt = steer_tune[item_index]; // Play the Drive/Steer config tune
	        break;		
	case 4: nt = low_bat_tune[item_index];	// Play low battery tune
		    break;			
	// Case 5 & 6 called only by dance behavior
	case 5: nt = EEPROMread(item_index + EE_DANCE_OS);// Dance step from EEPROM
	        break;
	default: nt = bi_dance[item_index];	 // Dance step from flash
	}
	return nt;
}

// Beeper behavior parameters [tones.c]
//unsigned char pb_tempo = 100;	// Tempo * Beat = note duration in mS 
//unsigned char pb_select = 5;	// Select a function for the beeper behavior


extern union { unsigned char u; signed char s;} p_arr[PARAM_MAX]; // from acquire.c

// A song array must have at least TWO items: a count and 1 note.
void sing(unsigned char song_index) {
  extern unsigned int  clock;
  static unsigned char state = 0;
  static unsigned char index = 0;
  static unsigned int  time; // = 0;
  static unsigned int  note_time; // = 0;
  if (song_index == 255)
    {index = 0; state = 0;}	 // Reset condition
  else {
  	 switch (state) { 
	 // State 0 -- Play note and compute time
     case 0: index++;	 // Point at next note (index = 0 is special)
		  	 tone(pitch(get_step(song_index,index))); // Play this note
			 note_time = pb_tempo * beat(get_step(song_index,index));
			 time = clock;
			 // time = clock + (pb_tempo * beat(get_step(song_index,index)));
			 state++;	 // Next state
			 break;
	 // State 2 -- wait for time out and end of song.
	 default: if (time_out(time, note_time)) { //(clock - time > note_time) {	// Remain in state 1 until timeout
	          state = 0;
			  if (index >= get_step(song_index,0)) // If index>note coutn, reset index
			    index = 0;
			  }
			  break;
	}
  }
}

// Tone controls the buzzer.  Given an index do one of these things:
//  0 -- Beeper makes no sound
//  1 -- PhotoCell difference: Freq = f(PCL - PCR)
//  2 -- PhotoCell sum: Freq = f(PCL + PCR)
//  3 -- Bumper: Beep if bumper in non-zero
//  4 -- IR detect: Beep fir IR detection: None - quiet L -lo, R-hi, LR-warb
//  5 -- Play tune in flash
//  6 -- Play tune in EEPROM

void buzzer_beh(unsigned char buzz_select) {
//  extern unsigned char photo_left, photo_right, bump, IR_detect;
  extern unsigned char sensor[];
  extern unsigned char choose_halt, logic_bat;
  unsigned char buzz_sel_internal;
  if (choose_halt)
    buzz_sel_internal = 0;
  else if (logic_bat < LOGIC_BAT_THRESH)
    buzz_sel_internal = BUZZ_BATLOW;
  else
    buzz_sel_internal = buzz_select;
  switch(buzz_sel_internal) {  		// Select one of the ways to control the beeper 
    // 0 => No sound from buzzer
    case 0: buzzer(0);		// Buzzer makes no sound
		 	break;
	// 1 => PC difference
	case 1: buzzer( notes[ (photo_left - photo_right + 256)>>4 ] );
		 	break;	
   	// 2 => PC sum
	case 2: buzzer( notes[ (photo_left + photo_right) >> 4] );
		 	break;
	// This clause explodes the compiler in 6.22A and B
	// 3 => Beep when the bumper goes off
	case 3: buzzer( notes[ (bump * 20) ] ); // High A or OFF
		 	break;
	// 4 => IR detect
	case 4: buzzer( notes[ IR_detect * 10 ] ); // note = 0, 10, 20, or 30
		 	break;
	// case 5 [sing(0)] => Flash tune,   case 6 [sing(1)] => EEPROM tune
	// case 7 [sing(2)] => DiffDrv tune, case 8 [sing(3)] => DrvStr tune
	// case 9 [sing(4)] => EEPROM Dance not tune!
	// case 10 [sing(5)] => BatLow tune
	default: sing(buzz_sel_internal - 5);
			break;
   }

}
