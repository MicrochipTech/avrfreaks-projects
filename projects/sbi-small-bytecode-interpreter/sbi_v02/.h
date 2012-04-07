/* ========================================================================== */
/*                                                                            */
/*   sbi.h                                                                    */
/*   (c) 2012 Gi@cky98                                                        */
/*                                                                            */
/* ========================================================================== */

#ifndef _SBI_H
	#define _SBI_H
	
	#define VARIABLESNUM				64
	#define LABELMAXNUM					32
	#define USERFUNCTIONSN			32
	
	typedef unsigned char byte;
	
	#define HEADER_0						0xAA
	#define HEADER_1						0x1B
	
	#define SEPARATOR						0xA3
	
	#define FOOTER_0						0x3A
	#define FOOTER_1						0xF0
	
	#define _istr_assign				0x01
	#define _istr_move					0x02
	#define _istr_add						0x10
	#define _istr_sub						0x11
	#define _istr_mul						0x12
	#define _istr_div						0x13
	#define _istr_incr					0x20
	#define _istr_decr					0x21
	#define _istr_inv						0x22
	#define _istr_tob						0x23
	#define _istr_cmp						0x30
	#define _istr_high					0x31
	#define _istr_low						0x32
	#define _istr_label					0x40
	#define _istr_jump					0x41
	#define _istr_cmpjump				0x42
	#define	_istr_ret						0x43
	#define _istr_debug					0x50
	#define _istr_debugstr			0x51
	#define _istr_error					0x52
	#define _istr_sint					0x60
	#define _istr_int						0x61
	#define _istr_exit					0xFF
	
	#define _varid							0x04
	#define _value							0xF4
	
	int (*_sbifuncs[USERFUNCTIONSN])(byte[]);
	
	extern byte _t[VARIABLESNUM];
	extern int _labels[LABELMAXNUM];
	
	extern int _cursor;
	
	extern byte _lasterror;
	
	byte (*_getfch)(void);
	void (*_setfpos)(int);
	
	byte _getval(byte type, byte val);
	byte _setval(byte type, byte num, byte val);
	
	void _sbi_begin(void);
	byte _sbi_run(void);
	
#endif
