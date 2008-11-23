//Serial.cpp
//
// AVCX sample application by Johannes P.M. de Rie
// Last update: Nov 21, 2008
//
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include "serial.h"
#include "bitmap.h"

// Flow control flags

#define FC_DTRDSR 0x01
#define FC_RTSCTS 0x02
#define FC_XONXOFF 0x04

#define AVCXLINES 240
#define AVCXROWS 160

// ascii definitions

#define ASCII_BEL 0x07
#define ASCII_BS 0x08
#define ASCII_LF 0x0A
#define ASCII_CR 0x0D
#define ASCII_XON 0x11
#define ASCII_XOFF 0x13
//using namespace std;
// variables used with the com port
BOOL bPortReady;
DCB dcb;
COMMTIMEOUTS CommTimeouts;
BOOL bWriteRC;
BOOL bReadRC;
DWORD iBytesWritten;
DWORD iBytesRead;
HANDLE SerPrt = INVALID_HANDLE_VALUE;

HANDLE SerialInit(char *ComPortName, int BaudRate) 
{
HANDLE hCom;

hCom = CreateFileA(ComPortName, 
GENERIC_READ | GENERIC_WRITE,
0, // exclusive access
NULL, // no security
OPEN_EXISTING,
0, // no overlapped I/O
NULL); // null template

if (hCom == INVALID_HANDLE_VALUE) {
	DWORD err = GetLastError();
 char lpMsg[512];
 FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)&lpMsg, 0, NULL);

	return hCom;
}

bPortReady = SetupComm(hCom, 2, 128); // set buffer sizes


bPortReady = GetCommState(hCom, &dcb);
dcb.BaudRate = BaudRate;
dcb.ByteSize = 8;
dcb.Parity = NOPARITY;
// dcb.Parity = EVENPARITY;
dcb.StopBits = ONESTOPBIT;
dcb.fAbortOnError = TRUE;

// set XON/XOFF
dcb.fOutX = FALSE; // XON/XOFF off for transmit
dcb.fInX = FALSE; // XON/XOFF off for receive
// set RTSCTS
dcb.fOutxCtsFlow = FALSE; // turn on CTS flow control
dcb.fRtsControl = RTS_CONTROL_DISABLE; // 
// set DSRDTR
dcb.fOutxDsrFlow = FALSE; // turn on DSR flow control
dcb.fDtrControl = DTR_CONTROL_DISABLE; // 
// dcb.fDtrControl = DTR_CONTROL_DISABLE; // 
// dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; // 

bPortReady = SetCommState(hCom, &dcb);

// Communication timeouts are optional

bPortReady = GetCommTimeouts (hCom, &CommTimeouts);

CommTimeouts.ReadIntervalTimeout = 5000;
CommTimeouts.ReadTotalTimeoutConstant = 5000;
CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
CommTimeouts.WriteTotalTimeoutConstant = 5000;
CommTimeouts.WriteTotalTimeoutMultiplier = 1000;

bPortReady = SetCommTimeouts (hCom, &CommTimeouts);

return hCom;
}

char SerialGetc(HANDLE *hCom)
{
char rxchar;
BOOL bReadRC;
static DWORD iBytesRead;

bReadRC = ReadFile(*hCom, &rxchar, 1, &iBytesRead, NULL);

return rxchar;
}

void SerialPutc(HANDLE *hCom, char txchar)
{
BOOL bWriteRC;
static DWORD iBytesWritten;

bWriteRC = WriteFile(*hCom, &txchar, 1, &iBytesWritten,NULL);
if (1 != bWriteRC) {
	DWORD err = GetLastError();	//returns 995, Operation Aborted
}

return;
}

void AVCXReSync(void)
{
	// Find a way to do this
}

void AVCXClearScreen(void)
{
	SerialPutc(&SerPrt, 0x43);
	Sleep(200);
}

void AVCXSetColor(int Blue, int Green, int Red)
{
	SerialPutc(&SerPrt, 0x88 | Blue);
	SerialPutc(&SerPrt, (Green << 3) | Red);
}

void AVCXPrintString(char * txt)
{
	char *txt1 = txt;
	SerialPutc(&SerPrt, 0xb0);
	while (*txt) {
		SerialPutc(&SerPrt,*txt++);
	}
	SerialPutc(&SerPrt,0);
	printf("%s\r\n", txt1);
}

void AVCXPutChar(char ch)
{
		SerialPutc(&SerPrt, 0xa0);
		SerialPutc(&SerPrt, ch);
}

void AVCXSetXY(int x, int y)
{
	SerialPutc(&SerPrt, 0x91);
	SerialPutc(&SerPrt, x);
	SerialPutc(&SerPrt, 0x92);
	SerialPutc(&SerPrt, y);
}

int AVCXOnePage(char *str, int Xpos)
{
int Ypos;
AVCXReSync();
AVCXSetColor(7,7,7);
AVCXClearScreen();
Ypos = 0;
AVCXSetColor(7,0,0);
AVCXSetXY(Xpos, Ypos);
AVCXPrintString(str); Ypos += 8; Xpos += 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(0,7,0);
AVCXPrintString(str); Ypos += 8; Xpos += 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(0,0,7);
AVCXPrintString(str); Ypos += 8; Xpos += 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(0,7,0);
AVCXPrintString(str); Ypos += 8; Xpos += 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(7,0,0);
AVCXPrintString(str); Ypos += 8; Xpos += 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(0,0,0);
AVCXPrintString(str); Ypos += 8; Xpos += 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(7,0,0);
AVCXPrintString(str); Ypos += 8; Xpos += 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(0,7,0);
AVCXPrintString(str); Ypos += 8; Xpos -= 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(0,0,7);
AVCXPrintString(str); Ypos += 8; Xpos -= 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(0,7,0);
AVCXPrintString(str); Ypos += 8; Xpos -= 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(7,0,0);
AVCXPrintString(str); Ypos += 8; Xpos -= 8;
AVCXSetXY(Xpos, Ypos);
AVCXSetColor(0,0,0);
AVCXPrintString(str); Ypos += 8; Xpos -= 8;
AVCXSetXY(Xpos, Ypos);
return (12 * strlen(str));
}



