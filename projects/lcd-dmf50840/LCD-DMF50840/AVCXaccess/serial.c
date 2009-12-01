//Serial.c
//
// LCD controller sample application by Jan de Rie
// Last update: November 30, 2009
//
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include "serial.h"
#include "bitmap.h"

#define LCDDEMO 0xB8
#define	LCDCOLOR 0x88
#define LCDXPLUS 0x41
#define LCDYPLUS 0x42
#define LCDXMIN 0x49
#define LCDYMIN 0x4a
#define LCDPSETXPLUS 0x45
#define LCDPSETYPLUS 0x46
#define LCDPSETXMIN 0x4d
#define LCDPSETYMIN 0x4e
#define LCDCLRSCRN 0x43
#define LCDCLOCKON 0x7E
#define LCDCLOCKOFF 0x7F
#define LCDPRNTSTRING 0xb0
#define LCDPUTCHAR 0xa0
#define	LCDSETSCREEN 0x98
#define LCDDRAWLINE 0xa8	// takes 4 arguments (8 bytes)
#define LCDDRAWCIRCLE 0xc8	// takes 3 arguments (6 bytes)
#define LCDSETX 0x91		// takes 2 bytes argument
#define LCDSETY 0x92		// takes 1 byte argument
#define LCDSETXPSET 0x95	// takes 2 bytes argument
#define LCDSETYPSET 0x96	// takes 1 byte argument
#define	LCDBITMAP 0xc0
// Flow control flags

#define FC_DTRDSR 0x01
#define FC_RTSCTS 0x02
#define FC_XONXOFF 0x04

#define AVCXCOLUMNS 320
#define AVCXROWS 240
#define CHAR_HEIGHT 8
#define CHAR_WIDTH 8

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
DWORD dwRead;
HANDLE SerPrt = INVALID_HANDLE_VALUE;
OVERLAPPED osReader = {0};
DWORD dwEvtMask;
BOOL fWaitingOnRead = FALSE;
#define READ_BUF_SIZE 1
char InBuf[READ_BUF_SIZE];

