;FastAVR Basic Compiler, ver.3.1.7, by MicroDesign
;Created 20:57:43, 12-10-2003
;
.include	"C:\FastAVR\inc\8535DEF.INC"
;
		.DSEG
pt:						.byte	 1
pl:						.byte	 1
scan:					.byte	 1
adr:					.byte	 1
count:					.byte	 1
count1:					.byte	 1
cmd:					.byte	 1
a:						.byte	 1
b:						.byte	 1
c:						.byte	 1
d:						.byte	 1
e:						.byte	 1
f:						.byte	 1
p:						.byte	 1
t:						.byte	 1
a2:						.byte	 2
b2:						.byte	 2
c2:						.byte	 2
d2:						.byte	 2
e2:						.byte	 2
f2:						.byte	 2
p2:						.byte	 2
t2:						.byte	 2
rx:						.byte	 32
prx:					.byte	 1
ple:					.byte	 1
cpt0:					.byte	 1
cpt1:					.byte	 1
speed0:					.byte	 1
speed1:					.byte	 1
dir0:					.byte	 1
dir1:					.byte	 1
wd:						.byte	 1
tempms:					.byte	 1
temps:					.byte	 1
t0:						.byte	 1
t1:						.byte	 1
wd0:					.byte	 2
wd1:					.byte	 2
version:				.byte	 1
revision:				.byte	 1
error:					.byte	 1
status:					.byte	 1
pgm:					.byte	 64
pco:					.byte	 1
token:					.byte	 1
token2:					.byte	 1
var:					.byte	 128
math1:					.byte	 2
math2:					.byte	 2
test:					.byte	 2
vlow:					.byte	 1
vmedium:				.byte	 1
vfast:					.byte	 1
param:					.byte	 2
c_timer:				.byte	 2
c_counter0:				.byte	 2
c_counter1:				.byte	 2
mode:					.byte	 1
pidl:					.byte	 1
epid:					.byte	 1
ebumper:				.byte	 1
bumper0:				.byte	 1
bumper1:				.byte	 1
obumper0:				.byte	 1
obumper1:				.byte	 1
raddress:				.byte	 1
		
		.CSEG
.ORG	0
		rjmp	_Reset
.ORG		INT0addr
		rjmp	IntN1
.ORG		INT1addr
		rjmp	IntN2
.ORG		OC2addr 
		reti
.ORG		OVF2addr
		reti
.ORG		ICP1addr
		reti
.ORG		OC1Aaddr
		reti
.ORG		OC1Baddr
		reti
.ORG		OVF1addr
		reti
.ORG		OVF0addr
		rjmp	IntN9
.ORG		SPIaddr 
		reti
.ORG		URXCaddr
		rjmp	IntN11

_Reset:
		ldi		yl,low(RAMEND)
		out		SPL,yl
		ldi		yh,high(RAMEND)
		out		SPL+1,yh
		sbiw	yl,40
		ldi		zl,0x18
		out		UCR,zl

		ldi		zh,high(25)
		ldi		zl,low(25)
		out		UBRR,zl

		ldi		zl,161
		out		TCCR1A,zl
		ldi		zl,0x1f
		out		WDTCR,zl
		ldi		zl,0x17
		out		WDTCR,zl
		sbi		DDRC,5
		cbi		PORTC,5

;******	USERS BASIC CODE **********************

;-Line--0449----DDRA  =&h00        			' ALL Analog Input Port--
		ldi		zl,low(0)
		out		ddra,zl

;-Line--0450----PORTA =&h00                 ' NO PULL UP--
		ldi		zl,low(0)
		out		porta,zl

;-Line--0452----DDRB.0=1                    ' CODEUR 1   /  LED GREEN--
		sbi		DDRB,0

;-Line--0453----DDRB.1=0                    ' CODEUR 2--
		cbi		DDRB,1

;-Line--0454----DDRB.2=1                    ' IR TX--
		sbi		DDRB,2

;-Line--0455----DDRB.3=1                    ' RESERVE    /  LED GREEN--
		sbi		DDRB,3

;-Line--0456----DDRB.4=0                    ' D1 I/O SS   Input Port--
		cbi		DDRB,4

;-Line--0457----DDRB.5=0                    ' D2 I/O MOSI Input Port--
		cbi		DDRB,5

;-Line--0458----DDRB.6=0                    ' D3 I/O MISO Input Port--
		cbi		DDRB,6

;-Line--0459----DDRB.7=0                    ' D4 I/O SCK  Input Port--
		cbi		DDRB,7

;-Line--0461----PORTB.0 = 1                 ' PULL UP--
		sbi		PORTB,0

;-Line--0462----PORTB.1 = 1--
		sbi		PORTB,1

;-Line--0463----PORTB.2 = 0--
		cbi		PORTB,2

;-Line--0464----PORTB.3 = 0--
		cbi		PORTB,3

;-Line--0465----PORTB.4 = 1--
		sbi		PORTB,4

;-Line--0466----PORTB.5 = 1--
		sbi		PORTB,5

;-Line--0467----PORTB.6 = 1--
		sbi		PORTB,6

;-Line--0468----PORTB.7 = 1--
		sbi		PORTB,7

;-Line--0470----DDRC= 252                   ' 255-1-2=252 I2C and Motors Port--
		ldi		zl,low(252)
		out		ddrc,zl

;-Line--0472----DDRD.2 = 0                  ' INT 0--
		cbi		DDRD,2

;-Line--0473----DDRD.3 = 0                  ' INT 1--
		cbi		DDRD,3

;-Line--0474----DDRD.4 = 1                  ' PWM B--
		sbi		DDRD,4

;-Line--0475----DDRD.5 = 1                  ' PWM A--
		sbi		DDRD,5

;-Line--0476----DDRD.6 = 0                  ' IRRX--
		cbi		DDRD,6

;-Line--0477----DDRD.7 = 1                  ' SERVO--
		sbi		DDRD,7

;-Line--0479----PORTD.2 = 1                 ' INT 0 PULL UP--
		sbi		PORTD,2

;-Line--0480----PORTD.3 = 1                 ' INT 1 PULL UP--
		sbi		PORTD,3

;-Line--0481----PORTD.6 = 1                 ' IRRX  PULL UP--
		sbi		PORTD,6

;-Line--0483----Int0 Falling--
		in		zl,MCUCR
		sbr		zl,0x02
		cbr		zl,0x01
		out		MCUCR,zl

;-Line--0484----Int1 Falling--
		in		zl,MCUCR
		sbr		zl,0x08
		cbr		zl,0x04
		out		MCUCR,zl

;-Line--0486----Timer0    = &h83             ' Timer 1 mS--
		ldi		zl,low(131)
		out		TCNT0,zl

;-Line--0487----Timer1    = &h00             ' Timer PWM--
		ldi		zl,low(0)
		out		TCNT1H,zh
		out		TCNT1L,zl

;-Line--0488----Timer2    = &h00             ' Timer SERVO--
		ldi		zl,low(0)
		out		TCNT2,zl

;-Line--0490----Enable  Ovf0--
		in		zl,TIMSK
		sbr		zl,0x01
		out		TIMSK,zl

;-Line--0491----Enable  Urxc--
		sbi		UCR,7

;-Line--0492----Enable  Int0--
		in		zl,GIMSK
		sbr		zl,0x40
		out		GIMSK,zl

;-Line--0493----Enable  Int1--
		in		zl,GIMSK
		sbr		zl,0x80
		out		GIMSK,zl

;-Line--0494----Disable Interrupts--
		cli

;-Line--0496----Stop  WatchDog         ' ??--
		in		zl,WDTCR
		sbr		zl,0x18
		out		WDTCR,zl
		cbr		zl,0x08
		out		WDTCR,zl

;-Line--0497----Start Timer0--
		in		zl,TCCR0
		sbr		zl,3
		out		TCCR0,zl

;-Line--0498----Start Timer1--
		in		zl,TCCR1B
		sbr		zl,1
		out		TCCR1B,zl

;-Line--0499----Start Timer2--
		in		zl,TCCR2
		sbr		zl,4
		out		TCCR2,zl

;-Line--0500----Start Adc--
		ldi		zl,0x86
		out		ADCSR,zl

;-Line--0503----Randomize(123)--
		.DSEG
_rnd:	.byte	1
		.CSEG
		ldi		zl,low(123)
		sts		_rnd,zl

;-Line--0505----ERROR  =0--
		ldi		zl,low(0)
		sts		error,zl

;-Line--0506----STATUS =0--
		ldi		zl,low(0)
		sts		status,zl

;-Line--0508----VAR(SECURITY)=0--
		ldi		zl,low(40)
		ldi		zh,high(40)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0509----VAR(RUN)=0--
		ldi		zl,low(41)
		ldi		zh,high(41)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0511----COUNT  = 0               ' Timer 0 PWM Count--
		ldi		zl,low(0)
		sts		count,zl

;-Line--0512----COUNT1 = 0               ' Timer 0     Count--
		ldi		zl,low(0)
		sts		count1,zl

;-Line--0514----For P=0 To 30          ' RESET RX Buffer  ??--
		ldi		zl,low(0)
		sts		p,zl
		ldi		zl,low(30)
		push	r5
		inc		zl
		mov		r5,zl
L0000:

;-Line--0515----RX(P)=0--
		lds		zl,p
		clr		zh
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(rx)
		ldi		xh,high(rx)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0516----Next--
		lds		zl,p
		inc		zl
		sts		p,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0000
		pop		r5
L0001:

;-Line--0518----PRX=0--
		ldi		zl,low(0)
		sts		prx,zl

;-Line--0519----CMD=0--
		ldi		zl,low(0)
		sts		cmd,zl

;-Line--0520----MF=0--
		clt
		bld		r3,0

;-Line--0522----BREAK()              ' INITIALISATION--
		rcall	break

;-Line--0523----SPEED()              ' SET SPEED and DIR--
		rcall	speed

;-Line--0525----ADR    = ReadEE(1)      	' Card LAN ADDRESS--
		ldi		zl,low(1)
		ldi		zh,high(1)
		mov		r24,zl
		mov		r25,zh
		rcall	_RdEE
		sts		adr,zl

;-Line--0527----VLOW   = ReadEE(3)--
		ldi		zl,low(3)
		ldi		zh,high(3)
		mov		r24,zl
		mov		r25,zh
		rcall	_RdEE
		sts		vlow,zl

;-Line--0528----If VLOW=0 Then VLOW=100--
		lds		r24,vlow
		ldi		zl,low(0)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0003
L0004:
		ldi		zl,low(100)
		sts		vlow,zl
L0003:

;-Line--0530----VMEDIUM= ReadEE(4)--
		ldi		zl,low(4)
		ldi		zh,high(4)
		mov		r24,zl
		mov		r25,zh
		rcall	_RdEE
		sts		vmedium,zl

;-Line--0531----If VMEDIUM=0 Then VMEDIUM=150--
		lds		r24,vmedium
		ldi		zl,low(0)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0006
L0007:
		ldi		zl,low(150)
		sts		vmedium,zl
L0006:

;-Line--0533----VFAST  = ReadEE(5)--
		ldi		zl,low(5)
		ldi		zh,high(5)
		mov		r24,zl
		mov		r25,zh
		rcall	_RdEE
		sts		vfast,zl

;-Line--0534----If VFAST=0 Then VFAST=255--
		lds		r24,vfast
		ldi		zl,low(0)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0009
L0010:
		ldi		zl,low(255)
		sts		vfast,zl
L0009:

;-Line--0537----VERSION=1   		' Version--
		ldi		zl,low(1)
		sts		version,zl

;-Line--0538----REVISION=19         ' Revision--
		ldi		zl,low(19)
		sts		revision,zl

;-Line--0541----PCO=1--
		ldi		zl,low(1)
		sts		pco,zl

;-Line--0542----MATH1=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		math1,zl
		sts		math1+1,zh

;-Line--0543----MATH2=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		math2,zl
		sts		math2+1,zh

;-Line--0544----ERROR=0--
		ldi		zl,low(0)
		sts		error,zl

;-Line--0546----Print "ROBOT GLU READY "--
		ldi		zl,low(S00*2)
		ldi		zh,high(S00*2)
		rcall	_PSc
		rcall	_PCL

