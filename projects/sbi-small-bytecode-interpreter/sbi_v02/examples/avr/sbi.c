/* ========================================================================== */
/*                                                                            */
/*   sbi.c                                                                    */
/*   (c) 2012 Gi@cky98                                                        */
/*                                                                            */
/* ========================================================================== */

#include "sbi.h"

#include "funclib.h"

#define _debug(d)				debugn(d)

byte _t[VARIABLESNUM];
int _labels[LABELMAXNUM];

int _lastjump=0;
int _cursor = 0;
byte _lasterror = 0;

byte _userfid = 0;

void _error(byte id) // Causes and error (prints error data on debug stream)
{
	_debug(0xFF);
	_debug(id);
	_debug(0xFF);
	_lasterror = id;
	return;
}

byte _getval(byte type, byte val) // Gets the value of a parameter
{
	if (type==_varid) return _t[val]; else return val;
}

byte _setval(byte type, byte num, byte val) // 	Sets the value of a variable
																						//	Useful for userfunctions
																						//	Return:
																						//			0: 	All ok
																						//			1: 	The specified parameter
																						//					is not a variable
{
	if (type==_varid)
	{
		_t[num]=val;
		return 0;
	} else {
		return 1;
	}
}

void _sbi_begin(void)
{
	// Init
	_inituserfunc();
}

