;Navigation.asm
;
;version 2.1b
;(c) L Gijssel
;nov 2005
;lgijssel@neon-networking.nl
;
;What does it do?
;Navigation system for a small robot platform. For this purpose, the program creates a virtual
;two-dimensional grid (X,Y) using the ODO-ticks as the basic units. The program runs entirely 
;in AVR assembler and should on anything beyond a Mega16. 
;It depends partly on the division/multiplication routines as presented in Atmels appnote 200
;although I made minor modifications to them to suit the requirements of this program.
;
;How does it work?
;This code calculates the relative position based on (1) a direction or heading and (2) motion,
;expressed in the form of wheel displacement. It also calculates the direction and the 
;distance to a gridpoint that was specified as the target. The calculation method used is 
;quite similar to what sailors did in earlier days. Now don't come argueing that I should have 
;developed a system using satellite guidance. These systems still only work well when outdoors 
;and their accuracy is +/-3 to 5m. What we have here is designed for indoor use and with an 
;accuracy that, altough far from 100% should still be better than that.

;Requirements:
;	Odometry signal to register wheel displacement.
;	A robot should ideally be fitted with two wheel sensors.
;
;	Compass or other device to read the current compass heading.
;	I am using a Devantech CMPS03 module with I2C interface for this.
;
;You may need to hack the way you enter this data into the program. 
;I left my proprietary solution to give an idea of how this can be done. 
;
;Performance:
;The range that can be covered with this solution is defined by the fact that the grid 
;position is stored as a 16bit signed number. (32767 till -32768 for X and Y)
;If 1 grid increment resembles 1cm, this allows a range of over 327m in any direction. 
;After a reset, the robot assumes grid position 00,00 but could be preset to any position.
;
;The resolution of the system is equal to the smallest wheel displacement that is seen by 
;the ODO system. For my case this distance is 1cm. Wheel encoders with more divisions will
;increase accuracy but reduce the range. 
;
;The system does not make use of floating point numbers nor does it calculate sin/cos values. 
;The goniometric data that is used comes from a table in flash where each entry was multiplied 
;by a fixed number. Sin/cos were multiplied by 32768 (2^15) Tan was multiplied by 512 (2^9)
;After making the required calculations, the multiplication factor is rolled back sending 
;everything behind the comma to Oblivion. 
;
;The Goniometric data table has one entry per degree: 360 entries for sin/cos/tan. 
;Each entry is a 16bit word. The table-size is a second source of inaccuracy but once again, 
;this is a trade-off between performance and available resources. 
;The table could be extended to contain half degrees as well but this assumes you have lots of 
;flash. Besides, there is no real point in it (at least not for me) as the compass I am using 
;has an accuracy of about 3 degrees.
;
;These limitations makes the system pretty fast but you should not expect a 99,999% accuracy. 
;When used with an understanding of the calculation method used and taking it's limitations 
;into account, it should be possible to come pretty near to the point where you intended to go!
;
;Usage guidelines
;1: Everything behind the comma is lost!
;For optimal precision it is better to make calculations with large numbers whenever possible.
;This is because we do not take digits behind the comma into account. The error will be larger 
;when you are calculating 10 times a wheel displacement of 3 units as opposed to making this 
;calculation once for 30 units. 
;
;2:Drive straight!
;when driving in a straight line, (that is: remaining on the same heading +/- x degrees) the 
;program accumulates the distance and recalculates it's grid position from where it started 
;the straight line.
;
;3:Turn on the spot!
;The biggest errors occur when the robot is moving in a curved line. The program needs to make 
;many calculations over a relatively short distance and these errors will add up. 
;This can be prevented by turning on the spot. The wheel displacements will then be opposite 
;and practically zero eachother out. 
;
;Possible improvements:
;The code can still be made leaner & meaner. First goal was stability and a working result.
;- Target lists will be added to give the robot a row of coordinates to move to. The program 
;knows only one target but this can be altered by software when it has been reached.
;- Interaction with motor control is rather application specific and hence not a part of this
;code. Another application should take care of that using the input of the navigation system.
;
;Known issues:
;Overflow will occur in the calculation of the distance & angle to target when the robot has to 
;traverse a distance that is more than 32767 grid positions. This is because the distance is 
;saved as a 16bit signed number. Remember your highschool math!?
;
;Unknown issues:
;Huuh, no idea really but I assume there will be some. Please report any errors or 
;improvements that you add to this code so that I can correct/improve it. An updated release
;may be posted when appropriate. 
;
;That brings me to the following:
;I cannot accept liability when you hack this code into your fathers car and drive up the
;nearst lamppost. Actually, I do not accept any liabitlity for minor or major disasters 
;that may occur as a direct or indirect result of the use of this code. 
;
;Consider this code free for non commercial use, to be used at your own risk. 
;
;
;About the code
;The system consists of two separate routines:
;NAV01 
;	Calculates a grid position based on compass angle and wheel displacement
;
;NAVcdh
;	Calculates angle and distance to a specified grid coordinate (Target)
;
;The start at NAV00 is proprietary for my own implementation. I run this code in a loop that 
;excutes twice a second or so. The compass is read two t0ovf ticks before the code is started.
;You can use another approach to enter the correct values into the variables and run the routines
;as often as you like. Remember the Usage guidelines!
;
;The use of RAM memory is substantial (56 bytes). This is also because I am using my CPU registers 
;for many other purposes where many others seem to dedicate a large part of their MCU to just one 
;single task. RAM is used to off-load the data from the registers and still keep track. 
;I am using a Mega32 (2k RAM) 
;Due to the fact that all CPU registers are used for a variety of applications, I preferred to 
;keep the names for them at the Atmel default. No fancy register naming for me!
;Anyone may feel free to rename them in the source if this makes him/her happier;-)
;
;
.equ navcycles = 2 ;for testing
;.equ navcycles = 70 ;normal value, run twice per second
.equ navcompdiff=16 ;differnce of +- n/10 degrees

