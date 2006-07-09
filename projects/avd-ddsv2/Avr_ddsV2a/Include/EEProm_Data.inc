;******************************************************************************
; ASCII messages stored in EEPROM
;******************************************************************************
	.eseg

MSG_HELLO:
	.db	cr,lf,"MiniDDS V1.0",cr,lf,"Jun'06",cr,lf,0
MSG_CRLF:
	.db	cr,lf,0
MSG_SET:
	.db	"SET:",0
MSG_FRQ:
	.db	"FRQ:",0
MSG_HZ:
	.db	" Hz",0
MSG_U1:
	.db	"+1Hz ",0
MSG_U10:
	.db	"+10Hz ",0
MSG_U100:
	.db	"+100Hz ",0
MSG_U1000:
	.db	"+1KHz ",0
MSG_D1:
	.db	"-1Hz ",0
MSG_D10:
	.db	"-10Hz ",0
MSG_D100:
	.db	"-100Hz ",0
MSG_D1000:
	.db	"-1KHz ",0
MSG_SINE:
	.db	"Sin",cr,lf,0
MSG_SQUARE:
	.db	"Sqr",cr,lf,0
MSG_TRI:
	.db	"Tri",cr,lf,0
MSG_RAMP:
	.db	"Rmp",cr,lf,0
;MSG_REVRAMP:
;	.db	"RRmp",cr,lf,0
;MSG_NOISE:
;	.db	"Rnd",cr,lf,0
MSG_BYE:
	.db	"Bye",cr,lf,0