byte _sbi_run(void) // Runs a SBI program
												// Returns:
												// 					0: 	Reached end (no exit found)
												//					1: 	Program exited
												//					2: 	Program error
												//					3: 	User custom error
												//					4: 	No function pointers for _getfch
												//							and _setfpos
												//					5:	Old version of executable format
{
	// Vars
	byte rd;
	byte var1t;
	byte var1;
	byte var2t;
	byte var2;
	byte var3t;
	byte var3;
	byte b[16];
	byte i;
	byte _xt;
	
	_cursor=0;
	
	// Check function pointers
	if ((_getfch==0)||(_setfpos==0)) return 4;
	
	// Read head
	rd = (*_getfch)(); _cursor++;
	if (rd!=0xAA) return 2;
	rd = (*_getfch)(); _cursor++;
	if (rd!=0x2B) {
		if (rd==0x1B)
			return 5;
		else
			return 2;
	}
	
	// Scanning labels
	_cursor=2;
	_xt=0;
	(*_setfpos)(_cursor);
	while (_xt==0)
	{
		rd = (*_getfch)(); _cursor++;
		switch (rd)
		{
			case _istr_assign:
				_cursor+=2;
				break;
			case _istr_move:
				_cursor+=2;
				break;
			case _istr_add:
				_cursor+=5;
				break;
			case _istr_sub:
				_cursor+=5;
				break;
			case _istr_mul:
				_cursor+=5;
				break;
			case _istr_div:
				_cursor+=5;
				break;
			case _istr_incr:
				_cursor++;
				break;
			case _istr_decr:
				_cursor++;
				break;
			case _istr_inv:
				_cursor++;
				break;
			case _istr_tob:
				_cursor++;
				break;
			case _istr_cmp:
				_cursor+=5;
				break;
			case _istr_high:
				_cursor+=5;
				break;
			case _istr_low:
				_cursor+=5;
				break;
			case _istr_label:
				var1 = (*_getfch)(); _cursor++;
				_labels[var1] = _cursor;
				break;
			case _istr_jump:
				_cursor+=2;
				break;
			case _istr_cmpjump:
				_cursor+=6;
				break;
			case _istr_ret:
				// Nothing
				break;
			case _istr_debug:
				_cursor+=2;
				break;
			case _istr_error:
				_cursor+=2;
				return 3;
				break;
			case _istr_sint:
				_cursor+=2;
				break;
			case _istr_int:
				_cursor+=16;
				break;
			case _istr_exit:
				_xt=1;
				break;
			default:
				_error(0xB1);
				return 2;
				break;
		}
		(*_setfpos)(_cursor);
	}
	
	// Running code
	_cursor=2;
	(*_setfpos)(_cursor);
	while (1)
	{
		rd = (*_getfch)(); _cursor++;
		switch (rd)
		{
			case _istr_assign:
				var1 = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				_t[var1] = var2;
				break;
			case _istr_move:
				var1 = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				_t[var1] = _t[var2];
				break;
			case _istr_add:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				var2t = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				var3 = (*_getfch)(); _cursor++;
				_t[var3] = _getval(var1t, var1) + _getval(var2t, var2);
				break;
			case _istr_sub:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				var2t = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				var3 = (*_getfch)(); _cursor++;
				_t[var3] = _getval(var1t, var1) - _getval(var2t, var2);
				break;
			case _istr_mul:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				var2t = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				var3 = (*_getfch)(); _cursor++;
				_t[var3] = _getval(var1t, var1) * _getval(var2t, var2);
				break;
			case _istr_div:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				var2t = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				var3 = (*_getfch)(); _cursor++;
				_t[var3] = _getval(var1t, var1) / _getval(var2t, var2);
				break;
			case _istr_incr:
				var1 = (*_getfch)(); _cursor++;
				_t[var1]++;
				break;
			case _istr_decr:
				var1 = (*_getfch)(); _cursor++;
				_t[var1]--;
				break;
			case _istr_inv:
				var1 = (*_getfch)(); _cursor++;
				if (_t[var1]==0) _t[var1]=1; else _t[var1]=0;
				break;
			case _istr_tob:
				var1 = (*_getfch)(); _cursor++;
				if (_t[var1]>0) _t[var1]=1; else _t[var1]=0;
				break;
			case _istr_cmp:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				var2t = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				var3 = (*_getfch)(); _cursor++;
				if (_getval(var1t, var1)==_getval(var2t, var2)) _t[var3]=1; else _t[var3]=0;
				break;
			case _istr_high:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				var2t = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				var3 = (*_getfch)(); _cursor++;
				if (_getval(var1t, var1)>_getval(var2t, var2)) _t[var3]=1; else _t[var3]=0;
				break;
			case _istr_low:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				var2t = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				var3 = (*_getfch)(); _cursor++;
				if (_getval(var1t, var1)<_getval(var2t, var2)) _t[var3]=1; else _t[var3]=0;
				break;
			case _istr_label:
				var1 = (*_getfch)(); _cursor++;
				_labels[var1] = _cursor;
				break;
			case _istr_jump:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				_lastjump = _cursor;
				_cursor = _labels[_getval(var1t, var1)];
				(*_setfpos)(_cursor);
				break;
			case _istr_cmpjump:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				var2t = (*_getfch)(); _cursor++;
				var2 = (*_getfch)(); _cursor++;
				var3t = (*_getfch)(); _cursor++;
				var3 = (*_getfch)(); _cursor++;
				if (_getval(var1t, var1)==_getval(var2t, var2))
				{
					_cursor = _labels[_getval(var3t, var3)];
					(*_setfpos)(_cursor);
				}
				break;
			case _istr_ret:
				_cursor = _lastjump;
				(*_setfpos)(_cursor);
				break;
			case _istr_debug:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				_debug(_getval(var1t, var1));
				break;
			case _istr_error:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				_error(_getval(var1t, var1));
				return 3;
				break;
			case _istr_sint:
				var1t = (*_getfch)(); _cursor++;
				var1 = (*_getfch)(); _cursor++;
				_userfid=_getval(var1t, var1);
				break;
			case _istr_int:
				for (i=0; i<16; i++) { b[i] = (*_getfch)(); _cursor++; }
				_sbifuncs[_userfid](b);
				break;
			case _istr_exit:
				return 1;
				break;
			default:
				_error(0xB1);
				return 2;
				break;
		}
	}
	
	return 0;
}