.equ NAVbase = (RAMstart + 0x1A0) ;could be anywhere
.equ navloop = (NAVbase + 0)
.equ navcmpsdata00 = (NAVbase + 1)
.equ navcmpsdata01 = (NAVbase + 2)
.equ navcmpsdata02 = (NAVbase + 3)
.equ navcmpsdata03 = (NAVbase + 4)
.equ navcmpsdata04 = (NAVbase + 5)

;average distance traversed:
.equ navdist00H = (NAVbase + 6)
.equ navdist00L = (NAVbase + 7)

;00=current odo, 01=previous
.equ navodoleft00H = (NAVbase + 8)
.equ navodoleft00L = (NAVbase + 9)
.equ navodoright00H = (NAVbase + 10)
.equ navodoright00L = (NAVbase + 11)
.equ navodoleft01H = (NAVbase + 12)
.equ navodoleft01L = (NAVbase + 13)
.equ navodoright01H = (NAVbase + 14)
.equ navodoright01L = (NAVbase + 15)

;00=current angle, 01=angle to target
.equ navsin00H = (NAVbase + 16)
.equ navsin00L = (NAVbase + 17)
.equ navsin01H = (NAVbase + 18)
.equ navsin01L = (NAVbase + 19)
.equ navcos00H = (NAVbase + 20)
.equ navcos00L = (NAVbase + 21)
.equ navcos01H = (NAVbase + 22)
.equ navcos01L = (NAVbase + 23)
.equ navtan00H = (NAVbase + 24)
.equ navtan00L = (NAVbase + 25)
.equ navtan01H = (NAVbase + 26)
.equ navtan01L = (NAVbase + 27)
;grid positions, current(00) and previous(01)
.equ navgridX00H = (NAVbase + 28)
.equ navgridX00L = (NAVbase + 29)
.equ navgridY00H = (NAVbase + 30)
.equ navgridY00L = (NAVbase + 31)
.equ navgridX01H = (NAVbase + 32)
.equ navgridX01L = (NAVbase + 33)
.equ navgridY01H = (NAVbase + 34)
.equ navgridY01L = (NAVbase + 35)
;extra grid storage for recalculation of larger distances
.equ navgridX02H = (NAVbase + 36)
.equ navgridX02L = (NAVbase + 37)
.equ navgridY02H = (NAVbase + 38)
.equ navgridY02L = (NAVbase + 39)
;distance travelled in a straight line; only 16bits (max 32m straight ahead)
.equ navdist02H = (NAVbase + 40)
.equ navdist02L = (NAVbase + 41)
;compass reading for straight line
.equ navcmpss01 = (NAVbase + 42)
.equ navcmpss02 = (NAVbase + 43)
;control variables for straight line calculations
.equ navctrl02 = (NAVbase + 44)
;Target lists & ctrl variables
.equ navctrl03 = (NAVbase + 45)
.equ navTX00H = (NAVbase + 46)
.equ navTX00L = (NAVbase + 47)
.equ navTY00H = (NAVbase + 48)
.equ navTY00L = (NAVbase + 49)