HANDLE SerialInit(char *ComPortName, int BaudRate) 
{
	HANDLE hCom;

	hCom = CreateFileA(ComPortName, 
			GENERIC_READ | GENERIC_WRITE,
			0, // exclusive access
			NULL, // no security
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED, // overlapped I/O
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
	dcb.Parity = NOPARITY;		// EVENPARITY;
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
	dcb.fDtrControl = DTR_CONTROL_DISABLE; // DTR_CONTROL_HANDSHAKE;

	bPortReady = SetCommState(hCom, &dcb);

	// Communication timeouts are optional

	bPortReady = GetCommTimeouts (hCom, &CommTimeouts);

	CommTimeouts.ReadIntervalTimeout = 2500;
	CommTimeouts.ReadTotalTimeoutConstant = 2500;
	CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
	CommTimeouts.WriteTotalTimeoutConstant = 2500;
	CommTimeouts.WriteTotalTimeoutMultiplier = 1000;

	bPortReady = SetCommTimeouts (hCom, &CommTimeouts);

	// Create Readed event

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (NULL == osReader.hEvent) {
		printf("SerialInit: failure to create reader hEvent.\n");
	}

	return hCom;
}

void SerialStartRead(void)
{
	int i, err;
//	return;
	if (!fWaitingOnRead) {
		// Issue Read Operation
		if (!ReadFile(SerPrt, InBuf, READ_BUF_SIZE, &dwRead, &osReader)) {
			err = GetLastError();
			if ((ERROR_IO_PENDING != err) && (ERROR_IO_INCOMPLETE != err)) {
				printf("SerialStartRead: ReadFile failed error: %d ", err);
				ClearCommError(SerPrt, &err, NULL);
				printf("detail: %x\n", err);
			} else {
				fWaitingOnRead = TRUE;
			}
		} else {
			//ReadFile finished
			for (i = 0; i < (int)dwRead; i++) {
				printf("%c", InBuf[i]);
			}
			fflush(stdout);
		}
	}
}

void SerialCheckRead(void)
{
	DWORD dwRes;
	int i, err;

	if (fWaitingOnRead) {
		dwRes = WaitForSingleObject(osReader.hEvent, 0);	// don't wait
		switch (dwRes) {
		case WAIT_OBJECT_0:	// Read Completed
			if (!GetOverlappedResult(SerPrt, &osReader, &dwRead, FALSE)) {
				err = GetLastError();
				if ((ERROR_IO_PENDING != err) && (ERROR_IO_INCOMPLETE != err)) {
					printf("SerialCheckRead: GetOverlappedResult failed error: %d ", err);
					ClearCommError(SerPrt, &err, NULL);
					printf("detail: %x\n", err);
				} else {
					// Still waiting
				}
				break;
			}
			// success
			for (i = 0; i < (int)dwRead; i++) {
				printf("%c", InBuf[i]);
			}
			fflush(stdout);
			fWaitingOnRead = FALSE;		// ready for another operation
			break;
		case WAIT_TIMEOUT:
			// still waiting
			break;
		default:
			// error
			printf("SerialCheckRead: communication error: %d\n", dwRes);
		}
	}
}

void MySleep(int mSec)
{
	int i;
	if (mSec >= 100) {
		i = mSec / 100;
		for (i = 0; i < (mSec/100); i++) {
			SerialStartRead();
			SerialCheckRead();
			Sleep(100);
		}
	} else {
		SerialStartRead();
		SerialCheckRead();
		Sleep(mSec);
	}
}


void SerialPutc(HANDLE *hCom, unsigned char txchar)
{
	BOOL bWriteRC;
	static DWORD iBytesWritten;
	OVERLAPPED osWrite = {0};
	DWORD dwRes;

	// Create an event object for write operation

    osWrite.hEvent = CreateEvent(
        NULL,   // default security attributes 
        TRUE,   // manual-reset event 
        FALSE,  // not signaled 
        NULL    // no name
		);
    
	if (NULL == osWrite.hEvent) {
		printf("Error: unable to create Write Operation Event\n");
		return;
	}

	bWriteRC = WriteFile(*hCom, &txchar, 1, &iBytesWritten,&osWrite);
	if (!bWriteRC) {
		// Check if Write is pending. 
		if (ERROR_IO_PENDING != GetLastError()) {
			printf("SerialPutc: unexpected return %d from WriteFile()\n", GetLastError());
			return;
		}
		dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
		switch (dwRes) {
		case WAIT_OBJECT_0:
			if (0 == GetOverlappedResult(*hCom, &osWrite, &iBytesWritten, TRUE)) {
				printf("GetOverlappedResult failed with error %d.\n", GetLastError());
			} else {
				// success
			}
			break;
		default:
			printf("WaitForSingleObject failed with error %d.\n", dwRes);
			break;
		}
	} else {
		// WriteFile() returned TRUE
		DWORD err = GetLastError();	//returns 995, Operation Aborted
	}
	CloseHandle(osWrite.hEvent);

	SerialStartRead();
	SerialCheckRead();
}

void AVCXClearScreen(void)
{
	SerialPutc(&SerPrt, LCDCLRSCRN);
	MySleep(200);
}

void AVCXSetScreen(int color, int x0, int y0, int x1, int y1 )
{
	SerialPutc(&SerPrt, LCDSETSCREEN);
	SerialPutc(&SerPrt, color);
	SerialPutc(&SerPrt, (x0 & 0xff));
	SerialPutc(&SerPrt, (x0 >> 8));
	SerialPutc(&SerPrt, (y0 & 0xff));
	SerialPutc(&SerPrt, (y0 >> 8));
	SerialPutc(&SerPrt, (x1 & 0xff));
	SerialPutc(&SerPrt, (x1 >> 8));
	SerialPutc(&SerPrt, (y1 & 0xff));
	SerialPutc(&SerPrt, (y1 >> 8));
	MySleep(200);
}

void AVCXDemo(int val)
{
	SerialPutc(&SerPrt, LCDDEMO);
	SerialPutc(&SerPrt, (unsigned char)val);
}

void AVCXSetRGBColor(int Blue, int Green, int Red)
{
	//SerialPutc(&SerPrt, LCDCOLOR | (Red << 2) | (Green << 1) | Blue);
}

void AVCXSetBWColor(int color)
{
	SerialPutc(&SerPrt, LCDCOLOR | color);
	SerialPutc(&SerPrt, 0);
}

void AVCXClockOn(void)
{
	SerialPutc(&SerPrt, LCDCLOCKON);
}

void AVCXClockOff(void)
{
	SerialPutc(&SerPrt, LCDCLOCKOFF);
}

void AVCXPrintString(char * txt)
{
	char *txt1 = txt;
	SerialPutc(&SerPrt, LCDPRNTSTRING);
	while (*txt) {
		SerialPutc(&SerPrt,*txt++);
	}
	SerialPutc(&SerPrt,0);
//	printf("%s\r\n", txt1);
}

void AVCXPutChar(char ch)
{
	SerialPutc(&SerPrt, LCDPUTCHAR);
	SerialPutc(&SerPrt, ch);
}

void AVCXSetXY(int x, int y)
{
	SerialPutc(&SerPrt, LCDSETX);
	SerialPutc(&SerPrt, (x & 0xff));
	SerialPutc(&SerPrt, (x >> 8));
	SerialPutc(&SerPrt, LCDSETY);
	SerialPutc(&SerPrt, (y & 0xff));
	SerialPutc(&SerPrt, (y >> 8));
}

void AVCXPSetXPSetY(int x, int y)
{
	SerialPutc(&SerPrt, LCDSETXPSET);
	SerialPutc(&SerPrt, (x & 0xff));
	SerialPutc(&SerPrt, (x >> 8));
	SerialPutc(&SerPrt, LCDSETYPSET);
	SerialPutc(&SerPrt, (y & 0xff));
	SerialPutc(&SerPrt, (y >> 8));
}

void AVCXSetXYPSet(unsigned int x, unsigned int y)
{
	SerialPutc(&SerPrt, LCDSETX);
	SerialPutc(&SerPrt, (x & 0xff));
	SerialPutc(&SerPrt, (x >> 8));
	SerialPutc(&SerPrt, LCDSETYPSET);
	SerialPutc(&SerPrt, (y & 0xff));
	SerialPutc(&SerPrt, (y >> 8));
}

int AVCXOnePage(char *str, int Xpos)
{
	int Ypos;
	AVCXClearScreen();
	Ypos = 0;
	AVCXSetRGBColor(1,0,0);
	AVCXSetXY(Xpos, Ypos);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,1,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,0,1);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,1,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,1,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,0,1);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,1,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,1,1);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,0,1);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,1,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,1,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos += CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,1,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,0,1);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,1,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(1,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	AVCXSetRGBColor(0,0,0);
	AVCXPrintString(str); Ypos += CHAR_HEIGHT; Xpos -= CHAR_WIDTH;
	AVCXSetXY(Xpos, Ypos);
	return (12 * strlen(str));
}

