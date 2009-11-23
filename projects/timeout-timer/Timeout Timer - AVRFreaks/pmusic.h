//      pmusic.h
//
//      header file for Butterfly piezo "music"

extern void start_song(const int *);
extern char get_next_note(void);
extern void play_note(void);
extern void stop_playing(void);


extern char playing;           // whether a song is cuurently playing
extern char cnt_tempo;         // tempo countdown during playing
extern char cnt_dur;           // note duration countdown during playing
extern char cnt_vol;           // volume
extern char cur_tempo;         // current tempo for active song
extern char cur_dur;           // current duration for current note in active song
extern char cur_rpt;           // repeat count for current song
extern const int *cur_song;    // pointer to current song (for repeats)
extern const int *cur_note;    // pointer to next note in current song


// note definitions
#define N_CMD  0x7000           // mask for recognizing commands
#define N_NOTE 0x0000           // 0x0--- is a note
#define N_TEMP 0x1000           // 0x1--- is tempo
#define N_LEN  0x2000           // 0x2--- is duration
#define N_VOL  0x3000           // 0x3--- is volume
#define N_REST 1                // a rest (using current duration)

#define TEMPO 2                 // default tempo
#define DURATION 12             // default duration
#define VOLUME 100              // default volume

// a song consists of a repeat count followed by notes and/or parameters
// (tempo, duration, volume) terminated by a 0.  all values are ints.  when
// the song starts the default tempo, duration, and volume are in effect,
// and each stays in effect until changed. these notes are relative, not
// absolute (that is, the numbers are not the frequencies generated).

#define N_A   2273              // this fits in 12 bits
#define N_As  2145               
#define N_Bf  2145
#define N_B   2024

#define N0_C  1911
#define N0_Cs 1804
#define N0_Df 1804
#define N0_D  1703
#define N0_Ds 1607
#define N0_Ef 1607
#define N0_E  1517
#define N0_F  1432
#define N0_Fs 1351
#define N0_Gf 1351
#define N0_G  1275
#define N0_Gs 1204
#define N0_Af 1204
#define N0_A  1136
#define N0_As 1073
#define N0_Bf 1073
#define N0_B  1012

#define N1_C  956
#define N1_Cs 902
#define N1_Df 902
#define N1_D  851
#define N1_Ds 804
#define N1_Ef 804
#define N1_E  758
#define N1_F  716
#define N1_Fs 676
#define N1_Gf 676
#define N1_G  638
#define N1_Gs 602
#define N1_Af 602
#define N1_A  568
#define N1_As 536
#define N1_Bf 536
#define N1_B  506

#define N2_C  478
#define N2_Cs 451
#define N2_Df 451
#define N2_D  426
#define N2_Ds 402
#define N2_Ef 402
#define N2_E  379
#define N2_F  356
#define N2_Fs 338
#define N2_Gf 338
#define N2_G  319
#define N2_Gs 301
#define N2_Af 301
#define N2_A  284
#define N2_As 268
#define N2_Bf 268
#define N2_B  253

#define N3_C  239
#define N3_Cs 225
#define N3_Df 225
#define N3_D  213
#define N3_Ds 201
#define N3_Ef 201
#define N3_E  190
#define N3_F  179
#define N3_Fs 169
#define N3_Gf 169
#define N3_G  159
#define N3_Gs 150
#define N3_Af 150
#define N3_A  142
#define N3_As 134
#define N3_Bf 134
#define N3_B  127

#define N4_C  119
