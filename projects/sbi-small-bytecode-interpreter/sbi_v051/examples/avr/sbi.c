/* ========================================================================== */
/*                                                                            */
/*   sbi.c                                                                    */
/*   (c) 2012 Gi@cky98                                                        */
/*                                                                            */
/* ========================================================================== */

#include "sbi.h"

#include "funclib.h"

// Debug functions
#define _debug(d)				debugn(d)
#define _error(d)				errorn(d)

// Variables, labels and subroutines
byte _t[VARIABLESNUM];
int* _labels;
int _returnaddresses[RETURNADDRESSESN];

// Interrupts
int* _interrupts;
byte _exec = 0;
byte _intinqueue = 0;
byte _queuedint = 0;

// User functions
int (*_sbifuncs[USERFUNCTIONSN])(byte[]);
byte _userfid = 0;

/*
	Gets the value of a parameter
 */
byte _getval(byte type, byte val)
{
	if (type==_varid) return _t[val]; else return val;
}

/*
	Sets the value of a variable
	Useful for user functions
		Return:
			0: 	All ok
			1: 	The specified parameter
			is not a variable
 */
byte _setval(byte type, byte num, byte val) 
{
	if (type==_varid)
	{
		_t[num]=val;
		return 0;
	} else {
		return 1;
	}
}

/*
	Common variables
 */
byte rd;
byte var1t;
byte var1;
byte var2t;
byte var2;
byte var3t;
byte var3;
byte b[16];
byte i;

/*
	Initializes the interpreter
 */
void _sbi_init(void)
{
	// Init
	_inituserfunc();
}

/*
	Begins program execution
 */
byte _sbi_begin(void)		// Returns:
												// 					0: 	No errors
												//					1: 	No function pointers for _getfch,
												//							_setfpos and _getfpos
												//					2:	Old version of executable format
												//					3:	Invalid program file
{
	// Check function pointers
	if ((_getfch==0)||(_setfpos==0)||(_getfpos==0)) return 1;
	
	// Read head
	if ((*_getfch)()!=HEADER_0) return 3;
	rd = (*_getfch)();
	if (rd!=HEADER_1) {
		if ((rd==0x1B)||(rd==0x2B)||(rd==0x3B))
			return 2;
		else
			return 3;
	}
	
	// Getting labels
	if ((*_getfch)()!=LABELSECTION) return 3;
	byte ln = (*_getfch)();
	_labels = malloc(ln * sizeof(int));
	byte c = 0;
	while (ln--)
	{
    _labels[c] = (*_getfch)() | ((*_getfch)() << 8);
    c++;
	}
	if ((*_getfch)()!=SEPARATOR) return 3;
	
	// Getting interrupts addresses
	if ((*_getfch)()!=INTERRUPTSSECTION) return 3;
	ln = (*_getfch)();
	_interrupts = malloc(ln * sizeof(int));
	c = 0;
	while (ln--)
	{
    _interrupts[c] = (*_getfch)() | ((*_getfch)() << 8);
    c++;
	}
	if ((*_getfch)()!=SEPARATOR) return 3;
	
	// Done
	return 0;
}

/*
	Executes the program
 */