;-Line--0547----Print "ADRESSE "; ADR--
		ldi		zl,low(S01*2)
		ldi		zh,high(S01*2)
		rcall	_PSc
		lds		zl,adr
		rcall	_B2Str
		rcall	_PrBW
		rcall	_PCL

;-Line--0548----Print "Version B "; VERSION ; "." ; REVISION--
		ldi		zl,low(S02*2)
		ldi		zh,high(S02*2)
		rcall	_PSc
		lds		zl,version
		rcall	_B2Str
		rcall	_PrBW
		ldi		zl,low(S03*2)
		ldi		zh,high(S03*2)
		rcall	_PSc
		lds		zl,revision
		rcall	_B2Str
		rcall	_PrBW
		rcall	_PCL

;-Line--0549----Print--
		rcall	_PCL

;-Line--0551----PL=0--
		ldi		zl,low(0)
		sts		pl,zl

;-Line--0553----Enable Interrupts--
		sei

;-Line--0554----Wait 1--
		ldi		zl,low(1)
		rcall	_ws

;-Line--0556----Do--
L0011:

;-Line--0557----SCAN=0--
		ldi		zl,low(0)
		sts		scan,zl

;-Line--0558----Reset WatchDog--
		wdr

;-Line--0559----STATUS=1--
		ldi		zl,low(1)
		sts		status,zl

;-Line--0561----For P=0 To 7              ' ANALOG INPUTS  10 Bits--
		ldi		zl,low(0)
		sts		p,zl
		ldi		zl,low(7)
		push	r5
		inc		zl
		mov		r5,zl
L0013:

;-Line--0562----VAR(P)=Adc8(P)--
		lds		zl,p
		clr		zh
		push	zh
		push	zl
		lds		zl,p
		out		ADMUX,zl
		rcall	_Adc8
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0563----Next--
		lds		zl,p
		inc		zl
		sts		p,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0013
		pop		r5
L0014:

;-Line--0565----For P=0 To 7--
		ldi		zl,low(0)
		sts		p,zl
		ldi		zl,low(7)
		push	r5
		inc		zl
		mov		r5,zl
L0015:

;-Line--0566----A=PINB.P              ' DIGITAL INPUTS--
		in		zl,PINB
		lds		r24,p
		rcall	_VaBiL
		rcall	_StBit
		clr		r24
		bld		r24,0
		sts		a,r24

;-Line--0567----VAR(8+P)=Abs(A)--
		ldi		zl,low(8)
		ldi		zh,high(8)
		push	zh
		push	zl
		lds		zl,p
		clr		zh
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		push	zh
		push	zl
		lds		zl,a
		sbrs	zl,0x07
		rjmp	L0017
		com		zl
		inc		zl
L0017:
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0568----Next--
		lds		zl,p
		inc		zl
		sts		p,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0015
		pop		r5
L0016:

;-Line--0570----EBUMPER=VAR(42)         ' ENABLE BUMPERS--
		ldi		zl,low(42)
		ldi		zh,high(42)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		ebumper,zl

;-Line--0571----OBUMPER0=BUMPER0        ' OLD VALUE BUMPER--
		lds		zl,bumper0
		sts		obumper0,zl

;-Line--0572----OBUMPER1=BUMPER1--
		lds		zl,bumper1
		sts		obumper1,zl

;-Line--0573----BUMPER0=VAR(8)          ' NEW VALUE BUMPER--
		ldi		zl,low(8)
		ldi		zh,high(8)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		bumper0,zl

;-Line--0574----BUMPER1=VAR(9)--
		ldi		zl,low(9)
		ldi		zh,high(9)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		bumper1,zl

;-Line--0576----VAR(39)=PIND.6     ' IR RX--
		ldi		zl,low(39)
		ldi		zh,high(39)
		push	zh
		push	zl
		in		zl,PIND
		bst		zl,6			;store zl,bit 6 to T flag
		clr		zl
		bld		zl,0			;store T flag to zl bit 0
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0578----If MF=1 Then                 ' EXECUTE AVAILABLE MESSAGE--
		sbrs	r3,0
		rjmp	L0019
L0020:

;-Line--0579----COMMUNICATION()--
		rcall	communication

;-Line--0580----End If--
L0019:
L0018:

;-Line--0582----SPEED0=VAR(16)--
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		speed0,zl

;-Line--0583----SPEED1=VAR(17)--
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		speed1,zl

;-Line--0584----DIR0=VAR(36)--
		ldi		zl,low(36)
		ldi		zh,high(36)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		dir0,zl

;-Line--0585----DIR1=VAR(37)--
		ldi		zl,low(37)
		ldi		zh,high(37)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		dir1,zl

;-Line--0586----WD=VAR(40)--
		ldi		zl,low(40)
		ldi		zh,high(40)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		wd,zl

;-Line--0588----If VAR(SECURITY)>0 Then--
		ldi		zl,low(40)
		ldi		zh,high(40)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(0)
		inc		zl
		pop		r24
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0022
L0023:

;-Line--0589----DEAD()						 ' DETECT DEAD Moteurs bloques--
		rcall	dead

;-Line--0590----End If                      ' RESET VARS SPEED--
L0022:
L0021:

;-Line--0592----If EBUMPER=1 Then    'ENABLE BUMPER SECURITY--
		lds		r24,ebumper
		ldi		zl,low(1)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0025
L0026:

;-Line--0594----If BUMPER0 > OBUMPER0 Then ' DETECTION FLANC MONTANT--
		lds		r24,bumper0
		lds		zl,obumper0
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0028
L0029:

;-Line--0595----SPEED0=0--
		ldi		zl,low(0)
		sts		speed0,zl

;-Line--0596----SPEED1=0--
		ldi		zl,low(0)
		sts		speed1,zl

;-Line--0597----End If--
L0028:
L0027:

;-Line--0599----If BUMPER1 > OBUMPER1 Then--
		lds		r24,bumper1
		lds		zl,obumper1
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0031
L0032:

;-Line--0600----SPEED0=0--
		ldi		zl,low(0)
		sts		speed0,zl

;-Line--0601----SPEED1=0--
		ldi		zl,low(0)
		sts		speed1,zl

;-Line--0602----End If--
L0031:
L0030:

;-Line--0604----End If--
L0025:
L0024:

;-Line--0606----SPEED()                     ' SET MOTORS OUTPUTS--
		rcall	speed

;-Line--0608----If VAR(18)=1 Then--
		ldi		zl,low(18)
		ldi		zh,high(18)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(1)
		pop		r24
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0034
L0035:

;-Line--0609----Set P1--
		sbi		PORTC,5

;-Line--0610----Else--
		rjmp	L0033
L0034:

;-Line--0611----Reset P1--
		cbi		PORTC,5

;-Line--0612----End If--
L0033:

;-Line--0614----If VAR(19)=1 Then--
		ldi		zl,low(19)
		ldi		zh,high(19)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(1)
		pop		r24
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0037
L0038:

;-Line--0615----Set P2--
		sbi		PORTC,4

;-Line--0616----Else--
		rjmp	L0036
L0037:

;-Line--0617----Reset P2--
		cbi		PORTC,4

;-Line--0618----End If--
L0036:

;-Line--0620----If VAR(20)=1 Then--
		ldi		zl,low(20)
		ldi		zh,high(20)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(1)
		pop		r24
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0040
L0041:

;-Line--0621----Set P3--
		sbi		PORTC,3

;-Line--0622----Else--
		rjmp	L0039
L0040:

;-Line--0623----Reset P3--
		cbi		PORTC,3

;-Line--0624----End If--
L0039:

;-Line--0626----If VAR(21)=1 Then--
		ldi		zl,low(21)
		ldi		zh,high(21)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(1)
		pop		r24
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0043
L0044:

;-Line--0627----Set P4--
		sbi		PORTC,2

;-Line--0628----Else--
		rjmp	L0042
L0043:

;-Line--0629----Reset P4--
		cbi		PORTC,2

;-Line--0630----End If--
L0042:

;-Line--0633----For P=22 To 25--
		ldi		zl,low(22)
		sts		p,zl
		ldi		zl,low(25)
		push	r5
		inc		zl
		mov		r5,zl
L0045:

;-Line--0634----A=VAR(P)--
		lds		zl,p
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		a,zl

;-Line--0635----Set D1--
		sbi		PORTB,4

;-Line--0636----Timer2=0         ' ?????????????????????--
		ldi		zl,low(0)
		out		TCNT2,zl

;-Line--0637----ATTENTE:--
attente:

;-Line--0638----B=Timer2--
		in		zl,TCNT2
		sts		b,zl

;-Line--0639----If B < A Then GoTo ATTENTE--
		lds		r24,b
		lds		zl,a
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0048
L0049:
		rjmp		attente
L0048:

;-Line--0640----Reset D1--
		cbi		PORTB,4

;-Line--0641----WaitUs 5--
		ldi		zl,low(5)
		rcall	_wus

;-Line--0642----Next P--
		lds		zl,p
		inc		zl
		sts		p,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0045
		pop		r5
L0046:

;-Line--0644----If VAR(38)=1 Then         '  IRTX--
		ldi		zl,low(38)
		ldi		zh,high(38)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(1)
		pop		r24
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0051
L0052:

;-Line--0645----Set IRTX--
		sbi		PORTB,2

;-Line--0646----Else--
		rjmp	L0050
L0051:

;-Line--0647----Reset IRTX--
		cbi		PORTB,2

;-Line--0648----End If--
L0050:

;-Line--0650----If VAR(RUN)=1 Then         ' 41--
		ldi		zl,low(41)
		ldi		zh,high(41)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(1)
		pop		r24
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0054
L0055:

;-Line--0651----INTERPRETER()--
		rcall	interpreter

;-Line--0652----Else--
		rjmp	L0053
L0054:

;-Line--0653----PCO=1--
		ldi		zl,low(1)
		sts		pco,zl

;-Line--0654----End If--
L0053:

;-Line--0656----VAR(48)=PCO    ' Program Pointer--
		ldi		zl,low(48)
		ldi		zh,high(48)
		push	zh
		push	zl
		lds		zl,pco
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0658----VAR(30)=CPT0   ' Counter INT 0--
		ldi		zl,low(30)
		ldi		zh,high(30)
		push	zh
		push	zl
		lds		zl,cpt0
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0659----VAR(32)=CPT1   ' Counter INT 1--
		ldi		zl,low(32)
		ldi		zh,high(32)
		push	zh
		push	zl
		lds		zl,cpt1
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0661----VAR(34)=STATUS--
		ldi		zl,low(34)
		ldi		zh,high(34)
		push	zh
		push	zl
		lds		zl,status
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0662----VAR(35)=ERROR--
		ldi		zl,low(35)
		ldi		zh,high(35)
		push	zh
		push	zl
		lds		zl,error
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0664----VAR(45)=TEMPS  ' Timer mS--
		ldi		zl,low(45)
		ldi		zh,high(45)
		push	zh
		push	zl
		lds		zl,temps
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0666----VAR(46)=T0     ' PERIOD INT0--
		ldi		zl,low(46)
		ldi		zh,high(46)
		push	zh
		push	zl
		lds		zl,t0
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0667----VAR(47)=T1     ' PERIOD INT1--
		ldi		zl,low(47)
		ldi		zh,high(47)
		push	zh
		push	zl
		lds		zl,t1
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0669----ATTENTE2:--
attente2:

;-Line--0670----If SCAN<20 Then GoTo ATTENTE2    ' SCANTIME WAIT 20 mS pour les servos--
		lds		r24,scan
		ldi		zl,low(20)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0057
L0058:
		rjmp		attente2
L0057:

;-Line--0672----Incr PL--
		lds		zl,pl
		inc		zl
		sts		pl,zl

;-Line--0673----If PL > 20 Then--
		lds		r24,pl
		ldi		zl,low(20)
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0060
L0061:

;-Line--0674----If MODE=0 Then--
		lds		r24,mode
		ldi		zl,low(0)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0063
L0064:

;-Line--0675----Toggle LED       ' Toggle every 400 ms--
		in		zl,PORTB
		ldi		r23,0x08
		eor		zl,r23
		out		PORTB,zl