;temp for X/Y distance to target
.equ navTscratch00L = (NAVbase + 50)
.equ navTscratch00H = (NAVbase + 51)
.equ navTscratch01L = (NAVbase + 52)
.equ navTscratch01H = (NAVbase + 53)
.equ navdist03H = (NAVbase + 54)
.equ navdist03L = (NAVbase + 55)



NAVinit:
ldi YL,low(NAVbase)
ldi YH,high(NAVbase)
ldi r16,56
clr r13 ;always zero

NAVinit01:
st Y+,r13
dec r16
brne NAVinit01

ldi r16,(navcycles/2)
sts navloop,r16
ret

NAV00:
lds r16,navloop
cp r16,r13
breq NAV01

cpi r16,2 ;2 cycles =14,22ms, this should be more than sufficient to read cmps
brne  NAVexit

call NAVgetCMPS ;initiate I2C update of compass reading
ret

NAV01: ;navigation routine entry point
;when navloop=0 it must be reset to the starting value
ldi r16,navcycles 
sts navloop,r16

call TWIchkCMPS ;check if compass has been read
andi r24,$80 ;if $80: data ready, exit otherwise
brne NAV02

ldi r16,1 ;retry after 7ms
sts navloop,r16 

NAVexit:
ret

NAV02: ;data in Q
ldi YL,low(navcmpsdata00)
ldi YH,high(navcmpsdata00)
call TWIreadCMPS
;compass bearing is now in navcmpsdata01,02  01=high, 02=low byte. 
;The CMPS03 reports the angle as a 2byte number between 0 and 3599. 
;Example: 45 degrees equals 450.
;
;check if the compass differs from last reading
call navtstS
;check if we have moved, exit if not
call NAVchkmove
breq NAVexit

NAV03: ;we have moved, gather data to calculate new position
lds ZH,navcmpsdata01
lds ZL,navcmpsdata02
adiw ZH:ZL,5
ldi r18,10 ;set up divisor to divide Z-contents by ten (unsigned)
ldi r19,0 ;the remainder can be ignored
call div16u ;with regards to atmel appnote200
;adjust for 16bit organization of flashmemory
lsl ZL
rol ZH
sts navcmpsdata03,ZH
sts navcmpsdata04,ZL

rcall goniolookup ;find goniometric values for current compass angle

rcall navdistance 
;calculates the distance traversed. returns a 16bit positive number
;result is in navdist00L,H and also in r16,r17
;when navctrl02 = 0, navdist02L,H are cleared
;when navctrl02 > 0, navdist02L,H are updated

lds r14,navctrl02
and r14,r14
cpse r14,r13
rjmp NAV05

;calculating a new grid position uses compass angle and traversed distance
;there are two versions of this routine:
;NAV04 when navctrl02 = 0, course is not identical so the most recent grid position is used
;NAV05 when navctrl02 > 0, we are on the same course as in the previous interval.
;in this special case the grid position where we started driving straight is used
;the first case uses navdist00 with navgrid01 as starting point. 
;updates navgrid00,01,02 and resets navdist02
;in the second case navdist02 is used with navgrid02 as starting point.
;navgrid02 remains the same as long as we are driving straight
;updates navgrid00,01


