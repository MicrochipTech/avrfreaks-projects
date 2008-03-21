//#ifndef 1WIRE_H_
//#define 1WIRE_H_I

BYTE TxResetPuls(void);         // сделать Ресет на шине
BYTE TxPresenPuls(void);        // сделать пресет на шине
BYTE TxByte(BYTE Data);         // передать байт
BYTE Exeq1Wire(BYTE * Buff);

//#endif // 1WIRE_H_INCLUDED

