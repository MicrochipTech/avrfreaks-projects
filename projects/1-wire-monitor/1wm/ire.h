//#ifndef 1WIRE_H_
//#define 1WIRE_H_I

BYTE TxResetPuls(void);         // ������� ����� �� ����
BYTE TxPresenPuls(void);        // ������� ������ �� ����
BYTE TxByte(BYTE Data);         // �������� ����
BYTE Exeq1Wire(BYTE * Buff);

//#endif // 1WIRE_H_INCLUDED