void FillScreenWithText(void)
{
	char ch = ' ';
	int i,j;
	AVCXClearScreen();
	for (i = 0; i < (AVCXROWS / CHAR_HEIGHT); i++) {		//for each line
		AVCXSetXY(0, (i * CHAR_HEIGHT));			//start at beginning of line
		for (j = 0; j < (AVCXCOLUMNS / CHAR_WIDTH); j++) {
			AVCXPutChar(ch++);		//go through all 255 char values
//			if (ch > 'z') ch = ' ';
		}
	}
}

typedef          char s08;
typedef          short s16;
typedef unsigned char u08;
typedef unsigned short u16;
typedef unsigned int  u32;
//
// AVCX line drawing
//
void line(u16 x0, u16 y0, u16 x1, u16 y1)
{
	SerialPutc(&SerPrt, LCDDRAWLINE);
	SerialPutc(&SerPrt, (x0 & 0xff));
	SerialPutc(&SerPrt, (x0 >> 8));
	SerialPutc(&SerPrt, (y0 & 0xff));
	SerialPutc(&SerPrt, (y0 >> 8));
	SerialPutc(&SerPrt, (x1 & 0xff));
	SerialPutc(&SerPrt, (x1 >> 8));
	SerialPutc(&SerPrt, (y1 & 0xff));
	SerialPutc(&SerPrt, (y1 >> 8));
}
void DrawTriangle(unsigned int x1,unsigned int y1, unsigned int x2, unsigned int y2,unsigned int x3,unsigned int y3)
{
	if (x1 > (AVCXCOLUMNS - 1)) x1 = AVCXCOLUMNS - 1;
	if (x2 > (AVCXCOLUMNS - 1)) x2 = AVCXCOLUMNS - 1;
	if (x3 > (AVCXCOLUMNS - 1)) x3 = AVCXCOLUMNS - 1;
	if (y1 > (AVCXROWS - 1)) y1 = AVCXROWS - 1;
	if (y2 > (AVCXROWS - 1)) y2 = AVCXROWS - 1;
	if (y3 > (AVCXROWS - 1)) y3 = AVCXROWS - 1;

	line(x1, y1, x2, y2);
	line(x2, y2, x3, y3);
	line(x3, y3, x1, y1);
}

void AVCXLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int color)
{
	AVCXSetBWColor(color);
	line (x0, y0, x1, y1);
}

void AVCXCircle(unsigned int x, unsigned int y, unsigned int radius, unsigned int color)
{
	AVCXSetBWColor(color);
	SerialPutc(&SerPrt, LCDDRAWCIRCLE);
	SerialPutc(&SerPrt, (x & 0xff));
	SerialPutc(&SerPrt, (x >> 8));
	SerialPutc(&SerPrt, (y & 0xff));
	SerialPutc(&SerPrt, (y >> 8));
	SerialPutc(&SerPrt, (radius & 0xff));
	SerialPutc(&SerPrt, (radius >> 8));
}

BOOL SendBitMap(BOOL Short, unsigned int HorOffset, unsigned int VertOffset, char *pImageName) {
//	BITMAPINFO	BM;
	BITMAPINFO	*pBM;	// = &BM;
	GLubyte * pImage;
	GLubyte * pSave;
	WORD RedMask = 0xF800;
	WORD GreenMask = 0x07E0;
	WORD BlueMask = 0x001F;
	WORD RedPix, GreenPix, BluePix;
	int RedShift = 10;
	int GreenShift = 5;
	int BlueShift = 0;
	int nColumns, nRows, i, j;
	int	NumCount;
	BOOL first = TRUE;
	unsigned char b1,b2,b3,b4;
	unsigned char Cluster[8];
	int clust = 0;
	int nBytes = 0;
	int size = 0;
	int rowSize;

	pImage = LoadDIBitmap(pImageName, &pBM);

	if (NULL == pImage) {
		DWORD err = GetLastError();
		return FALSE;
	}
	pSave = pImage;

	if (pBM->bmiHeader.biBitCount != 1) { // Expect a monochrome image
		free(pImage);
		free(pBM);
		return FALSE;
	}

	if ((nColumns = pBM->bmiHeader.biWidth) > AVCXCOLUMNS) {
		free(pImage);
		free(pBM);
		return FALSE;
	}

	if ((nColumns + HorOffset) >  AVCXCOLUMNS) {
		free(pImage);
		free(pBM);
		return FALSE;
	}

	if ((nRows = pBM->bmiHeader.biHeight) > AVCXROWS) {
		int delta = (pBM->bmiHeader.biHeight - AVCXROWS) / 2;	// May lose one line here
		// skip initial delta lines in image
		pImage = (GLubyte *)((DWORD)pImage + delta * (nColumns * sizeof(WORD)));
		nRows -= delta;
	}

	if ((nRows + VertOffset) >  AVCXROWS) {
		free(pImage);
		free(pBM);
		return FALSE;
	}

	if (pBM->bmiHeader.biCompression != BI_RGB) { // need straight bitmap
		free(pImage);
		free(pBM);
		return FALSE;
	}

//	printf("send bitmap: %d %d %d %d\n", HorOffset/4, VertOffset, (HorOffset/4 + (nColumns / 4) - 1), (VertOffset + (nRows - 1)));
	//
	// conversion to pixel argument for X: last pixel address doesn't match last nibble address
	//
	SerialPutc(&SerPrt, LCDBITMAP);							// Bitmap
	SerialPutc(&SerPrt, HorOffset/4);						//x0		//TODO sb pixel address
	SerialPutc(&SerPrt, VertOffset);						//y0
	SerialPutc(&SerPrt, (HorOffset/4 + (nColumns / 4) - 1));	//x1
	SerialPutc(&SerPrt, (VertOffset + (nRows - 1)));		//y1

	for (i = 0; i < nRows; i++) {	// for each row
		rowSize = 0;
		for (j = 0; j < (nColumns/16); j++) { // send 16 bits at a time
			WORD pix = *pImage++;
			size += 2; rowSize += 2;
			// send 2 nibbles per byte.

				SerialPutc(&SerPrt, ~(pix & 0xff));
				SerialPutc(&SerPrt, ~((pix >> 8) & 0xff));
		}
		// each row is padded so that size is a multiple of 4
		if (rowSize & 0x03) {
			++pImage;
			size += 2;
		}
	}

	if (size != pBM->bmiHeader.biSizeImage) {
//		printf("Size mismatch: header=%d count=%d\n", pBM->bmiHeader.biSizeImage, size);
	}
	if (nRows > AVCXROWS) {
		nRows -= AVCXROWS;
		// skip tail lines in image
		pImage = (GLubyte *)((DWORD)pImage + nRows * (nColumns * sizeof(WORD)));
	} else if (nRows < AVCXROWS) {
		//TODO spit out dummy lines
		free(pSave);
		free(pBM);
		return FALSE;
	} else if (nRows != AVCXROWS) {	//assert
		free(pSave);
		free(pBM);
		return FALSE;
	}

	free(pSave);
	free(pBM);
	return TRUE;
}			

