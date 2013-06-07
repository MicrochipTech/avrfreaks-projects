/*
 * CX80_Keycodes.h
 *
 * Created: 03/04/2013 11:19:34
 *  Author: Calogiuri Enzo Antonio
 */ 


#ifndef CX80_KEYCODES_H_
#define CX80_KEYCODES_H_

#define SHIFT_FLAG		1
#define CTL_FLAG		2
#define ALT_FLAG		4
#define UP_FLAG			8
#define CFG_FLAG		16
#define CLOCK_FLAG		32

#define NOT_USED 0

// Control characters

#define NUL   0
#define CTL_A 1
#define CTL_B 2
#define CTL_C 3
#define CTL_D 4
#define CTL_E 5
#define CTL_F 6
#define CTL_G 7
#define CTL_H 8
#define CTL_I 9
#define CTL_J 10
#define CTL_K 11
#define CTL_L 12
#define CTL_M 13
#define CTL_N 14
#define CTL_O 15
#define CTL_P 16
#define CTL_Q 17
#define CTL_R 18
#define CTL_S 19
#define CTL_T 20
#define CTL_U 21
#define CTL_V 22
#define CTL_W 23
#define CTL_X 24
#define CTL_Y 25
#define CTL_Z 26

#define ALT_A  0xd0
#define ALT_B  0xd1
#define ALT_C  0xd2
#define ALT_D  0xd3
#define ALT_E  0xd4
#define ALT_F  0xd5
#define ALT_G  0xd6
#define ALT_H  0xd7
#define ALT_I  0xd8
#define ALT_J  0xd9
#define ALT_K  0xda
#define ALT_L  0xdb
#define ALT_M  0xdc
#define ALT_N  0xdd
#define ALT_O  0xde
#define ALT_P  0xdf
#define ALT_Q  0xe0
#define ALT_R  0xe1
#define ALT_S  0xe2
#define ALT_T  0xe3
#define ALT_U  0xe4
#define ALT_V  0xe5
#define ALT_W  0xe6
#define ALT_X  0xe7
#define ALT_Y  0xe8
#define ALT_Z  0xe9

//  Function keys

#define F1 0x81
#define F2 0x81
#define F3 0x82
#define F4 0x83
#define F5 0x84
#define F6 0x85
#define F7 0x86
#define F8 0x87
#define F9 0x88
#define F10 0x8a
#define F11 0x8b
#define F12 0x8c

#define SHIFT_F1 0x91
#define SHIFT_F2 0x92
#define SHIFT_F3 0x93
#define SHIFT_F4 0x94
#define SHIFT_F5 0x95
#define SHIFT_F6 0x96
#define SHIFT_F7 0x97
#define SHIFT_F8 0x98
#define SHIFT_F9 0x99
#define SHIFT_F10 0x9a
#define SHIFT_F11 0x9b
#define SHIFT_F12 0x9c

#define ALT_F1 0xa1
#define ALT_F2 0xa2
#define ALT_F3 0xa3
#define ALT_F4 0xa4
#define ALT_F5 0xa5
#define ALT_F6 0xa6
#define ALT_F7 0xa7
#define ALT_F8 0xa8
#define ALT_F9 0xa9
#define ALT_F10 0xaa
#define ALT_F11 0xab
#define ALT_F12 0xac

#define CTL_F1 0xb1
#define CTL_F2 0xb2
#define CTL_F3 0xb3
#define CTL_F4 0xb4
#define CTL_F5 0xb5
#define CTL_F6 0xb6
#define CTL_F7 0xb7
#define CTL_F8 0xb8
#define CTL_F9 0xb9
#define CTL_F10 0xba
#define CTL_F11 0xbb
#define CTL_F12 0xbc

#define ESC 0x1b
#define TAB 0x09
#define SPACE 0x20
#define INS 0xbd
#define DEL 0xbe
#define BS 0x08

#define SHIFT_ESC 0x1b
#define SHIFT_TAB 0x09
#define SHIFT_INS 0xad
#define SHIFT_DEL 0xae
#define SHIFT_BS 0xae

#define CTL_ESC 0xa0
#define CTL_TAB 0x90
#define CTL_INS 0x8d
#define CTL_DEL 0x8e
#define CTL_BS 0x8f

#define MENU 0xaf
#define WIN 0xb0

#define UP 0xc0
#define DOWN 0xc1
#define LEFT 0xc2
#define RIGHT 0xc3
#define PGUP 0xc4
#define PGDN 0xc5
#define HOME 0xc6
#define END 0xc7

#define SHIFT_UP 0xc8
#define SHIFT_DOWN 0xc9
#define SHIFT_LEFT 0xca
#define SHIFT_RIGHT 0xcb
#define SHIFT_PGUP 0xcc
#define SHIFT_PGDN 0xcd
#define SHIFT_HOME 0xce
#define SHIFT_END 0xcf

#define CTL_UP 0xc8
#define CTL_DOWN 0xc9
#define CTL_LEFT 0xca
#define CTL_RIGHT 0xcb
#define CTL_PGUP 0xcc
#define CTL_PGDN 0xcd
#define CTL_HOME 0xce
#define CTL_END 0xcf

#define C_LOCK 0xff

#endif /* CX80_KEYCODES_H_ */