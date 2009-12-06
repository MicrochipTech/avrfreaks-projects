
test_lcd.o:     file format elf32-avr

Disassembly of section .text:

00000000 <__vector_2>:
   0:	1f 92       	push	r1
   2:	0f 92       	push	r0
   4:	0f b6       	in	r0, 0x3f	; 63
   6:	0f 92       	push	r0
   8:	11 24       	eor	r1, r1
   a:	8f 93       	push	r24
   c:	9f 93       	push	r25
   e:	82 ed       	ldi	r24, 0xD2	; 210
  10:	94 e0       	ldi	r25, 0x04	; 4
  12:	90 93 00 00 	sts	0x0000, r25
  16:	80 93 00 00 	sts	0x0000, r24
  1a:	9f 91       	pop	r25
  1c:	8f 91       	pop	r24
  1e:	0f 90       	pop	r0
  20:	0f be       	out	0x3f, r0	; 63
  22:	0f 90       	pop	r0
  24:	1f 90       	pop	r1
  26:	18 95       	reti

00000028 <__vector_14>:
  28:	78 94       	sei
  2a:	1f 92       	push	r1
  2c:	0f 92       	push	r0
  2e:	0f b6       	in	r0, 0x3f	; 63
  30:	0f 92       	push	r0
  32:	11 24       	eor	r1, r1
  34:	2f 93       	push	r18
  36:	3f 93       	push	r19
  38:	8f 93       	push	r24
  3a:	9f 93       	push	r25
  3c:	84 b1       	in	r24, 0x04	; 4
  3e:	25 b1       	in	r18, 0x05	; 5
  40:	33 27       	eor	r19, r19
  42:	32 2f       	mov	r19, r18
  44:	22 27       	eor	r18, r18
  46:	99 27       	eor	r25, r25
  48:	28 2b       	or	r18, r24
  4a:	39 2b       	or	r19, r25
  4c:	30 93 00 00 	sts	0x0000, r19
  50:	20 93 00 00 	sts	0x0000, r18
  54:	10 92 00 00 	sts	0x0000, r1
  58:	80 91 00 00 	lds	r24, 0x0000
  5c:	90 91 00 00 	lds	r25, 0x0000
  60:	01 96       	adiw	r24, 0x01	; 1
  62:	90 93 00 00 	sts	0x0000, r25
  66:	80 93 00 00 	sts	0x0000, r24
  6a:	9f 91       	pop	r25
  6c:	8f 91       	pop	r24
  6e:	3f 91       	pop	r19
  70:	2f 91       	pop	r18
  72:	0f 90       	pop	r0
  74:	0f be       	out	0x3f, r0	; 63
  76:	0f 90       	pop	r0
  78:	1f 90       	pop	r1
  7a:	18 95       	reti

