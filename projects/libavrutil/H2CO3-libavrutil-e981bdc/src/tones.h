/*
 * tones.h
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTIL_TONES_H__
#define __AVRUTIL_TONES_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Frequencies of the tones of the equally tempered music scale.
 * The frequencies are rounded to integers. A1 is set to the
 * standard 440 Hertz frequency.
 *
 * Naming convention:
 * The tone names are constructed using two or three parts.
 * Each part is separated from the previous by an underscore (_)
 * The first part is the tone name itself: either C D E F G A or B.
 * The second part is optional, it indicates a flat (F) or sharp (S) intonation.
 * The third part indicates the octave: subcontra (SUBC), contra (CONTRA),
 * great (GREAT), small (SMALL), one-lined (1), two-lined (2),
 * three-lined (3), four-lined (4), five-lined (5) or six-lined (6).
 */

#define PAUSE	    0
#define END	 (-1)

#define A_SUBC	   28
#define A_S_SUBC   29

#define B_F_SUBC   29
#define B_SUBC	   31
#define B_S_SUBC   33

#define C_F_CONTRA 31
#define C_CONTRA   33
#define C_S_CONTRA 35

#define D_F_CONTRA 35
#define D_CONTRA   37
#define D_S_CONTRA 39

#define E_F_CONTRA 39
#define E_CONTRA   41
#define E_S_CONTRA 44

#define F_F_CONTRA 41
#define F_CONTRA   44
#define F_S_CONTRA 46

#define G_F_CONTRA 46
#define G_CONTRA   49
#define G_S_CONTRA 52

#define A_F_CONTRA 52
#define A_CONTRA   55
#define A_S_CONTRA 58

#define B_F_CONTRA 58
#define B_CONTRA   62
#define B_S_CONTRA 65

#define C_F_GREAT  62
#define C_GREAT	   65
#define C_S_GREAT  69

#define D_F_GREAT  69
#define D_GREAT	   73
#define D_S_GREAT  78

#define E_F_GREAT  78
#define E_GREAT	   82
#define E_S_GREAT  87

#define F_F_GREAT  82
#define F_GREAT	   87
#define F_S_GREAT  92

#define G_F_GREAT  92
#define G_GREAT	   98
#define G_S_GREAT 104

#define A_F_GREAT 104
#define A_GREAT	  110
#define A_S_GREAT 117

#define B_F_GREAT 117
#define B_GREAT	  123
#define B_S_GREAT 131

#define C_F_SMALL 123
#define C_SMALL	  131
#define C_S_SMALL 139

#define D_F_SMALL 139
#define D_SMALL	  147
#define D_S_SMALL 156

#define E_F_SMALL 156
#define E_SMALL	  165
#define E_S_SMALL 175

#define F_F_SMALL 165
#define F_SMALL	  175
#define F_S_SMALL 185

#define G_F_SMALL 185
#define G_SMALL	  196
#define G_S_SMALL 208

#define A_F_SMALL 208
#define A_SMALL	  220
#define A_S_SMALL 233

#define B_F_SMALL 233
#define B_SMALL	  247
#define B_S_SMALL 262

#define C_F_1	  247
#define C_1	  262
#define C_S_1	  277

#define D_F_1	  277
#define D_1	  294
#define D_S_1	  311

#define E_F_1	  311
#define E_1	  330
#define E_S_1	  349

#define F_F_1	  330
#define F_1	  349
#define F_S_1	  370

#define G_F_1	  370
#define G_1	  392
#define G_S_1	  415

#define A_F_1	  415
#define A_1	  440
#define A_S_1	  466

#define B_F_1	  466
#define B_1	  494
#define B_S_1	  523

#define C_F_2	  494
#define C_2	  523
#define C_S_2	  554

#define D_F_2	  554
#define D_2	  587
#define D_S_2	  622

#define E_F_2	  622
#define E_2	  659
#define E_S_2	  698

#define F_F_2	  659
#define F_2	  698
#define F_S_2	  740

#define G_F_2	  740
#define G_2	  784
#define G_S_2	  831

#define A_F_2	  831
#define A_2	  880
#define A_S_2	  932

#define B_F_2	  932
#define B_2	  988
#define B_S_2	 1047

#define C_F_3	  988
#define C_3	 1047
#define C_S_3	 1109

#define D_F_3	 1109
#define D_3	 1175
#define D_S_3	 1245

#define E_F_3	 1245
#define E_3	 1319
#define E_S_3	 1397

#define F_F_3	 1319
#define F_3	 1397
#define F_S_3	 1480

#define G_F_3	 1480
#define G_3	 1568
#define G_S_3	 1661

#define A_F_3	 1661
#define A_3	 1760
#define A_S_3	 1865

#define B_F_3	 1865
#define B_3	 1976
#define B_S_3	 2093

#define C_F_4	 1976
#define C_4	 2093
#define C_S_4	 2217

#define D_F_4	 2217
#define D_4	 2349
#define D_S_4	 2489

#define E_F_4	 2489
#define E_4	 2637
#define E_S_4	 2794

#define F_F_4	 2637
#define F_4	 2794
#define F_S_4	 2960

#define G_F_4	 2960
#define G_4	 3136
#define G_S_4	 3322

#define A_F_4	 3322
#define A_4	 3520
#define A_S_4	 3729

#define B_F_4	 3729
#define B_4	 3951
#define B_S_4	 4186

#define C_F_5	 3951
#define C_5	 4186
#define C_S_5	 4435

#define D_F_5	 4435
#define D_5	 4699
#define D_S_5	 4978

#define E_F_5	 4978
#define E_5	 5274
#define E_S_5	 5588

#define F_F_5	 5274
#define F_5	 5588
#define F_S_5	 5920

#define G_F_5	 5920
#define G_5	 6272
#define G_S_5	 6645

#define A_F_5	 6645
#define A_5	 7040
#define A_S_5	 7459

#define B_F_5	 7459
#define B_5	 7902
#define B_S_5	 8372

#define C_F_6	 7902
#define C6	 8372

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__AVRUTIL_TONES_H__ */