;-Line--0676----Toggle LED1      ' only for test '''''''''''''--
		in		zl,PORTB
		ldi		r23,0x01
		eor		zl,r23
		out		PORTB,zl

;-Line--0677----Else--
		rjmp	L0062
L0063:

;-Line--0678----Reset LED--
		cbi		PORTB,3

;-Line--0679----Reset LED1--
		cbi		PORTB,0

;-Line--0680----End If--
L0062:

;-Line--0681----PL=0--
		ldi		zl,low(0)
		sts		pl,zl

;-Line--0682----End If--
L0060:
L0059:

;-Line--0695----If MODE=1 And C_TIMER=0 Then--
		lds		r24,mode
		ldi		zl,low(1)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0066
		lds		r24,c_timer
		lds		r25,c_timer+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0066
L0067:

;-Line--0696----VAR(16)=0--
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0697----VAR(17)=0--
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0698----MODE=99--
		ldi		zl,low(99)
		sts		mode,zl

;-Line--0699----End If--
L0066:
L0065:

;-Line--0701----If MODE=2 Then--
		lds		r24,mode
		ldi		zl,low(2)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0069
L0070:

;-Line--0702----If C_COUNTER0=0 Then VAR(16)=0--
		lds		r24,c_counter0
		lds		r25,c_counter0+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0072
L0073:
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl
L0072:

;-Line--0703----If C_COUNTER1=0 Then VAR(17)=0--
		lds		r24,c_counter1
		lds		r25,c_counter1+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0075
L0076:
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl
L0075:

;-Line--0704----If C_COUNTER0=0 And C_COUNTER1=0 Then MODE=99--
		lds		r24,c_counter0
		lds		r25,c_counter0+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0078
		lds		r24,c_counter1
		lds		r25,c_counter1+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0078
L0079:
		ldi		zl,low(99)
		sts		mode,zl
L0078:

;-Line--0705----End If--
L0069:
L0068:

;-Line--0708----EPID=VAR(44)--
		ldi		zl,low(44)
		ldi		zh,high(44)
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		epid,zl

;-Line--0710----If MODE=2 And EPID=1 Then--
		lds		r24,mode
		ldi		zl,low(2)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0081
		lds		r24,epid
		ldi		zl,low(1)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0081
L0082:

;-Line--0711----Incr PIDL--
		lds		zl,pidl
		inc		zl
		sts		pidl,zl

;-Line--0713----If PIDL>50 Then           ' 1000 mS SCANNING PID--
		lds		r24,pidl
		ldi		zl,low(50)
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0084
L0085:

;-Line--0714----PIDL=0--
		ldi		zl,low(0)
		sts		pidl,zl

;-Line--0715----PID()--
		rcall	pid

;-Line--0716----End If--
L0084:
L0083:

;-Line--0717----End If--
L0081:
L0080:

;-Line--0719----If MODE=99 Then--
		lds		r24,mode
		ldi		zl,low(99)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0087
L0088:

;-Line--0720----MODE=0--
		ldi		zl,low(0)
		sts		mode,zl

;-Line--0721----End If--
L0087:
L0086:

;-Line--0723----Loop--
		rjmp	L0011
L0012:

;-Line--0725----End--
L0089:
		rjmp	L0089


;-Line--0728----Interrupt Ovf0(), Save All      ' ALL ??????????????????--

;-Line--0729----Timer0 = &h83                   ' RELOAD 1 mS--
IntN9:
		in		r2,SREG
		push	zl
		push	zh
		push	r0
		push	r1
		push	r2
		push	r24
		push	r25
		push	xl
		push	xh
		push	r19
		push	r20
		push	r21
		push	r22
		push	r23
		ldi		zl,low(131)
		out		TCNT0,zl

;-Line--0730----Incr COUNT--
		lds		zl,count
		inc		zl
		sts		count,zl

;-Line--0731----If COUNT>99 Then--
		lds		r24,count
		ldi		zl,low(99)
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0091
L0092:

;-Line--0732----    COUNT=0--
		ldi		zl,low(0)
		sts		count,zl

;-Line--0733---- 	Incr TEMPS             ' every 100 MS--
		lds		zl,temps
		inc		zl
		sts		temps,zl

;-Line--0734---- 	Incr WD0--
		lds		zl,wd0
		lds		zh,wd0+1
		adiw	zl,1
		sts		wd0,zl
		sts		wd0+1,zh

;-Line--0735---- 	Incr WD1--
		lds		zl,wd1
		lds		zh,wd1+1
		adiw	zl,1
		sts		wd1,zl
		sts		wd1+1,zh

;-Line--0736---- 	For PT=26 To 29        ' GESTION TIMERS  100 mS--
		ldi		zl,low(26)
		sts		pt,zl
		ldi		zl,low(29)
		push	r5
		inc		zl
		mov		r5,zl
L0093:

;-Line--0737----If VAR(PT)>0 Then--
		lds		zl,pt
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(0)
		inc		zl
		pop		r24
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0096
L0097:

;-Line--0738----Decr VAR(PT)--
		lds		zl,pt
		push	zh
		push	zl
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		dec		zl
		st		X,zl

;-Line--0739----End If--
L0096:
L0095:

;-Line--0740----Next--
		lds		zl,pt
		inc		zl
		sts		pt,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0093
		pop		r5
L0094:

;-Line--0741----End If--
L0091:
L0090:

;-Line--0743----Incr SCAN--
		lds		zl,scan
		inc		zl
		sts		scan,zl

;-Line--0745----If C_TIMER>0 Then--
		lds		r24,c_timer
		lds		r25,c_timer+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		adiw	zl,1
		cp		r24,zl
		cpc		r25,zh
		brge	PC+0x02
		rjmp	L0099
L0100:

;-Line--0746----Decr C_TIMER--
		lds		zl,c_timer
		lds		zh,c_timer+1
		sbiw	zl,1
		sts		c_timer,zl
		sts		c_timer+1,zh

;-Line--0747----End If--
L0099:
L0098:

;-Line--0749----End Interrupt--
		pop		r23
		pop		r22
		pop		r21
		pop		r20
		pop		r19
		pop		xh
		pop		xl
		pop		r25
		pop		r24
		pop		r2
		pop		r1
		pop		r0
		pop		zh
		pop		zl
		out		SREG,r2
		reti

;-Line--0752----Interrupt Urxc(), Save All--

;-Line--0753----If MF=0 Then                    ' PAS REENTRANT--
IntN11:
		in		r2,SREG
		push	zl
		push	zh
		push	r0
		push	r1
		push	r2
		push	r24
		push	r25
		push	xl
		push	xh
		push	r19
		push	r20
		push	r21
		push	r22
		push	r23
		sbrc	r3,0
		rjmp	L0102
L0103:

;-Line--0754----InputBin CMD                ' STX CMD DATA ETX--
		rcall	_GetCh
		sts		cmd,zl

;-Line--0755----If CMD>96 And CMD<123 Then  ' CONVERT TO UPPERCASE--
		lds		r24,cmd
		ldi		zl,low(96)
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0105
		lds		r24,cmd
		ldi		zl,low(123)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0105
L0106:

;-Line--0756----CMD=CMD-32--
		lds		r24,cmd
		ldi		zl,low(32)
		sub		r24,zl
		mov		zl,r24
		sts		cmd,zl

;-Line--0757----End If--
L0105:
L0104:

;-Line--0758----RX(PRX)=CMD--
		lds		zl,prx
		clr		zh
		push	zh
		push	zl
		lds		zl,cmd
		ldi		xl,low(rx)
		ldi		xh,high(rx)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0759----If PRX < 31 Then--
		lds		r24,prx
		ldi		zl,low(31)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0108
L0109:

;-Line--0760----Incr PRX--
		lds		zl,prx
		inc		zl
		sts		prx,zl

;-Line--0761----Else--
		rjmp	L0107
L0108:

;-Line--0762----ERROR=100--
		ldi		zl,low(100)
		sts		error,zl

;-Line--0763----PRX=0--
		ldi		zl,low(0)
		sts		prx,zl

;-Line--0764----End If--
L0107:

;-Line--0766----Select Case CMD--
		lds		zl,cmd
		mov		r24,zl

;-Line--0767----Case STX         		'  "S" START MESSAGE--
		ldi		zl,low(83)
		cp		r24,zl
		brne	L0111

;-Line--0768----PRX=1--
		ldi		zl,low(1)
		sts		prx,zl

;-Line--0770----Case ETX         		' END MESSAGE  "CR"--
		rjmp	L0110
L0111:
		ldi		zl,low(13)
		cp		r24,zl
		brne	L0112

;-Line--0771----If PRX > 2 Then--
		lds		r24,prx
		ldi		zl,low(2)
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0114
L0115:

;-Line--0772----MF = 1          ' MESSAGE READY TO BE EXECUTED--
		set
		bld		r3,0

;-Line--0773----Else--
		rjmp	L0113
L0114:

;-Line--0774----PRX=0--
		ldi		zl,low(0)
		sts		prx,zl

;-Line--0775----End If--
L0113:

;-Line--0777----Case SPACE              ' DO NOT CARE--
		rjmp	L0110
L0112:
		ldi		zl,low(32)
		cp		r24,zl
		brne	L0116

;-Line--0778----    Decr PRX	--
		lds		zl,prx
		dec		zl
		sts		prx,zl

;-Line--0780----Case Else--
		rjmp	L0110
L0116:

;-Line--0781----If PRX=1 Then--
		lds		r24,prx
		ldi		zl,low(1)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0119
L0120:

;-Line--0782----PRX=0--
		ldi		zl,low(0)
		sts		prx,zl

;-Line--0783----End If--
L0119:
L0118:

;-Line--0784----End Select--
L0110:

;-Line--0785----End If--
L0102:
L0101:

;-Line--0786----End Interrupt--
		pop		r23
		pop		r22
		pop		r21
		pop		r20
		pop		r19
		pop		xh
		pop		xl
		pop		r25
		pop		r24
		pop		r2
		pop		r1
		pop		r0
		pop		zh
		pop		zl
		out		SREG,r2
		reti

;-Line--0790----Interrupt Int0(), Save All--

;-Line--0791----Incr CPT0--
IntN1:
		in		r2,SREG
		push	zl
		push	zh
		push	r0
		push	r1
		push	r2
		push	r24
		push	r25
		push	xl
		push	xh
		push	r19
		push	r20
		push	r21
		push	r22
		push	r23
		lds		zl,cpt0
		inc		zl
		sts		cpt0,zl

;-Line--0796----T0=WD0--
		lds		zl,wd0
		sts		t0,zl

;-Line--0797----WD0=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		wd0,zl
		sts		wd0+1,zh

;-Line--0799----If C_COUNTER0 > 0 Then--
		lds		r24,c_counter0
		lds		r25,c_counter0+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		adiw	zl,1
		cp		r24,zl
		cpc		r25,zh
		brge	PC+0x02
		rjmp	L0122
L0123:

;-Line--0800----Decr C_Counter0--
		lds		zl,c_counter0
		lds		zh,c_counter0+1
		sbiw	zl,1
		sts		c_counter0,zl
		sts		c_counter0+1,zh

;-Line--0801----End If--
L0122:
L0121:

;-Line--0803----End Interrupt--
		pop		r23
		pop		r22
		pop		r21
		pop		r20
		pop		r19
		pop		xh
		pop		xl
		pop		r25
		pop		r24
		pop		r2
		pop		r1
		pop		r0
		pop		zh
		pop		zl
		out		SREG,r2
		reti

;-Line--0807----Interrupt Int1(), Save All--

;-Line--0808----Incr CPT1--
IntN2:
		in		r2,SREG
		push	zl
		push	zh
		push	r0
		push	r1
		push	r2
		push	r24
		push	r25
		push	xl
		push	xh
		push	r19
		push	r20
		push	r21
		push	r22
		push	r23
		lds		zl,cpt1
		inc		zl
		sts		cpt1,zl

;-Line--0813----T1=WD1--
		lds		zl,wd1
		sts		t1,zl

;-Line--0814----WD1=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		wd1,zl
		sts		wd1+1,zh