BOOL SendFullBitMap(BOOL Short, char *pImageName) {
	return SendBitMap(Short, 0, 0, pImageName);
}

#define BITMAPNAME0 "Tree.BMP"
#define BITMAPNAME1 "Marvin.BMP"
#define BITMAPNAME2 "Rose.BMP"
#define BITMAPNAME3 "Church.BMP"
#define BITMAPNAME4 "Flower.BMP"
#define BITMAPNAME5 "Cartoon.BMP"
#define BITMAPNAME6 "Planets.BMP"
#define BITMAPNAME7 "Planets2.BMP"
#define BITMAPNAME8 "Planets3.BMP"

void ShowTriangles(unsigned int BaseX, unsigned int BaseY, unsigned int SleepVal)
{
	int i;
	if ((BaseX < 100) || (BaseX > 160) || (BaseY < 50) || (BaseY > 220)) {
		printf("ShowTriangles: Invalid arguments %d and %d\n", BaseX, BaseY);
	}
	AVCXSetBWColor(1);
	for (i = 0; i < 20; i++) {
		DrawTriangle(BaseX + (i * 5),BaseY - 50 + (i * 5), BaseX + 100 + (i * 3),BaseY + (i * 5), BaseX - 100 + (i * 5),BaseY + (i * 5));
	}
	MySleep(SleepVal);
#if 0
	AVCXClearScreen();
	AVCXSetXY(0, 29 * 8);	// expects pixel argument
	AVCXPrintString("Built-in Graphics");
#else
	AVCXSetBWColor(0);
	for (i = 0; i < 20; i++) {
		DrawTriangle(BaseX + (i * 5),BaseY - 50 + (i * 5), BaseX + 100 + (i * 3),BaseY + (i * 5), BaseX - 100 + (i * 5),BaseY + (i * 5));
	}
	AVCXSetBWColor(1);
	MySleep(SleepVal);
#endif
}

