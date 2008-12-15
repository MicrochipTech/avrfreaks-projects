// Comms.cpp

#define COMMS

#include "stdafx.h"
#include "Comms.h"

#include "Globals.h"

_COMMTIMEOUTS timeouts;
OVERLAPPED overlapped;
HANDLE idComDev = NULL;
int comPort = 0;

void OpenComPort()
{
	char buffer[1000];
	_DCB dcb;
	DWORD comError;

	if (comPort != 0 && project->selectedComPort != comPort)
		CloseComPort();					// Close ComPort

	if (project->selectedComPort == comPort)
		return;							// Already open!!
	
	strcpy(buffer, "COM");
	itoa(project->selectedComPort, &buffer[3], 10);

#ifdef COMMSDEBUG
debug<<"Comms.cpp: OpenCommPort(): Opening "<<buffer<<endl;
#endif
	
	idComDev = CreateFile(buffer, GENERIC_READ|GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL/*FILE_FLAG_OVERLAPPED*/, 0); // Open the Comm port
	if (idComDev == INVALID_HANDLE_VALUE)
	{									// If OpenFile failed
		switch (GetLastError())			// Switch on error
		{
			case 2:
				MessageBox(NULL, "Port Not Found", "Comms Error",
					MB_OK|MB_ICONINFORMATION); // Display the Error
				break;
			case 5:
			case 32:
				MessageBox(NULL, "Access Denied, Port in use.", "Comms Error",
					MB_OK|MB_ICONINFORMATION); // Display the Error
				break;
			default:
				break;
		}
		return;
	}
	
	SetupComm(idComDev, 65536, 1024);	// BIG input buffer!!

	GetCommState(idComDev, &dcb);
//	dcb.DCBlength = sizeof(_DCB);
	dcb.BaudRate = project->selectedBaud;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = TRUE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = TRUE;
//	dcb.fDummy2;
	dcb.wReserved = 0;
	dcb.XonLim = 1024;
	dcb.XoffLim = 1024;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
//	dcb.XonChar;
//	dcb.XoffChar;
//	dcb.ErrorChar;
//	dcb.EofChar;
//	dcb.EvtChar;
//	dcb.wReserved1;

	if (!SetCommState(idComDev, &dcb))
	{
		comError = GetLastError();
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, comError, 0,
			&buffer[500], 500, NULL);
		strcpy(buffer, "Set Com Port State error:\r\n");
		strcat(buffer, &buffer[500]);
		MessageBox(NULL, buffer, "Comms Error",
			MB_OK|MB_ICONINFORMATION);	// Display the Error
		CloseHandle(idComDev);			// Close the Comm port
		return;
	}

//	SetCommMask(idComDev, EV_RXCHAR|EV_TXEMPTY);

	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0; //10;
	timeouts.WriteTotalTimeoutConstant = 0; //50;
	SetCommTimeouts(idComDev, &timeouts);

	overlapped.hEvent = NULL;
	overlapped.Offset = 0;
	overlapped.OffsetHigh = 0;

	PurgeComm(idComDev, PURGE_TXCLEAR);	// Flush transmit queue
	PurgeComm(idComDev, PURGE_RXCLEAR);	// Flush receive queue
	
	totalRxd = 0;

	comPort = project->selectedComPort;
	dataIndex = 0;
}


void CloseComPort()
{
	if (comPort)
	{
		CloseHandle(idComDev);
		comPort = 0;
	}
}


bool ComPortOpen()
{
	if (comPort)
		return true;
	return false;
}


bool ComPortExists(int port)
{
	HANDLE handle = NULL;
	char buffer[1000];
	bool exists = false;
	DWORD comError;

	strcpy(buffer, "COM");
	itoa(port, &buffer[3], 10);
	handle = CreateFile(buffer, GENERIC_READ|GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); // Open the Comm port
	if (handle == INVALID_HANDLE_VALUE)
	{									// If OpenFile failed
		comError = GetLastError();
#ifdef COMDEBUG
debug<<"ComPortExists():"<<endl;
debug<<"   ("<<(int)comError<<") ";
FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, comError, 0,
	&buffer[500], 500, NULL);
strcat(buffer, ": ");
strcat(buffer, &buffer[500]);
debug<<buffer<<endl;
#endif
		
		switch (comError)				// Switch on error
		{
			case 2:						// Port not found
				exists = false;
				break;
			case 5:						// Port in use
			case 32:
				exists = true;
				break;
			default:
				break;
		}
	}
	else
	{
		exists = true;
		CloseHandle(handle);
	}
	return exists;
}