;-Line--0815----If C_COUNTER1>0 Then--
		lds		r24,c_counter1
		lds		r25,c_counter1+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		adiw	zl,1
		cp		r24,zl
		cpc		r25,zh
		brge	PC+0x02
		rjmp	L0125
L0126:

;-Line--0816----Decr C_Counter1--
		lds		zl,c_counter1
		lds		zh,c_counter1+1
		sbiw	zl,1
		sts		c_counter1,zl
		sts		c_counter1+1,zh

;-Line--0817----End If--
L0125:
L0124:

;-Line--0819----End Interrupt--
		pop		r23
		pop		r22
		pop		r21
		pop		r20
		pop		r19
		pop		xh
		pop		xl
		pop		r25
		pop		r24
		pop		r2
		pop		r1
		pop		r0
		pop		zh
		pop		zl
		out		SREG,r2
		reti

;-Line--0822----Sub BREAK()--
break:

;-Line--0825----For P=22 To 25       ' RESET SERVO 1500 µS--
		sbiw	yl,1
		ldi		zl,low(22)
		std		y+0,zl
		ldi		zl,low(25)
		push	r5
		inc		zl
		mov		r5,zl
L0128:

;-Line--0826----VAR(P)=200--
		ldd		zl,y+0
		clr		zh
		push	zh
		push	zl
		ldi		zl,low(200)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0827----Next--
		ldd		zl,y+0
		inc		zl
		std		y+0,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0128
		pop		r5
L0129:

;-Line--0829----VAR(16)=0             ' RESET SPEED--
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0830----VAR(17)=0--
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0832----For P=18 To 21        ' RESET PX--
		ldi		zl,low(18)
		std		y+0,zl
		ldi		zl,low(21)
		push	r5
		inc		zl
		mov		r5,zl
L0130:

;-Line--0833----VAR(P)=0--
		ldd		zl,y+0
		clr		zh
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0834----Next--
		ldd		zl,y+0
		inc		zl
		std		y+0,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0130
		pop		r5
L0131:

;-Line--0836----End Sub--
L0127:
		adiw	yl,1
		ret

;-Line--0863----Sub COMMUNICATION()--
communication:

;-Line--0867----CMD = RX(1)--
		sbiw	yl,2
		ldi		zl,low(1)
		ldi		zh,high(1)
		push	zl
		push	zh
		ldi		xl,low(rx)
		ldi		xh,high(rx)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		cmd,zl

;-Line--0868----PLE = 2                    ' READ POINTER--
		ldi		zl,low(2)
		sts		ple,zl

;-Line--0869----Select Case CMD--
		lds		zl,cmd
		mov		r24,zl

;-Line--0871----Case &H52                    'Asc("R") R RESET             SR(CR) OK--
		ldi		zl,low(82)
		cp		r24,zl
		brne	L0134

;-Line--0872---- If PRX=3 Then--
		lds		r24,prx
		ldi		zl,low(3)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0136
L0137:

;-Line--0873----BREAK()--
		rcall	break

;-Line--0875----ACKNO()--
		rcall	ackno

;-Line--0876----Else--
		rjmp	L0135
L0136:

;-Line--0877----NOACK()--
		rcall	noack

;-Line--0878----End If--
L0135:

;-Line--0880----Case &H41                   'ASC("A") A  READ VAR           SAXX(CR)   XX HEX OK--
		rjmp	L0133
L0134:
		ldi		zl,low(65)
		cp		r24,zl
		brne	L0138

;-Line--0881---- If PRX=5 Then--
		lds		r24,prx
		ldi		zl,low(5)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0140
L0141:

;-Line--0882----A=READHEX()--
		rcall	readhex
		std		y+1,zl

;-Line--0884----WRITEHEX(VAR(A))--
		ldd		zl,y+1
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		st		-Y,zl
		rcall	writehex
		adiw	yl,1

;-Line--0885----Else--
		rjmp	L0139
L0140:

;-Line--0886----NOACK()--
		rcall	noack

;-Line--0887----End If--
L0139:

;-Line--0889----Case &H42                   'ASC("B") B  READ PGM         SBXX(CR) OK--
		rjmp	L0133
L0138:
		ldi		zl,low(66)
		cp		r24,zl
		brne	L0142

;-Line--0890---- If PRX=5 Then--
		lds		r24,prx
		ldi		zl,low(5)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0144
L0145:

;-Line--0891----A=READHEX()--
		rcall	readhex
		std		y+1,zl

;-Line--0893----WRITEHEX(PGM(A))--
		ldd		zl,y+1
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		st		-Y,zl
		rcall	writehex
		adiw	yl,1

;-Line--0894----Else--
		rjmp	L0143
L0144:

;-Line--0895----NOACK()--
		rcall	noack

;-Line--0896----End If--
L0143:

;-Line--0898----Case &H43                   'ASC("C") C  READ EE     SCXX(CR) OK--
		rjmp	L0133
L0142:
		ldi		zl,low(67)
		cp		r24,zl
		brne	L0146

;-Line--0899---- If PRX=5 Then--
		lds		r24,prx
		ldi		zl,low(5)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0148
L0149:

;-Line--0900----A=READHEX()--
		rcall	readhex
		std		y+1,zl

;-Line--0902----WRITEHEX(ReadEE(A))--
		ldd		zl,y+1
		clr		zh
		mov		r24,zl
		mov		r25,zh
		rcall	_RdEE
		st		-Y,zl
		rcall	writehex
		adiw	yl,1

;-Line--0903----Else--
		rjmp	L0147
L0148:

;-Line--0904----NOACK()--
		rcall	noack

;-Line--0905----End If--
L0147:

;-Line--0908----Case &H56                   'Asc("V")  WRITE VAR    SVXXYY(CR) OK--
		rjmp	L0133
L0146:
		ldi		zl,low(86)
		cp		r24,zl
		brne	L0150

;-Line--0909---- If PRX=7 Then--
		lds		r24,prx
		ldi		zl,low(7)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0152
L0153:

;-Line--0910----A=READHEX()--
		rcall	readhex
		std		y+1,zl

;-Line--0911----B=READHEX()--
		rcall	readhex
		std		y+0,zl

;-Line--0912----If A<128 Then--
		ldd		r24,y+1
		ldi		zl,low(128)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0155
L0156:

;-Line--0913----VAR(A)=B--
		ldd		zl,y+1
		clr		zh
		push	zh
		push	zl
		ldd		zl,y+0
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0915----Else--
		rjmp	L0154
L0155:

;-Line--0916----NOACK()--
		rcall	noack

;-Line--0917----End If--
L0154:

;-Line--0918----Else--
		rjmp	L0151
L0152:

;-Line--0919----NOACK()--
		rcall	noack

;-Line--0920----End If--
L0151:

;-Line--0922----Case &H57                   'Asc("W")  SWXXYY(CR)  WRITE PGM  OK--
		rjmp	L0133
L0150:
		ldi		zl,low(87)
		cp		r24,zl
		brne	L0157

;-Line--0923---- If PRX=7 Then--
		lds		r24,prx
		ldi		zl,low(7)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0159
L0160:

;-Line--0924----A=READHEX()--
		rcall	readhex
		std		y+1,zl

;-Line--0925----B=READHEX()--
		rcall	readhex
		std		y+0,zl

;-Line--0926----If A<64 Then				--
		ldd		r24,y+1
		ldi		zl,low(64)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0162
L0163:

;-Line--0927----PGM(A)=B--
		ldd		zl,y+1
		clr		zh
		push	zh
		push	zl
		ldd		zl,y+0
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0929----Else--
		rjmp	L0161
L0162:

;-Line--0930----NOACK()--
		rcall	noack

;-Line--0931----End If--
L0161:

;-Line--0932----Else--
		rjmp	L0158
L0159:

;-Line--0933----NOACK()--
		rcall	noack

;-Line--0934----End If--
L0158:

;-Line--0936----Case &H58                'Asc("X")  SCXXYY(CR)  WRITE EE --PAGE0-- OK--
		rjmp	L0133
L0157:
		ldi		zl,low(88)
		cp		r24,zl
		brne	L0164

;-Line--0937---- If PRX=7 Then--
		lds		r24,prx
		ldi		zl,low(7)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0166
L0167:

;-Line--0938----  A=READHEX()--
		rcall	readhex
		std		y+1,zl

;-Line--0939----  B=READHEX()--
		rcall	readhex
		std		y+0,zl

;-Line--0940----   If B<>ReadEE(A) Then--
		ldd		zl,y+0
		push	zl
		ldd		zl,y+1
		clr		zh
		mov		r24,zl
		mov		r25,zh
		rcall	_RdEE
		pop		r24
		cp		r24,zl
		brne	PC+0x02
		rjmp	L0169
L0170:

;-Line--0941----    WriteEE(A,B)--
		ldd		zl,y+1
		clr		zh
		mov		r24,zl
		mov		r25,zh
		ldd		zl,y+0
		rcall	_WrtEE

;-Line--0944----   End If--
L0169:
L0168:

;-Line--0945----   ACKNO()--
		rcall	ackno

;-Line--0946---- Else--
		rjmp	L0165
L0166:

;-Line--0947----  NOACK()--
		rcall	noack

;-Line--0948---- End If--
L0165:

;-Line--0950----Case &H4C                'Asc("L")  SLXX(CR)  WRITE LAN ADDRESS     OK--
		rjmp	L0133
L0164:
		ldi		zl,low(76)
		cp		r24,zl
		brne	L0171

;-Line--0951---- If PRX=5 Then--
		lds		r24,prx
		ldi		zl,low(5)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0173
L0174:

;-Line--0952----  A=READHEX()--
		rcall	readhex
		std		y+1,zl

;-Line--0953----   If A<>ReadEE(1) Then--
		ldd		zl,y+1
		push	zl
		ldi		zl,low(1)
		ldi		zh,high(1)
		mov		r24,zl
		mov		r25,zh
		rcall	_RdEE
		pop		r24
		cp		r24,zl
		brne	PC+0x02
		rjmp	L0176
L0177:

;-Line--0954----    WriteEE(1,A)--
		ldi		zl,low(1)
		ldi		zh,high(1)
		mov		r24,zl
		mov		r25,zh
		ldd		zl,y+1
		rcall	_WrtEE

;-Line--0956----ACKNO()--
		rcall	ackno

;-Line--0957----   End If--
L0176:
L0175:

;-Line--0958---- Else--
		rjmp	L0172
L0173:

;-Line--0959----  NOACK()--
		rcall	noack

;-Line--0960---- End If--
L0172:

;-Line--0962----Case &H54            'Asc("T")    PRINT TEST     ST(CR) OK--
		rjmp	L0133
L0171:
		ldi		zl,low(84)
		cp		r24,zl
		brne	L0178

;-Line--0963----    If PRX=3 Then--
		lds		r24,prx
		ldi		zl,low(3)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0180
L0181:

;-Line--0964----PTEST()--
		rcall	ptest

;-Line--0965----Else--
		rjmp	L0179
L0180:

;-Line--0966----NOACK()--
		rcall	noack

;-Line--0967----End If--
L0179:

;-Line--0969----Case &H4D           ' Asc("M") XXYY ZZ Execute une Commande ZZ and SET Param XXYY    OK--
		rjmp	L0133
L0178:
		ldi		zl,low(77)
		cp		r24,zl
		brne	L0182

;-Line--0970----If PRX=9 Then--
		lds		r24,prx
		ldi		zl,low(9)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0184
L0185:

;-Line--0971----PARAMETRE()--
		rcall	parametre

;-Line--0972----PGM(1)=READHEX()--
		ldi		zl,low(1)
		ldi		zh,high(1)
		push	zh
		push	zl
		rcall	readhex
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0974----EXECUTE(1)--
		ldi		zl,low(1)
		st		-Y,zl
		rcall	execute
		adiw	yl,1

;-Line--0975----Else--
		rjmp	L0183
L0184:

;-Line--0976----NOACK()--
		rcall	noack

;-Line--0977----End If--
L0183:

;-Line--0979----Case &h4E             ' Asc("N") RUN PROGRAM     SN(CR)     OK--
		rjmp	L0133
L0182:
		ldi		zl,low(78)
		cp		r24,zl
		brne	L0186

