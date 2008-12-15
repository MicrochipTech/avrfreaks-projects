// Comms.h

#ifndef COMMS_H
#define COMMS_H

#ifndef COMMS

extern void OpenComPort();
extern void CloseComPort();
extern bool ComPortOpen();
extern bool ComPortExists(int port);
extern BOOL TxChar(BYTE data);
extern BOOL Tx(BYTE * buffer, int count);
extern bool Rx(int count);
extern void SetDTR();
extern void ClearDTR();
extern void PurgeBuffers();

#else

void OpenComPort(int port);
void CloseComPort();
bool ComPortOpen();
bool ComPortExists(int port);
BOOL TxChar(BYTE data);
BOOL Tx(BYTE * buffer, int count);
bool Rx(int count);
void SetDTR();
void ClearDTR();
void PurgeBuffers();

#endif
#endif