BOOL SendBitMap(BOOL Short) {
	BITMAPINFO	BM;
	BITMAPINFO	*pBM = &BM;
	GLubyte * pImage;
	GLubyte * pSave;
	WORD RedMask = 0xF800;
	WORD GreenMask = 0x07E0;
	WORD BlueMask = 0x001F;
	WORD RedPix, GreenPix, BluePix;
	int RedShift = 10;
	int GreenShift = 5;
	int BlueShift = 0;
	int nLines, nRows, i, j;

	AVCXReSync();

	pImage = LoadDIBitmap("..\\..\\Pict_01.BMP", &pBM);

	if (NULL == pImage) {
		DWORD err = GetLastError();
		return FALSE;
	}
	pSave = pImage;

	if (pBM->bmiHeader.biBitCount != 16) { // can't deal with colorMap
		free(pImage);
		return FALSE;
	}

	if ((nLines = pBM->bmiHeader.biWidth) != AVCXLINES) {
		free(pImage);
		return FALSE;
	}

	if ((nRows = pBM->bmiHeader.biHeight) != AVCXROWS) {
		if (pBM->bmiHeader.biHeight > AVCXROWS) {
			int delta = (pBM->bmiHeader.biHeight - AVCXROWS) / 2;	// May lose one line here
			// skip initial delta lines in image
			pImage = (GLubyte *)((DWORD)pImage + delta * (nLines * sizeof(WORD)));
			nRows -= delta;
		} else {
			free(pImage);
			return FALSE;
		}
	}

	if (pBM->bmiHeader.biCompression == BI_BITFIELDS) { // extract bitfield masks
		DWORD *pMask = (DWORD *) &pBM->bmiColors;
		WORD tmp;
		tmp = RedMask = (WORD)*pMask++;
		RedShift = 0;
		while ((tmp & 1) == 0) 
		{tmp >>= 1;
		RedShift++;}
		tmp = GreenMask = (WORD)*pMask++;
		GreenShift = 0;
		while ((tmp & 1) == 0) {tmp >>= 1; GreenShift++;}
		tmp = BlueMask = (WORD)*pMask++;
		BlueShift = 0;
		while ((tmp & 1) == 0) {tmp >>= 1; BlueShift++;}
	}

	SerialPutc(&SerPrt, 0xc0 + (int)Short); // Bitmap

	for (i = 0; i < min(nRows, AVCXROWS); i++) {	// for each row
		for (j = 0; j < nLines; j++) { // for each pixel
			char b1,b2;
			WORD pix = *pImage++;
			RedPix = (pix & RedMask) >> RedShift;
			GreenPix = (pix & GreenMask) >> GreenShift;
			BluePix = (pix & BlueMask) >> BlueShift;
			while (RedPix > 7) {
				RedPix >>= 1;
			}
			while (GreenPix > 7) {
				GreenPix >>= 1;
			}
			while (BluePix > 7) {
				BluePix >>= 1;
			}
			if (Short) {
				BluePix >>= 1;	// lose bit 0
				b1 = RedPix | (GreenPix << 3) | ((BluePix & 3) << 6);
				SerialPutc(&SerPrt, b1); // Pixel
			} else {
				b1 = RedPix | (GreenPix << 3) | ((BluePix & 3) << 6);
				b2 = BluePix >> 2;
				SerialPutc(&SerPrt, b1); // Pixel
				SerialPutc(&SerPrt, b2); // Pixel
			}
		}
	}

	if (nRows > AVCXROWS) {
		nRows -= AVCXROWS;
		// skip tail lines in image
		pImage = (GLubyte *)((DWORD)pImage + nRows * (nLines * sizeof(WORD)));
	} else if (nRows < AVCXROWS) {
		//TODO spit out dummy lines
		return FALSE;
	} else if (nRows != AVCXROWS) {	//assert
		return FALSE;
	}

	free(pSave);
	return TRUE;
}			

int main()
{
	int Xpos = 1;
	int	cnt = 0;
	char strbuf[256];

	SerPrt = SerialInit("\\\\.\\COM3",115200);
	if (SerPrt == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	AVCXReSync();

	do {
		cnt += AVCXOnePage("This is test1", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		SendBitMap(FALSE);
		Sleep(500);
		cnt += AVCXOnePage("This is another test", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		SendBitMap(TRUE);
		Sleep(500);
		cnt += AVCXOnePage("How about another test", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		SendBitMap(FALSE);
		Sleep(500);
		AVCXOnePage("Let's do another test", Xpos);
		SendBitMap(TRUE);
		Sleep(500);
		AVCXOnePage("Now onto test2", Xpos);
		SendBitMap(FALSE);
		Sleep(500);
		AVCXOnePage("Another test2", Xpos);
		SendBitMap(TRUE);
		Sleep(500);
		AVCXOnePage("More of test2", Xpos);
		SendBitMap(FALSE);
		Sleep(500);
		AVCXOnePage("Last test2", Xpos);
		SendBitMap(TRUE);
		Sleep(500);
		Xpos += 20;
		if (Xpos > 100) Xpos = 0;
	} while (1);
	return 0;

}

