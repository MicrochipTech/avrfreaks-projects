#include <stdint.h>

void mulacc_16_64(uint16_t p, uint16_t q, uint64_t *r) {
	asm (
		"clr	r2" "\n\t"

		"ld	r16, Z" "\n\t"
		"ldd r17, Z+1" "\n\t"
		"ldd r18, Z+2" "\n\t"
		"ldd r19, Z+3" "\n\t"

		"mul	%A[p], %A[q]		; al * bl" "\n\t"
		"add	r16, r0" "\n\t"
		"st	Z, r16" "\n\t"
		"clr	r16" "\n\t"
		"adc	r17, r1" "\n\t"
		"adc	r18, r2" "\n\t"
		"adc	r19, r2" "\n\t"
		"adc	r16, r2" "\n\t"

		"mul	%B[p], %B[q]		; ah * bh" "\n\t"
		"add	r18, r0" "\n\t"
		"adc	r19, r1" "\n\t"
		"adc	r16, r2" "\n\t"

		"mul	%B[p], %A[q]		; ah * bl" "\n\t"
		"add	r17, r0" "\n\t"
		"adc	r18, r1" "\n\t"
		"adc	r19, r2" "\n\t"
		"adc	r16, r2" "\n\t"

		"mul	%B[q], %A[p]		; bh * al" "\n\t"
		"add	r17, r0" "\n\t"
		"adc	r18, r1" "\n\t"
		"adc	r19, r2" "\n\t"
		"adc	r16, r2" "\n\t"

		"std	Z+1, r17" "\n\t"
		"std	Z+2, r18" "\n\t"
		"std	Z+3, r19" "\n\t"

		"ldd r19, Z+4" "\n\t"
		"add	r19, r16" "\n\t"
		"std	Z+4, r19" "\n\t"

		"brcc	nocarry_mulacc_16_64" "\n\t"

		"ldd r19, Z+5" "\n\t"
		"adc	r19, r2" "\n\t"
		"std	Z+5, r19" "\n\t"

		"brcc	nocarry_mulacc_16_64" "\n\t"

		"ldd r19, Z+6" "\n\t"
		"adc	r19, r2" "\n\t"
		"std	Z+6, r19" "\n\t"

		"brcc	nocarry_mulacc_16_64" "\n\t"

		"ldd r19, Z+7" "\n\t"
		"adc	r19, r2" "\n\t"
		"std	Z+7, r19" "\n\t"

		"nocarry_mulacc_16_64:" "\n\t"

		"clr	__zero_reg__" "\n\t"
		: 
		: [p] "r" (p), [q] "r" (q), "z" (r)
		: "r0", "r1", "r2", "r16", "r17", "r18", "r19"
	);
	return;
}

void mulacc_12_64(uint16_t p, uint16_t q, uint64_t *r) {
	asm (
		"clr	r2" "\n\t"

		"ld	r16, Z" "\n\t"
		"ldd r17, Z+1" "\n\t"
		"ldd r18, Z+2" "\n\t"

		"mul	%A[p], %A[q]		; al * bl" "\n\t"
		"add	r16, r0" "\n\t"
		"st	Z, r16" "\n\t"
		"clr	r16" "\n\t"
		"adc	r17, r1" "\n\t"
		"adc	r18, r2" "\n\t"
		"adc	r16, r2" "\n\t"

		"mul	%B[p], %B[q]		; ah * bh" "\n\t"
		"add	r18, r0" "\n\t"
		"adc	r16, r2" "\n\t"

		"mul	%B[p], %A[q]		; ah * bl" "\n\t"
		"add	r17, r0" "\n\t"
		"adc	r18, r1" "\n\t"
		"adc	r16, r2" "\n\t"

		"mul	%B[q], %A[p]		; bh * al" "\n\t"
		"add	r17, r0" "\n\t"
		"adc	r18, r1" "\n\t"
		"adc	r16, r2" "\n\t"

		"std	Z+1, r17" "\n\t"
		"std	Z+2, r18" "\n\t"

		"ldd r18, Z+3" "\n\t"
		"add	r18, r16" "\n\t"
		"std	Z+3, r18" "\n\t"

		"brcc	nocarry_mulacc_12_64" "\n\t"

		"ldd r18, Z+4" "\n\t"
		"adc	r18, r2" "\n\t"
		"std	Z+4, r18" "\n\t"

		"brcc	nocarry_mulacc_12_64" "\n\t"

		"ldd r18, Z+5" "\n\t"
		"adc	r18, r2" "\n\t"
		"std	Z+5, r18" "\n\t"

		"brcc	nocarry_mulacc_12_64" "\n\t"

		"ldd r18, Z+6" "\n\t"
		"adc	r18, r2" "\n\t"
		"std	Z+6, r18" "\n\t"

		"brcc	nocarry_mulacc_12_64" "\n\t"

		"ldd r18, Z+7" "\n\t"
		"adc	r18, r2" "\n\t"
		"std	Z+7, r18" "\n\t"

		"nocarry_mulacc_12_64:" "\n\t"

		"clr	__zero_reg__" "\n\t"
		: 
		: [p] "r" (p), [q] "r" (q), "z" (r)
		: "r0", "r1", "r2", "r16", "r17", "r18"
	);
	return;
}

void sqracc_12_64(uint16_t p, uint64_t *r) {
	asm (
		"clr	r2" "\n\t"

		"ld	r16, Z" "\n\t"
		"ldd r17, Z+1" "\n\t"
		"ldd r18, Z+2" "\n\t"

		"mul	%A[p], %A[p]		; al * bl" "\n\t"
		"add	r16, r0" "\n\t"
		"st	Z, r16" "\n\t"
		"clr	r16" "\n\t"
		"adc	r17, r1" "\n\t"
		"adc	r18, r2" "\n\t"
		"adc	r16, r2" "\n\t"

		"mul	%B[p], %B[p]		; ah * bh" "\n\t"
		"add	r18, r0" "\n\t"
		"adc	r16, r2" "\n\t"

		"mul	%B[p], %A[p]		; ah * bl" "\n\t"
		"add	r17, r0" "\n\t"
		"adc	r18, r1" "\n\t"
		"adc	r16, r2" "\n\t"
		"add	r17, r0" "\n\t"
		"adc	r18, r1" "\n\t"
		"adc	r16, r2" "\n\t"

		"std	Z+1, r17" "\n\t"
		"std	Z+2, r18" "\n\t"

		"ldd r18, Z+3" "\n\t"
		"add	r18, r16" "\n\t"
		"std	Z+3, r18" "\n\t"

		"brcc	nocarry_sqracc_12_64" "\n\t"

		"ldd r18, Z+4" "\n\t"
		"adc	r18, r2" "\n\t"
		"std	Z+4, r18" "\n\t"

		"brcc	nocarry_sqracc_12_64" "\n\t"

		"ldd r18, Z+5" "\n\t"
		"adc	r18, r2" "\n\t"
		"std	Z+5, r18" "\n\t"

		"brcc	nocarry_mulacc_12_64" "\n\t"

		"ldd r18, Z+6" "\n\t"
		"adc	r18, r2" "\n\t"
		"std	Z+6, r18" "\n\t"

		"brcc	nocarry_mulacc_12_64" "\n\t"

		"ldd r18, Z+7" "\n\t"
		"adc	r18, r2" "\n\t"
		"std	Z+7, r18" "\n\t"

		"nocarry_sqracc_12_64:" "\n\t"

		"clr	__zero_reg__" "\n\t"
		: 
		: [p] "r" (p), "z" (r)
		: "r0", "r1", "r2", "r16", "r17", "r18"
	);
	return;
}