bool Rx(int count)
{
#ifdef COMDEBUG
debug<<"Rx(): ->"<<endl;
#endif

//	char message[1000];
	DWORD dwBytesLeft = count;
	DWORD dwBytesToRead;
	DWORD dwBytesRead;
	DWORD dwBlock = 256;
	DWORD dwCommsError;
	BOOL bResult;

	rxCount = 0;
	if (!comPort)
		return false;

/*	do
	{
		bResult = ReadFile(idComDev, &rxBuffer[dataIndex+rxCount],
			dwBytesToRead, &dwBytesRead, &overlapped);
		if (!bResult)
		{								// If ReadFile() failed
			dwCommsError = GetLastError();
			if (dwCommsError == ERROR_IO_PENDING)
			{
				bResult = GetOverlappedResult(idComDev, &overlapped, 
					&dwBytesRead, TRUE); 
			}
		}
		if (dwBytesRead)				// If some bytes were received
		{
			dwBytesToRead -= dwBytesRead; // Calc remaining bytes to read

			totalRxd += dwBytesRead;	// Add to the total counter (DEBUG)
#ifdef COMDEBUG
int i = rxBuffer[dataIndex+rxCount];
debug<<"Rx(): rxBuffer["<<dataIndex+rxCount<<"] = "<<i;
if (i > 32)
	debug<<" ("<<(char)i<<")";
debug<<endl;
debug<<"Rx(): totalRxd = "<<totalRxd<<endl;
#endif
			rxCount += dwBytesRead;		// Add to the byte count
			if (count && (rxCount == count))
				return true;			// Received the asked for amount
			if (dataIndex+rxCount == 65536+1) // Don't pop the buffer
			{
#ifdef COMDEBUG
debug<<"Rx(): Popped the Buffer"<<endl;
#endif
				return false;			// Popped the buffer
			}
		}
	}
	while (dwBytesToRead);
*/

	do
	{
		if (dwBytesLeft && dwBytesLeft < dwBlock)
			dwBytesToRead = dwBytesLeft;
		else
			dwBytesToRead = dwBlock;

		bResult = ReadFile(idComDev, &rxBuffer[dataIndex+rxCount],
			dwBytesToRead, &dwBytesRead, NULL); //&overlapped))
		if (!bResult)
		{
			ClearCommError(idComDev, &dwCommsError, NULL);
/*			switch (dwCommsError)
			{
				case CE_BREAK:
					strcpy(message,
						"The hardware detected a break condition.");
					break;
				case CE_FRAME:
					strcpy(message,
						"The hardware detected a framing error.");
					break;
				case CE_OVERRUN:
					strcpy(message,
						"A character-buffer overrun has occurred.\r\n\
The next character is lost.");
					break;
				case CE_RXOVER:
					strcpy(message,
						"An input buffer overflow has occurred.\r\n\
There is either no room in the input buffer,\r\n\
or a character was received after the end-of-file (EOF) character.");
					break;
				case CE_RXPARITY:
					strcpy(message, "The hardware detected a parity error.");
					break;
				case CE_TXFULL:
					strcpy(message,
						"The application tried to transmit a character,\r\n\
but the output buffer was full.");
					break;
				default:
					strcpy(message, "Unforseen Comms Error\r\n\
Error Code = ");
					itoa(dwCommsError, &message[strlen(message)], 10);
					break;
			}
			::MessageBox(NULL, message, "Rx(): Comms Error", MB_OK|MB_ICONSTOP);
*/
		}
		if (dwBytesRead)
		{
			dwBytesLeft -= dwBytesRead;

			totalRxd += dwBytesRead;
#ifdef COMDEBUG
int i = rxBuffer[dataIndex+rxCount];
debug<<"Rx(): rxBuffer["<<dataIndex+rxCount<<"] = "<<i;
if (i > 32)
	debug<<" ("<<(char)i<<")";
debug<<endl;
debug<<"Rx(): totalRxd = "<<totalRxd<<endl;
#endif
			rxCount += dwBytesRead;
			if (count && (rxCount == count))
				return true;			// Received the asked for amount
			if (dataIndex+rxCount == 65536+1) // Don't pop the buffer
				return false;			// Popped the buffer
		}
	}
	while (dwBytesRead);

//	while (ReadFile(idComDev, &rxBuffer[dataIndex+rxCount], 1, &read,
//		&overlapped))
//	{
//		if (read)
//		{
//			totalRxd++;
//#ifdef COMDEBUG
//int i = rxBuffer[dataIndex+rxCount];
//debug<<"Rx(): rxBuffer["<<dataIndex+rxCount<<"] = "<<i;
//if (i > 32)
//	debug<<" ("<<(char)i<<")";
//debug<<endl;
//debug<<"Rx(): totalRxd = "<<totalRxd<<endl;
//#endif
//			rxCount++;
//			if (count && rxCount == count)
//				return true;			// Received the asked for amount
//			if (dataIndex+rxCount == 65536+1) // Don't pop the buffer
//				return false;			// Popped the buffer
//		}
//		else
//			break;
//	}
	if (rxCount)
		return true;					// Received bytes
	return false;						// Received none
}


