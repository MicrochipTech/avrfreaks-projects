.dseg
horigin:	.byte	2 ; The horizontal graphics origin (in BBC BASIC graphics units), as set by ORIGIN or VDU 29. 
vorigin:	.byte	2 ; The vertical graphics origin (in BBC BASIC graphics units), as set by ORIGIN or VDU 29. 
new:		.byte	4	;new co-ordinate
current:	.byte	4	;current co-ordinate
old:		.byte	4	;old co-ordinate   	These must be contiguous and in this order
new_ext:	.byte 4	;new in external
current_ext:.byte 4;the current coordinate in external units
graphl:  	.byte 2  ; The left edge of the graphics window (in pixels), as set by VDU 28. 
graphb:  	.byte 2  ; The bottom edge of the graphics window (in pixels), as set by VDU 28. 
graphr:  	.byte 2  ; The right of the graphics window (in pixels), as set by VDU 28. 
grapht:  	.byte 2  ; The top of the graphics window (in pixels), as set by VDU 28. 
fgcol:   	.byte 1  ; The current foreground graphics colour
fgact:   	.byte 1  ; The current foreground actions, as set by GCOL. 
bgcol:   	.byte 1  ; The current background graphics colour
bgact:   	.byte 1  ; The current background actions, as set by GCOL.
ftcol:   	.byte 1  ; The current foreground text colour
btcol:   	.byte 1  ; background text colours, as set by COLOUR. 
screenw: 	.byte 1	; The width, in pixels, of BASIC's output 'screen', as set by MODE or VDU 23,22. 
screenh: 	.byte 1	; The height, in pixels, of BASIC's output 'screen', as set by MODE or VDU 23,22. 
textwl:  	.byte 1  ; Text window left in characters
textwb:  	.byte 1  ; Text window bottom
textwr:  	.byte 1  ; Text window right
textwt:  	.byte 1  ; Text window top
incurh:  	.byte 1  ; horizontal position of input (copy) cursor in chars
incurv:  	.byte 1  ; vertical position of input cursor
pattern: 	.byte 1	;background pattern for the screen
dx:		.byte	4	;For line drawing
dy:		.byte	4	;for line drawing
dz:		.byte 2
da:		.byte 2
error:	.byte	4	;for line drawing
yerror:	.byte 2
startx:	.byte 2	;for lines
starty:	.byte	2	;
endx:		.byte 2	;
endy:		.byte 2	;
zerror:	.byte 2	;MUST BE BEFORE tstart sequence!!!
tstartz:	.byte 2
tstartx:	.byte 2	;for triangles
tstarty:	.byte	2	;
tendx:	.byte 2	;
tendy:	.byte 2	;
tmidx:	.byte	2	;for triangles
tmidy:	.byte 2	;
cstartx:	.byte 2	;for circles
cstarty:	.byte	2	;
cendx:	.byte 2	;
cendy:	.byte 2	;
final_y:	.byte 4	;and for circles
fill_block:	.byte	8	;for fast lines

plot_command:	.byte 1	;denotes type of plotting to do.
ZMASK:	.byte 2	;plotting byte masks
ZSTART:	.byte	1	;number of pixels up in page
ZMEMC:	.byte 1	;column offset in page (screen column 0-63)

ignore_char:	.byte 1	;printer ignore character
cursor_pattern:	.byte 1	;cursor pattern
old_cursor_pattern: .byte 1	;
mode:		.byte 1	;current mode number
tv:		.byte	1	; holds *TV setting

char_table:	.byte	16	;temporary character storage
vduque:	.byte 16  	; 16 spaces for VDU sequences
.set	newx = vduque + 2
.set	newy = vduque + 4

line_count:	.byte	1	;For paged mode
time:		.byte 5	;5 byte elapsed time counter held here
user_flag:	.byte 1	;User flag for compatibility
user_chars:	.byte	256	;user defined characters
Padding_byte:	.byte 2	;to make buffer start at 0x0200 
buffer:	.byte 0x100	;Gives 256 byte buffer for i/o
top_of_data: .byte 0
keyboard_buffer:	.byte 0x100	;gives 512 byte buffer for keyboard input
keyboard_pointer:	.byte 1	;indicates next vacant space in buffer
keyboard_time:	.byte 1
keyboard_bit:	.byte 1
