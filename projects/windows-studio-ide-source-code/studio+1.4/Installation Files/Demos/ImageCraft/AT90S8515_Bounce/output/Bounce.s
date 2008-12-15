	.module Bounce.c
	.area data(ram, con, rel)
_memory::
	.blkb 16
	.area idata
	.byte 32,32,32,'F,'l,'a,'s,'h,32,'D,'e,'m,'o,32,32,32
	.area data(ram, con, rel)
	.blkb 16
	.area idata
	.byte 32,32,32,'A,'V,'R,32,'B,'o,'u,'n,'c,'e,32,32,32
	.area data(ram, con, rel)
	.blkb 128
	.area idata
	.word 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.word 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte 0,0,0,0,0,0,0,0
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.dbsym s memory _memory A[160:10:16]c
_message::
	.blkb 16
	.area idata
	.byte 32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.blkb 16
	.area idata
	.byte 32,32,'T,'H,'E,32,'W,'O,'R,'L,'D,39,'S,32,32,32
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.blkb 16
	.area idata
	.byte 32,32,32,'S,'M,'A,'L,'L,'E,'S,'T,44,32,32,32,32
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.blkb 16
	.area idata
	.byte 32,32,32,'F,'A,'S,'T,'E,'S,'T,44,32,32,32,32,32
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.blkb 16
	.area idata
	.byte 32,32,32,32,'N,'O,45,'I,'C,'E,44,32,32,32,32,32
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.blkb 16
	.area idata
	.byte 32,32,32,'I,'N,45,'S,'Y,'S,'T,'E,'M,32,32,32,32
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.blkb 16
	.area idata
	.byte 32,32,32,'D,'E,'B,'U,'G,'G,'E,'R,33,32,32,32,32
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.blkb 16
	.area idata
	.byte 32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32
	.area data(ram, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.dbsym s message _message A[128:8:16]c
	.area text(rom, con, rel)
	.dbfile C:\Studio+\Demos\ImageCraft\AT90S8515_Bounce\Bounce.c
	.dbfunc s UART0_Init _UART0_Init fI
	.even
_UART0_Init::
	.dbline -1
	.dbline 46
; // Bounce.c
; //
; // Flash Studio+ & In-System-Debugger (ISD) "Bounce" demo.
; //
; // Use with any ATMEL AVR CPU/starter kit that has an RS232
; // port or with Flash AVR modules for added features.
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
; #define SIZEX 16						// Size of bounce area
; #define SIZEY 8
; 
; char memory[SIZEY+2][SIZEX] = {"   Flash Demo   ","   AVR Bounce   "};
; char message[SIZEY][SIZEX] = {"                ","  THE WORLD'S   ",
; 	"   SMALLEST,    ","   FASTEST,     ","    NO-ICE,     ",
; 	"   IN-SYSTEM    ","   DEBUGGER!    ","                "};
; 
; 
; void UART0_Init(void)
; {
	.dbline 47
; 	UCR  = 0x00;						// Disable while setting baud rate
	clr R2
	out 0xa,R2
	.dbline 48
; 	UBRR = (FOSC/(16*BAUD))-1;			// Set baud rate
	ldi R24,1
	out 0x9,R24
	.dbline 49
; 	UCR  = 0x18;						// Enable Tx & Rx
	ldi R24,24
	out 0xa,R24
	.dbline -2
	.dbline 50
; }
L1:
	.dbline 0 ; func end
	ret
	.dbfunc s INT0_Init _INT0_Init fI
	.even
_INT0_Init::
	.dbline -1
	.dbline 54
; 
; 
; void INT0_Init(void)
; {
	.dbline 56
; #ifdef AT90S8515
; 	DDRD |= 0x08;						// Set up PD3 (AVRDIL LED) = Output
	sbi 0x11,3
	.dbline 57
; 	PORTD |= 0x04;						// Turn on Pull-Up PD2 (INT0) = 1
	sbi 0x12,2
	.dbline -2
	.dbline 62
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
	.dbline 66
; 
; 
; void MakeLedRed(void)
; {
	.dbline 67
; 	DDRD &= 0xf7;						// Turn on red LED
	in R24,0x11
	andi R24,247
	out 0x11,R24
	.dbline -2
	.dbline 68
; }
L3:
	.dbline 0 ; func end
	ret
	.dbfunc s MakeLedGreen _MakeLedGreen fI
	.even
_MakeLedGreen::
	.dbline -1
	.dbline 72
; 
; 
; void MakeLedGreen(void)
; {
	.dbline 73
; 	DDRD |= 0x08;						// Turn off red LED
	sbi 0x11,3
	.dbline -2
	.dbline 74
; }
L4:
	.dbline 0 ; func end
	ret
	.dbfunc s main _main fI
;             dy -> R14
;             dx -> R10
;           oldY -> R20
;           oldX -> R22
;              y -> y+0
;              x -> R12
	.even
_main::
	sbiw R28,1
	.dbline -1
	.dbline 78
; 
; 
; void main(void)
; {
	.dbline 82
; 	unsigned char x, y, oldX, oldY;
; 	char dx, dy;
; 
; 	MCUCR |= 0x80;						// Enable external sram (optional)
	in R24,0x35
	ori R24,128
	out 0x35,R24
	.dbline 84
; 
; 	INT0_Init();
	rcall _INT0_Init
	.dbline 85
; 	UART0_Init();
	rcall _UART0_Init
	.dbline 87
; #ifdef AT90S8515
; 	GIMSK |= 0x40;						// Enable INT0 (AT90S8515)
	in R24,0x3b
	ori R24,64
	out 0x3b,R24
	.dbline 92
; #endif
; #ifdef ATMEGA103
; 	EIMSK |= 0x01;						// Enable INT0 (ATmega103)
; #endif
; 	SEI();								// Enable interrupts
	sei
	.dbline 94
; 
; 	for (y=0; y<SIZEY; y++)				// Initialise grid
	clr R0
	std y+0,R0
	rjmp L9
L6:
	.dbline 95
; 	{
	.dbline 96
; 		for (x=0; x<SIZEX; x++)
	clr R12
	rjmp L13
L10:
	.dbline 97
	.dbline 98
	ldi R24,<_message
	ldi R25,>_message
	ldd R2,y+0
	clr R3
	lsl R2
	rol R3
	lsl R2
	rol R3
	lsl R2
	rol R3
	lsl R2
	rol R3
	add R2,R24
	adc R3,R25
	mov R30,R12
	clr R31
	add R30,R2
	adc R31,R3
	ldd R2,z+0
	ldi R24,<_memory
	ldi R25,>_memory
	ldd R30,y+0
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
	mov R26,R12
	clr R27
	add R26,R30
	adc R27,R31
	st x,R2
	.dbline 99
L11:
	.dbline 96
	inc R12
L13:
	.dbline 96
	mov R24,R12
	cpi R24,16
	brlo L10
	.dbline 100
L7:
	.dbline 94
	ldd R0,y+0
	inc R0
	std y+0,R0
L9:
	.dbline 94
	ldd R24,y+0
	cpi R24,8
	brlo L6
	.dbline 102
; 		{
; 			memory[y+2][x] = message[y][x];
; 		}
; 	}
; 
; 	MakeLedRed();
	rcall _MakeLedRed
	.dbline 103
; 	x = 0;								// Starting position
	clr R12
	.dbline 104
; 	y = 3;
	ldi R24,3
	std y+0,R24
	.dbline 105
; 	memory[y+2][x] = 'O';
	ldi R24,<_memory
	ldi R25,>_memory
	ldd R30,y+0
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
	mov R26,R12
	clr R27
	add R26,R30
	adc R27,R31
	ldi R24,79
	st x,R24
	.dbline 106
; 	oldX = x;
	mov R22,R12
	.dbline 107
; 	oldY = y;
	ldd R20,y+0
	.dbline 108
; 	dx = 1;								// Right & down
	clr R10
	inc R10
	.dbline 109
; 	dy = 1;
	clr R14
	inc R14
	.dbline 111
; 
; 	for (;;)							// Loop forever!!
L14:
	.dbline 112
; 	{
	.dbline 113
; 		memory[2+oldY][oldX] = message[oldY][oldX];
	ldi R24,<_message
	ldi R25,>_message
	mov R2,R20
	clr R3
	lsl R2
	rol R3
	lsl R2
	rol R3
	lsl R2
	rol R3
	lsl R2
	rol R3
	add R2,R24
	adc R3,R25
	mov R30,R22
	clr R31
	add R30,R2
	adc R31,R3
	ldd R2,z+0
	ldi R24,<_memory
	ldi R25,>_memory
	mov R30,R20
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
	mov R26,R22
	clr R27
	add R26,R30
	adc R27,R31
	st x,R2
	.dbline 114
; 		x += dx;						// Update ball's position
	add R12,R10
	.dbline 115
; 		y += dy;
	ldd R2,y+0
	add R2,R14
	std y+0,R2
	.dbline 116
; 		memory[2+y][x] = 'O';
	ldd R30,y+0
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
	mov R26,R12
	clr R27
	add R26,R30
	adc R27,R31
	ldi R24,79
	st x,R24
	.dbline 118
; 
; 		if (x == 0 || x == (SIZEX-1))	// If ball is at the left or right edge
	tst R12
	breq L20
	mov R24,R12
	cpi R24,15
	brne L18
L20:
	.dbline 119
; 		{
	.dbline 120
; 			dx =- dx;					// Change direction
	neg R10
	.dbline 121
; 			MakeLedGreen();
	rcall _MakeLedGreen
	.dbline 122
; 		}
L18:
	.dbline 124
; 
; 		SetBreakHere();	   				// breakpoint set here
	rcall _SetBreakHere
	.dbline 126
; 
; 		if (y==0 || y== (SIZEY-1))		// If ball is at the top or bottom edge
	ldd R0,y+0
	tst R0
	breq L23
	ldd R24,y+0
	cpi R24,7
	brne L21
L23:
	.dbline 127
; 		{
	.dbline 128
; 			dy =- dy;					// Change direction
	neg R14
	.dbline 129
; 			MakeLedRed();
	rcall _MakeLedRed
	.dbline 130
; 		}
L21:
	.dbline 132
	mov R22,R12
	.dbline 133
	ldd R20,y+0
	.dbline 134
	.dbline 111
	.dbline 111
	rjmp L14
X0:
	.dbline -2
L5:
	adiw R28,1
	.dbline 0 ; func end
	ret
	.dbsym r dy 14 c
	.dbsym r dx 10 c
	.dbsym r oldY 20 c
	.dbsym r oldX 22 c
	.dbsym l y 0 c
	.dbsym r x 12 c