byte _sbi_run(void) // Runs a SBI program
												// Returns:
												// 					0: 	No errors
												// 					1: 	Reached end (no exit found)
												//					2: 	Program exited
												//					3: 	Wrong instruction code
												//					4: 	Can't understand byte
												//					5: 	User error
{
	_exec = 1;
	
	rd = (*_getfch)();
	switch (rd)
	{
		case _istr_assign:
			var1 = (*_getfch)();
			_t[var1] = (*_getfch)();
			break;
		case _istr_move:
			var1 = (*_getfch)();
			_t[var1] = _t[(*_getfch)()];
			break;
		case _istr_add:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			var2t = (*_getfch)();
			var2 = (*_getfch)();
			_t[(*_getfch)()] = _getval(var1t, var1) + _getval(var2t, var2);
			break;
		case _istr_sub:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			var2t = (*_getfch)();
			var2 = (*_getfch)();
			_t[(*_getfch)()] = _getval(var1t, var1) - _getval(var2t, var2);
			break;
		case _istr_mul:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			var2t = (*_getfch)();
			var2 = (*_getfch)();
			_t[(*_getfch)()] = _getval(var1t, var1) * _getval(var2t, var2);
			break;
		case _istr_div:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			var2t = (*_getfch)();
			var2 = (*_getfch)();
			_t[(*_getfch)()] = _getval(var1t, var1) / _getval(var2t, var2);
			break;
		case _istr_incr:
			_t[(*_getfch)()]++;
			break;
		case _istr_decr:
			_t[(*_getfch)()]--;
			break;
		case _istr_inv:
			var1 = (*_getfch)();
			if (_t[var1]==0) _t[var1]=1; else _t[var1]=0;
			break;
		case _istr_tob:
			var1 = (*_getfch)();
			if (_t[var1]>0) _t[var1]=1; else _t[var1]=0;
			break;
		case _istr_cmp:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			var2t = (*_getfch)();
			var2 = (*_getfch)();
			if (_getval(var1t, var1)==_getval(var2t, var2)) _t[(*_getfch)()]=1; else _t[(*_getfch)()]=0;
			break;
		case _istr_high:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			var2t = (*_getfch)();
			var2 = (*_getfch)();
			if (_getval(var1t, var1)>_getval(var2t, var2)) _t[(*_getfch)()]=1; else _t[(*_getfch)()]=0;
			break;
		case _istr_low:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			var2t = (*_getfch)();
			var2 = (*_getfch)();
			if (_getval(var1t, var1)<_getval(var2t, var2)) _t[(*_getfch)()]=1; else _t[(*_getfch)()]=0;
			break;
		case _istr_jump:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			if ((*_getfch)() > 0)
			{
				for (i=RETURNADDRESSESN-2; i>0; i--) _returnaddresses[i+1] = _returnaddresses[i];
				_returnaddresses[1] = _returnaddresses[0];
				_returnaddresses[0] = (*_getfpos)();
			}
			(*_setfpos)(_labels[_getval(var1t, var1)]);
			break;
		case _istr_cmpjump:
			var1t = (*_getfch)();
			var1 = (*_getfch)();
			var2t = (*_getfch)();
			var2 = (*_getfch)();
			var3t = (*_getfch)();
			var3 = (*_getfch)();
			if ((*_getfch)() > 0)
			{
				for (i=RETURNADDRESSESN-2; i>0; i--) _returnaddresses[i+1] = _returnaddresses[i];
				_returnaddresses[1] = _returnaddresses[0];
				_returnaddresses[0] = (*_getfpos)();
			}
			if (_getval(var1t, var1)==_getval(var2t, var2))
			{
				(*_setfpos)(_labels[_getval(var3t, var3)]);
			}
			break;
		case _istr_ret:
			(*_setfpos)(_returnaddresses[0]);
			for (i=1; i<RETURNADDRESSESN; i++) _returnaddresses[i-1] = _returnaddresses[i];
			break;
		case _istr_debug:
			var1t = (*_getfch)();
			_debug(_getval(var1t, (*_getfch)()));
			break;
		case _istr_error:
			var1t = (*_getfch)();
			_error(_getval(var1t, (*_getfch)()));
			return 5;
			break;
		case _istr_sint:
			var1t = (*_getfch)();
			_userfid=_getval(var1t, (*_getfch)());
			break;
		case _istr_int:
			for (i=0; i<16; i++) b[i] = (*_getfch)();
			_sbifuncs[_userfid](b);
			break;
		case _istr_exit:
			return 2;
			break;
		case FOOTER_0:
			if ((*_getfch)()==FOOTER_1) return 1; else return 4;
		default:
			_error(0xB1);
			return 3;
			break;
	}
	
	_exec = 0;
	
	if (_intinqueue==1) _interrupt(_queuedint); // If there are interrupts in
																							// in the queue, do it
	
	return 0;
}

void _interrupt(byte id)
{
	if (_exec==1) // Some code in execution, queue interrupt
	{
		_intinqueue = 1;
		_queuedint = id;
		return;
	}
	
	for (i=RETURNADDRESSESN-2; i>0; i--) _returnaddresses[i+1] = _returnaddresses[i];
	_returnaddresses[1] = _returnaddresses[0];
	_returnaddresses[0] = (*_getfpos)();
	
	(*_setfpos)(_interrupts[id]); // Set the program counter to interrupt's address
	
	_intinqueue = 0; // Be sure to clean the queue
	
	return;
}
