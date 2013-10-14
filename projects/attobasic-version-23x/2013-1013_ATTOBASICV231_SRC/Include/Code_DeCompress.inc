;**********************************************************************************
; Decompress function
; size_t decompress(void *s_start, void *d_start, size_t s_len, size_t d_len) {}
;**********************************************************************************
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r16
	push	r17
	push	r28
	push	r29
	movw	r26, r18

;	uint16_t *src = s_start;
	mov		r30, r24
	mov		r31, r25

;	uint16_t *dst = d_start;
	movw	r18, r22

;	uint16_t *s_end = (uint16_t *)s_start + s_len;
	movw	r14, r20
	add		r14, r14
	adc		r15, r15
	add		r14, r30
	adc		r15, r31

;	uint16_t *d_end = (uint16_t *)d_start + d_len;
	movw	r10, r26
	add		r10, r10
	adc		r11, r11
	add		r10, r22
	adc		r11, r23

;	uint16_t *cpy, copymap;
;	int copymask = 1 << (NBBY - 1);

;	if (s_len >= d_len) {
	cp		r20, r26
	cpc		r21, r27
	brcc	L6644      	; 0x6644 <decompress+0x3e>
	movw	r26, r22
	ldi		r18, 0x80	; 128
	ldi		r19, 0x00	; 0
	rjmp	L66d2    	; 0x66d2 <decompress+0xcc>
L6644:
	movw	r30, r24
	movw	r24, r26
	rjmp	L6658     	; 0x6658 <decompress+0x52>
;		size_t d_rem = d_len;
;		while (d_rem-- != 0)
;			*dst++ = *src++;
L664a:
	ld		r20, Z+
	ld		r21, Z+
	movw	r28, r18
	st		Y+, r20
	st		Y+, r21
	movw	r18, r28
	sbiw	r24, 0x01	; 1
;	uint16_t *cpy, copymap;
;	int copymask = 1 << (NBBY - 1);

;	if (s_len >= d_len) {
;		size_t d_rem = d_len;
;		while (d_rem-- != 0)
L6658:
	sbiw	r24, 0x00	; 0
	brne	L664a     	; 0x664a <decompress+0x44>
	rjmp	L66e8    	; 0x66e8 <decompress+0xe2>
;			*dst++ = *src++;
;		return (d_len);
;	}
;
;	while (src < s_end && dst < d_end) {
;		if ((copymask <<= 1) == (1 << NBBY)) {
L665e:
	add	r18, r18
	adc	r19, r19
	ldi	r20, 0x01	; 1
L6664:
	cpi	r18, 0x00	; 0
	cpc	r19, r20
	brne	L6672     	; 0x6672 <decompress+0x6c>

;			copymask = 1;
;			copymap = *src++;
	ld	r12, Z+
	ld	r13, Z+
;		return (d_len);
;	}

;	while (src < s_end && dst < d_end) {
;		if ((copymask <<= 1) == (1 << NBBY)) {
;			copymask = 1;
	ldi	r18, 0x01	; 1
	ldi	r19, 0x00	; 0
;			copymap = *src++;
;		}
;		if (copymap & copymask) {
L6672:
	movw	r24, r18
	and		r24, r12
	and		r25, r13
	ld		r16, Z
	ldd		r17, Z+1	; 0x01
	sbiw	r24, 0x00	; 0
;	breq	PC+76     	; 0x66cc <decompress+0xc6>
	breq	L66cc     	; 0x66cc <decompress+0xc6>

;			int mlen = (src[0] >> (NBBY - MATCH_BITS)) + MATCH_MIN;
;			int offset = ((src[0] << NBBY) | src[1]) & OFFSET_MASK;
	mov		r25, r16
	eor		r24, r24
	ldd		r20, Z+2	; 0x02
	ldd		r21, Z+3	; 0x03
	or		r24, r20
	or		r25, r21
	andi	r25, 0x03	; 3

;			src += 2;
	adiw	r30, 0x04	; 4

;			if ((cpy = dst - offset) >= (uint16_t *)d_start)
	add		r24, r24
	adc		r25, r25
	movw	r28, r26
	sub		r28, r24
	sbc		r29, r25
	movw	r24, r28
	cp		r28, r22
	cpc		r29, r23
	brcs	L66e0     	; 0x66e0 <decompress+0xda>
;		if ((copymask <<= 1) == (1 << NBBY)) {
;			copymask = 1;
;			copymap = *src++;
;		}
;		if (copymap & copymask) {
;			int mlen = (src[0] >> (NBBY - MATCH_BITS)) + MATCH_MIN;
	lsr		r17
	ror		r16
	lsr		r17
	ror		r16
	subi	r16, 0xFD	; 253
	sbci	r17, 0xFF	; 255
	rjmp	L66bc     	; 0x66bc <decompress+0xb6>
;			int offset = ((src[0] << NBBY) | src[1]) & OFFSET_MASK;
;			src += 2;
;			if ((cpy = dst - offset) >= (uint16_t *)d_start)
;				while (--mlen >= 0 && dst < d_end)
;					*dst++ = *cpy++;
L66b0:
	movw	r28, r24
	ld		r20, Y+	
	ld		r21, Y+
	movw	r24, r28
	st		X+, r20
	st		X+, r21

;		if (copymap & copymask) {
;			int mlen = (src[0] >> (NBBY - MATCH_BITS)) + MATCH_MIN;
;			int offset = ((src[0] << NBBY) | src[1]) & OFFSET_MASK;
;			src += 2;
;			if ((cpy = dst - offset) >= (uint16_t *)d_start)
;				while (--mlen >= 0 && dst < d_end)
L66bc:
	subi	r16, 0x01	; 1
	sbci	r17, 0x00	; 0
	sbrc	r17, 7
	rjmp	L66d2     	; 0x66d2 <decompress+0xcc>
	cp		r26, r10
	cpc		r27, r11
	brcs	L66b0     	; 0x66b0 <decompress+0xaa>
	rjmp	L66d2      	; 0x66d2 <decompress+0xcc>
;				 * offset before start of destination buffer
;				 * indicates corrupt source data
;				 */
;				return (dst - (uint16_t *)d_start);
;		} else {
;			*dst++ = *src++;
L66cc:
	st		X+, r16
	st		X+, r17
	adiw	r30, 0x02	; 2

;		while (d_rem-- != 0)
;			*dst++ = *src++;
;		return (d_len);
;	}

;	while (src < s_end && dst < d_end) {
L66d2:
	cp		r30, r14
	cpc		r31, r15
	brcc	PC+8      	; 0x66e0 <decompress+0xda>
	cp		r26, r10
	cpc		r27, r11
	brcc	L66E0      	; 0x66e0 <decompress+0xda>
	rjmp	L665E   	; 0x665e <decompress+0x58>
;				return (dst - (uint16_t *)d_start);
;		} else {
;			*dst++ = *src++;
;		}
;	}
;	return (dst - (uint16_t *)d_start);
L66e0:
	sub		r26, r22
	sbc		r27, r23
	asr		r27
	ror		r26
;}
L66e8:
	movw	r24, r26
	pop		r29
	pop		r28
	pop		r17
	pop		r16
	pop		r15
	pop		r14
	pop		r13
	pop		r12
	pop		r11
	pop		r10
	ret