NAV04:
;calculate new grid X-position when navctrl02 = 0
lds r16,navdist00L
lds r17,navdist00H
lds r18,navsin00L
lds r19,navsin00H
rcall mpy16s ;with regards to atmel appnote200

;perform division by 32768. fastest solution is to use the high bytes with one shift left.
;this equals multiplying by 65536 divided by 2
clc
rol r19 ;(required to adjust r20's lsb, r18 is not at all significant)
rol r20
rol r21
;result is now in r20,r21

lds r18,navgridX01L
lds r19,navgridX01H

add r18,r20
adc r19,r21
sts navgridX00L,r18 ;update current postion
sts navgridX00H,r19
sts navgridX01L,r18 ;update previous postion
sts navgridX01H,r19
sts navgridX02L,r18 ;update long range startpostion
sts navgridX02H,r19

;calculate new grid Y-position when navctrl02 = 0
lds r16,navdist00L
lds r17,navdist00H
lds r18,navcos00L
lds r19,navcos00H
rcall mpy16s

;perform division by 32768. fastest solution is to use the high bytes with one shift left.
;this equals x65536/2
clc
rol r19 ;(to adjust r20,lsb)
rol r20
rol r21
;result is now in r20,r21

lds r18,navgridY01L
lds r19,navgridY01H

add r18,r20
adc r19,r21
sts navgridY00L,r18 ;update current postion
sts navgridY00H,r19
sts navgridY01L,r18 ;update previous position
sts navgridY01H,r19
sts navgridY02L,r18 ;update long range startpostion
sts navgridY02H,r19
ret

NAV05:
;calculate new grid X-position when navctrl02 > 0
lds r16,navdist02L
lds r17,navdist02H
lds r18,navsin00L
lds r19,navsin00H
rcall mpy16s ;with regards to atmel appnote200

;perform division by 32768. fastest solution is to use the high bytes with one shift left.
;this equals multiplying by 65536 divided by 2
clc
rol r19 ;(required to adjust r20's lsb, r18 is not at all significant)
rol r20
rol r21
;result is now in r20,r21

lds r18,navgridX02L
lds r19,navgridX02H

add r18,r20
adc r19,r21
sts navgridX00L,r18 ;update current postion
sts navgridX00H,r19
sts navgridX01L,r18 ;update previous postion
sts navgridX01H,r19

;calculate new grid Y-position when navctrl02 > 0
lds r16,navdist02L
lds r17,navdist02H
lds r18,navcos00L
lds r19,navcos00H
rcall mpy16s

;perform division by 32768. fastest solution is to use the high bytes with one shift left.
;this equals x 65536/2
clc
rol r19 ;(to adjust r20,lsb, r18 is not significant)
rol r20
rol r21
;result is now in r20,r21

lds r18,navgridY01L
lds r19,navgridY01H

add r18,r20
adc r19,r21
sts navgridY00L,r18 ;update current postion
sts navgridY00H,r19
sts navgridY01L,r18 ;update previous position
sts navgridY01H,r19

ret

;********************** NAV services below this line *****************************

NAVgetCMPS:
;initiate reading of the compass bearing
ldi ZL,low((NAVgetbearing)*2)
ldi ZH,high((NAVgetbearing)*2)
call TWIwrdfCMPS
ret


;Qlen=5 bytes, SLA+W, reg 2, SLA+R, bearing
NAVgetbearing:
.db 5,$C0,2,$C1

NAVchkmove:
;this routine checks the low bytes of the ODO-system.
;exit with zero-flag set if they are unchanged
;exit with new ODOvalues in navodoleft/right when we have moved, zero-flag cleared
;only the lower two bytes of ODO are copied. 
;distances traversed must never be more than 32767cm per interval.

sts navctrl03,r13 ;clear control variable

lds r15,ODOleft00
lds r16,navodoleft00L
cpse r15,r16
rcall NAVmove01 ;update if left=changed

lds r15,ODOright00
lds r16,navodoright00L
cpse r15,r16
rcall NAVmove02 ;update if right=changed

lds r16,navctrl03
andi r16,$C0
ret


NAVmove01:
;update byte00
sts navodoleft01L,r16
sts navodoleft00L,r15
;update byte01
lds r15,(ODOleft00+1)
lds r16,navodoleft00H
sts navodoleft01H,r16
sts navodoleft00H,r15
 
lds r16,navctrl03
sbr r16,$80
sts navctrl03,r16
ret

NAVmove02:
;update byte00
sts navodoright01L,r16
sts navodoright00L,r15
;update byte01
lds r15,(ODOright00+1)
lds r16,navodoright00H
sts navodoright01H,r16
sts navodoright00H,r15
 
lds r16,navctrl03
sbr r16,$40
sts navctrl03,r16
ret


div16u:
;***************************************************************************
;* AVR appnote 200
;* "div16u" - 16/16 Bit Unsigned Division
;*
;* This subroutine divides the two 16-bit numbers 
;* "ZH:ZL" (dividend) and "r19:r18" (divisor). 
;* The result is placed in "ZH:ZL" and the remainder in
;* "r12:r11".
;*  
;* Number of words	:19
;* Number of cycles	:235/251 (Min/Max)
;* Low registers used	:2 (r11,r12)
;* High registers used  :5 (ZL/ZL,ZH/ZH,r18,r19,
;*			    r20)
;*
;*Modified to return result in Z-reg
;***************************************************************************

;***** Subroutine Register Variables

;	r11
;	r12
;	r18
;	r19
;	r20

;save registers that may be used elsewhere
push r20

div16u0:
	clr	r11	;clear remainder Low byte
	sub	r12,r12;clear remainder High byte and carry
	ldi	r20,17	;init loop counter
d16u_1:	rol	ZL		;shift left dividend
	rol	ZH
	dec	r20		;decrement counter
	brne	d16u_2		;if done
	pop r20
	ret			;return

d16u_2:	rol	r11	;shift dividend into remainder
	rol	r12
	sub	r11,r18	;remainder = remainder - divisor
	sbc	r12,r19	;
	brcc	d16u_3		;if result negative
	add	r11,r18	;restore remainder
	adc	r12,r19
	clc			;clear carry to be shifted into result
	rjmp	d16u_1		;else
d16u_3:	sec			;set carry to be shifted into result
	rjmp	d16u_1
	

goniolookup:
;finds sin/cos/tan that belong to current angle and saves them
;on entry, Z contains the offset, also in navcmpsdata03/04

;lookup sin @
ldi r16,low((sin00)*2)
ldi r17,high((sin00)*2)
add ZL,r16
adc ZH,r17

lpm r16,Z+
sts navsin00L,r16
lpm r17,Z+
sts navsin00H,r17

;lookup cos @
lds ZH,navcmpsdata03
lds ZL,navcmpsdata04
ldi r16,low((cos00)*2)
ldi r17,high((cos00)*2)
add ZL,r16
adc ZH,r17

lpm r16,Z+
sts navcos00L,r16
lpm r17,Z+
sts navcos00H,r17

;lookup tan @
lds ZH,navcmpsdata03
lds ZL,navcmpsdata04
ldi r16,low((tan00)*2)
ldi r17,high((tan00)*2)
add ZL,r16
adc ZH,r17

lpm r16,Z+
sts navtan00L,r16
lpm r17,Z+
sts navtan00H,r17
ret

goniolookup2:
;finds sin/cos that belong to target angle and saves them in navsin01,navcos01
;on entry, Z contains the offset, also in navcmpsdata03/04

;lookup sin @
ldi r16,low((sin00)*2)
ldi r17,high((sin00)*2)
add ZL,r16
adc ZH,r17

lpm r16,Z+
sts navsin01L,r16
lpm r17,Z+
sts navsin01H,r17

;lookup cos @
lds ZH,navcmpsdata03
lds ZL,navcmpsdata04
ldi r16,low((cos00)*2)
ldi r17,high((cos00)*2)
add ZL,r16
adc ZH,r17

lpm r16,Z+
sts navcos01L,r16
lpm r17,Z+
sts navcos01H,r17
ret


navdistance:
;calculate the average distance traversed

lds r16,navodoleft01L
lds r17,navodoleft01H
lds r18,navodoleft00L
lds r19,navodoleft00H

sub r18,r16
sbc r19,r17

sts navdist00L,r18
sts navdist00H,r19

lds r16,navodoright01L
lds r17,navodoright01H
lds r18,navodoright00L
lds r19,navodoright00H

sub r18,r16
sbc r19,r17

lds r16,navdist00L
lds r17,navdist00H

add r16,r18
adc r17,r19
brpl navdistance01

;perform two's complement when the result is minus
com r16
com r17
subi	r16,low(-1)
sbci	r17,high(-1)

navdistance01: ;get average (divide by 2)
lsr r17
ror r16
sts navdist00L,r16
sts navdist00H,r17

lds r14,navctrl02
and r14,r14
breq navdistance02 ;navdist02 is cleared when 

lds r18,navdist02L
lds r19,navdist02H
add r18,r16
adc r19,r17
sts navdist02L,r18
sts navdist02H,r19
ret

navdistance02:
sts navdist02L,r13
sts navdist02H,r13
ret


;***************************************************************************
;*
;* "mpy16s" - 16x16 Bit Signed Multiplication
;*
;* This subroutine multiplies signed the two 16-bit register variables 
;* r19:r18 and r17:r16.
;* The result is placed in r21:r20:r19:r18.
;* The routine is an implementation of Booth's algorithm. If all 32 bits
;* in the result are needed, avoid calling the routine with
;* -32768 ($8000) as multiplicand
;*  
;* Number of words	:16 + return
;* Number of cycles	:210/226 (Min/Max) + return
;* Low registers used	:None
;* High registers used  :7 (r18,r19,r16/r18,r17/r19,
;*			    r20,r21,r22)	
;*
;***************************************************************************

;***** Subroutine Register Variables

;r16		;multiplicand low byte
;r17		;multiplicand high byte
;r18		;multiplier low byte
;r19		;multiplier high byte
;r18		;result byte 0 (LSB)
;r19		;result byte 1
;r20		;result byte 2
;r21		;result byte 3 (MSB)
;r22		;loop counter

mpy16s:	clr	r21		;clear result byte 3
	sub	r20,r20	;clear result byte 2 and carry
	ldi	r22,16	;init loop counter
m16s_1:	brcc	m16s_2		;if carry (previous bit) set
	add	r20,r16	;    add multiplicand Low to result byte 2
	adc	r21,r17	;    add multiplicand High to result byte 3
m16s_2:	sbrc	r18,0	;if current bit set
	sub	r20,r16	;    sub multiplicand Low from result byte 2
	sbrc	r18,0	;if current bit set
	sbc	r21,r17	;    sub multiplicand High from result byte 3
	asr	r21		;shift right result and multiplier
	ror	r20
	ror	r19
	ror	r18
	dec	r22		;decrement counter
	brne	m16s_1		;if not done, loop more	
	ret

navtstS: ;compass compare
;check if compass data remains within certain boundaries
;used for straight-line calculations
lds r16, navcmpsdata02 ;low byte
lds r24, navcmpss02
lds r17, navcmpsdata01 ;high byte
lds r25, navcmpss01


sub r24,r16
sbc r25,r17
;result is in r24:25
brpl navtstS01

;negative result: invert first
com r24
com r25
subi	r24,low(-1)
sbci	r25,high(-1)

navtstS01: 
cpi r25,0
breq navtstS03

navtstS02: ;exit if result is not near enough: update compass course, set navctrl02=0
sts navcmpss01,r17
sts navcmpss02,r16
sts navctrl02,r13

ret

navtstS03: ;high byte = equal
cpi r24, (navcompdiff)
brge navtstS02

;result < limit
sts navcmpss01,r17 ;store course, inc navctrl02
sts navcmpss02,r16
lds r16,navctrl02
inc r16
sts navctrl02,r16
ret


;calculate distance & heading to target

NAVcdh:
;Tx - Px(n)/Ty - Py(n)= tan (compass angle to destination)
lds r16,navgridX01L
lds r17,navgridX01H
lds r18,navTX00L
lds r19,navTX00H

sub r18,r16
sbc r19,r17

sts navTscratch00H,r19
sts navTscratch00L,r18

lds r16,navgridY01L
lds r17,navgridY01H
lds r18,navTY00L
lds r19,navTY00H

sub r18,r16
sbc r19,r17
;calculate tan@
;we must multiply the dividend 512 so that the result will have
;the same resolution as used for the stored tan-data. 

sts navTscratch01H,r19 ;divisor
sts navTscratch01L,r18
mov r20,r18
mov r21,r19

;setup dividend
clr r16
lds r17,navTscratch00L ;dividend x 2^8 while stored at +8bits
lds r18,navTscratch00H
clr r19
sbrc r18,7
com r19 ;invert r19 when dividend < 0

lsl r16 ;not needed but correct
rol r17 ;multiply by 2 once more: 2^9
rol r18
rol r19

navca09:
call div32s 	;result in r16(L),r17(H)

sts navtan01L,r16 ;this is the tan of the heading to the target
sts navtan01H,r17 ;

;determine sign of Ty - Py(n)/Tx - Px(n)
lds r19,navTscratch01H
cp r19,r13
brpl navca00

;Y<0, test if X >0
lds r18,navTscratch00H
cp r18,r13
brpl navca03

;------------------------
navca02: ;X<0,Y<0
;search range 180-270
ldi ZL,low((tan00+180)*2)
ldi ZH,high((tan00+180)*2)
rjmp navca10
;------------------------
navca00: ;Y>0, test if X >0
lds r18,navTscratch00H
cp r18,r13
brmi navca01

;X>0,Y>0
;search range 0-90
ldi ZL,low((tan00+0)*2)
ldi ZH,high((tan00+0)*2)
rjmp navca10

;------------------------
navca03: ;X>0,Y<0
;search range 90-180
ldi ZL,low((tan00+90)*2)
ldi ZH,high((tan00+90)*2)
rjmp navca10

;------------------------
navca01: ;X<0,Y>0
;search range 270-360
ldi ZL,low((tan00+270)*2)
ldi ZH,high((tan00+270)*2)

navca10: ;Lookup tan
;find nearest matching value. 
;registers used:
;r10-r11	smallest difference found
;r14,r15	value to be tested
;r16,r17	value to be compared
;r18,r19	scratch space
;Z-reg		pointer in flash
;r20 loopcounter

;initiate loop by reading the first value
clr r20
inc r20
lpm r14,Z+
lpm r15,Z+
mov r19,r15
mov r18,r14
sub r18,r16
sbc r19,r17
brpl navca11

com r18
com r19
subi r18,low(-1)
sbci r19,high(-1)

navca11:
mov r11,r19
mov r10,r18

navca12:
inc r20
lpm r14,Z+
lpm r15,Z+
mov r19,r15
mov r18,r14
sub r18,r16
sbc r19,r17
brpl navca13

com r18		;two complement inverse
com r19
subi r18,low(-1)
sbci r19,high(-1)

navca13:
cp r19,r11 ;compare msb
brlo navca11 ;branch if result-msb is smaller

cp r18,r10
brlo navca11 ;branch if result-lsb is smaller
;result is the same or higher than previous > USE PREVIOUS RESULT

;adjust Z-reg: 4 places back & offset from start of table
subi ZL,low((tan00+2)*2)
sbci ZH,high((tan00+2)*2)

sts navcmpsdata03,ZH ;temp store for offset
sts navcmpsdata04,ZL

call goniolookup2 ;modified to lookup sin & cos

;calculate distance to target 
;(Tx - Px(n)*32768) / (sin (angle to destination)*32768)
;	or
;(Ty - Py(n)*32768) / (cos (angle to destination)*32768)
;dividend must be a 32bit number to keep the resolution
;use sin unless it is zero. due to the multiplication by 32768, msb of sin > 0 unless sin=0

lds r21,navsin01H
cp r21,r13
breq navca16

;use sin
lds r20,navsin01L ;setup divisor
lds r21,navsin01H
lds r19,navTscratch00H ;setup dividend
lds r18,navTscratch00L
clr r17
clr r16
clc
sbrc r19,7
sec
ror r19 ;adjust for 2^15
ror r18
ror r17
rjmp navca17

navca16:
lds r21,navcos01H
lds r20,navcos01L ;setup divisor
lds r19,navTscratch01H ;setup dividend
lds r18,navTscratch01L
clr r17
clr r16
clc
sbrc r19,7
sec
ror r19 ;adjust for 2^15
ror r18
ror r17

navca17:
call div32s

;result is in r17:16
sbrc r17,7
rcall navinverse ;invert result when minus. Distance should be absolute (i.e. positive)

sts navdist03L,r16
sts navdist03H,r17

ret

navinverse:
com r16
com r17
subi r16,low(-1)
sbci r17,high(-1)
ret

;***************************************************************************
;*
;* "div32s" - 32/16 Bit Signed Division
;*
;* This subroutine divides two signed numbers. dividend 32bit, divisor 16 bit  
;* "r19:r16" (dividend) and "r21:r20" (divisor). 
;* The result is placed in "r19:r16" and the remainder in
;* "r15:r14".
;*  
;* Number of words	:
;* Number of cycles	: (Min/Max)
;* Low registers used	:3 (r12,r14,r15)
;* High registers used  :7 (r16 tm r22)
;* Adapted for 32 bit dividend
;* LG nov 2005
;***************************************************************************
;***** Subroutine Register Variables

; r12		;sign register
; r14		;remainder low byte		
; r15		;remainder high byte
; r16		;result byte 0
; r17		;result byte 1
; r18		;result byte 2
; r19		;result byte 3
; r16		;dividend byte 0
; r17		;dividend byte 1
; r18		;dividend byte 2
; r19		;dividend byte 3
; r20		;divisor low byte
; r21		;divisor high byte
; r22		;loop counter

div32s:	mov	r12,r19	;move dividend High to sign register
	eor	r12,r21	;xor divisor High with sign register
	sbrs	r19,7	;if MSB in dividend set
	rjmp	d32s_1
	com	r19		;    change sign of dividend
	com	r18
	com	r17
	com	r16		
	subi	r16,low(-1)
	sbci	r17,high(-1)
	sbci	r18,high(-1)
	sbci	r19,high(-1)

d32s_1:	sbrs	r21,7	;if MSB in divisor set
	rjmp	d32s_2
	com	r21		;change sign of divisor
	com	r20		
	subi	r20,low(-1)
	sbci	r21,high(-1)

d32s_2:	clr	r14	;clear remainder Low byte
	sub	r15,r15;clear remainder High byte and carry
	ldi	r22,33	;init loop counter

d32s_3:	rol	r16		;shift left dividend
	rol	r17
	rol	r18
	rol	r19
	dec	r22		;decrement counter
	brne	d32s_5		;if done
	sbrs	r12,7		; if MSB in sign register set
	rjmp	d32s_4
	com	r19	; change sign of result
	com	r18
	com	r17
	com	r16		
	subi	r16,low(-1)
	sbci	r17,high(-1)
	sbci	r18,high(-1)
	sbci	r19,high(-1)
d32s_4:	ret			;    return

d32s_5:	rol	r14	;shift dividend into remainder
	rol	r15
	sub	r14,r20	;remainder = remainder - divisor
	sbc	r15,r21	;
	brcc	d32s_6		;if result negative
	add	r14,r20	;restore remainder
	adc	r15,r21
	clc			;clear carry to be shifted into result
	rjmp	d32s_3		; else
d32s_6:	sec			;set carry to be shifted into result
	rjmp	d32s_3