;-Line--0980----If PRX=3 Then--
		lds		r24,prx
		ldi		zl,low(3)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0188
L0189:

;-Line--0981----ERROR=0--
		ldi		zl,low(0)
		sts		error,zl

;-Line--0982----VAR(RUN)=1--
		ldi		zl,low(41)
		ldi		zh,high(41)
		push	zh
		push	zl
		ldi		zl,low(1)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--0984----Else--
		rjmp	L0187
L0188:

;-Line--0985----NOACK()--
		rcall	noack

;-Line--0986----End If--
L0187:

;-Line--0988----Case &h4F             ' ASC("O") SET MODE OK ??????????????--
		rjmp	L0133
L0186:
		ldi		zl,low(79)
		cp		r24,zl
		brne	L0190

;-Line--0989----If PRX=5 Then--
		lds		r24,prx
		ldi		zl,low(5)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0192
L0193:

;-Line--0990----C_TIMER=PARAM--
		lds		zl,param
		lds		zh,param+1
		sts		c_timer,zl
		sts		c_timer+1,zh

;-Line--0991----C_COUNTER0=PARAM--
		lds		zl,param
		lds		zh,param+1
		sts		c_counter0,zl
		sts		c_counter0+1,zh

;-Line--0992----C_COUNTER1=PARAM--
		lds		zl,param
		lds		zh,param+1
		sts		c_counter1,zl
		sts		c_counter1+1,zh

;-Line--0993----MODE=READHEX()--
		rcall	readhex
		sts		mode,zl

;-Line--0994----ACKNO()--
		rcall	ackno

;-Line--0995----Else--
		rjmp	L0191
L0192:

;-Line--0996----NOACK()--
		rcall	noack

;-Line--0997----End If--
L0191:

;-Line--0999----Case &H50           ' Asc("P") XX... YYZZ Execute 8 Command XX and SET Param YYZZ    OK--
		rjmp	L0133
L0190:
		ldi		zl,low(80)
		cp		r24,zl
		brne	L0194

;-Line--1000----If PRX=23 Then--
		lds		r24,prx
		ldi		zl,low(23)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0196
L0197:

;-Line--1001----PARAMETRE()--
		rcall	parametre

;-Line--1002----For P=1 To 8--
		ldi		zl,low(1)
		sts		p,zl
		ldi		zl,low(8)
		push	r5
		inc		zl
		mov		r5,zl
L0198:

;-Line--1003----PGM(P)=READHEX()--
		lds		zl,p
		clr		zh
		push	zh
		push	zl
		rcall	readhex
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1004----Next--
		lds		zl,p
		inc		zl
		sts		p,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0198
		pop		r5
L0199:

;-Line--1006----EXECUTE(8)--
		ldi		zl,low(8)
		st		-Y,zl
		rcall	execute
		adiw	yl,1

;-Line--1007----Else--
		rjmp	L0195
L0196:

;-Line--1008----NOACK()--
		rcall	noack

;-Line--1009----End If--
L0195:

;-Line--1012----End Select--
L0194:
L0133:

;-Line--1014----CMD=0--
		ldi		zl,low(0)
		sts		cmd,zl

;-Line--1015----PRX=0--
		ldi		zl,low(0)
		sts		prx,zl

;-Line--1016----MF=0                  ' ENABLE RX Doit etre en dernier--
		clt
		bld		r3,0

;-Line--1017----End Sub--
L0132:
		adiw	yl,2
		ret

;-Line--1021----Sub WRITE2HEX( X As Integer )--
write2hex:

;-Line--1030----End Sub--
L0201:
		ret

;-Line--1035----Sub WRITEHEX( X As Byte )   ' TESTED OK--
writehex:

;-Line--1039----LA = X mod 16    		' LOW CAR--
		sbiw	yl,2
		ldd		zl,y+2
		push	zl
		ldi		zl,low(16)
		pop		r24
		rcall	Div8u
		mov		zl,r22
		std		y+1,zl

;-Line--1040----LB = (X-LA)/16    		' HIGH CAR--
		ldd		zl,y+2
		push	zl
		ldd		zl,y+1
		pop		r24
		sub		r24,zl
		mov		zl,r24
		push	zl
		ldi		zl,low(16)
		pop		r24
		rcall	Div8u
		std		y+0,zl

;-Line--1041----If LA < 10 Then--
		ldd		r24,y+1
		ldi		zl,low(10)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0204
L0205:

;-Line--1042----LA = LA + 48   		' this is zero offset--
		ldd		r24,y+1
		ldi		zl,low(48)
		add		zl,r24
		std		y+1,zl

;-Line--1043----Else--
		rjmp	L0203
L0204:

;-Line--1044----LA = LA + 55    	' A here already have offset 10--
		ldd		r24,y+1
		ldi		zl,low(55)
		add		zl,r24
		std		y+1,zl

;-Line--1045----End If			    	' 55=65-10--
L0203:

;-Line--1046----If LB < 10 Then--
		ldd		r24,y+0
		ldi		zl,low(10)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0207
L0208:

;-Line--1047----LB = LB + 48--
		ldd		r24,y+0
		ldi		zl,low(48)
		add		zl,r24
		std		y+0,zl

;-Line--1048----Else--
		rjmp	L0206
L0207:

;-Line--1049----LB = LB + 55--
		ldd		r24,y+0
		ldi		zl,low(55)
		add		zl,r24
		std		y+0,zl

;-Line--1050----End If--
L0206:

;-Line--1051----PrintBin LB ; LA;--
		ldd		zl,y+0
		mov		r24,zl
		rcall	_Pch
		ldd		zl,y+1
		mov		r24,zl
		rcall	_Pch

;-Line--1052----End Sub--
L0202:
		adiw	yl,2
		ret

;-Line--1054----Sub ACKNO()   ' OK--
ackno:

;-Line--1055----PrintBin CACK ; CACK--
		ldi		zl,low(62)
		mov		r24,zl
		rcall	_Pch
		ldi		zl,low(62)
		mov		r24,zl
		rcall	_Pch

;-Line--1056----End Sub--
L0209:
		ret

;-Line--1059----Sub NOACK()   ' KO--
noack:

;-Line--1060----PrintBin CNACK ; CNACK--
		ldi		zl,low(63)
		mov		r24,zl
		rcall	_Pch
		ldi		zl,low(63)
		mov		r24,zl
		rcall	_Pch

;-Line--1061----End Sub--
L0210:
		ret

;-Line--1063----Sub SPEED()--
speed:

;-Line--1070----If SPEED0=0 Then        ' STOP RESET WACHDOG--
		lds		r24,speed0
		ldi		zl,low(0)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0213
L0214:

;-Line--1071----WD0=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		wd0,zl
		sts		wd0+1,zh

;-Line--1072----End If--
L0213:
L0212:

;-Line--1074----If SPEED1=0 Then--
		lds		r24,speed1
		ldi		zl,low(0)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0216
L0217:

;-Line--1075----WD1=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		wd1,zl
		sts		wd1+1,zh

;-Line--1076----End If--
L0216:
L0215:

;-Line--1078----If DIR0=1 Then            ' SET DIR I/O--
		lds		r24,dir0
		ldi		zl,low(1)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0219
L0220:

;-Line--1079----Set PDIR0--
		sbi		PORTC,6

;-Line--1080----Else--
		rjmp	L0218
L0219:

;-Line--1081----Reset PDIR0--
		cbi		PORTC,6

;-Line--1082----End If--
L0218:

;-Line--1083----Pwm1A=SPEED0--
		lds		zl,speed0
		out		OCR1AL,zl

;-Line--1085----If DIR1=1 Then--
		lds		r24,dir1
		ldi		zl,low(1)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0222
L0223:

;-Line--1086----Set PDIR1--
		sbi		PORTC,7

;-Line--1087----Else--
		rjmp	L0221
L0222:

;-Line--1088----Reset PDIR1--
		cbi		PORTC,7

;-Line--1089----End If--
L0221:

;-Line--1090----Pwm1B=SPEED1--
		lds		zl,speed1
		out		OCR1BL,zl

;-Line--1092----End Sub--
L0211:
		ret

;-Line--1094----Sub PTEST()--
ptest:

;-Line--1097----Print--
		sbiw	yl,1
		rcall	_PCL

;-Line--1098----   For P=0 To 64--
		ldi		zl,low(0)
		std		y+0,zl
		ldi		zl,low(64)
		push	r5
		inc		zl
		mov		r5,zl
L0225:

;-Line--1099----   If P=8 Then Print--
		ldd		r24,y+0
		ldi		zl,low(8)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0228
L0229:
		rcall	_PCL
L0228:

;-Line--1100----   If P=16 Then Print--
		ldd		r24,y+0
		ldi		zl,low(16)
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0231
L0232:
		rcall	_PCL
L0231:

;-Line--1101----   	Print P;--
		ldd		zl,y+0
		rcall	_B2Str
		rcall	_PrBW

;-Line--1102----   	Print "= ";--
		ldi		zl,low(S04*2)
		ldi		zh,high(S04*2)
		rcall	_PSc

;-Line--1103----   	Print VAR(P)--
		ldd		zl,y+0
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		rcall	_B2Str
		rcall	_PrBW
		rcall	_PCL

;-Line--1104----   Next--
		ldd		zl,y+0
		inc		zl
		std		y+0,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0225
		pop		r5
L0226:

;-Line--1105----   Print--
		rcall	_PCL

;-Line--1106----   Print "TIME ";TEMPS--
		ldi		zl,low(S05*2)
		ldi		zh,high(S05*2)
		rcall	_PSc
		lds		zl,temps
		rcall	_B2Str
		rcall	_PrBW
		rcall	_PCL

;-Line--1107----   Print--
		rcall	_PCL

;-Line--1108----End Sub--
L0224:
		adiw	yl,1
		ret

;-Line--1110----Sub DEAD()--
dead:

;-Line--1113----If WD0 > WD Then     ' WATCHDOG--
		sbiw	yl,2
		lds		r24,wd0
		lds		r25,wd0+1
		lds		zl,wd
		clr		zh
		adiw	zl,1
		cp		r24,zl
		cpc		r25,zh
		brsh	PC+0x02
		rjmp	L0235
L0236:

;-Line--1114----SPEED0=0--
		ldi		zl,low(0)
		sts		speed0,zl

;-Line--1115----SPEED1=0--
		ldi		zl,low(0)
		sts		speed1,zl

;-Line--1116----ERROR=1--
		ldi		zl,low(1)
		sts		error,zl

;-Line--1117----MODE=99--
		ldi		zl,low(99)
		sts		mode,zl

;-Line--1118----Print "WD0 "--
		ldi		zl,low(S06*2)
		ldi		zh,high(S06*2)
		rcall	_PSc
		rcall	_PCL

;-Line--1119----End If--
L0235:
L0234:

;-Line--1121----If WD1 > WD Then--
		lds		r24,wd1
		lds		r25,wd1+1
		lds		zl,wd
		clr		zh
		adiw	zl,1
		cp		r24,zl
		cpc		r25,zh
		brsh	PC+0x02
		rjmp	L0238
L0239:

;-Line--1122----SPEED1=0--
		ldi		zl,low(0)
		sts		speed1,zl

;-Line--1123----SPEED0=0--
		ldi		zl,low(0)
		sts		speed0,zl

;-Line--1124----ERROR=2--
		ldi		zl,low(2)
		sts		error,zl

;-Line--1125----MODE=99--
		ldi		zl,low(99)
		sts		mode,zl

;-Line--1126----Print "WD1 "--
		ldi		zl,low(S07*2)
		ldi		zh,high(S07*2)
		rcall	_PSc
		rcall	_PCL

;-Line--1127----End If--
L0238:
L0237:

;-Line--1129----End Sub--
L0233:
		adiw	yl,2
		ret

;-Line--1132----Function READ2HEX() As Integer--
read2hex:

;-Line--1140----End Function--
L0240:
		ret


;-Line--1144----Function READHEX() As Byte  ' TESTED OK--
readhex:

;-Line--1148----LA = RX(PLE)--
		sbiw	yl,2
		lds		zl,ple
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(rx)
		ldi		xh,high(rx)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		std		y+1,zl

