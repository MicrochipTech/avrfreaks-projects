// ad_spkr.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated

#include "ad_spkr.h"

__flash int allnotes[] = {                // every note
    2, N_DUR|6,
    N_C2,N_C2S,N_D2,N_D2S,N_E2,N_F2,N_F2S,N_G2,N_G2S,N_A2,N_A2S,N_B2,
    N_C3,N_C3S,N_D3,N_D3S,N_E3,N_F3,N_F3S,N_G3,N_G3S,N_A3,N_A3S,N_B3,
    N_C4,N_C4S,N_D4,N_D4S,N_E4,N_F4,N_F4S,N_G4,N_G4S,N_A4,N_A4S,N_B4,
    N_C5,N_C5S,N_D5,N_D5S,N_E5,N_F5,N_F5S,N_G5,N_G5S,N_A5,N_A5S,N_B5,
    N_C6,N_C6S,N_D6,N_D6S,N_E6,N_F6,N_F6S,N_G6,N_G6S,N_A6,N_A6S,N_B6,
    N_C7,N_C7S,N_D7,N_D7S,N_E7,N_F7,N_F7S,N_G7,N_G7S,N_A7,N_A7S,N_B7, 0
};

// modified from Atmel Butterfly application - Beethoven's Fur Elise
__flash int elise[] =
  {1,N_DUR|24,N_E5,N_D5S,N_E5,N_D5S,N_E5,N_B4,N_D5,N_C5,N_A4|N_2,
    N_REST,N_C4,N_E4,N_A4,N_B4|N_2,N_REST,N_E4,N_G4S,N_B4,N_C5|N_2,
    N_REST,N_E4,N_E5,N_D5S,N_E5,N_D5S,N_E5,N_B4,N_D5,N_C5,N_A4|N_2,
    N_REST,N_C4,N_E4,N_A4,N_B4|N_2,N_REST,N_E4,N_C5,N_B4,N_A4|N_2,
    N_REST|N_1, N_REST|N_1, 0
  };

volatile char playing;  // status: 0 = nada, 1 = ready to go, 2 = playing, 3 = fini

char cnt_tempo;         // tempo countdown during playing
char cnt_dur;           // note duration countdown during playing
char cur_tempo;         // current tempo for active song
char cur_dur;           // current duration for current note in active song
char cur_rpt;           // repeat count for current song
__flash int *cur_song;  // pointer to current song (for repeats)
__flash int *cur_note;  // pointer to next note in current song

// -------------------------------------------------------------
// init_spkr() - initialize the speaker

void init_spkr(void) {
  DDRD |= 0x80;                 // speaker is output
  playing = 0;                  // nothing going on
}

// -------------------------------------------------------
// start_song(&song) - initiate playing a tune
//
// song - pointer (in flash) to the song definition
//
// song will be started by the interrupt handler
// this will terminate any song in progress

void start_song(__flash int *s) {
  stop_playing();               // shut down anything going

  cur_dur = DURATION;           // default duration
  cur_tempo = TEMPO;            // default tempo

  cur_rpt = *s++ & 0x7f;        // 128 repeats is enough
  if (!cur_rpt) cur_rpt = 1;    // 1 is minimum
  cur_song = cur_note = s;      // pointer to first note or command

  playing = 1;                  // tell the interrupt handler to go
}

// -------------------------------------------------------
//      stop_playing() - stop that infernal racket

void stop_playing(void) {

  TCCR2 = 0x00;                 // stop timer 2
  playing = 0;                  // tell the interrupt handler
}


