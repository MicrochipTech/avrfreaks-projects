//      pmusic.c
//
//      support for Butterfly piezo sounds

#include <iom169v.h>
#include "pmusic.h"

char playing;           // whether a song is currently playing
char cnt_tempo;         // tempo countdown during playing
char cnt_dur;           // note duration countdown during playing
char cnt_vol;           // volume
char cur_tempo;         // current tempo for active song
char cur_dur;           // current duration for current note in active song
char cur_rpt;           // repeat count for current song
const int *cur_song;    // pointer to current song (for repeats)
const int *cur_note;    // pointer to next note in current song

// -------------------------------------------------------
//      start_song(&song) - start playing a tune

void start_song(const int *s) {
  cnt_dur = DURATION;           // default note duration
  cnt_tempo = TEMPO;            // default tempo
  cnt_vol = VOLUME;             // default volume
  cur_rpt = *s++ & 0x07;        // 7 repeats is enough
  if (!cur_rpt) cur_rpt = 1;    // 1 is minimum
  cur_song = cur_note = s;      // pointer to first note or command

  if (get_next_note()) {        // process any leading commands
    cur_dur = cnt_dur;          // int handler: initial duration to use
    cur_tempo = cnt_tempo;      // int handler: initial tempo to use
    play_note();                // load timer 1 with note value
    playing = 1;                // start the interrupt handler timing
  }
}

// -------------------------------------------------------
//      play_note() - strike up the band
//
//      cur_note points at next note of song
//      preceed every call to this with a call to get_next_note()
//      timer1 - phase freq correct PWM, TOP = ICR1, prescale 8

void play_note(void) {

  TCCR1B = 0x00;                // stop timer 1
  TCCR1A = (1<<COM1A1)|(1<<COM1A0); // Set OC1A when upcounting, clear when downcounting
      
  OCR1AH = 0;                   // volume into OCR1A register (in theory)
  OCR1AL = cnt_vol;             // lower value = higher volume

  ICR1H = *cur_note >> 8;       // the note frequency, high byte
  ICR1L = *cur_note++ & 0xff;   // low byte

  TCNT1H = 0x00;                // the counter
  TCNT1L = 0x00;

  TCCR1B = (1<<WGM13)|(1<<CS11);   // start timer 1
}

// -------------------------------------------------------
//      stop_playing() - stop that infernal racket

void stop_playing(void) {

  TCCR1B = 0x00;                // stop timer 1
  playing = 0;                  // tell the interrupt handler
}

// -------------------------------------------------------
//      get_next_note() - process song parameters until next note
//
//      cur_note points at next note of song
//      this will apply any parameters and position to next note, if any
//      returns: 1 if note, 0 if end of song (all repeats finished)

char get_next_note(void) {
  int nxt;

  while (1) {
    nxt = *cur_note;            // the next note or command
    if (nxt) {                  // are we at the end of the song?
      switch (nxt & N_CMD) {
        case N_NOTE:            // it's a note - all done
          return 1;
        case N_TEMP:            // tempo
          cnt_tempo = (nxt & 0x0ff);
          if (!cnt_tempo) cnt_tempo++;  // minimum of 1
          break;
        case N_LEN:             // duration
          cnt_dur = (nxt & 0xff);
          if (!cnt_dur) cnt_dur++;
          break;
        case N_VOL:             // volume
          cnt_vol = nxt & 0xff;
          if (!cnt_vol) cnt_vol++;
          break;
        default:                // unknown - ignore
          break;
      }
      cur_note++;               // point to next command or note
    } else {                    // we are at the end of the song
      cur_rpt--;                // one less repeat to go
      if (cur_rpt) {
        cur_note = cur_song;    // play it again, sam
        cnt_dur = DURATION;     // default note duration
        cnt_tempo = TEMPO;      // default tempo
        cnt_vol = VOLUME;       // default volume
      } else {
        stop_playing();         // all done, maestro
        return 0;
      }
    }
  }
  return 0;                     // to kill compiler warnings
}