0000007c <__vector_4>:
  7c:	1f 92       	push	r1
  7e:	0f 92       	push	r0
  80:	0f b6       	in	r0, 0x3f	; 63
  82:	0f 92       	push	r0
  84:	11 24       	eor	r1, r1
  86:	2f 93       	push	r18
  88:	3f 93       	push	r19
  8a:	4f 93       	push	r20
  8c:	5f 93       	push	r21
  8e:	6f 93       	push	r22
  90:	7f 93       	push	r23
  92:	8f 93       	push	r24
  94:	9f 93       	push	r25
  96:	af 93       	push	r26
  98:	bf 93       	push	r27
  9a:	ef 93       	push	r30
  9c:	ff 93       	push	r31
  9e:	80 91 00 00 	lds	r24, 0x0000
  a2:	90 91 00 00 	lds	r25, 0x0000
  a6:	01 96       	adiw	r24, 0x01	; 1
  a8:	90 93 00 00 	sts	0x0000, r25
  ac:	80 93 00 00 	sts	0x0000, r24
  b0:	80 91 00 00 	lds	r24, 0x0000
  b4:	90 91 00 00 	lds	r25, 0x0000
  b8:	44 97       	sbiw	r24, 0x14	; 20
  ba:	09 f0       	breq	.+2      	; 0xbe <__vector_4+0x42>
  bc:	a1 c0       	rjmp	.+322    	; 0x200 <__vector_4+0x184>
  be:	60 e0       	ldi	r22, 0x00	; 0
  c0:	80 e0       	ldi	r24, 0x00	; 0
  c2:	00 d0       	rcall	.+0      	; 0xc4 <__vector_4+0x48>
  c4:	80 e0       	ldi	r24, 0x00	; 0
  c6:	90 e0       	ldi	r25, 0x00	; 0
  c8:	00 d0       	rcall	.+0      	; 0xca <__vector_4+0x4e>
  ca:	80 91 00 00 	lds	r24, 0x0000
  ce:	90 91 00 00 	lds	r25, 0x0000
  d2:	4a e0       	ldi	r20, 0x0A	; 10
  d4:	50 e0       	ldi	r21, 0x00	; 0
  d6:	60 e0       	ldi	r22, 0x00	; 0
  d8:	70 e0       	ldi	r23, 0x00	; 0
  da:	00 d0       	rcall	.+0      	; 0xdc <__vector_4+0x60>
  dc:	80 e0       	ldi	r24, 0x00	; 0
  de:	90 e0       	ldi	r25, 0x00	; 0
  e0:	00 d0       	rcall	.+0      	; 0xe2 <__vector_4+0x66>
  e2:	80 e0       	ldi	r24, 0x00	; 0
  e4:	90 e0       	ldi	r25, 0x00	; 0
  e6:	00 d0       	rcall	.+0      	; 0xe8 <__vector_4+0x6c>
  e8:	80 e0       	ldi	r24, 0x00	; 0
  ea:	90 e0       	ldi	r25, 0x00	; 0
  ec:	00 d0       	rcall	.+0      	; 0xee <__vector_4+0x72>
  ee:	80 91 00 00 	lds	r24, 0x0000
  f2:	90 91 00 00 	lds	r25, 0x0000
  f6:	60 e9       	ldi	r22, 0x90	; 144
  f8:	71 e0       	ldi	r23, 0x01	; 1
  fa:	00 d0       	rcall	.+0      	; 0xfc <__vector_4+0x80>
  fc:	60 93 00 00 	sts	0x0000, r22
 100:	70 93 00 00 	sts	0x0000, r23
 104:	80 93 00 00 	sts	0x0000, r24
 108:	90 93 00 00 	sts	0x0000, r25
 10c:	4a e0       	ldi	r20, 0x0A	; 10
 10e:	50 e0       	ldi	r21, 0x00	; 0
 110:	60 e0       	ldi	r22, 0x00	; 0
 112:	70 e0       	ldi	r23, 0x00	; 0
 114:	80 91 00 00 	lds	r24, 0x0000
 118:	90 91 00 00 	lds	r25, 0x0000
 11c:	00 d0       	rcall	.+0      	; 0x11e <__vector_4+0xa2>
 11e:	80 e0       	ldi	r24, 0x00	; 0
 120:	90 e0       	ldi	r25, 0x00	; 0
 122:	00 d0       	rcall	.+0      	; 0x124 <__vector_4+0xa8>
 124:	80 e0       	ldi	r24, 0x00	; 0
 126:	90 e0       	ldi	r25, 0x00	; 0
 128:	00 d0       	rcall	.+0      	; 0x12a <__vector_4+0xae>
 12a:	60 91 00 00 	lds	r22, 0x0000
 12e:	70 91 00 00 	lds	r23, 0x0000
 132:	88 27       	eor	r24, r24
 134:	77 fd       	sbrc	r23, 7
 136:	80 95       	com	r24
 138:	98 2f       	mov	r25, r24
 13a:	00 d0       	rcall	.+0      	; 0x13c <__vector_4+0xc0>
 13c:	2d ec       	ldi	r18, 0xCD	; 205
 13e:	3c ec       	ldi	r19, 0xCC	; 204
 140:	4c ec       	ldi	r20, 0xCC	; 204
 142:	5e e3       	ldi	r21, 0x3E	; 62
 144:	00 d0       	rcall	.+0      	; 0x146 <__vector_4+0xca>
 146:	20 e0       	ldi	r18, 0x00	; 0
 148:	30 e0       	ldi	r19, 0x00	; 0
 14a:	48 ec       	ldi	r20, 0xC8	; 200
 14c:	52 e4       	ldi	r21, 0x42	; 66
 14e:	00 d0       	rcall	.+0      	; 0x150 <__vector_4+0xd4>
 150:	88 23       	and	r24, r24
 152:	1c f4       	brge	.+6      	; 0x15a <__vector_4+0xde>
 154:	80 e0       	ldi	r24, 0x00	; 0
 156:	90 e0       	ldi	r25, 0x00	; 0
 158:	00 d0       	rcall	.+0      	; 0x15a <__vector_4+0xde>
 15a:	60 91 00 00 	lds	r22, 0x0000
 15e:	70 91 00 00 	lds	r23, 0x0000
 162:	88 27       	eor	r24, r24
 164:	77 fd       	sbrc	r23, 7
 166:	80 95       	com	r24
 168:	98 2f       	mov	r25, r24
 16a:	00 d0       	rcall	.+0      	; 0x16c <__vector_4+0xf0>
 16c:	2d ec       	ldi	r18, 0xCD	; 205
 16e:	3c ec       	ldi	r19, 0xCC	; 204
 170:	4c ec       	ldi	r20, 0xCC	; 204
 172:	5e e3       	ldi	r21, 0x3E	; 62
 174:	00 d0       	rcall	.+0      	; 0x176 <__vector_4+0xfa>
 176:	20 e0       	ldi	r18, 0x00	; 0
 178:	30 e0       	ldi	r19, 0x00	; 0
 17a:	40 e2       	ldi	r20, 0x20	; 32
 17c:	51 e4       	ldi	r21, 0x41	; 65
 17e:	00 d0       	rcall	.+0      	; 0x180 <__vector_4+0x104>
 180:	88 23       	and	r24, r24
 182:	1c f4       	brge	.+6      	; 0x18a <__vector_4+0x10e>
 184:	80 e0       	ldi	r24, 0x00	; 0
 186:	90 e0       	ldi	r25, 0x00	; 0
 188:	00 d0       	rcall	.+0      	; 0x18a <__vector_4+0x10e>
 18a:	60 91 00 00 	lds	r22, 0x0000
 18e:	70 91 00 00 	lds	r23, 0x0000
 192:	88 27       	eor	r24, r24
 194:	77 fd       	sbrc	r23, 7
 196:	80 95       	com	r24
 198:	98 2f       	mov	r25, r24
 19a:	00 d0       	rcall	.+0      	; 0x19c <__vector_4+0x120>
 19c:	2d ec       	ldi	r18, 0xCD	; 205
 19e:	3c ec       	ldi	r19, 0xCC	; 204
 1a0:	4c ec       	ldi	r20, 0xCC	; 204
 1a2:	5e e3       	ldi	r21, 0x3E	; 62
 1a4:	00 d0       	rcall	.+0      	; 0x1a6 <__vector_4+0x12a>
 1a6:	00 d0       	rcall	.+0      	; 0x1a8 <__vector_4+0x12c>
 1a8:	dc 01       	movw	r26, r24
 1aa:	cb 01       	movw	r24, r22
 1ac:	4a e0       	ldi	r20, 0x0A	; 10
 1ae:	50 e0       	ldi	r21, 0x00	; 0
 1b0:	60 e0       	ldi	r22, 0x00	; 0
 1b2:	70 e0       	ldi	r23, 0x00	; 0
 1b4:	00 d0       	rcall	.+0      	; 0x1b6 <__vector_4+0x13a>
 1b6:	80 e0       	ldi	r24, 0x00	; 0
 1b8:	90 e0       	ldi	r25, 0x00	; 0
 1ba:	00 d0       	rcall	.+0      	; 0x1bc <__vector_4+0x140>
 1bc:	80 e0       	ldi	r24, 0x00	; 0
 1be:	90 e0       	ldi	r25, 0x00	; 0
 1c0:	00 d0       	rcall	.+0      	; 0x1c2 <__vector_4+0x146>
 1c2:	80 91 00 00 	lds	r24, 0x0000
 1c6:	90 91 00 00 	lds	r25, 0x0000
 1ca:	4a e0       	ldi	r20, 0x0A	; 10
 1cc:	50 e0       	ldi	r21, 0x00	; 0
 1ce:	60 e0       	ldi	r22, 0x00	; 0
 1d0:	70 e0       	ldi	r23, 0x00	; 0
 1d2:	00 d0       	rcall	.+0      	; 0x1d4 <__vector_4+0x158>
 1d4:	80 e0       	ldi	r24, 0x00	; 0
 1d6:	90 e0       	ldi	r25, 0x00	; 0
 1d8:	00 d0       	rcall	.+0      	; 0x1da <__vector_4+0x15e>
 1da:	80 91 00 00 	lds	r24, 0x0000
 1de:	90 91 00 00 	lds	r25, 0x0000
 1e2:	4a e0       	ldi	r20, 0x0A	; 10
 1e4:	50 e0       	ldi	r21, 0x00	; 0
 1e6:	60 e0       	ldi	r22, 0x00	; 0
 1e8:	70 e0       	ldi	r23, 0x00	; 0
 1ea:	00 d0       	rcall	.+0      	; 0x1ec <__vector_4+0x170>
 1ec:	80 e0       	ldi	r24, 0x00	; 0
 1ee:	90 e0       	ldi	r25, 0x00	; 0
 1f0:	00 d0       	rcall	.+0      	; 0x1f2 <__vector_4+0x176>
 1f2:	80 e0       	ldi	r24, 0x00	; 0
 1f4:	90 e0       	ldi	r25, 0x00	; 0
 1f6:	00 d0       	rcall	.+0      	; 0x1f8 <__vector_4+0x17c>
 1f8:	10 92 00 00 	sts	0x0000, r1
 1fc:	10 92 00 00 	sts	0x0000, r1
 200:	10 92 00 00 	sts	0x0000, r1
 204:	10 92 00 00 	sts	0x0000, r1
 208:	8c e3       	ldi	r24, 0x3C	; 60
 20a:	84 bd       	out	0x24, r24	; 36
 20c:	36 9a       	sbi	0x06, 6	; 6
 20e:	ff 91       	pop	r31
 210:	ef 91       	pop	r30
 212:	bf 91       	pop	r27
 214:	af 91       	pop	r26
 216:	9f 91       	pop	r25
 218:	8f 91       	pop	r24
 21a:	7f 91       	pop	r23
 21c:	6f 91       	pop	r22
 21e:	5f 91       	pop	r21
 220:	4f 91       	pop	r20
 222:	3f 91       	pop	r19
 224:	2f 91       	pop	r18
 226:	0f 90       	pop	r0
 228:	0f be       	out	0x3f, r0	; 63
 22a:	0f 90       	pop	r0
 22c:	1f 90       	pop	r1
 22e:	18 95       	reti