;-Line--1149----If LA > 57 Then--
		ldd		r24,y+1
		ldi		zl,low(57)
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0243
L0244:

;-Line--1150---- 	LA = LA - 55	' A here already have offset 10--
		ldd		r24,y+1
		ldi		zl,low(55)
		sub		r24,zl
		mov		zl,r24
		std		y+1,zl

;-Line--1151----Else				' it is ASCII all the time--
		rjmp	L0242
L0243:

;-Line--1152----LA = LA - 48--
		ldd		r24,y+1
		ldi		zl,low(48)
		sub		r24,zl
		mov		zl,r24
		std		y+1,zl

;-Line--1153----End If--
L0242:

;-Line--1155----Incr PLE--
		lds		zl,ple
		inc		zl
		sts		ple,zl

;-Line--1157----LB = RX(PLE)--
		lds		zl,ple
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(rx)
		ldi		xh,high(rx)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		std		y+0,zl

;-Line--1158----If LB > 57 Then--
		ldd		r24,y+0
		ldi		zl,low(57)
		inc		zl
		cp		r24,zl
		brsh	PC+0x02
		rjmp	L0246
L0247:

;-Line--1159----LB = LB - 55--
		ldd		r24,y+0
		ldi		zl,low(55)
		sub		r24,zl
		mov		zl,r24
		std		y+0,zl

;-Line--1160----Else--
		rjmp	L0245
L0246:

;-Line--1161----LB = LB - 48--
		ldd		r24,y+0
		ldi		zl,low(48)
		sub		r24,zl
		mov		zl,r24
		std		y+0,zl

;-Line--1162----End If--
L0245:

;-Line--1163----Incr PLE--
		lds		zl,ple
		inc		zl
		sts		ple,zl

;-Line--1165----Return ( LA * 16 + LB )--
		ldd		zl,y+1
		push	zl
		ldi		zl,low(16)
		pop		r24
		rcall	Mpy8u
		push	zl
		ldd		zl,y+0
		pop		r24
		add		zl,r24

;-Line--1166----End Function--
L0241:
		adiw	yl,2
		ret


;-Line--1170----Sub INTERPRETER()--
interpreter:

;-Line--1174----TOKEN=PGM(PCO)--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		token,zl

;-Line--1175----Print "TOKEN=";Hex(TOKEN)--
		ldi		zl,low(S08*2)
		ldi		zh,high(S08*2)
		rcall	_PSc
		lds		zl,token
		rcall	_Phb
		rcall	_PCL

;-Line--1177----Incr PCO--
		lds		zl,pco
		inc		zl
		sts		pco,zl

;-Line--1179----If TOKEN < 30 Then                   ' L0 ?????--
		lds		r24,token
		ldi		zl,low(30)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0250
L0251:

;-Line--1180----On token GoTo L0,L1,L2,L3,L4,L5,L6,L7,L8,L9,L10,L11,L12,L13,L14,L15,L16,L17,L18,L19,L20,L21,L22,L23,L24,L25,L26,L27,L28,L29--
		lds		zl,token
		clr		zh
		ldi		r24,low(L0252)
		ldi		r25,high(L0252)
		add		zl,r24
		adc		zh,r25
		ijmp
L0252:
		rjmp	l0
		rjmp	l1
		rjmp	l2
		rjmp	l3
		rjmp	l4
		rjmp	l5
		rjmp	l6
		rjmp	l7
		rjmp	l8
		rjmp	l9
		rjmp	l10
		rjmp	l11
		rjmp	l12
		rjmp	l13
		rjmp	l14
		rjmp	l15
		rjmp	l16
		rjmp	l17
		rjmp	l18
		rjmp	l19
		rjmp	l20
		rjmp	l21
		rjmp	l22
		rjmp	l23
		rjmp	l24
		rjmp	l25
		rjmp	l26
		rjmp	l27
		rjmp	l28
		rjmp	l29

;-Line--1181----Else--
		rjmp	L0249
L0250:

;-Line--1182----TOKEN2=TOKEN-30--
		lds		r24,token
		ldi		zl,low(30)
		sub		r24,zl
		mov		zl,r24
		sts		token2,zl

;-Line--1183----On TOKEN2 GoTo L30,L31,L32,L33,L34,L35,L36,L37,L38,L39,L40--
		lds		zl,token2
		clr		zh
		ldi		r24,low(L0254)
		ldi		r25,high(L0254)
		add		zl,r24
		adc		zh,r25
		ijmp
L0254:
		rjmp	l30
		rjmp	l31
		rjmp	l32
		rjmp	l33
		rjmp	l34
		rjmp	l35
		rjmp	l36
		rjmp	l37
		rjmp	l38
		rjmp	l39
		rjmp	l40

;-Line--1184----End If--
L0249:

;-Line--1186----back2:--
back2:

;-Line--1187----Incr PCO--
		lds		zl,pco
		inc		zl
		sts		pco,zl

;-Line--1188----back1:--
back1:

;-Line--1189----Incr PCO--
		lds		zl,pco
		inc		zl
		sts		pco,zl

;-Line--1190----back:--
back:

;-Line--1192----End Sub--
L0248:
		ret

;-Line--1195----L0:--
l0:

;-Line--1197----GoTo back--
		rjmp		back

;-Line--1199----L80:--
l80:

;-Line--1201----If TEST=1 Then--
		lds		r24,test
		lds		r25,test+1
		ldi		zl,low(1)
		ldi		zh,high(1)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0257
L0258:

;-Line--1202----RADDRESS=PCO+1--
		lds		r24,pco
		ldi		zl,low(1)
		add		zl,r24
		sts		raddress,zl

;-Line--1203----PCO=PGM(PCO)--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		pco,zl

;-Line--1204----End If--
L0257:
L0256:

;-Line--1205----GoTo back1--
		rjmp		back1

;-Line--1207----L1:--
l1:

;-Line--1209----PCO=PGM(PCO)--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		pco,zl

;-Line--1210----GoTo back1--
		rjmp		back1

;-Line--1212----L2:--
l2:

;-Line--1214----If TEST=1 Then--
		lds		r24,test
		lds		r25,test+1
		ldi		zl,low(1)
		ldi		zh,high(1)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0260
L0261:

;-Line--1215----PCO=PGM(PCO)--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		pco,zl

;-Line--1216----End If--
L0260:
L0259:

;-Line--1217----GoTo back1--
		rjmp		back1

;-Line--1219----L3:--
l3:

;-Line--1221----RADDRESS=PCO+1--
		lds		r24,pco
		ldi		zl,low(1)
		add		zl,r24
		sts		raddress,zl

;-Line--1222----PCO=PGM(PCO)--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		pco,zl

;-Line--1223----GoTo back1--
		rjmp		back1

;-Line--1225----L4:--
l4:

;-Line--1227----PCO=RADDRESS--
		lds		zl,raddress
		sts		pco,zl

;-Line--1228----GoTo back--
		rjmp		back

;-Line--1230----L5:--
l5:

;-Line--1232----VAR(RUN)=0--
		ldi		zl,low(41)
		ldi		zh,high(41)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1233----PCO=1--
		ldi		zl,low(1)
		sts		pco,zl

;-Line--1234----GoTo back--
		rjmp		back

;-Line--1237----L6:--
l6:

;-Line--1239----MATH1=PGM(PCO)--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		sts		math1,zl
		sts		math1+1,zh

;-Line--1240----GoTo back1--
		rjmp		back1

;-Line--1242----L7:--
l7:

;-Line--1244----MATH1=PGM(PCO+1)*256+PGM(PCO)--
		lds		zl,pco
		clr		zh
		push	zh
		push	zl
		ldi		zl,low(1)
		ldi		zh,high(1)
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		push	zh
		push	zl
		ldi		zl,low(256)
		ldi		zh,high(256)
		pop		r24
		pop		r25
		rcall	Mp16s
		push	zh
		push	zl
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		sts		math1,zl
		sts		math1+1,zh

;-Line--1245----GoTo back2--
		rjmp		back2

;-Line--1247----L8:--
l8:

;-Line--1249----MATH1=VAR(PGM(PCO))--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X+
		ld		zh,X
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		sts		math1,zl
		sts		math1+1,zh

;-Line--1250----GoTo back1--
		rjmp		back1

;-Line--1252----L9:--
l9:

;-Line--1254----MATH2=MATH1--
		lds		zl,math1
		lds		zh,math1+1
		sts		math2,zl
		sts		math2+1,zh

;-Line--1256----GoTo back--
		rjmp		back

;-Line--1258----L10:--
l10:

;-Line--1260----VAR(PGM(PCO))=MATH1--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X+
		ld		zh,X
		push	zh
		push	zl
		lds		zl,math1
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1261----GoTo back1--
		rjmp		back1

;-Line--1263----L11:--
l11:

;-Line--1265----MATH1=MATH1+MATH2--
		lds		r24,math1
		lds		r25,math1+1
		lds		zl,math2
		lds		zh,math2+1
		add		zl,r24
		adc		zh,r25
		sts		math1,zl
		sts		math1+1,zh

;-Line--1266----GoTo back--
		rjmp		back

;-Line--1268----L12:--
l12:

;-Line--1270----If MATH1=MATH2 Then--
		lds		r24,math1
		lds		r25,math1+1
		lds		zl,math2
		lds		zh,math2+1
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0263
L0264:

;-Line--1271----TEST=1--
		ldi		zl,low(1)
		ldi		zh,high(1)
		sts		test,zl
		sts		test+1,zh

;-Line--1272----Else--
		rjmp	L0262
L0263:

;-Line--1273----TEST=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		test,zl
		sts		test+1,zh

;-Line--1274----End If--
L0262:

;-Line--1275----GoTo back--
		rjmp		back

;-Line--1277----L13:--
l13:

;-Line--1279----If MATH1<>MATH2 Then--
		lds		r24,math1
		lds		r25,math1+1
		lds		zl,math2
		lds		zh,math2+1
		cp		r24,zl
		cpc		r25,zh
		brne	PC+0x02
		rjmp	L0266
L0267:

;-Line--1280----TEST=1--
		ldi		zl,low(1)
		ldi		zh,high(1)
		sts		test,zl
		sts		test+1,zh

;-Line--1281----Else--
		rjmp	L0265
L0266:

;-Line--1282----TEST=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		test,zl
		sts		test+1,zh

;-Line--1283----End If--
L0265:

;-Line--1284----GoTo back--
		rjmp		back

;-Line--1286----L14:--
l14:

;-Line--1288----If MATH1=1 Or MATH2=1 Then--
		lds		r24,math1
		lds		r25,math1+1
		ldi		zl,low(1)
		ldi		zh,high(1)
		cp		r24,zl
		cpc		r25,zh
		brne	PC+0x02
		rjmp	L0270
		lds		r24,math2
		lds		r25,math2+1
		ldi		zl,low(1)
		ldi		zh,high(1)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0269
L0270:

;-Line--1289----TEST=1--
		ldi		zl,low(1)
		ldi		zh,high(1)
		sts		test,zl
		sts		test+1,zh

;-Line--1290----Else--
		rjmp	L0268
L0269:

;-Line--1291----TEST=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		test,zl
		sts		test+1,zh

;-Line--1292----End If--
L0268:

;-Line--1293----GoTo back--
		rjmp		back

;-Line--1295----L15:--
l15:

;-Line--1297----If MATH1=1 And MATH2=1 Then--
		lds		r24,math1
		lds		r25,math1+1
		ldi		zl,low(1)
		ldi		zh,high(1)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0272
		lds		r24,math2
		lds		r25,math2+1
		ldi		zl,low(1)
		ldi		zh,high(1)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0272
L0273:

;-Line--1298----TEST=1--
		ldi		zl,low(1)
		ldi		zh,high(1)
		sts		test,zl
		sts		test+1,zh

;-Line--1299----Else--
		rjmp	L0271
L0272:

;-Line--1300----TEST=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		test,zl
		sts		test+1,zh

;-Line--1301----End If--
L0271:

;-Line--1302----GoTo back--
		rjmp		back

;-Line--1304----L16:--
l16:

;-Line--1306----If MATH1=0 Then--
		lds		r24,math1
		lds		r25,math1+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0275
L0276:

