// Registers.cpp

#define REGISTERSWIN // Clashes with REGISTERS in "Comm.h"

#define WIN32_LEAN_AND_MEAN				// No MFC, we want speed!!

#include "stdafx.h"
#include <windows.h>					// All the windows headers
#include <windowsx.h>					// Useful macros
#include <stdio.h>						// Standard IO
#include <io.h>							// _lseek()
#include <math.h>
#include <fstream.h>					// File IO
#include <wingdi.h>

#include "Registers.h"
#include "Memory.h"
#include "Comms.h"
#include "Resource.h"					// IDB_BLUEVALUES
#include "Globals.h"


// DEFINES ////////////////////////////////////////////////////////////////////

#define REGISTERSWINDOW_CLASS_NAME "REGISTERSWINDOW_CLASS_NAME"
#define WIDTH 240
#define HEIGHT 300
#define NAMECOLOUR 0x00000000
#define VALUECOLOUR 0x00000099
#define VALUEBACKCOLOUR 0x00ffffff
#define BACKCOLOUR 0x00ffffff
#define DARKCOLOUR 0x00000000
#define LIGHTCOLOUR 0x00dfdfdf

#define TOP 5
#define LEFT 35
#define COLUMNS 4
#define COLUMNINTERVAL 55
#define ROWS 8
#define ROWINTERVAL 22
#define XYZINTERVAL 72
#define XPOSX LEFT
#define XPOSY TOP+(ROWS*ROWINTERVAL)
#define YPOSX LEFT+XYZINTERVAL
#define YPOSY XPOSY
#define ZPOSX LEFT+(2*XYZINTERVAL)
#define ZPOSY YPOSY
#define SREGPOSX LEFT
#define SREGPOSY ZPOSY+ROWINTERVAL
#define SREGFLAGSPOSX LEFT
#define SREGFLAGSPOSY SREGPOSY+ROWINTERVAL
#define SREGFLAGSINTERVAL 25
#define SPPOSX LEFT
#define SPPOSY SREGFLAGSPOSY+ROWINTERVAL
#define PCPOSX LEFT+XYZINTERVAL
#define PCPOSY SPPOSY
#define BEVEL 4

// GLOBALS ////////////////////////////////////////////////////////////////////

HWND registersWindowHandle;
HFONT regNamesFont;
HFONT regValuesFont;

int registersX, registersY;
bool registersWindowRegistered = false;

char * hexChars = "0123456789ABCDEF";
char sregFlags[]={'I',0,'T',0,'H',0,'S',0,'V',0,'N',0,'Z',0,'C',0};


// FUNCTIONS //////////////////////////////////////////////////////////////////

BOOL OpenRegistersWindow()
{
	WNDCLASSEX winClass;
	int min, max;

	for (int i=0; i<37; i++)
		registers[i] = 0;

	// Fill the window class structure
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = RegistersProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInstance;
	winClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REGISTERS));
	winClass.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REGISTERS));
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;		// I'm painting it
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = REGISTERSWINDOW_CLASS_NAME;

	// Register the class
	if (!registersWindowRegistered && !RegisterClassEx(&winClass))
		return FALSE;

	registersWindowRegistered = true;

	// Find the window's preferred position
	registersX = project->registersWindowInfo.x;
	registersY = project->registersWindowInfo.y;
	min = project->registersWindowInfo.minimized;
	max = project->registersWindowInfo.maximized;

	// Create the window
	if (!(registersWindowHandle = CreateWindowEx(
		NULL,							// Extended Style
		REGISTERSWINDOW_CLASS_NAME,		// Class
		"Registers",					// Title
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |WS_MINIMIZEBOX, // Style
		registersX, registersY,			// X & Y
		WIDTH, HEIGHT,					// Width & Height
		clientAreaHandle,				// Parent
		NULL,							// Menu
		hInstance,						// This Application
		NULL)))							// Extra parameters
	{
		return FALSE;
	}
	::SetParent(registersWindowHandle, clientAreaHandle);
	if (min)
		::ShowWindow(registersWindowHandle, SW_MINIMIZE);
	if (max)
		::ShowWindow(registersWindowHandle, SW_MAXIMIZE);

	project->registersWindowInfo.open = TRUE;

	return TRUE;
}