BOOL TxChar(BYTE data)
{
	BYTE txChar = data;
//	char message[1000];
	DWORD dwBytesWritten;
	DWORD dwCommsError;
	BOOL bResult;

	if (!comPort)
		return false;

/*	bResult = WriteFile(idComDev, &txChar, 1, &dwBytesWritten, &overlapped);
	if (!bResult)
	{
		dwCommsError = GetLastError();
		if (dwCommsError == ERROR_IO_PENDING)
		{
			bResult = GetOverlappedResult(idComDev, &overlapped, 
				&dwBytesWritten, TRUE);
			if (bResult == TRUE)
				return true;
		}
	}
	else
		return true;
*/
	
	bResult = WriteFile(idComDev, &txChar, 1, &dwBytesWritten, NULL); //&overlapped);
	if (bResult)
		return TRUE;

	ClearCommError(idComDev, &dwCommsError, NULL);
/*	switch (dwCommsError)
	{
		case 0:
			FormatMessage( 
			    FORMAT_MESSAGE_FROM_SYSTEM | 
			    FORMAT_MESSAGE_IGNORE_INSERTS,
			    NULL, GetLastError(),
			    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			    message, 1000, NULL);
			break;
		case CE_BREAK:
			strcpy(message,
				"The hardware detected a break condition.");
			break;
		case CE_FRAME:
			strcpy(message,
				"The hardware detected a framing error.");
			break;
		case CE_IOE:
			strcpy(message, "CE_IOE\r\n\
An I/O error occurred during communications with the device.");
			break;
		case CE_MODE:
			strcpy(message, "CE_MODE\r\n\
The requested mode is not supported,\r\n\
or the hFile parameter is invalid.\r\n\
If this value is specified, it is the only valid error.");
			break;
		case CE_OVERRUN:
			strcpy(message,
				"A character-buffer overrun has occurred.\r\n\
The next character is lost.");
			break;
		case CE_RXOVER:
			strcpy(message,
				"An input buffer overflow has occurred.\r\n\
There is either no room in the input buffer,\r\n\
or a character was received after the end-of-file (EOF) character.");
			break;
		case CE_RXPARITY:
			strcpy(message, "The hardware detected a parity error.");
			break;
		case CE_TXFULL:
			strcpy(message,
				"The application tried to transmit a character,\r\n\
but the output buffer was full.");
			break;
		default:
			strcpy(message, "Unforseen Comms Error\r\n\
Error Code = ");
			itoa(dwCommsError, &message[strlen(message)], 10);
			break;
	}
	::MessageBox(NULL, message, "TxChar(): Comms Error", MB_OK|MB_ICONSTOP);
*/
	return FALSE;
}


BOOL Tx(BYTE * buffer, int count)
{


	return FALSE;
}


void SetDTR()
{
	EscapeCommFunction(idComDev, SETDTR);
}


void ClearDTR()
{
	EscapeCommFunction(idComDev, CLRDTR);
}


void PurgeBuffers()
{
	PurgeComm(idComDev, PURGE_TXCLEAR);	// Flush transmit queue
	PurgeComm(idComDev, PURGE_RXCLEAR);	// Flush receive queue
}
