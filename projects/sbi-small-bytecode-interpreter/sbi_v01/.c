/* ========================================================================== */
/*                                                                            */
/*   sbi.c                                                                    */
/*   (c) 2012 Gi@cky98                                                        */
/*                                                                            */
/* ========================================================================== */

#include "sbi.h"

#include "funclib.h"

#define _debug(d)				debugn(d)

byte _t[64];
int _labels[32];
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

byte _intbts(byte b[8]) // Interprets an instruction (8 bytes)
												// Returns:
												// 					0: All ok
												//					1: Program exited
												//					2: Program error
												//					3: User custom error
{
	if (b[7]!=0xBA) { _error(0xA0); return 2; }
	switch (b[0])
	{
		case _istr_assign:
			if ((b[1]!=_varid)&&(b[3]!=_value)) { _error(0x01); return 2; }
			_t[b[2]] = b[4];
			break;
		case _istr_move:
			if ((b[1]!=_varid)&&(b[3]!=_varid)) { _error(0x02); return 2; }
			_t[b[4]] = _t[b[2]];
			break;
		case _istr_add:
			if (b[5]!=_varid) { _error(0x03); return 2; }
			_t[b[6]] = _getval(b[1], b[2]) + _getval(b[3], b[4]);
			break;
		case _istr_sub:
			if (b[5]!=_varid) { _error(0x04); return 2; }
			_t[b[6]] = _getval(b[1], b[2]) - _getval(b[3], b[4]);
			break;
		case _istr_mul:
			if (b[5]!=_varid) { _error(0x05); return 2; }
			_t[b[6]] = _getval(b[1], b[2]) * _getval(b[3], b[4]);
			break;
		case _istr_div:
			if (b[5]!=_varid) { _error(0x06); return 2; }
			_t[b[6]] = _getval(b[1], b[2]) / _getval(b[3], b[4]);
			break;
		case _istr_incr:
			_t[b[2]]++;
			break;
		case _istr_decr:
			_t[b[2]]--;
			break;
		case _istr_inv:
			if (b[1]!=_varid) { _error(0x07); return 2; }
			if (_t[b[2]]==0) _t[b[2]]=1; else _t[b[2]]=0;
			break;
		case _istr_tob:
			if (b[1]!=_varid) { _error(0x08); return 2; }
			if (_t[b[2]]>0) _t[b[2]]=1; else _t[b[2]]=0;
			break;
		case _istr_cmp:
			if (b[5]!=_varid) { _error(0x09); return 2; }
			if (_getval(b[1], b[2])==_getval(b[3], b[4])) _t[b[6]]=1; else _t[b[6]]=0;
			break;
		case _istr_high:
			if (b[5]!=_varid) { _error(0x0A); return 2; }
			if (_getval(b[1], b[2])>_getval(b[3], b[4])) _t[b[6]]=1; else _t[b[6]]=0;
			break;
		case _istr_low:
			if (b[5]!=_varid) { _error(0x0B); return 2; }
			if (_getval(b[1], b[2])<_getval(b[3], b[4])) _t[b[6]]=1; else _t[b[6]]=0;
			break;
		case _istr_label:
			_labels[_getval(b[1], b[2])] = _cursor;
			break;
		case _istr_jump:
			_lastjump = _cursor;
			_cursor = _labels[_getval(b[1], b[2])];
			break;
		case _istr_cmpjump:
			if (_getval(b[1], b[2])==_getval(b[3], b[4])) _cursor = _labels[_getval(b[5], b[6])];
			break;
		case _istr_ret:
			_cursor = _lastjump;
			break;
		case _istr_debug:
			_debug(_getval(b[1], b[2]));
			break;
		case _istr_error:
			_error(_getval(b[1], b[2]));
			return 3;
			break;
		case _istr_sint:
			_userfid=_getval(b[1], b[2]);
			break;
		case _istr_int:
			_sbifuncs[_userfid](_getval(b[1], b[2]), _getval(b[3], b[4]), _getval(b[5], b[6]));
			break;
		case _istr_exit:
			return 1;
			break;
		default:
			_error(0xB1);
			return 2;
			break;
	}
	return 0;
}

byte _scanbts(byte b[8]) // Scan the executable in search of labels
												 // Return:
												 //					0: Label found
												 //         1: No label found
												 //         2: Exited (no errors)
												 //					3: Wrong instruction code
{
	if (b[7]!=0xBA) { _error(0xA0); return 2; }
	switch (b[0])
	{
		case _istr_label:
			_labels[_getval(b[1], b[2])] = _cursor;
			break;
		case _istr_exit:
			return 2;
			break;
		default:
			return 1;
			break;
	}
	return 0;
}