LRESULT CALLBACK RegistersProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// This is the main message handler for the Project window
	PAINTSTRUCT ps;						// Used in WM_PAINT
	HDC hDC;							// Handle to a device context
	RECT rect;
	WINDOWPLACEMENT wndPlacement;
	int i, j;
	int x, y;
	int value;
	char buffer[10];

	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	// Switch on message
	switch (msg)
	{
		case WM_CREATE:
		{
			regNamesFont = ::CreateFont(14, 0, 0, 0, FW_BOLD, false, false, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");
			regValuesFont = ::CreateFont(16, 0, 0, 0, FW_MEDIUM, false, false, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, FIXED_PITCH|FF_MODERN, "Courier New");
			return FALSE;
			break;
		}

		case WM_PAINT:
		{
			hDC = BeginPaint(hWnd, &ps);

			if (project->registersWindowInfo.minimized)
			{
				EndPaint(hWnd, &ps);
				return FALSE;
			}

			// First paint the background
			TRIVERTEX vert[2];
			GRADIENT_RECT gRect;
			vert[0].x = 0;
			vert[0].y = 0;
			vert[0].Red = (GRADIENTTOP&0xff)<<8; // 0x5000;
			vert[0].Green = GRADIENTTOP&0xff00; //0x5000;
			vert[0].Blue = (GRADIENTTOP&0xff0000)>>8; //0xff00;
			vert[0].Alpha = 0x0000;

			vert[1].x = WIDTH+1;
			vert[1].y = HEIGHT+1;
			vert[1].Red = (GRADIENTBOTTOM&0xff)<<8; // 0xff00;
			vert[1].Green = GRADIENTBOTTOM&0xff00; //0xff00;
			vert[1].Blue = (GRADIENTBOTTOM&0xff0000)>>8; //0xff00;
			vert[1].Alpha = 0x0000;

			gRect.UpperLeft = 0;
			gRect.LowerRight = 1;
			::GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

			SIZE charSize;
			// Write the register's names
			::SelectObject(hDC, regNamesFont);
			::SetTextColor(hDC, NAMECOLOUR);
			::SetBkColor(hDC, BACKCOLOUR);
			::SetBkMode(hDC, TRANSPARENT);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			for (i=0; i<COLUMNS; i++)
			{
				rect.left = (LEFT+(i*COLUMNINTERVAL))-(3*charSize.cx+5);
				rect.right = (LEFT+(i*COLUMNINTERVAL))-5;
				for (j=0; j<ROWS; j++)
				{
					rect.top = TOP+(j*ROWINTERVAL);
					rect.bottom = rect.top+charSize.cy;
					buffer[0] = 'R';
					itoa((i*ROWS)+j, &buffer[1], 10);
					::DrawText(hDC, buffer, strlen(buffer), &rect,
						DT_RIGHT|DT_NOCLIP|DT_VCENTER);
				}
			}
			rect.left = XPOSX-(charSize.cx+5); // X, Y & Z
			rect.right = XPOSX-5;
			rect.top = XPOSY;
			rect.bottom = rect.top+charSize.cy;
			::DrawText(hDC, "X", 1, &rect, DT_RIGHT|DT_NOCLIP|DT_VCENTER);
			rect.left = YPOSX-(charSize.cx+5);
			rect.right = YPOSX-5;
			::DrawText(hDC, "Y", 1, &rect, DT_RIGHT|DT_NOCLIP|DT_VCENTER);
			rect.left = ZPOSX-(charSize.cx+5);
			rect.right = ZPOSX-5;
			::DrawText(hDC, "Z", 1, &rect, DT_RIGHT|DT_NOCLIP|DT_VCENTER);
			rect.left = SREGPOSX-(4*charSize.cx+5); // SREG
			rect.right = SREGPOSX-5;
			rect.top = SREGPOSY;
			rect.bottom = rect.top+charSize.cy;
			::DrawText(hDC, "SREG", 4, &rect, DT_RIGHT|DT_NOCLIP|DT_VCENTER);
			rect.top = SREGFLAGSPOSY;	// SREG flags
			rect.bottom = rect.top+charSize.cy;
			for (i=0; i<8; i++)
			{
				rect.left = (SREGFLAGSPOSX+(i*SREGFLAGSINTERVAL))-
					(charSize.cx+5);
				rect.right = (SREGFLAGSPOSX+(i*SREGFLAGSINTERVAL))-5;
				::DrawText(hDC, &sregFlags[i*2], 1, &rect,
					DT_RIGHT|DT_NOCLIP|DT_VCENTER);
			}
			rect.left = SPPOSX-(2*charSize.cx+5); // SP
			rect.right = SPPOSX-5;
			rect.top = SPPOSY;
			rect.bottom = rect.top+charSize.cy;
			::DrawText(hDC, "SP", 2, &rect,
				DT_RIGHT|DT_NOCLIP|DT_VCENTER);
			rect.left = PCPOSX-(2*charSize.cx+5); // PC
			rect.right = PCPOSX-5;
			rect.top = PCPOSY;
			rect.bottom = rect.top+charSize.cy;
			::DrawText(hDC, "PC", 2, &rect,
				DT_RIGHT|DT_NOCLIP|DT_VCENTER);

			// Write the register's values
			::SelectObject(hDC, regValuesFont);
			::SetTextColor(hDC, VALUECOLOUR);
			::SetBkColor(hDC, VALUEBACKCOLOUR);
			::SetBkMode(hDC, OPAQUE);

			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			buffer[2] = 0;
			for (i=0; i<COLUMNS; i++)
			{
				x = LEFT+(i*COLUMNINTERVAL);
				for (j=0; j<ROWS; j++)
				{
					y = TOP+(j*ROWINTERVAL);

					RegDrawIndentedRect(hDC, x-BEVEL, y-2,
						(2*BEVEL)+(2*charSize.cx), 4+charSize.cy);

					if (registersMask[(i*8)+j])
					{
						value = registers[(i*8)+j];
						buffer[0] = hexChars[value>>4];
						buffer[1] = hexChars[value&0xf];
					}
					else
					{
						buffer[0] = '?';
						buffer[1] = '?';
					}
					::TextOut(hDC, x, y, buffer, 2);
				}
			}

			y = XPOSY;
			buffer[4] = 0;
			for (i=0; i<3; i++)			// X, Y & Z
			{
				value = registers[26+(i*2+1)];
				value <<= 8;
				value += registers[26+(i*2)];
				if (registersMask[26+(i*2+1)])
				{
					buffer[0] = hexChars[value>>12];
					buffer[1] = hexChars[(value>>8)&0xf];
				}
				else
				{
					buffer[0] = '?';
					buffer[1] = '?';
				}
				if (registersMask[26+(i*2)])
				{
					buffer[2] = hexChars[(value>>4)&0xf];
					buffer[3] = hexChars[value&0xf];
				}
				else
				{
					buffer[2] = '?';
					buffer[3] = '?';
				}
				x = XPOSX+(i*XYZINTERVAL); // X pos
				RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+(4*charSize.cx),
					4+charSize.cy);
				::TextOut(hDC, x, y, buffer, 4);
			}

			x = SREGPOSX;				// SREG
			y = SREGPOSY;
			if (registersMask[32])
			{
				value = registers[32];
				buffer[0] = hexChars[value>>4];
				buffer[1] = hexChars[value&0xf];
			}
			else
			{
				buffer[0] = '?';
				buffer[1] = '?';
			}
			RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+(2*charSize.cx),
				4+charSize.cy);
			::TextOut(hDC, x, y, buffer, 2);

			y = SREGFLAGSPOSY;			// SREG flags
			for (i=0; i<8; i++)
			{
				x = SREGFLAGSPOSX+((7-i)*SREGFLAGSINTERVAL);
				RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+charSize.cx,
					4+charSize.cy);
				if (registersMask[32])
				{
					if (registers[32]&(1<<i))
						::TextOut(hDC, x, y, "1", 1);
					else
						::TextOut(hDC, x, y, "0", 1);
				}
				else
					::TextOut(hDC, x, y, "?", 1);
			}

			y = SPPOSY;
			buffer[4] = 0;
			for (i=0; i<2; i++)			// SP & PC
			{
				value = registers[33+(i*2+1)];
				value <<= 8;
				value += registers[33+(i*2)];
				if (registersMask[33+(i*2+1)])
				{
					buffer[0] = hexChars[value>>12];
					buffer[1] = hexChars[(value>>8)&0xf];
				}
				else
				{
					buffer[0] = '?';
					buffer[1] = '?';
				}
				if (registersMask[33+(i*2)])
				{
					buffer[2] = hexChars[(value>>4)&0xf];
					buffer[3] = hexChars[value&0xf];
				}
				else
				{
					buffer[2] = '?';
					buffer[3] = '?';
				}
				x = SPPOSX+(i*XYZINTERVAL); // X pos
				RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+(4*charSize.cx),
					4+charSize.cy);
				::TextOut(hDC, x, y, buffer, 4);
			}
			EndPaint(hWnd, &ps);

			return FALSE;
			break;
		}

		case WM_LBUTTONDOWN:
		{
			::PostMessage(registersWindowHandle, WM_NCLBUTTONDOWN,
				HTCAPTION, lParam);
			return FALSE;
			break;
		}

		case WM_MOVE:
		{
			if (!project->registersWindowInfo.minimized &&
				!project->registersWindowInfo.maximized)
			{
				::GetWindowPlacement(hWnd, &wndPlacement);
				registersX = wndPlacement.rcNormalPosition.left;
				registersY = wndPlacement.rcNormalPosition.top;
				project->registersWindowInfo.x = registersX;
				project->registersWindowInfo.y = registersY;
			}
			return FALSE;
			break;
		}

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				project->registersWindowInfo.maximized = FALSE;
				project->registersWindowInfo.minimized = TRUE;
				return FALSE;
			}
			if (wParam == SIZE_MAXIMIZED)
			{
				project->registersWindowInfo.minimized = FALSE;
				project->registersWindowInfo.maximized = TRUE;
				return FALSE;
			}
			if (wParam == SIZE_RESTORED)
			{
				project->registersWindowInfo.minimized = FALSE;
				project->registersWindowInfo.maximized = FALSE;
			}
			return FALSE;
			break;
		}

		case WM_CLOSE:
		{
			project->registersWindowInfo.open = FALSE;
			break;
		}

		case WM_DESTROY:
		{
			::DeleteObject(regNamesFont);
			::DeleteObject(regValuesFont);
			registersWindowHandle = NULL;
			break;
		}

		default:
			break;
	}

	// Process any messages that we didn't take care of
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


