/** @file sound.h Definitions of tones and Sound Module exported functions.

Each tone is set up with a value which will give the right frequency when
applied to a 16-bits timer with PWM. These values are based on a CLKcpu
running @ 1Mhz.

The frequency for all tones is given from the following formula:
Frequency = Bf * 2^(ToneX/12)

where:
ToneX: the actual tone, e.g. C0 = 3
Bf: Base frequency = 220Hz (A)

E.g: For tone C0 this would be: C0 = 220 * 2^(3/12)= 261,6256

The value of the 16-bits timer with PWM that will generate this freequency
is given from the following formula:

Timer value = 1Mhz / ToneHz / 2

E.g: For tone C0 this would be:

Timer value = 1000000 / 261,6256... / 2 = 1911

The 16-bits Timer1 should be configured in Phase/Freq-correct PWM mode,
 top value = ICR1, set OC1A when upcounting, clear when downcounting.

*/


#ifndef __SOUND_HEADER__
#define __SOUND_HEADER__

#define A   2273
#define xA  2145
#define B   2025
#define C0  1911
#define xC0 1804
#define D0  1703
#define xD0 1607
#define E0  1517
#define F0  1432
#define xF0 1351
#define G0  1275
#define xG0 1204
#define A0  1136
#define xA0 1073
#define B0  1012
#define C1  956
#define xC1 902
#define D1  851
#define xD1 804
#define E1  758
#define F1  716
#define xF1 676
#define G1  638
#define xG1 602
#define A1  568
#define xA1 536
#define B1  506
#define C2  478
#define xC2 451
#define D2  426
#define xD2 402
#define E2  379
#define F2  356
#define xF2 338
#define G2  319
#define xG2 301
#define A2  284
#define xA2 268
#define B2  253
#define C3  239
#define xC3 225
#define D3  213
#define xD3 201
#define E3  190
#define F3  179
#define xF3 169
#define G3  159
#define xG3 150
#define A3  142
#define xA3 134
#define B3  127
#define C4  119

#define Ax  xA
#define Cx0 xC0
#define Dx0 xD0
#define Fx0 xF0
#define Gx0 xG0
#define Ax0 xA0
#define Cx1 xC1
#define Dx1 xD1
#define Fx1 xF1
#define Gx1 xG1
#define Ax1 xA1
#define Cx2 xC2
#define Dx2 xD2
#define Fx2 xF2
#define Gx2 xG2
#define Ax2 xA2
#define Cx3 xC3
#define Dx3 Dx3
#define Fx3 xF3
#define Gx3 xG3
#define Ax3 xA3

#define P   1
#define p   P

#define a   A
#define xa  xA
#define ax  Ax
#define b   B
#define c0  C0
#define xc0 xC0
#define cx0 Cx0
#define d0  D0
#define xd0 xD0
#define dx0 Dx0
#define e0  E0
#define f0  F0
#define xf0 xF0
#define fx0 Fx0
#define g0  G0
#define xg0 xG0
#define gx0 Gx0
#define a0  A0
#define xa0 xA0
#define ax0 Ax0
#define b0  B0
#define c1  C1
#define xc1 xC1
#define cx1 Cx1
#define d1  D1
#define xd1 xD1
#define dx1 Dx1
#define e1  E1
#define f1  F1
#define xf1 xF1
#define fx1 Fx1
#define g1  G1
#define xg1 xG1
#define gx1 Gx1
#define a1  A1
#define xa1 xA1
#define ax1 Ax1
#define b1  B1
#define c2  C2
#define xc2 xC2
#define cx2 Cx2
#define d2  D2
#define xd2 xD2
#define dx2 Dx2
#define e2  E2
#define f2  F2
#define xf2 xF2
#define fx2 Fx2
#define g2  G2
#define xg2 xG2
#define gx2 Gx2
#define a2  A2
#define xa2 xA2
#define ax2 Ax2
#define b2  B2
#define c3  C3
#define xc3 xC3
#define cx3 Cx3
#define d3  D3
#define xd3 xD3
#define dx3 Dx3
#define e3  E3
#define f3  F3
#define xf3 xF3
#define fx3 Fx3
#define g3  G3
#define xg3 xG3
#define gx3 Gx3
#define a3  A3
#define xa3 xA3
#define ax3 Ax3
#define b3  B3
#define c4  C4


/*******************************************************************************
* Exported variables
*******************************************************************************/

#include "songs.h"

extern SongInfo currentSong;

/*******************************************************************************
* Exported functions
*******************************************************************************/

extern void SoundInit(void);
extern void SoundVolumeUp(void);
extern void SoundVolumeDown(void);
extern void SoundStartTone(uint16_t tone);
extern void SoundStop(void);
extern void SoundSongStart(void);
extern void SoundSongStop(void);
extern BOOL SoundIsSongPlaying(void);
extern BOOL SoundIsOn(void);

#endif //__SOUND_HEADER__
