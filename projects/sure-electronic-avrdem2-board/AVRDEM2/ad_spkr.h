// ad_spkr.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//

#include <iom16v.h>
#include <macros.h>
#include <stdio.h>
#define OK_SPKR

// ----------- function prototypes
void init_spkr(void);           // initialize the speaker
void start_song(__flash int *); // start a song
void stop_playing(void);        // stop a song

extern __flash int allnotes[];
extern __flash int elise[];

extern volatile char playing;   // whether a song is cuurently playing
extern char cnt_tempo;          // tempo countdown during playing
extern char cnt_dur;            // note duration countdown during playing
extern char cur_tempo;          // current tempo for active song
extern char cur_dur;            // current duration for current note in active song
extern char cur_rpt;            // repeat count for current song
extern __flash int *cur_song;   // pointer to current song (for repeats)
extern __flash int *cur_note;   // pointer to next note in current song

// command definitions
#define MSK_CMD  0x6000         // mask for recognizing commands
#define N_TEMP   0x4000         // change tempo
#define N_DUR    0x6000         // change duration

// individual notes can be expressed as eighth, quarter, half, or whole notes
// (or rests) without changing the duration
#define MSK_LEN  0x1800         // mask for temp note duration
#define N_4      0x0000         // quarter note (default)
#define N_8      0x0800         // eighth note
#define N_2      0x1000         // half note
#define N_1      0x1800         // whole note

#define N_REST 1                // rest (may combine with N_8, N_1, etc.)

// each quarter note is TEMPO * DURATION milliseconds long
// half notes are 2 * DURATION, eighth notes are DURATION / 2

#define TEMPO 12                // default tempo
#define DURATION 24             // default duration

// a song consists of a repeat count followed by notes and/or parameters
// (tempo, duration, rests) terminated by a 0.  all values are ints.  when
// the song starts the default tempo, duration, and volume are in effect,
// and each stays in effect until changed.
//
// the note definitions are the compare count (low 8 bits) and prescaler
// (bits 8 - 10) for the timer using CTC with output toggle.

#define N_C2   ((7<<8) | 178)        // C2
#define N_C2S  ((7<<8) | 168)        // C2 sharp
#define N_D2F  ((7<<8) | 168)        // D2 flat
#define N_D2   ((7<<8) | 159)        // D2
#define N_D2S  ((7<<8) | 150)        // D2 sharp
#define N_E2F  ((7<<8) | 150)        // E2 flat
#define N_E2   ((7<<8) | 141)        // E2
#define N_F2   ((7<<8) | 133)        // F2
#define N_F2S  ((7<<8) | 126)        // F2 sharp
#define N_G2F  ((7<<8) | 126)        // G2 flat
#define N_G2   ((7<<8) | 119)        // G2
#define N_G2S  ((7<<8) | 112)        // G2 sharp
#define N_A2F  ((7<<8) | 112)        // A2 flat
#define N_A2   ((7<<8) | 105)        // A2
#define N_A2S  ((7<<8) | 100)        // A2 sharp
#define N_B2F  ((7<<8) | 100)        // B2 flat
#define N_B2   ((7<<8) | 94)         // B2

#define N_C3   ((7<<8) | 89)         // C3
#define N_C3S  ((7<<8) | 84)         // C3 sharp
#define N_D3F  ((7<<8) | 84)         // D3 flat
#define N_D3   ((7<<8) | 79)         // D3
#define N_D3S  ((7<<8) | 74)         // D3 sharp
#define N_E3F  ((7<<8) | 74)         // E3 flat
#define N_E3   ((7<<8) | 70)         // E3
#define N_F3   ((7<<8) | 66)         // F3
#define N_F3S  ((6<<8) | 252)        // F3 sharp
#define N_G3F  ((6<<8) | 252)        // G3 flat
#define N_G3   ((6<<8) | 238)        // G3
#define N_G3S  ((6<<8) | 225)        // G3 sharp
#define N_A3F  ((6<<8) | 225)        // A3 flat
#define N_A3   ((6<<8) | 212)        // A3
#define N_A3S  ((6<<8) | 200)        // A3 sharp
#define N_B3F  ((6<<8) | 200)        // B3 flat
#define N_B3   ((6<<8) | 189)        // B3