void ShowGraphics(void)
{
	int i, y, lasti;
	AVCXClockOff();
	AVCXSetScreen(0, 0, 0, AVCXCOLUMNS - 1, AVCXROWS - 1);
	AVCXSetXY(0, 29 * 8);	// expects pixel argument
	AVCXPrintString("Built-in Graphics");
	for (y = 7; y < AVCXROWS; y += 8) {				//Draw lines pattern
		AVCXLine(0, y, AVCXCOLUMNS - 1, y, 1);
	}
	for (i = 10; i < 120; i += 5) {					//Draw circles
		AVCXCircle(160, 120, i, 1);
		MySleep(100);
	}
	lasti = i;
	for (i = lasti; i >= 10; i -= 5) {
		AVCXCircle(160, 120, i, 0);					//Clear circles
		if (i == lasti) {							//Redraw text if necessary
			AVCXSetXY(0, 29 * 8);					//Expects pixel argument
			AVCXPrintString("Built-in Graphics");
		}
		for (y = 7; y < AVCXROWS; y += 8) {			//Redraw lines is necessary
			if ((y >= (120 - i)) && (y <= (120 + i)))
				AVCXLine(0, y, AVCXCOLUMNS - 1, y, 1);
		}
		MySleep(200);
	}
	for (y = 7; y < AVCXROWS; y += 8) {				//Clear lines pattern
		AVCXLine(0, y, AVCXCOLUMNS - 1, y, 0);
	}
	MySleep(500);

	ShowTriangles(100, 50, 500);
	ShowTriangles(125, 75, 500);
	ShowTriangles(150, 100, 500);
}