00000230 <__vector_8>:
 230:	78 94       	sei
 232:	1f 92       	push	r1
 234:	0f 92       	push	r0
 236:	0f b6       	in	r0, 0x3f	; 63
 238:	0f 92       	push	r0
 23a:	11 24       	eor	r1, r1
 23c:	2f 93       	push	r18
 23e:	8f 93       	push	r24
 240:	9f 93       	push	r25
 242:	81 ec       	ldi	r24, 0xC1	; 193
 244:	9f ef       	ldi	r25, 0xFF	; 255
 246:	9d bd       	out	0x2d, r25	; 45
 248:	8c bd       	out	0x2c, r24	; 44
 24a:	80 91 00 00 	lds	r24, 0x0000
 24e:	90 91 00 00 	lds	r25, 0x0000
 252:	01 96       	adiw	r24, 0x01	; 1
 254:	90 93 00 00 	sts	0x0000, r25
 258:	80 93 00 00 	sts	0x0000, r24
 25c:	80 91 00 00 	lds	r24, 0x0000
 260:	88 23       	and	r24, r24
 262:	19 f0       	breq	.+6      	; 0x26a <__vector_8+0x3a>
 264:	81 30       	cpi	r24, 0x01	; 1
 266:	31 f5       	brne	.+76     	; 0x2b4 <__vector_8+0x84>
 268:	14 c0       	rjmp	.+40     	; 0x292 <__vector_8+0x62>
 26a:	80 91 00 00 	lds	r24, 0x0000
 26e:	90 91 00 00 	lds	r25, 0x0000
 272:	01 96       	adiw	r24, 0x01	; 1
 274:	90 93 00 00 	sts	0x0000, r25
 278:	80 93 00 00 	sts	0x0000, r24
 27c:	80 91 00 00 	lds	r24, 0x0000
 280:	90 91 00 00 	lds	r25, 0x0000
 284:	84 58       	subi	r24, 0x84	; 132
 286:	93 40       	sbci	r25, 0x03	; 3
 288:	a9 f4       	brne	.+42     	; 0x2b4 <__vector_8+0x84>
 28a:	81 e0       	ldi	r24, 0x01	; 1
 28c:	80 93 00 00 	sts	0x0000, r24
 290:	11 c0       	rjmp	.+34     	; 0x2b4 <__vector_8+0x84>
 292:	80 91 00 00 	lds	r24, 0x0000
 296:	90 91 00 00 	lds	r25, 0x0000
 29a:	01 97       	sbiw	r24, 0x01	; 1
 29c:	90 93 00 00 	sts	0x0000, r25
 2a0:	80 93 00 00 	sts	0x0000, r24
 2a4:	80 91 00 00 	lds	r24, 0x0000
 2a8:	90 91 00 00 	lds	r25, 0x0000
 2ac:	c2 97       	sbiw	r24, 0x32	; 50
 2ae:	11 f4       	brne	.+4      	; 0x2b4 <__vector_8+0x84>
 2b0:	10 92 00 00 	sts	0x0000, r1
 2b4:	80 91 00 00 	lds	r24, 0x0000
 2b8:	90 91 00 00 	lds	r25, 0x0000
 2bc:	9b bd       	out	0x2b, r25	; 43
 2be:	8a bd       	out	0x2a, r24	; 42
 2c0:	9f 91       	pop	r25
 2c2:	8f 91       	pop	r24
 2c4:	2f 91       	pop	r18
 2c6:	0f 90       	pop	r0
 2c8:	0f be       	out	0x3f, r0	; 63
 2ca:	0f 90       	pop	r0
 2cc:	1f 90       	pop	r1
 2ce:	18 95       	reti

