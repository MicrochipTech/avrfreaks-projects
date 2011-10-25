;compiler: avrasm

;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: MOD/S3M decoder tables
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************

	;ScreamTracker3 periods for octave 0
TS3Moctper:	.dw	1712,1616,1524,1440,1356,1280,1208,1140,1076,1016,960,907

	;ProTracker finetune coefficients: 8287/freq(fine_tune) [fx 1.15]
TMODfint:	.dw	(8287<<15)/8287	; 0
	.dw	(8287<<15)/8345	;+1
	.dw	(8287<<15)/8404	;+2
	.dw	(8287<<15)/8465	;+3
	.dw	(8287<<15)/8526	;+4
	.dw	(8287<<15)/8588	;+5
	.dw	(8287<<15)/8650	;+6
	.dw	(8287<<15)/8712	;+7
	.dw	(8287<<15)/7821	;-8
	.dw	(8287<<15)/7881	;-7
	.dw	(8287<<15)/7934	;-6
	.dw	(8287<<15)/7997	;-5
	.dw	(8287<<15)/8051	;-4
	.dw	(8287<<15)/8107	;-3
	.dw	(8287<<15)/8172	;-2
	.dw	(8287<<15)/8229	;-1


	;ProTracker periods
TMODarpHS:	.dw	57	;octave 4
	.dw	60
	.dw	64
	.dw	67
	.dw	71
	.dw	76
	.dw	80
	.dw	85
	.dw	90
	.dw	95
	.dw	101
	.dw	107	;octave 4
	.dw	113	;octave 3
	.dw	120
	.dw	127
	.dw	135
	.dw	143
	.dw	151
	.dw	160
	.dw	170
	.dw	180
	.dw	190
	.dw	202
	.dw	214	;octave 3
	.dw	226	;octave 2
	.dw	240
	.dw	254
	.dw	269
	.dw	285
	.dw	302
	.dw	320
	.dw	339
	.dw	360
	.dw	381
	.dw	404
	.dw	428	;octave 2
	.dw	453	;octave 1
	.dw	480
	.dw	508
	.dw	538
	.dw	570
	.dw	604
	.dw	640
	.dw	678
	.dw	720
	.dw	762
	.dw	808
	.dw	856	;octave 1
	.dw	907	;octave 0
	.dw	961
	.dw	1017
	.dw	1077
	.dw	1141
	.dw	1209
	.dw	1281
	.dw	1357
	.dw	1440
	.dw	1525
	.dw	1616
	.dw	1712	;octave 0
	
	;single sine period table: 64 values per period, amplitude +/-127 
TMODsine64:	.db	0,12,25,37,49,60,71,81,90,98,106,112,117,122,125,126,127,126,125,122,117,112,106,98,90,81,71,60,49,37,25,12
	.db	0,-12,-25,-37,-49,-60,-71,-81,-90,-98,-106,-112,-117,-122,-125,-126,-127,-126,-125,-122,-117,-112,-106,-98,-90,-81,-71,-60,-49,-37,-25,-12

	;single ramp period table: 32 values per period +1, amplitude -128/+127
;TMODramp32:	.db	-128,-120,-112,-104,-96,-88,-80,-72,-64,-56,-48,-40,-32,-24,-16,-8,0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,-128