//
// accept TWO optional arguments digit 0..9 to indicate the serial port to use and the bitrate.
// default: COM2 at 115200
//
int main(int argc, char *argv[])
{
	int i, x, Xpos = 1;
	int	tmp, cnt = 0;
	char strbuf[256];
	int BaseX, BaseY;
	int port = 2;
	char SerialPortStr[] = "\\\\.\\COM2";
	int baudrate = 115200;
	BOOL foundBaud = FALSE;

	i = 1;
	while (argc-- > 1) {
		tmp = atoi(argv[i++]);
		if (tmp > 9)  {
			// assume this is the baudrate if not specified yet
			if (!foundBaud) {
				baudrate = tmp;
				foundBaud = TRUE;
			} else {
				printf("Invalid COM port number detected. Using COM0:\n");
				port = 0;
			}
		} else {
			port = tmp;
		}
	}
	SerialPortStr[7] = port + '0';
	printf("Using COM%c: at %d baud\n", port + '0', baudrate);
	SerPrt = SerialInit(SerialPortStr, baudrate);
	if (SerPrt == INVALID_HANDLE_VALUE) {
		printf("Unable to Open COM%c:\n", port + '0');
		return FALSE;
	}
	do {
		AVCXSetScreen(1, 0, 0, AVCXCOLUMNS - 1, AVCXROWS - 1);
		MySleep(1000);
		AVCXClearScreen();
		AVCXClockOn();
		//
		// Display images in the right pane
		//
		SendBitMap(FALSE, 240, 0, BITMAPNAME8);
		MySleep(500);
		SendBitMap(FALSE, 240, 60, BITMAPNAME8);
		MySleep(500);
		SendBitMap(FALSE, 240, 120, BITMAPNAME8);
		MySleep(500);
		SendBitMap(FALSE, 240, 180, BITMAPNAME8);
		MySleep(10000);
		AVCXClockOff();
		MySleep(1000);
		AVCXDemo(1);
		MySleep(1000);
		AVCXDemo(2);
		MySleep(1000);
		AVCXDemo(3);
		MySleep(1000);
		AVCXDemo(4);
		MySleep(8000);
		FillScreenWithText();
		MySleep(3000);
		AVCXClearScreen();
		AVCXSetScreen(1, 240, 0, AVCXCOLUMNS - 1, AVCXROWS - 1);
		AVCXSetBWColor(1);		// This sets the Background color back to BLACK
		AVCXClockOn();
		MySleep(10000);
		AVCXClockOff();
		MySleep(1000);
		AVCXSetScreen(1, 0, 0, AVCXCOLUMNS - 1, AVCXROWS - 1);
		AVCXSetBWColor(0);		// This sets the Background color back to WHITE
		AVCXClockOn();
		MySleep(10000);
		AVCXClockOff();
		MySleep(1000);
		ShowGraphics();
		cnt += AVCXOnePage("This is test1", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME0);
		MySleep(1500);
		SendFullBitMap(FALSE, BITMAPNAME1);
		MySleep(1500);
		SendFullBitMap(FALSE, BITMAPNAME2);
		MySleep(1500);
		SendFullBitMap(FALSE, BITMAPNAME3);
		MySleep(1500);
		SendFullBitMap(FALSE, BITMAPNAME4);
		MySleep(1500);
		SendFullBitMap(FALSE, BITMAPNAME5);
		MySleep(1500);
		FillScreenWithText();
		MySleep(1500);
		AVCXClearScreen();
		AVCXClockOn();
		MySleep(10000);
		AVCXClockOff();
		MySleep(1000);
		ShowGraphics();
		//
		// Send partial images
		//
		AVCXClearScreen();
		SendBitMap(FALSE, 0, 0, BITMAPNAME6);
		MySleep(1000);
		SendBitMap(FALSE, 160, 0, BITMAPNAME7);
		MySleep(1000);
		SendBitMap(FALSE, 0, 120, BITMAPNAME7);
		MySleep(1000);
		SendBitMap(FALSE, 160, 120, BITMAPNAME6);
		MySleep(2000);

		cnt += AVCXOnePage("This is another test", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		AVCXClearScreen();
		AVCXClockOn();
		MySleep(10000);
		AVCXClockOff();
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME2);
		MySleep(1500);
		cnt += AVCXOnePage("How about another test", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME3);
		MySleep(1500);
		cnt += AVCXOnePage("Let's do another test", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME4);
		MySleep(1500);
		ShowGraphics();
		cnt += AVCXOnePage("Now onto test2", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME0);
		MySleep(1500);
		cnt += AVCXOnePage("Another test2", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		AVCXClearScreen();
		AVCXClockOn();
		MySleep(10000);
		AVCXClockOff();
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME1);
		MySleep(1500);
		cnt += AVCXOnePage("More of test2", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME2);
		MySleep(1500);
		cnt += AVCXOnePage("Almost last test2", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME3);
		MySleep(1500);
		cnt += AVCXOnePage("Almost almost last test2", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		SendFullBitMap(FALSE, BITMAPNAME4);
		MySleep(1500);
		cnt += AVCXOnePage("Finally last test2", Xpos);
		sprintf_s(strbuf, 256, "cnt = %d", cnt); //assume 0 termination
		AVCXPrintString(strbuf);
		MySleep(1000);
		AVCXClearScreen();
		AVCXClockOn();
		MySleep(10000);
		AVCXClockOff();
		MySleep(1000);
		ShowGraphics();
		SendFullBitMap(FALSE, BITMAPNAME2);
		MySleep(5000);
		Xpos += 20;
		if (Xpos > 100) Xpos = 0;
	} while (1);
return 0;

}