;-Line--1307----TEST=1--
		ldi		zl,low(1)
		ldi		zh,high(1)
		sts		test,zl
		sts		test+1,zh

;-Line--1308----Else--
		rjmp	L0274
L0275:

;-Line--1309----TEST=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		test,zl
		sts		test+1,zh

;-Line--1310----End If--
L0274:

;-Line--1311----GoTo back--
		rjmp		back

;-Line--1313----L17:--
l17:

;-Line--1315----If MATH1=1 Then--
		lds		r24,math1
		lds		r25,math1+1
		ldi		zl,low(1)
		ldi		zh,high(1)
		cp		r24,zl
		cpc		r25,zh
		breq	PC+0x02
		rjmp	L0278
L0279:

;-Line--1316----TEST=1--
		ldi		zl,low(1)
		ldi		zh,high(1)
		sts		test,zl
		sts		test+1,zh

;-Line--1317----Else--
		rjmp	L0277
L0278:

;-Line--1318----TEST=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		test,zl
		sts		test+1,zh

;-Line--1319----End If--
L0277:

;-Line--1320----GoTo back--
		rjmp		back

;-Line--1322----L18:--
l18:

;-Line--1324----If MATH1<>0 Then--
		lds		r24,math1
		lds		r25,math1+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		cp		r24,zl
		cpc		r25,zh
		brne	PC+0x02
		rjmp	L0281
L0282:

;-Line--1325----TEST=1--
		ldi		zl,low(1)
		ldi		zh,high(1)
		sts		test,zl
		sts		test+1,zh

;-Line--1326----Else--
		rjmp	L0280
L0281:

;-Line--1327----TEST=0--
		ldi		zl,low(0)
		ldi		zh,high(0)
		sts		test,zl
		sts		test+1,zh

;-Line--1328----End If--
L0280:

;-Line--1329----GoTo back--
		rjmp		back

;-Line--1331----L19:--
l19:

;-Line--1333----VAR(PGM(PCO))=0--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X+
		ld		zh,X
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1334----GoTo back1--
		rjmp		back1

;-Line--1336----L20:--
l20:

;-Line--1338----VAR(PGM(PCO))=VAR(PGM(PCO))+1--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X+
		ld		zh,X
		push	zh
		push	zl
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X+
		ld		zh,X
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(1)
		pop		r24
		add		zl,r24
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1339----GoTo back1--
		rjmp		back1

;-Line--1341----L21:--
l21:

;-Line--1343----VAR(PGM(PCO))=VAR(PGM(PCO))-1--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X+
		ld		zh,X
		push	zh
		push	zl
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X+
		ld		zh,X
		push	zl
		push	zh
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(1)
		pop		r24
		sub		r24,zl
		mov		zl,r24
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1344----GoTo back1--
		rjmp		back1

;-Line--1348----L22:--
l22:

;-Line--1350----VAR(36)=1--
		ldi		zl,low(36)
		ldi		zh,high(36)
		push	zh
		push	zl
		ldi		zl,low(1)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1351----VAR(37)=1--
		ldi		zl,low(37)
		ldi		zh,high(37)
		push	zh
		push	zl
		ldi		zl,low(1)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1352----GoTo back--
		rjmp		back

;-Line--1354----L23:--
l23:

;-Line--1356----VAR(36)=0--
		ldi		zl,low(36)
		ldi		zh,high(36)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1357----VAR(37)=0--
		ldi		zl,low(37)
		ldi		zh,high(37)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1358----GoTo back--
		rjmp		back

;-Line--1360----L24:--
l24:

;-Line--1362----VAR(36)=1--
		ldi		zl,low(36)
		ldi		zh,high(36)
		push	zh
		push	zl
		ldi		zl,low(1)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1363----VAR(37)=0--
		ldi		zl,low(37)
		ldi		zh,high(37)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1364----GoTo back--
		rjmp		back

;-Line--1366----L25:--
l25:

;-Line--1368----VAR(36)=0--
		ldi		zl,low(36)
		ldi		zh,high(36)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1369----VAR(37)=1--
		ldi		zl,low(37)
		ldi		zh,high(37)
		push	zh
		push	zl
		ldi		zl,low(1)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1370----GoTo back--
		rjmp		back

;-Line--1372----L26:--
l26:

;-Line--1374----VAR(16)=0--
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1375----VAR(17)=0--
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zh
		push	zl
		ldi		zl,low(0)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1376----GoTo back--
		rjmp		back

;-Line--1378----L27:--
l27:

;-Line--1380----VAR(16)=VLOW--
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		lds		zl,vlow
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1381----VAR(17)=VLOW--
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zh
		push	zl
		lds		zl,vlow
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1382----GoTo back--
		rjmp		back

;-Line--1384----L28:--
l28:

;-Line--1386----VAR(16)=VMEDIUM--
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		lds		zl,vmedium
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1387----VAR(17)=VMEDIUM--
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zh
		push	zl
		lds		zl,vmedium
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1388----GoTo back--
		rjmp		back

;-Line--1390----L29:--
l29:

;-Line--1392----VAR(16)=VFAST--
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		lds		zl,vfast
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1393----VAR(17)=VFAST--
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zh
		push	zl
		lds		zl,vfast
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1394----GoTo back--
		rjmp		back

;-Line--1396----L30:--
l30:

;-Line--1398----VAR(16)=MATH1--
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		lds		zl,math1
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1399----VAR(17)=MATH1--
		ldi		zl,low(17)
		ldi		zh,high(17)
		push	zh
		push	zl
		lds		zl,math1
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1400----GoTo back--
		rjmp		back

;-Line--1402----L31:--
l31:

;-Line--1404----C_TIMER=PARAM--
		lds		zl,param
		lds		zh,param+1
		sts		c_timer,zl
		sts		c_timer+1,zh

;-Line--1405----MODE=1--
		ldi		zl,low(1)
		sts		mode,zl

;-Line--1406----GoTo back--
		rjmp		back

;-Line--1408----L32:--
l32:

;-Line--1410----C_COUNTER0=PARAM--
		lds		zl,param
		lds		zh,param+1
		sts		c_counter0,zl
		sts		c_counter0+1,zh

;-Line--1411----MODE=2--
		ldi		zl,low(2)
		sts		mode,zl

;-Line--1412----GoTo back--
		rjmp		back

;-Line--1414----L33:--
l33:

;-Line--1416----C_COUNTER1=PARAM--
		lds		zl,param
		lds		zh,param+1
		sts		c_counter1,zl
		sts		c_counter1+1,zh

;-Line--1417----MODE=2--
		ldi		zl,low(2)
		sts		mode,zl

;-Line--1418----GoTo back--
		rjmp		back

;-Line--1420----L60:--
l60:

;-Line--1422----C_COUNTER0=PARAM--
		lds		zl,param
		lds		zh,param+1
		sts		c_counter0,zl
		sts		c_counter0+1,zh

;-Line--1423----C_COUNTER1=PARAM--
		lds		zl,param
		lds		zh,param+1
		sts		c_counter1,zl
		sts		c_counter1+1,zh

;-Line--1424----MODE=2--
		ldi		zl,low(2)
		sts		mode,zl

;-Line--1425----GoTo back--
		rjmp		back

;-Line--1427----L34:--
l34:

;-Line--1429----If C_TIMER>0 Then--
		lds		r24,c_timer
		lds		r25,c_timer+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		adiw	zl,1
		cp		r24,zl
		cpc		r25,zh
		brge	PC+0x02
		rjmp	L0284
L0285:

;-Line--1430----Decr PCO--
		lds		zl,pco
		dec		zl
		sts		pco,zl

;-Line--1431----End If--
L0284:
L0283:

;-Line--1432----GoTo back--
		rjmp		back

;-Line--1434----L35:--
l35:

;-Line--1436----If C_COUNTER0 > 0 Then--
		lds		r24,c_counter0
		lds		r25,c_counter0+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		adiw	zl,1
		cp		r24,zl
		cpc		r25,zh
		brge	PC+0x02
		rjmp	L0287
L0288:

;-Line--1437----Decr PCO--
		lds		zl,pco
		dec		zl
		sts		pco,zl

;-Line--1438----End If--
L0287:
L0286:

;-Line--1439----GoTo back--
		rjmp		back

;-Line--1441----L36:--
l36:

;-Line--1443----If C_COUNTER0 > 0 Then--
		lds		r24,c_counter0
		lds		r25,c_counter0+1
		ldi		zl,low(0)
		ldi		zh,high(0)
		adiw	zl,1
		cp		r24,zl
		cpc		r25,zh
		brge	PC+0x02
		rjmp	L0290
L0291:

;-Line--1444----Decr PCO--
		lds		zl,pco
		dec		zl
		sts		pco,zl

;-Line--1445----End If--
L0290:
L0289:

;-Line--1446----GoTo back--
		rjmp		back

;-Line--1448----L37:--
l37:

;-Line--1450----If MODE <> 99 Then     ' Attention 99 -> 0--
		lds		r24,mode
		ldi		zl,low(99)
		cp		r24,zl
		brne	PC+0x02
		rjmp	L0293
L0294:

;-Line--1451----Decr PCO--
		lds		zl,pco
		dec		zl
		sts		pco,zl

;-Line--1452----End If--
L0293:
L0292:

;-Line--1453----GoTo back--
		rjmp		back

;-Line--1455----L38:--
l38:

;-Line--1457----Decr PCO--
		lds		zl,pco
		dec		zl
		sts		pco,zl

;-Line--1458----GoTo back--
		rjmp		back

;-Line--1460----L39:--
l39:

;-Line--1462----If PGM(PCO)=0 Then--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		push	zl
		ldi		zl,low(0)
		pop		r24
		cp		r24,zl
		breq	PC+0x02
		rjmp	L0296
L0297:

;-Line--1463----C_TIMER=2000+20*Rnd()--
		ldi		zl,low(2000)
		ldi		zh,high(2000)
		push	zh
		push	zl
		ldi		zl,low(20)
		ldi		zh,high(20)
		push	zh
		push	zl
		ldi		xl,_rnd
		clr		xh
		ldi		zl,1
		rcall	_Crc8
		sts		_rnd,zl
		clr		zh
		pop		r24
		pop		r25
		rcall	Mp16s
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		sts		c_timer,zl
		sts		c_timer+1,zh

;-Line--1464----Else--
		rjmp	L0295
L0296:

;-Line--1465----C_TIMER=PGM(PCO)*100--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		push	zh
		push	zl
		ldi		zl,low(100)
		ldi		zh,high(100)
		pop		r24
		pop		r25
		rcall	Mp16s
		sts		c_timer,zl
		sts		c_timer+1,zh

;-Line--1466----End If--
L0295:

;-Line--1467----GoTo back--
		rjmp		back

;-Line--1469----L40:--
l40:

;-Line--1471----A=PGM(PCO)--
		lds		zl,pco
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		sts		a,zl

;-Line--1472----Sound A,A*4--
		lds		zl,a
		mov		zh,zl
		lds		zl,a
		push	zl
		ldi		zl,low(4)
		pop		r24
		rcall	Mpy8u
		rcall	_Sound

;-Line--1473----GoTo back--
		rjmp		back

;-Line--1475----L41:--
l41:

;-Line--1477----MODE=1--
		ldi		zl,low(1)
		sts		mode,zl

;-Line--1478----GoTo back--
		rjmp		back

;-Line--1480----L42:--
l42:

;-Line--1482----MODE=2--
		ldi		zl,low(2)
		sts		mode,zl

;-Line--1483----GoTo back--
		rjmp		back

;-Line--1486----Sub DOWNLOAD(NUM As Byte)  ' Move Script from EEPROM to RAM, 31 SCRIPTS of 15 STEP--
download:

;-Line--1489----BASE=16+NUM*16--
		sbiw	yl,3
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		ldd		zl,y+3
		clr		zh
		push	zh
		push	zl
		ldi		zl,low(16)
		ldi		zh,high(16)
		pop		r24
		pop		r25
		rcall	Mp16u
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		std		y+0,zl
		std		y+1,zh

;-Line--1491----For P=1 To 16--
		ldi		zl,low(1)
		std		y+2,zl
		ldi		zl,low(16)
		push	r5
		inc		zl
		mov		r5,zl
