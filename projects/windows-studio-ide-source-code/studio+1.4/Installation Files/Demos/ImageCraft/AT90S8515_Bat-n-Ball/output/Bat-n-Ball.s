	.module Bat-n-Ball.c
	.area data(ram, con, rel)
_memory::
	.blkb 33
	.area idata
	.byte 'B,'a,'t,45,'n,45,'B,'a,'l,'l,32,'D,'e,'m,'o,32
	.byte 32,32,32,32,48,48,32,58,32,48,48,32,32,32,32,32
	.byte 0
	.area data(ram, con, rel)
	.blkb 127
	.area idata
	.word 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.word 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte 0,0,0,0,0,0,0
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bat-n-Ball\Bat-n-Ball.c
	.dbsym s memory _memory A[160:160]c
	.area text(rom, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bat-n-Ball\Bat-n-Ball.c
	.dbfunc s UART0_Init _UART0_Init fI
	.even
_UART0_Init::
	.dbline -1
	.dbline 45
; // Bat-n-Ball.c
; // Chris Brown 6th Feb, 2002
; //
; // Flash Studio+ & In-System-Debugger (ISD) "Bat-n-Ball" demo.
; //
; // Use with any ATMEL AVR CPU that has internal SRAM and an RS232
; // port or with Flash 5in1 AVR Starter Kits for added features.
; //
; // Requires a version 1.6 (or later) ISD library or source file:
; //    libISD16ATx.a or ISD16_ATx.s
; 
; // ****** USER SETTINGS ******
; 
; // Un-comment the CPU you are using.
; // Remember to set the CPU & ISD library (if you are using one)
; // in your ImageCraft project settings too!
; #define AT90S8515
; //#define ATMEGA103
; 
; // Clock frequency - set this to your clock frequency
; #define FOSC 3686400					// 3.6864Mhz
; 
; // Serial comms Baud rate - set this to your desired baud rate
; #define BAUD 115200
; 
; // ***** END USER SETTINGS *****
; 
; #include <stdio.h>
; #include <ina90.h>
; #ifdef AT90S8515
; #include <io8515.h>
; #endif
; #ifdef ATMEGA103
; #include <iom103.h>
; #endif
; 
; #define SIZEX 15						// Size of the court
; #define SIZEY 6
; 
; char memory[(SIZEY+4)*(SIZEX+1)] = {"Bat-n-Ball Demo     00 : 00     "};
; unsigned char leftScore, rightScore;
; 
; 
; void UART0_Init(void)
; {
	.dbline 46
; 	UCR  = 0x00;						// Disable while setting baud rate
	clr R2
	out 0xa,R2
	.dbline 47
; 	UBRR = (FOSC/(16*BAUD))-1;			// Set baud rate
	ldi R24,1
	out 0x9,R24
	.dbline 48
; 	UCR  = 0x18;						// Enable Tx & Rx
	ldi R24,24
	out 0xa,R24
	.dbline -2
	.dbline 49
; }
L1:
	.dbline 0 ; func end
	ret
	.dbfunc s INT0_Init _INT0_Init fI
	.even
_INT0_Init::
	.dbline -1
	.dbline 53
; 
; 
; void INT0_Init(void)
; {
	.dbline 55
; #ifdef AT90S8515
; 	DDRD |= 0x08;						// Set up PD3 (AVRDIL LED) = Output
	sbi 0x11,3
	.dbline 56
; 	PORTD |= 0x04;						// Turn on Pull-Up PD2 (INT0) = 1
	sbi 0x12,2
	.dbline -2
	.dbline 61
; #endif
; #ifdef ATMEGA103
; 	PORTD |= 0x01;						// Turn on Pull-Up PD0 (INT0) = 1
; #endif
; }
L2:
	.dbline 0 ; func end
	ret
	.dbfunc s MakeLedRed _MakeLedRed fI
	.even
_MakeLedRed::
	.dbline -1
	.dbline 65
; 
; 
; void MakeLedRed(void)
; {
	.dbline 66
; 	DDRD &= 0xf7;						// Turn on red LED
	in R24,0x11
	andi R24,247
	out 0x11,R24
	.dbline -2
	.dbline 67
; }
L3:
	.dbline 0 ; func end
	ret
	.dbfunc s MakeLedGreen _MakeLedGreen fI
	.even
_MakeLedGreen::
	.dbline -1
	.dbline 71
; 
; 
; void MakeLedGreen(void)
; {
	.dbline 72
; 	DDRD |= 0x08;						// Turn off red LED
	sbi 0x11,3
	.dbline -2
	.dbline 73
; }
L4:
	.dbline 0 ; func end
	ret
	.dbfunc s DrawScores _DrawScores fI
	.even
_DrawScores::
	.dbline -1
	.dbline 77
; 
; 
; void DrawScores(void)
; {
	.dbline 78
; 	memory[16+4] = (leftScore/10)+'0';
	ldi R17,10
	lds R16,_leftScore
	rcall div8u
	mov R24,R16
	subi R24,208	; addi 48
	sts _memory+20,R24
	.dbline 79
; 	memory[16+5] = (leftScore-((leftScore/10)*10))+'0';
	ldi R17,10
	lds R16,_leftScore
	rcall div8u
	mov R17,R16
	ldi R16,10
	rcall mpy8s
	lds R24,_leftScore
	sub R24,R16
	subi R24,208	; addi 48
	sts _memory+21,R24
	.dbline 80
; 	memory[16+9] = (rightScore/10)+'0';
	ldi R17,10
	lds R16,_rightScore
	rcall div8u
	mov R24,R16
	subi R24,208	; addi 48
	sts _memory+25,R24
	.dbline 81
; 	memory[16+10] = (rightScore-((rightScore/10)*10))+'0';
	ldi R17,10
	lds R16,_rightScore
	rcall div8u
	mov R17,R16
	ldi R16,10
	rcall mpy8s
	lds R24,_rightScore
	sub R24,R16
	subi R24,208	; addi 48
	sts _memory+26,R24
	.dbline -2
	.dbline 82
; }
L5:
	.dbline 0 ; func end
	ret
	.dbfunc s main _main fI
;           oldY -> R20
;           oldX -> R22
;             dy -> R14
;       rightPos -> R12
;        leftPos -> y+0
;             dx -> R10
;              y -> R6
;              x -> R4
	.even
_main::
	sbiw R28,1
	.dbline -1
	.dbline 86
; 
; 
; void main(void)
; {
	.dbline 93
; 	unsigned char x, y, oldX, oldY;
; 	char dx, dy;
; 	unsigned char leftPos, rightPos;
; 
; //	MCUCR |= 0x80;						// Enable external sram (optional)
; 
; 	INT0_Init();
	rcall _INT0_Init
	.dbline 94
; 	UART0_Init();
	rcall _UART0_Init
	.dbline 96
; #ifdef AT90S8515
; 	GIMSK |= 0x40;						// Enable INT0 (AT90S8515)
	in R24,0x3b
	ori R24,64
	out 0x3b,R24
	.dbline 101
; #endif
; #ifdef ATMEGA103
; 	EIMSK |= 0x01;						// Enable INT0 (ATmega103)
; #endif
; 	SEI();								// Enable interrupts
	sei
	.dbline 103
; 
; 	for (y=0; y<SIZEY+2; y++)			// Initialise the court
	clr R6
	rjmp L14
L11:
	.dbline 104
; 	{
	.dbline 105
; 		for (x=0; x<SIZEX+1; x++)
	clr R4
	rjmp L18
L15:
	.dbline 106
; 		{
	.dbline 107
; 			if (x == 7)
	mov R24,R4
	cpi R24,7
	brne L19
	.dbline 108
; 				memory[((y+2)<<4)+7] = '|';
	ldi R24,<_memory+7
	ldi R25,>_memory+7
	mov R30,R6
	clr R31
	adiw R30,2
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R24
	adc R31,R25
	ldi R24,124
	std z+0,R24
	rjmp L20
L19:
	.dbline 110
; 			else
; 				memory[((y+2)<<4)+x] = ' ';
	mov R2,R4
	clr R3
	mov R30,R6
	clr R31
	adiw R30,2
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R2
	adc R31,R3
	ldi R24,<_memory
	ldi R25,>_memory
	add R30,R24
	adc R31,R25
	ldi R24,32
	std z+0,R24
L20:
	.dbline 111
L16:
	.dbline 105
	inc R4
L18:
	.dbline 105
	mov R24,R4
	cpi R24,16
	brlo L15
	.dbline 112
L12:
	.dbline 103
	inc R6
L14:
	.dbline 103
	mov R24,R6
	cpi R24,8
	brlo L11
	.dbline 113
; 		}
; 	}
; 	for (x=0; x<15; x++)
	clr R4
	rjmp L25
L22:
	.dbline 114
	.dbline 115
	ldi R24,<_memory+32
	ldi R25,>_memory+32
	mov R30,R4
	clr R31
	add R30,R24
	adc R31,R25
	ldi R24,45
	std z+0,R24
	.dbline 116
	ldi R24,<_memory+144
	ldi R25,>_memory+144
	mov R30,R4
	clr R31
	add R30,R24
	adc R31,R25
	ldi R24,45
	std z+0,R24
	.dbline 117
L23:
	.dbline 113
	inc R4
L25:
	.dbline 113
	mov R24,R4
	cpi R24,15
	brlo L22
	.dbline 119
; 	{
; 		memory[32+x] = '-';
; 		memory[48+(SIZEY*16)+x] = '-';
; 	}
; 
; 	leftScore = 10;						// Start new game
	ldi R24,10
	sts _leftScore,R24
	.dbline 120
; 	rightScore = 10;
	sts _rightScore,R24
	.dbline 121
; 	leftPos = 3;
	ldi R24,3
	std y+0,R24
	.dbline 122
; 	rightPos = 3;
	mov R12,R24
	.dbline 124
; 
; 	for (;;)							// Loop forever!!
L28:
	.dbline 125
; 	{
	.dbline 127
; 
; 		SetBreakHere();					// Hard Breakpoint here
	push R4
	push R6
	rcall _SetBreakHere
	pop R6
	pop R4
	.dbline 129
; 
; 		if (leftScore == 10 || rightScore == 10) // If game is over
	lds R24,_leftScore
	cpi R24,10
	breq L34
	lds R24,_rightScore
	cpi R24,10
	breq X1
	rjmp L32
X1:
L34:
	.dbline 130
; 		{
	.dbline 131
; 			leftScore = 0;				// Reset scores
	clr R2
	sts _leftScore,R2
	.dbline 132
; 			rightScore = 0;
	sts _rightScore,R2
	.dbline 133
; 			memory[(3+leftPos)<<4] = ' '; // Erase the old bats
	ldi R24,<_memory
	ldi R25,>_memory
	ldd R30,y+0
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R24
	adc R31,R25
	ldi R24,32
	std z+0,R24
	.dbline 134
; 			memory[((3+rightPos)<<4)+(SIZEX-1)] = ' ';
	ldi R24,<_memory+14
	ldi R25,>_memory+14
	mov R30,R12
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R24
	adc R31,R25
	ldi R24,32
	std z+0,R24
	.dbline 135
; 			leftPos = 3;
	ldi R24,3
	std y+0,R24
	.dbline 136
; 			rightPos = 3;
	mov R12,R24
	.dbline 137
; 			memory[(3+3)<<4] = '|';		// Draw the bats
	ldi R24,124
	sts _memory+96,R24
	.dbline 138
; 			memory[((3+3)<<4)+(SIZEX-1)] = '|';
	sts _memory+110,R24
	.dbline 139
; 			x = 7;						// Ball's starting position
	ldi R24,7
	mov R4,R24
	.dbline 140
; 			y = 3;
	ldi R24,3
	mov R6,R24
	.dbline 141
; 			oldX = x;
	mov R22,R4
	.dbline 142
; 			oldY = y;
	mov R20,R6
	.dbline 143
; 			MakeLedGreen();
	push R4
	push R6
	rcall _MakeLedGreen
	pop R6
	pop R4
	.dbline 144
; 			dx = 1;						// Ball is going right & down
	clr R10
	inc R10
	.dbline 145
; 			dy = 1;
	clr R14
	inc R14
	.dbline 146
; 		}
L32:
	.dbline 148
; 
; 		DrawScores();					// Draw the scores
	push R4
	push R6
	rcall _DrawScores
	pop R6
	pop R4
	.dbline 150
; 
; 		if (oldX == 7)
	cpi R22,7
	brne L38
	.dbline 151
; 			memory[((oldY+3)<<4)+7] = '|';
	ldi R24,<_memory+7
	ldi R25,>_memory+7
	mov R30,R20
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R24
	adc R31,R25
	ldi R24,124
	std z+0,R24
	rjmp L39
L38:
	.dbline 153
; 		else
; 			memory[((oldY+3)<<4)+oldX] = ' ';
	mov R2,R22
	clr R3
	mov R30,R20
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R2
	adc R31,R3
	ldi R24,<_memory
	ldi R25,>_memory
	add R30,R24
	adc R31,R25
	ldi R24,32
	std z+0,R24
L39:
	.dbline 154
; 		x += dx;						// Update ball's position
	add R4,R10
	.dbline 155
; 		y += dy;
	add R6,R14
	.dbline 156
; 		memory[((3+y)<<4)+x] = 'o';
	mov R2,R4
	clr R3
	mov R30,R6
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R2
	adc R31,R3
	ldi R24,<_memory
	ldi R25,>_memory
	add R30,R24
	adc R31,R25
	ldi R24,111
	std z+0,R24
	.dbline 158
; 
; 		if (dx == -1)					// If ball is moving left
	mov R24,R10
	cpi R24,255
	brne L41
	.dbline 159
; 		{
	.dbline 160
; 			memory[(3+leftPos)<<4] = ' '; // Erase left's bat
	ldi R24,<_memory
	ldi R25,>_memory
	ldd R30,y+0
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R24
	adc R31,R25
	ldi R24,32
	std z+0,R24
	.dbline 161
; 			if (y > leftPos)			// Move bat towards the ball
	ldd R0,y+0
	cp R0,R6
	brsh L43
	.dbline 162
; 				leftPos++;
	inc R0
	std y+0,R0
L43:
	.dbline 163
; 			if (y < leftPos)
	ldd R0,y+0
	cp R6,R0
	brsh L45
	.dbline 164
; 				leftPos--;
	dec R0
	std y+0,R0
L45:
	.dbline 165
; 			memory[(3+leftPos)<<4] = '|'; // Draw left's bat
	ldi R24,<_memory
	ldi R25,>_memory
	ldd R30,y+0
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R24
	adc R31,R25
	ldi R24,124
	std z+0,R24
	.dbline 166
; 		}
L41:
	.dbline 168
; 
; 		if (dx == 1)					// If ball is moving right
	mov R24,R10
	cpi R24,1
	brne L47
	.dbline 169
; 		{
	.dbline 170
; 			memory[((3+rightPos)<<4)+(SIZEX-1)] = ' '; // Erase right's bat
	ldi R24,<_memory+14
	ldi R25,>_memory+14
	mov R30,R12
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R24
	adc R31,R25
	ldi R24,32
	std z+0,R24
	.dbline 171
; 			if (y > rightPos)			// Move bat towards the ball
	cp R12,R6
	brsh L50
	.dbline 172
; 				rightPos++;
	inc R12
L50:
	.dbline 173
; 			if (y < rightPos)
	cp R6,R12
	brsh L52
	.dbline 174
; 				rightPos--;
	dec R12
L52:
	.dbline 175
; 			memory[((3+rightPos)<<4)+(SIZEX-1)] = '|'; // Draw right's bat
	ldi R24,<_memory+14
	ldi R25,>_memory+14
	mov R30,R12
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R24
	adc R31,R25
	ldi R24,124
	std z+0,R24
	.dbline 176
; 		}
L47:
	.dbline 178
; 
; 		if (x == 1 && leftPos == y)		// If left is hitting the ball
	mov R24,R4
	cpi R24,1
	brne L55
	ldd R0,y+0
	cp R0,R6
	brne L55
	.dbline 179
; 		{
	.dbline 180
; 			dx = 1;						// Hit it back
	clr R10
	inc R10
	.dbline 181
; 			MakeLedGreen();
	push R4
	push R6
	rcall _MakeLedGreen
	pop R6
	pop R4
	.dbline 182
; 		}
L55:
	.dbline 184
; 
; 		if (x == SIZEX-2 && rightPos == y) // If right is hitting the ball
	mov R24,R4
	cpi R24,13
	brne L57
	cp R12,R6
	brne L57
	.dbline 185
; 		{
	.dbline 186
; 			dx = -1;					// Hit it back
	ldi R24,255
	mov R10,R24
	.dbline 187
; 			MakeLedRed();
	push R4
	push R6
	rcall _MakeLedRed
	pop R6
	pop R4
	.dbline 188
; 		}
L57:
	.dbline 190
; 
; 		if (x == 0)						// If left missed
	tst R4
	brne L59
	.dbline 191
; 		{
	.dbline 192
; 			rightScore++;				// Add 1 to right's score
	lds R24,_rightScore
	subi R24,255	; addi 1
	sts _rightScore,R24
	.dbline 193
; 			memory[((3+y)<<4)+x] = ' ';
	mov R2,R4
	clr R3
	mov R30,R6
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R2
	adc R31,R3
	ldi R24,<_memory
	ldi R25,>_memory
	add R30,R24
	adc R31,R25
	ldi R24,32
	std z+0,R24
	.dbline 194
; 			x = 1;						// Left serves
	clr R4
	inc R4
	.dbline 195
; 			y = leftPos;
	ldd R6,y+0
	.dbline 196
; 			dx = 1;
	clr R10
	inc R10
	.dbline 197
; 			dy = 1;
	clr R14
	inc R14
	.dbline 198
; 		}
L59:
	.dbline 200
; 
; 		if (x == SIZEX-1)				// If right missed
	mov R24,R4
	cpi R24,14
	brne L61
	.dbline 201
; 		{
	.dbline 202
; 			leftScore++;				// Add 1 to left's score
	lds R24,_leftScore
	subi R24,255	; addi 1
	sts _leftScore,R24
	.dbline 203
; 			memory[((3+y)<<4)+x] = ' ';
	mov R2,R4
	clr R3
	mov R30,R6
	clr R31
	adiw R30,3
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	lsl R30
	rol R31
	add R30,R2
	adc R31,R3
	ldi R24,<_memory
	ldi R25,>_memory
	add R30,R24
	adc R31,R25
	ldi R24,32
	std z+0,R24
	.dbline 204
; 			x = SIZEX-2;				// Right serves
	ldi R24,13
	mov R4,R24
	.dbline 205
; 			y = rightPos;
	mov R6,R12
	.dbline 206
; 			dx = -1;
	ldi R24,255
	mov R10,R24
	.dbline 207
; 			dy = 1;
	clr R14
	inc R14
	.dbline 208
; 		}
L61:
	.dbline 210
; 
; 		if (y == 0 || y == (SIZEY-1))	// If ball is at the top or bottom edge
	tst R6
	breq L65
	mov R24,R6
	cpi R24,5
	brne L63
L65:
	.dbline 211
; 			dy =- dy;					// Bounce
	neg R14
L63:
	.dbline 213
	mov R22,R4
	.dbline 214
	mov R20,R6
	.dbline 215
	.dbline 124
	.dbline 124
	rjmp L28
X0:
	.dbline -2
L10:
	adiw R28,1
	.dbline 0 ; func end
	ret
	.dbsym r oldY 20 c
	.dbsym r oldX 22 c
	.dbsym r dy 14 c
	.dbsym r rightPos 12 c
	.dbsym l leftPos 0 c
	.dbsym r dx 10 c
	.dbsym r y 6 c
	.dbsym r x 4 c
	.area bss(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bat-n-Ball\Bat-n-Ball.c
_rightScore::
	.blkb 1
	.dbsym s rightScore _rightScore c
_leftScore::
	.blkb 1
	.dbsym s leftScore _leftScore c