000002d0 <ioinit>:
 2d0:	f8 94       	cli
 2d2:	81 e0       	ldi	r24, 0x01	; 1
 2d4:	8e bd       	out	0x2e, r24	; 46
 2d6:	87 e0       	ldi	r24, 0x07	; 7
 2d8:	85 bd       	out	0x25, r24	; 37
 2da:	82 e0       	ldi	r24, 0x02	; 2
 2dc:	88 bb       	out	0x18, r24	; 24
 2de:	1b bc       	out	0x2b, r1	; 43
 2e0:	1a bc       	out	0x2a, r1	; 42
 2e2:	87 bb       	out	0x17, r24	; 23
 2e4:	84 e4       	ldi	r24, 0x44	; 68
 2e6:	89 bf       	out	0x39, r24	; 57
 2e8:	85 b7       	in	r24, 0x35	; 53
 2ea:	83 7f       	andi	r24, 0xF3	; 243
 2ec:	85 bf       	out	0x35, r24	; 53
 2ee:	8b b7       	in	r24, 0x3b	; 59
 2f0:	8f 77       	andi	r24, 0x7F	; 127
 2f2:	8b bf       	out	0x3b, r24	; 59
 2f4:	78 94       	sei
 2f6:	08 95       	ret

000002f8 <main>:
 2f8:	c0 e0       	ldi	r28, 0x00	; 0
 2fa:	d0 e0       	ldi	r29, 0x00	; 0
 2fc:	de bf       	out	0x3e, r29	; 62
 2fe:	cd bf       	out	0x3d, r28	; 61
 300:	8b 98       	cbi	0x11, 3	; 17
 302:	93 9a       	sbi	0x12, 3	; 18
 304:	8c e0       	ldi	r24, 0x0C	; 12
 306:	00 d0       	rcall	.+0      	; 0x308 <main+0x10>
 308:	e3 df       	rcall	.-58     	; 0x2d0 <ioinit>
 30a:	00 d0       	rcall	.+0      	; 0x30c <main+0x14>
 30c:	84 ec       	ldi	r24, 0xC4	; 196
 30e:	87 b9       	out	0x07, r24	; 7
 310:	8c e9       	ldi	r24, 0x9C	; 156
 312:	86 b9       	out	0x06, r24	; 6
 314:	ff cf       	rjmp	.-2      	; 0x314 <main+0x1c>