void UpdateRegisters(unsigned char * values, int offset, int count)
{
	int pos;

	if (offset >= registersSize)
		return;

	pos = offset;
	do
	{
		if (pos < 32)					// R0 -> R31
		{
			registers[31-pos] = values[pos];
			registersMask[31-pos] = 1;
		}
		else if (pos < 35)				// SREG & SP
		{
			registers[pos] = values[pos];
			registersMask[pos] = 1;
		}
		else if (pos == 35 || pos == 36) // PC
		{
			if (pos == 35)
			{
				registers[36] = values[pos]; // Hi & Lo bytes are swapped
				registersMask[36] = 1;
			}
			else
			{
				registers[35] = values[pos];
				registersMask[35] = 1;
			}
		}
		pos++;
	}
	while (pos < offset+count && pos < registersSize);
}


void DrawRegisterValues()
{
	SIZE charSize;
	int i, j;
	int x, y;
	int value;
	char buffer[10];

	if (!registersWindowHandle)
		return;

	HDC hDC = GetDC(registersWindowHandle);
	::SelectObject(hDC, regValuesFont);
	::SetTextColor(hDC, VALUECOLOUR);
	::SetBkColor(hDC, VALUEBACKCOLOUR);
	::SetBkMode(hDC, OPAQUE);
	::GetTextExtentPoint32(hDC, "E", 1, &charSize);

	buffer[2] = 0;
	for (i=0; i<COLUMNS; i++)
	{
		x = LEFT+(i*COLUMNINTERVAL);
		for (j=0; j<ROWS; j++)
		{
			y = TOP+(j*ROWINTERVAL);

			RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+(2*charSize.cx),
				4+charSize.cy);

			if (registersMask[(i*8)+j])
			{
				value = registers[(i*8)+j];
				buffer[0] = hexChars[value>>4];
				buffer[1] = hexChars[value&0xf];
			}
			else
			{
				buffer[0] = '?';
				buffer[1] = '?';
			}
			::TextOut(hDC, x, y, buffer, 2);
		}
	}

	y = XPOSY;
	buffer[4] = 0;
	for (i=0; i<3; i++)					// X, Y & Z
	{
		value = registers[26+(i*2+1)];
		value <<= 8;
		value += registers[26+(i*2)];
		if (registersMask[26+(i*2+1)])
		{
			buffer[0] = hexChars[value>>12];
			buffer[1] = hexChars[(value>>8)&0xf];
		}
		else
		{
			buffer[0] = '?';
			buffer[1] = '?';
		}
		if (registersMask[26+(i*2)])
		{
			buffer[2] = hexChars[(value>>4)&0xf];
			buffer[3] = hexChars[value&0xf];
		}
		else
		{
			buffer[2] = '?';
			buffer[3] = '?';
		}
		x = XPOSX+(i*XYZINTERVAL);		// X pos
		RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+(4*charSize.cx),
			4+charSize.cy);
		::TextOut(hDC, x, y, buffer, 4);
	}

	x = SREGPOSX;						// SREG
	y = SREGPOSY;
	if (registersMask[32])
	{
		value = registers[32];
		buffer[0] = hexChars[value>>4];
		buffer[1] = hexChars[value&0xf];
	}
	else
	{
		buffer[0] = '?';
		buffer[1] = '?';
	}
	RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+(2*charSize.cx),
		4+charSize.cy);
	::TextOut(hDC, x, y, buffer, 2);

	y = SREGFLAGSPOSY;					// SREG flags
	for (i=0; i<8; i++)
	{
		x = SREGFLAGSPOSX+((7-i)*SREGFLAGSINTERVAL);
		RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+charSize.cx,
			4+charSize.cy);
		if (registersMask[32])
		{
			if (registers[32]&(1<<i))
				::TextOut(hDC, x, y, "1", 1);
			else
				::TextOut(hDC, x, y, "0", 1);
		}
		else
			::TextOut(hDC, x, y, "?", 1);
	}

	y = SPPOSY;
	buffer[4] = 0;
	for (i=0; i<2; i++)					// SP & PC
	{
		value = registers[33+(i*2+1)];
		value <<= 8;
		value += registers[33+(i*2)];
		if (registersMask[33+(i*2+1)])
		{
			buffer[0] = hexChars[value>>12];
			buffer[1] = hexChars[(value>>8)&0xf];
		}
		else
		{
			buffer[0] = '?';
			buffer[1] = '?';
		}
		if (registersMask[33+(i*2)])
		{
			buffer[2] = hexChars[(value>>4)&0xf];
			buffer[3] = hexChars[value&0xf];
		}
		else
		{
			buffer[2] = '?';
			buffer[3] = '?';
		}
		x = SPPOSX+(i*XYZINTERVAL);		// X pos
		RegDrawIndentedRect(hDC, x-BEVEL, y-2, (2*BEVEL)+(4*charSize.cx),
			4+charSize.cy);
		::TextOut(hDC, x, y, buffer, 4);
	}

	ReleaseDC(registersWindowHandle, hDC);
}


void RegDrawIndentedRect(HDC hDC, int x, int y, int w, int h)
{
	RECT rect;

	rect.left = x;
	rect.top = y;
	rect.right = x+w;
	rect.bottom = y+h;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));

	rect.right = rect.right-1;
	rect.bottom = rect.bottom-1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));

	rect.left = rect.left+1;
	rect.top = rect.top+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));

	rect.right = rect.right-1;
	rect.bottom = rect.bottom-1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));

	rect.left = rect.left+1;
	rect.top = rect.top+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
}