#define N_C4   ((6<<8) | 178)        // C4   ((middle C)
#define N_C4S  ((6<<8) | 168)        // C4 sharp
#define N_D4F  ((6<<8) | 168)        // D4 flat
#define N_D4   ((6<<8) | 159)        // D4
#define N_D4S  ((6<<8) | 150)        // D4 sharp
#define N_E4F  ((6<<8) | 150)        // E4 flat
#define N_E4   ((6<<8) | 141)        // E4
#define N_F4   ((6<<8) | 133)        // F4
#define N_F4S  ((5<<8) | 252)        // F4 sharp
#define N_G4F  ((5<<8) | 252)        // G4 flat
#define N_G4   ((5<<8) | 238)        // G4
#define N_G4S  ((5<<8) | 225)        // G4 sharp
#define N_A4F  ((5<<8) | 225)        // A4 flat
#define N_A4   ((5<<8) | 212)        // A4
#define N_A4S  ((5<<8) | 200)        // A4 sharp
#define N_B4F  ((5<<8) | 200)        // B4 flat
#define N_B4   ((5<<8) | 189)        // B4

#define N_C5   ((5<<8) | 178)        // C5
#define N_C5S  ((5<<8) | 168)        // C5 sharp
#define N_D5F  ((5<<8) | 168)        // D5 flat
#define N_D5   ((5<<8) | 159)        // D5
#define N_D5S  ((5<<8) | 150)        // D5 sharp
#define N_E5F  ((5<<8) | 150)        // E5 flat
#define N_E5   ((5<<8) | 141)        // E5
#define N_F5   ((5<<8) | 133)        // F5
#define N_F5S  ((4<<8) | 252)        // F5 sharp
#define N_G5F  ((4<<8) | 252)        // G5 flat
#define N_G5   ((4<<8) | 238)        // G5
#define N_G5S  ((4<<8) | 225)        // G5 sharp
#define N_A5F  ((4<<8) | 225)        // A5 flat
#define N_A5   ((4<<8) | 212)        // A5
#define N_A5S  ((4<<8) | 200)        // A5 sharp
#define N_B5F  ((4<<8) | 200)        // B5 flat
#define N_B5   ((4<<8) | 189)        // B5

#define N_C6   ((4<<8) | 178)        // C6
#define N_C6S  ((4<<8) | 168)        // C6 sharp
#define N_D6F  ((4<<8) | 168)        // D6 flat
#define N_D6   ((4<<8) | 159)        // D6
#define N_D6S  ((4<<8) | 150)        // D6 sharp
#define N_E6F  ((4<<8) | 150)        // E6 flat
#define N_E6   ((4<<8) | 141)        // E6
#define N_F6   ((4<<8) | 133)        // F6
#define N_F6S  ((3<<8) | 252)        // F6 sharp
#define N_G6F  ((3<<8) | 252)        // G6 flat
#define N_G6   ((3<<8) | 238)        // G6
#define N_G6S  ((3<<8) | 225)        // G6 sharp
#define N_A6F  ((3<<8) | 225)        // A6 flat
#define N_A6   ((3<<8) | 212)        // A6
#define N_A6S  ((3<<8) | 200)        // A6 sharp
#define N_B6F  ((3<<8) | 200)        // B6 flat
#define N_B6   ((3<<8) | 189)        // B6

#define N_C7   ((3<<8) | 178)        // C7
#define N_C7S  ((3<<8) | 168)        // C7 sharp
#define N_D7F  ((3<<8) | 168)        // D7 flat
#define N_D7   ((3<<8) | 159)        // D7
#define N_D7S  ((3<<8) | 150)        // D7 sharp
#define N_E7F  ((3<<8) | 150)        // E7 flat
#define N_E7   ((3<<8) | 141)        // E7
#define N_F7   ((3<<8) | 133)        // F7
#define N_F7S  ((3<<8) | 126)        // F7 sharp
#define N_G7F  ((3<<8) | 126)        // G7 flat
#define N_G7   ((3<<8) | 119)        // G7
#define N_G7S  ((3<<8) | 112)        // G7 sharp
#define N_A7F  ((3<<8) | 112)        // A7 flat
#define N_A7   ((3<<8) | 106)        // A7
#define N_A7S  ((3<<8) | 100)        // A7 sharp
#define N_B7F  ((3<<8) | 100)        // B7 flat
#define N_B7   ((3<<8) | 94)         // B7