L0299:

;-Line--1492----PGM(P)=ReadEE(BASE+P)--
		ldd		zl,y+2
		clr		zh
		push	zh
		push	zl
		ldd		zl,y+0
		ldd		zh,y+1
		push	zh
		push	zl
		ldd		zl,y+2
		clr		zh
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		mov		r24,zl
		mov		r25,zh
		rcall	_RdEE
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1493----Next P--
		ldd		zl,y+2
		inc		zl
		std		y+2,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0299
		pop		r5
L0300:

;-Line--1495----End Sub--
L0298:
		adiw	yl,3
		ret

;-Line--1497----Sub UPLOAD(NUM As Byte)--
upload:

;-Line--1500----BASE=16+NUM*16--
		sbiw	yl,3
		ldi		zl,low(16)
		ldi		zh,high(16)
		push	zh
		push	zl
		ldd		zl,y+3
		clr		zh
		push	zh
		push	zl
		ldi		zl,low(16)
		ldi		zh,high(16)
		pop		r24
		pop		r25
		rcall	Mp16u
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		std		y+0,zl
		std		y+1,zh

;-Line--1502----For P=1 To 16--
		ldi		zl,low(1)
		std		y+2,zl
		ldi		zl,low(16)
		push	r5
		inc		zl
		mov		r5,zl
L0302:

;-Line--1503----WriteEE(BASE+P,PGM(P))--
		ldd		zl,y+0
		ldd		zh,y+1
		push	zh
		push	zl
		ldd		zl,y+2
		clr		zh
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		mov		r24,zl
		mov		r25,zh
		ldd		zl,y+2
		clr		zh
		push	zl
		push	zh
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r25
		pop		r24
		add		xl, r24
		adc		xh, r25
		ld		zl,X
		rcall	_WrtEE

;-Line--1504----Next--
		ldd		zl,y+2
		inc		zl
		std		y+2,zl
		cp		zl,r5
		brsh	PC+0x02
		rjmp	L0302
		pop		r5
L0303:

;-Line--1506----End Sub--
L0301:
		adiw	yl,3
		ret

;-Line--1508----Sub PID()      ' P mais pas I risque oscillation--
pid:

;-Line--1510----If ( C_COUNTER1 - C_COUNTER0 ) > 2 Then--
		lds		zl,c_counter1
		lds		zh,c_counter1+1
		push	zh
		push	zl
		lds		zl,c_counter0
		lds		zh,c_counter0+1
		pop		r24
		pop		r25
		sub		r24,zl
		sbc		r25,zh
		mov		zl,r24
		mov		zh,r25
		push	zh
		push	zl
		ldi		zl,low(2)
		ldi		zh,high(2)
		adiw	zl,1
		pop		r24
		pop		r25
		cp		r24,zl
		cpc		r25,zh
		brge	PC+0x02
		rjmp	L0306
L0307:

;-Line--1511----If SPEED0 < 255 Then                     ' VAR(16)--
		lds		r24,speed0
		ldi		zl,low(255)
		cp		r24,zl
		brlo	PC+0x02
		rjmp	L0309
L0310:

;-Line--1512----Incr SPEED0--
		lds		zl,speed0
		inc		zl
		sts		speed0,zl

;-Line--1513----Else--
		rjmp	L0308
L0309:

;-Line--1514----Decr SPEED1--
		lds		zl,speed1
		dec		zl
		sts		speed1,zl

;-Line--1515----End If--
L0308:

;-Line--1516----End If--
L0306:
L0305:

;-Line--1526----End Sub--
L0304:
		ret

;-Line--1528----Sub EXECUTE(NUM As Byte) ' 1 or 8 Command        OK--
execute:

;-Line--1529----PGM(NUM+1)=GEND--
		ldd		zl,y+0
		clr		zh
		push	zh
		push	zl
		ldi		zl,low(1)
		ldi		zh,high(1)
		pop		r24
		pop		r25
		add		zl,r24
		adc		zh,r25
		push	zh
		push	zl
		ldi		zl,low(5)
		ldi		xl,low(pgm)
		ldi		xh,high(pgm)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1530----ERROR=0--
		ldi		zl,low(0)
		sts		error,zl

;-Line--1531----PCO=1--
		ldi		zl,low(1)
		sts		pco,zl

;-Line--1532----VAR(RUN)=1--
		ldi		zl,low(41)
		ldi		zh,high(41)
		push	zh
		push	zl
		ldi		zl,low(1)
		ldi		xl,low(var)
		ldi		xh,high(var)
		pop		r24
		pop		r25
		add		xl, r24
		adc		xh, r25
		st		X,zl

;-Line--1533----End Sub--
L0311:
		ret

;-Line--1535----Sub PARAMETRE()  ' READ 2 BYTE and SET PARAM       OK--
parametre:

;-Line--1538----A=READHEX()--
		sbiw	yl,2
		rcall	readhex
		std		y+1,zl

;-Line--1539----B=READHEX()--
		rcall	readhex
		std		y+0,zl

;-Line--1540----PARAM=A*256--
		ldd		r24,y+1
		clr		r25
		ldi		zl,low(256)
		ldi		zh,high(256)
		rcall	Mp16s
		sts		param,zl
		sts		param+1,zh

;-Line--1541----PARAM=PARAM+B--
		lds		r24,param
		lds		r25,param+1
		ldd		zl,y+0
		clr		zh
		add		zl,r24
		adc		zh,r25
		sts		param,zl
		sts		param+1,zh

;-Line--1542----End Sub--
L0312:
		adiw	yl,2
		ret

;****** END OF USER BASIC CODE ****************

; String constants:
S00:	.db	"ROBOT GLU READY ", 0
S01:	.db	"ADRESSE ", 0
S02:	.db	"Version B ", 0
S03:	.db	".", 0
S04:	.db	"= ", 0
S05:	.db	"TIME ", 0
S06:	.db	"WD0 ", 0
S07:	.db	"WD1 ", 0
S08:	.db	"TOKEN=", 0


;////// Store BitVar //////////////////////
_StBit:	clt
		sbrc	zl,0
		set
		ret

;////// Load BitVar //////////////////////
_VaBiL:
		lsr		zl
		dec		r24
		brne	_VaBiL
		ret

;////// GetChar /////////////////////////////
_GetCh:	sbis	USR,0x07
		rjmp	_GetCh
		in		zl,UDR
		ret

;////// Print Hex Word ////////////////////
_PHw:	push	zl
		mov		zl,zh
		rcall	_PHb
		pop		zl

;////// Print Hex Byte ////////////////////
_PHb:	mov		r25,zl
		swap	zl
		rcall	_Pnb
		mov		zl,r25
_Pnb:	andi	zl,0xf
		subi	zl,-0x30
		cpi		zl,0x3a
		brlo	_Pn1
		subi	zl,-0x07
_Pn1:	mov		r24,zl
		rjmp	_Pch

;////// Print Byte & Word /////////////////
_PrBW:	ld		r24,Z+
		tst		r24
		breq	_PBW1
		rcall	_Pch
		rjmp	_PrBW
_PBW1:	ret

;////// Print string constants ////////////
_PSc:	lpm
		tst		r0
		breq	_PSc1
		mov		r24,r0
		rcall	_Pch
		adiw	zl,1
		rjmp	_PSc
_Psc1:	ret

;////// Print Cr, Lf & any char////////////
_PCL:	ldi		r24,13
		rcall	_Pch
		ldi		r24,10
_Pch:	sbis	USR,5
		rjmp	_Pch
		out		UDR,r24
		ret

;////// CalcCrc8 //////////////////////////
_Crc8:	mov		r25,zl
		clr		zl
		ldi		r21,0x8c
_Cr81:	ldi		r24,8
		ld		zh,X+
_Cr82:	mov		r23,zh
		andi	r23,0x01
		mov		r22,zl
		andi	r22,0x01
		eor		r22,r23
		lsr		zl
		lsr		zh
		sbrc	r22,0
		eor		zl,r21
		dec		r24
		brne	_Cr82
		dec		r25
		brne	_Cr81
		ret

;////// WriteEE ///////////////////////////
_WrtEE:	sbic	EECR,1
		rjmp	_WrtEE
		out		EEARH,r25
		out		EEARL,r24
		out		EEDR,zl
		sbi		EECR,2
		sbi		EECR,1
		ret

;////// ReadEE ////////////////////////////
_RdEE:	sbic	EECR,1
		rjmp	_RdEE
		out		EEARH,r25
		out		EEARL,r24
		sbi		EECR,0
		in		zl,EEDR
		ret

;////// ADC8 //////////////////////////////
_Adc8:	sbi		ADCSR,6
_Adc18:	sbic	ADCSR,6
		rjmp	_Adc18
		in		zl,ADCL
		in		zh,ADCH
		lsr		zh
		ror		zl
		lsr		zh
		ror		zl
		clr		zh
		ret

;////// Sound /////////////////////////////
_Sound:	mov		r25,zl
_Sou2:	ldi		r24,10
_Sou1:	sbi		PORTC,5
		mov		zl,zh
		rcall	_Wus
		cbi		PORTC,5
		mov		zl,zh
		rcall	_Wus
		dec		r24
		brne	_Sou1
		dec		r25
		brne	_Sou2
		ret

;////// ByteToStr ////////////////////////
_B2str:	clr		zh
		clt
		push	yl
		push	yh
		st		-Y,zh
_N2str:	ldi		xl,0x08
		sub		xh,xh
_N2st1:	lsr		zh
		rol		zl
		rol		xh
		rol		zh
		cpi		xh,0x0a
		brcs	_N2st2
		sbci	xh,0x0a
		inc		zl
_N2st2:	dec		xl
		brne	_N2st1
		subi	xh,-0x30
		st		-Y,xh
		tst		zl
		brne	_N2str
_N2st3:	mov		zl,yl
		mov		zh,yh
		pop		yh
		pop		yl
		ret

;////// _Waitms ///////////////////////////
_Wms:	ldi		r20,0x14
_Wms1:	ldi		r21,0x85
_Wms2:	dec		r21
		brne	_Wms2
		dec		r20
		brne	_Wms1
		dec		zl
		brne	_Wms
		ret

;////// _waitus ///////////////////////////
_wus:	ldi		r22,25
_wus1:	dec		r22
		brne	_wus1
		dec		zl
		brne	_wus
		ret

;////// _Waits ////////////////////////////
_Ws:	mov		zh,zl
_Ws3:	ldi		r22,0x04
_Ws1:	ldi		zl,0xfa
		rcall	_Wms
		dec		r22
		brne	_Ws1
		dec		zh
		brne	_Ws3
		ret

;////// 8x8 unsigned multiplay ////////////
Mpy8u:	push	zh
		clr		zh
		ldi		r19,0x08
		lsr		zl
m8u1:	brcc	m8u2
		add		zh,r24
m8u2:	ror		zh
		ror		zl
		dec		r19
		brne	m8u1
		pop		zh
		ret

;////// 8/8 unsigned division /////////////
Div8u:  Sub     r22,r22
        ldi     r23,0x09
        rjmp    Div81
Div80:  rol     r22
        cp      r22,zl
        brcs    Div81
        Sub     r22,zl
Div81:  rol     r24
        dec     r23
        brne    Div80
        Com     r24
        mov     zl,r24
        ret

;////// 16x16 unsigned multiplay //////////
Mp16u:	clr		r23
		clr		r22
		ldi		r21,0x10
		lsr		zh
		ror		zl
m16u1:	brcc	m16u2
		add		r22,r24
		adc		r23,r25
m16u2:	ror		r23
		ror		r22
		ror		zh
		ror		zl
		dec		r21
		brne	m16u1
		ret

;////// 16x16 signed multiplay ////////////
Mp16s:	clr		r23
		sub		r22,r22
		ldi		r21,0x10
m16s1:	brcc	m16s2
		add		r22,r24
		adc		r23,r25
m16s2:	sbrc	zl,0
		sub		r22,r24
		sbrc	zl,0
		sbc		r23,r25
		asr		r23
		ror		r22
		ror		zh
		ror		zl
		dec		r21
		brne	m16s1
		ret

;System Global Variables: 1  bytes
;User   Global Variables: 305  bytes
