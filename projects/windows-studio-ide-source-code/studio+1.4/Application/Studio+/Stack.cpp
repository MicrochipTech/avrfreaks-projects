// Stack.cpp

#define STACK

#define WIN32_LEAN_AND_MEAN				// No MFC, we want speed!!


// Includes ///////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>					// All the windows headers
#include <windowsx.h>					// Useful macros
#include <stdio.h>						// Standard IO
#include <io.h>							// _lseek()
#include <math.h>
#include <fstream.h>					// File IO

#include "Stack.h"
#include "Project.h"
#include "Registers.h"
#include "Comms.h"
//#include "Resource.h"
#include "Globals.h"


// DEFINES ////////////////////////////////////////////////////////////////////

#define STACKWINDOW_CLASS_NAME "STACKWINDOW_CLASS_NAME"

#define NAMECOLOUR 0x00000000
#define VALUECOLOUR 0x00000099
#define VALUEBACKCOLOUR 0x00ffffff
#define BACKCOLOUR 0x00ffffff
#define DARKCOLOUR 0x00000000
#define LIGHTCOLOUR 0x00dfdfdf

#define WIDTH 353
#define HEIGHT 300
#define BORDER 5
#define BEVEL 2


// GLOBALS ////////////////////////////////////////////////////////////////////

HWND stackWindowHandle = NULL;
HWND stackScroller = NULL;
WNDPROC stackOldScroll = NULL;
HFONT stackNamesFont;
HFONT stackValuesFont;
int stackX, stackY, stackW, stackH;
bool stackWindowRegistered = false;
int stackValuesX;
int stackTopY;
int stackAddressX;
int stackHexX;
int stackAsciiX;
int stackScrollX;
int stackClientW;
int stackClientH;
int stackMaxW;
int stackMinW;
int stackMinH;
SCROLLINFO stackScrollInfo;
int firstStackAddress = 0;
int stackPos;
int stackPage;
int stackMax;
bool stackResized;


// FUNCTIONS //////////////////////////////////////////////////////////////////

BOOL OpenStackWindow()
{
	WNDCLASSEX winClass;
	int min, max;

	stackWindowHandle = NULL;

	// Fill the window class structure
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_DBLCLKS | CS_OWNDC | CS_VREDRAW;
	winClass.lpfnWndProc = StackProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInstance;
	winClass.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STACK));
	winClass.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_STACK));
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;		// I'm painting it
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = STACKWINDOW_CLASS_NAME;

	// Register the class
	if (!stackWindowRegistered && !RegisterClassEx(&winClass))
		return FALSE;
	stackWindowRegistered = true;

	// Find the window's preferred position & size
	stackX = project->stackWindowInfo.x;
	stackY = project->stackWindowInfo.y;
	stackW = project->stackWindowInfo.width;
	stackH = project->stackWindowInfo.height;
	min = project->stackWindowInfo.minimized;
	max = project->stackWindowInfo.maximized;

	firstStackAddress = stackAddress;

	// Create the window
	if (!(stackWindowHandle = CreateWindowEx(
		NULL,							// Extended Style
		STACKWINDOW_CLASS_NAME,			// Class
		"Stack",						// Title
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, // Style
		stackX, stackY,					// X & Y
		stackW, stackH,					// Width & Height
		clientAreaHandle,				// Parent
		NULL,							// Menu
		hInstance,						// This Application
		NULL)))							// Extra parameters
	{
		return FALSE;
	}
	::SetParent(stackWindowHandle, clientAreaHandle);
	if (min)
		::ShowWindow(stackWindowHandle, SW_MINIMIZE);
	if (max)
		::ShowWindow(stackWindowHandle, SW_MAXIMIZE);
	project->stackWindowInfo.open = TRUE;

	return TRUE;
}


LRESULT CALLBACK StackProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// This is the main message handler for the SRAM window
	PAINTSTRUCT ps;						// Used in WM_PAINT
	HDC hDC;							// Handle to a device context
	RECT rect;
	WINDOWPLACEMENT wndPlacement;
	int i;
	int x, y;
	char buffer[5];
	SIZE charSize;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	// Switch on message
	switch (msg)
	{
		case WM_CREATE:
		{
			stackScroller = ::CreateWindow(TEXT("scrollbar"), NULL,
				WS_CHILD|WS_VISIBLE|WS_TABSTOP|SBS_VERT, 0, 0, 0, 0,
				hWnd, (HMENU)IDC_STACK_SCROLLER, hInstance, NULL);
			stackPos = (sramStart+(sramSize-1))-firstStackAddress;
			stackScrollInfo.cbSize = sizeof SCROLLINFO;
			stackScrollInfo.fMask = SIF_POS;
			stackScrollInfo.nPos = stackPos;
			::SetScrollInfo(stackScroller, SB_CTL, &stackScrollInfo, TRUE);
			stackOldScroll = (WNDPROC) ::SetWindowLong(stackScroller,
				GWL_WNDPROC, (LONG)StackScrollProc);

			stackNamesFont = ::CreateFont(14, 0, 0, 0, FW_BOLD, false, false, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");
			stackValuesFont = ::CreateFont(16, 0, 0, 0, FW_MEDIUM, false, false, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, FIXED_PITCH|FF_MODERN, "Courier New");

			stackResized = false;
			return FALSE;
			break;
		}

		case WM_PAINT:
		{
			HBRUSH notStackBrush = NULL;
			HBRUSH debugStackBrush = NULL;
			HBRUSH hBrush = NULL;
			int colour;
			bool newBrush;
			int valCharH;

			hDC = BeginPaint(hWnd, &ps);

			if (project->stackWindowInfo.minimized)
			{
				EndPaint(hWnd, &ps);
				return FALSE;
			}

			notStackBrush = ::CreateSolidBrush(NOTSTACKCOLOUR);
			debugStackBrush = ::CreateSolidBrush(DEBUGSTACKCOLOUR);

			// First paint the background
			TRIVERTEX vert[2];
			GRADIENT_RECT gRect;
			vert[0].x = 0;
			vert[0].y = 0;
			vert[0].Red = (GRADIENTTOP&0xff)<<8; // 0x5000;
			vert[0].Green = GRADIENTTOP&0xff00; //0x5000;
			vert[0].Blue = (GRADIENTTOP&0xff0000)>>8; //0xff00;
			vert[0].Alpha = 0x0000;
			vert[1].x = stackW+1;
			vert[1].y = stackH+1;
			vert[1].Red = (GRADIENTBOTTOM&0xff)<<8; // 0xff00;
			vert[1].Green = GRADIENTBOTTOM&0xff00; //0xff00;
			vert[1].Blue = (GRADIENTBOTTOM&0xff0000)>>8; //0xff00;
			vert[1].Alpha = 0x0000;
			gRect.UpperLeft = 0;
			gRect.LowerRight = 1;
			::GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

			// Write the labels
			::SetTextColor(hDC, NAMECOLOUR);
			::SetBkColor(hDC, BACKCOLOUR);
			::SetBkMode(hDC, TRANSPARENT);
			::SelectObject(hDC, stackValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			valCharH = charSize.cy;
			::SelectObject(hDC, stackNamesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);

			rect.top = BORDER+BEVEL;
			rect.bottom = rect.top+valCharH;
			rect.left = stackAddressX;
			rect.right = rect.left+(8*charSize.cx);
			::DrawText(hDC, "ADDRESS:", 8, &rect,
				DT_LEFT|DT_NOCLIP|DT_VCENTER);
			rect.top = rect.top+3*BEVEL+valCharH;
			rect.bottom = rect.top+valCharH;
			::DrawText(hDC, "POINTER:", 8, &rect,
				DT_LEFT|DT_NOCLIP|DT_VCENTER);

			rect.top = stackTopY-(BEVEL+charSize.cy);
			rect.bottom = stackTopY+charSize.cy;
			rect.left = stackAddressX;
			rect.right = rect.left+(5*charSize.cx);
			::DrawText(hDC, "ADDR.", 5, &rect, DT_LEFT|DT_NOCLIP|DT_VCENTER);
			rect.left = stackHexX;
			rect.right = rect.left+(2*charSize.cx);
			::DrawText(hDC, "H.", 2, &rect, DT_LEFT|DT_NOCLIP|DT_VCENTER);
			rect.left = stackAsciiX;
			rect.right = rect.left+(2*charSize.cx);
			::DrawText(hDC, "A.", 2, &rect, DT_LEFT|DT_NOCLIP|DT_VCENTER);
			::SetBkColor(hDC, VALUEBACKCOLOUR);
			::SetBkMode(hDC, OPAQUE);

			// Write the data
			::SelectObject(hDC, stackValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);

			y = BORDER+BEVEL;
			hBrush = ::CreateSolidBrush(STACKADDRESSCOLOUR);
			::SetBkColor(hDC, STACKADDRESSCOLOUR);
			StackDrawTopBevel(hDC, stackValuesX-BEVEL, y-BEVEL,
				(2*BEVEL)+(4*charSize.cx));
			StackDrawIndentedRect(hDC, stackValuesX-BEVEL, y,
				(2*BEVEL)+(4*charSize.cx), charSize.cy, hBrush);
			buffer[0] = hexChars[stackAddress>>12];
			buffer[1] = hexChars[(stackAddress>>8)&0xf];
			buffer[2] = hexChars[(stackAddress>>4)&0xf];
			buffer[3] = hexChars[stackAddress&0xf];
			buffer[4] = 0;
			::SetTextColor(hDC, NAMECOLOUR);
			::TextOut(hDC, stackValuesX, y, buffer, 4);
			StackDrawBottomBevel(hDC, stackValuesX-BEVEL,
				y+charSize.cy, (2*BEVEL)+(4*charSize.cx));
			::DeleteObject(hBrush);

			y += charSize.cy+3*BEVEL;
			hBrush = ::CreateSolidBrush(STACKPOINTERCOLOUR);
			::SetBkColor(hDC, STACKPOINTERCOLOUR);
			StackDrawTopBevel(hDC, stackValuesX-BEVEL, y-BEVEL,
				(2*BEVEL)+(4*charSize.cx));
			StackDrawIndentedRect(hDC, stackValuesX-BEVEL, y,
				(2*BEVEL)+(4*charSize.cx), charSize.cy, hBrush);
			buffer[0] = hexChars[stackPointer>>12];
			buffer[1] = hexChars[(stackPointer>>8)&0xf];
			buffer[2] = hexChars[(stackPointer>>4)&0xf];
			buffer[3] = hexChars[stackPointer&0xf];
			buffer[4] = 0;
			::SetTextColor(hDC, NAMECOLOUR);
			::TextOut(hDC, stackValuesX, y, buffer, 4);
			StackDrawBottomBevel(hDC, stackValuesX-BEVEL,
				y+charSize.cy, (2*BEVEL)+(4*charSize.cx));
			::DeleteObject(hBrush);

			StackDrawTopBevel(hDC, stackAddressX-BEVEL, stackTopY-BEVEL,
				(2*BEVEL)+(4*charSize.cx));
			StackDrawTopBevel(hDC, stackHexX-BEVEL, stackTopY-BEVEL,
				(2*BEVEL)+(2*charSize.cx));
			StackDrawTopBevel(hDC, stackAsciiX-BEVEL, stackTopY-BEVEL,
				(2*BEVEL)+charSize.cx);
			int pos = firstStackAddress;
			int rows = (stackClientH-(stackTopY+BEVEL+BORDER))/charSize.cy;
			for (i=0; i<rows; i++)		// Each visible row
			{
				y = stackTopY+(i*charSize.cy);
				x = stackAddressX;
				
				newBrush = false;
				colour = WHITE;
				hBrush = (HBRUSH)::GetStockObject(WHITE_BRUSH);
				if (pos == stackAddress)
				{
					::SetBkColor(hDC, STACKADDRESSCOLOUR);
					hBrush = ::CreateSolidBrush(STACKADDRESSCOLOUR);
					colour = STACKADDRESSCOLOUR;
					newBrush = true;
				}
				else if (pos == stackPointer)
				{
					hBrush = ::CreateSolidBrush(STACKPOINTERCOLOUR);
					colour = STACKPOINTERCOLOUR;
					newBrush = true;
				}
				else if (pos > stackAddress || pos < stackPointer)
				{
					hBrush = notStackBrush;
					colour = NOTSTACKCOLOUR;
				}
				else if (pos > stackPointer &&
					pos <= stackPointer+project->debugStackBytes)
				{
					hBrush = debugStackBrush;
					colour = DEBUGSTACKCOLOUR;
				}
				::SetBkColor(hDC, colour);

				rect.top = y;
				rect.bottom = y+charSize.cy;
				rect.left = x;
				rect.right = rect.left+(4*charSize.cx);
				::FillRect(hDC, &rect, hBrush);
				StackDrawIndentedRect(hDC, stackAddressX-BEVEL, y,
					(2*BEVEL)+(4*charSize.cx), charSize.cy, hBrush);

				rect.left = stackHexX;
				rect.right = rect.left+(2*charSize.cx);
				::FillRect(hDC, &rect, hBrush);
				StackDrawIndentedRect(hDC, stackHexX-BEVEL, y, (2*BEVEL)+
					(2*charSize.cx), charSize.cy, hBrush);

				rect.left = stackAsciiX;
				rect.right = rect.left+charSize.cx;
				::FillRect(hDC, &rect, hBrush);
				StackDrawIndentedRect(hDC, stackAsciiX-BEVEL, y,
					(2*BEVEL)+charSize.cx, charSize.cy, hBrush);
				
				if (pos >= sramStart)
				{
					// Address
					buffer[0] = hexChars[pos>>12];
					buffer[1] = hexChars[(pos>>8)&0xf];
					buffer[2] = hexChars[(pos>>4)&0xf];
					buffer[3] = hexChars[pos&0xf];
					buffer[4] = 0;
					::SetTextColor(hDC, NAMECOLOUR);
					::TextOut(hDC, x, y, buffer, 4);

					::SetTextColor(hDC, VALUECOLOUR);
					buffer[2] = 0;

					int value = sram[pos-sramStart];

					// Hex value
					x = stackHexX;
					if (sramMask[pos-sramStart])
					{
						buffer[0] = hexChars[value>>4];
						buffer[1] = hexChars[value&0xf];
					}
					else
					{
						buffer[0] = '?';
						buffer[1] = '?';
					}
					::TextOut(hDC, x, y, buffer, 2);

					// Ascii char
					x = stackAsciiX;
					if (sramMask[pos-sramStart])
					{
						if (value < 32)
							buffer[0] = '.';
						else
							buffer[0] = value;
					}
					else
						buffer[0] = '?';
					buffer[1] = 0;
					::TextOut(hDC, x, y, buffer, 1);
				}
				pos--;					// Need to change this for ascending

				if (newBrush && i != rows-1)
				{
					::DeleteObject(hBrush);
					hBrush = NULL;
				}
			}

			if (i*charSize.cy != stackClientH-(stackTopY+BEVEL+BORDER))
			{
				rect.top = stackTopY+(i*charSize.cy);
				rect.bottom = stackClientH-(BEVEL+BORDER);
				rect.left = stackAddressX;
				rect.right = rect.left+(4*charSize.cx);
				::FillRect(hDC, &rect, hBrush);
				StackDrawIndentedRect(hDC, stackAddressX-BEVEL,
					stackTopY+(i*charSize.cy), (2*BEVEL)+(4*charSize.cx),
					stackClientH-(stackTopY+BEVEL+BORDER+(i*charSize.cy)),
					hBrush);
				rect.left = stackHexX;
				rect.right = rect.left+(2*charSize.cx);
				::FillRect(hDC, &rect, hBrush);
				StackDrawIndentedRect(hDC, stackHexX-BEVEL,
					stackTopY+(i*charSize.cy), (2*BEVEL)+(2*charSize.cx),
					stackClientH-(stackTopY+BEVEL+BORDER+(i*charSize.cy)),
					hBrush);
				rect.left = stackAsciiX;
				rect.right = rect.left+charSize.cx;
				::FillRect(hDC, &rect, hBrush);
				StackDrawIndentedRect(hDC, stackAsciiX-BEVEL,
					stackTopY+(i*charSize.cy), (2*BEVEL)+charSize.cx,
					stackClientH-(stackTopY+BEVEL+BORDER+(i*charSize.cy)),
					hBrush);
			}
			if (newBrush)
			{
				::DeleteObject(hBrush);
				hBrush = NULL;
			}

			StackDrawBottomBevel(hDC, stackAddressX-BEVEL,
				stackClientH-(BEVEL+BORDER), (2*BEVEL)+(4*charSize.cx));
			StackDrawBottomBevel(hDC, stackHexX-BEVEL,
				stackClientH-(BEVEL+BORDER), (2*BEVEL)+(2*charSize.cx));
			StackDrawBottomBevel(hDC, stackAsciiX-BEVEL,
				stackClientH-(BEVEL+BORDER), (2*BEVEL)+charSize.cx);
			EndPaint(hWnd, &ps);

			::DeleteObject(notStackBrush);
			::DeleteObject(debugStackBrush);
			
			return (0);
			break;
		}

		case WM_LBUTTONDOWN:
		{
			::PostMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
			return FALSE;
			break;
		}

		case WM_MOVE:
		{
			if (!project->stackWindowInfo.minimized &&
				!project->stackWindowInfo.maximized)
			{
				::GetWindowPlacement(hWnd, &wndPlacement);
				stackX = wndPlacement.rcNormalPosition.left;
				stackY = wndPlacement.rcNormalPosition.top;
				project->stackWindowInfo.x = stackX;
				project->stackWindowInfo.y = stackY;
			}
			return FALSE;
			break;
		}

		case WM_SIZING:
		{
			RECT * size = (RECT *)lParam;
			if (size->right-size->left < stackMinW)
				size->right = size->left+stackMinW;
			if (size->right-size->left > stackMaxW)
				size->right = size->left+stackMaxW;
			if (size->bottom-size->top < stackMinH)
				size->bottom = size->top+stackMinH;
			if (endCondition)
				updateStack = true;
			stackResized = true;
			return FALSE;
			break;
		}

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				project->stackWindowInfo.maximized = FALSE;
				project->stackWindowInfo.minimized = TRUE;
				return FALSE;
			}
			if (wParam == SIZE_MAXIMIZED)
			{
				project->stackWindowInfo.minimized = FALSE;
				project->stackWindowInfo.maximized = TRUE;
				return FALSE;
			}
			if (wParam != SIZE_RESTORED)
				return FALSE;
			project->stackWindowInfo.minimized = FALSE;
			project->stackWindowInfo.maximized = FALSE;
			::GetWindowPlacement(hWnd, &wndPlacement);
			project->stackWindowInfo.width =
				wndPlacement.rcNormalPosition.right-
				wndPlacement.rcNormalPosition.left;
			project->stackWindowInfo.height =
				wndPlacement.rcNormalPosition.bottom-
				wndPlacement.rcNormalPosition.top;

			if (firstStackAddress < sramStart) // If sramStart has changed
				firstStackAddress = sramStart; // Update it
			if (firstStackAddress >= sramStart+sramSize)
				firstStackAddress = (sramStart+sramSize)-1;
			::GetWindowRect(hWnd, &rect);
			stackH = rect.bottom-rect.top;
			stackW = rect.right-rect.left;
			hDC = GetDC(hWnd);
			::SelectObject(hDC, stackValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			ReleaseDC(hWnd, hDC);
			stackClientW = LOWORD(lParam);
			stackClientH = HIWORD(lParam);
			stackTopY = 2*BORDER+6*BEVEL+3*charSize.cy;
			stackMinW = (stackW-stackClientW)+9*BORDER+6*BEVEL+
				(4+2+1+2)*charSize.cx;
			stackMaxW = stackMinW;
			stackMinH = (stackH-stackClientH)+stackTopY+BORDER+BEVEL+
				4*charSize.cy;
			stackAddressX = BORDER*3+BEVEL;
			stackHexX = stackAddressX+(4*charSize.cx)+BEVEL+BORDER+BEVEL;
			stackAsciiX = stackHexX+(2*charSize.cx)+BEVEL+BORDER+BEVEL;
			stackScrollX = stackAsciiX+charSize.cx+BEVEL+2*BORDER;
			stackValuesX = (stackScrollX+2*charSize.cx)-(2*BEVEL+4*charSize.cx);
			::MoveWindow(stackScroller, stackScrollX, stackTopY, 2*charSize.cx,
				(HIWORD(lParam))-(stackTopY+BEVEL+BORDER), TRUE);
			stackPage = ((stackClientH-(stackTopY+BEVEL+BORDER))/charSize.cy);
			stackMax = sramSize-1;
			project->stackStartAddress = max(firstStackAddress-(stackPage-1),
				sramStart);
			stackScrollInfo.fMask = SIF_RANGE|SIF_PAGE;
			stackScrollInfo.nMin = 0;
			stackScrollInfo.nMax = stackMax;
			stackScrollInfo.nPage = stackPage;
			::SetScrollInfo(stackScroller, SB_CTL, &stackScrollInfo, TRUE);
			stackScrollInfo.fMask = SIF_POS;
			::GetScrollInfo(stackScroller, SB_CTL, &stackScrollInfo);
			if (stackPos != stackScrollInfo.nPos && stackResized)
			{
				stackResized = false;
				stackPos = stackScrollInfo.nPos;
				firstStackAddress = (sramStart+stackMax)-stackPos;
				project->stackEndAddress = firstStackAddress+1;
				project->stackStartAddress = max(firstStackAddress-(stackPage-1),
					sramStart);
				if (project->stackStartAddress == project->stackEndAddress)
					project->stackEndAddress++;
			}
//			::SetFocus(stackWindowHandle); // Stop thumb from flashing
			if (stackPage > stackMax)
				ShowWindow(stackScroller, SW_HIDE);
			else
				ShowWindow(stackScroller, SW_SHOW);
			return FALSE;
			break;
		}

		case WM_VSCROLL:
		{
			switch (LOWORD(wParam))
			{
				case SB_PAGEDOWN:
					stackPos += stackPage-1;

				case SB_LINEDOWN:
					stackPos = min(stackMax-(stackPage-1), stackPos+1);
					break;

				case SB_PAGEUP:
					stackPos -= stackPage-1;

				case SB_LINEUP:
					stackPos = max(0, stackPos-1);
					break;

				case SB_TOP:			// Stack address
					stackPos = max(0, (sramStart+(sramSize-1))-
						(stackAddress+1));
					break;

				case SB_BOTTOM:			// Stack pointer
					stackPos = max(0, (sramStart+(sramSize-1))-
						(stackPointer+(stackPage-2)));
					break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:
					stackPos = HIWORD(wParam);
					break;

				default:
					break;
			}
			firstStackAddress = (sramStart+stackMax)-stackPos;
			project->stackEndAddress = firstStackAddress+1;
			project->stackStartAddress = max(firstStackAddress-(stackPage-1),
				sramStart);
			if (endCondition)
				updateStack = true;
			DrawStackValues();
			::SetScrollPos(stackScroller, SB_CTL, stackPos, TRUE);
//			::SetFocus(stackWindowHandle);
			return FALSE;
		}

		case WM_CLOSE:
			project->stackWindowInfo.open = FALSE;
			break;

		case WM_DESTROY:
		{
			::DeleteObject(stackNamesFont);
			::DeleteObject(stackValuesFont);
			stackWindowHandle = NULL;
			break;
		}

		default:
			break;
	}

	// Process any messages that we didn't take care of
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT CALLBACK StackScrollProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam)
{
	int id = ::GetWindowLong(hWnd, GWL_ID);

	return ::CallWindowProcA(stackOldScroll, hWnd, msg, wParam, lParam);
}


void DrawStackValues()
{
	char buffer[10];
	SIZE charSize;
	int rows;
	int pos, value;
	int i;
	int x, y;
	int colour;
	bool newBrush;
	HBRUSH hBrush = NULL;
	HBRUSH notStackBrush = ::CreateSolidBrush(NOTSTACKCOLOUR);
	HBRUSH debugStackBrush = ::CreateSolidBrush(DEBUGSTACKCOLOUR);

	HDC hDC = GetDC(stackWindowHandle);

	::SelectObject(hDC, stackValuesFont);
	::GetTextExtentPoint32(hDC, "E", 1, &charSize);

	// Write the data
	::SetBkColor(hDC, VALUEBACKCOLOUR);
	::SetBkMode(hDC, OPAQUE);

	y = BORDER+BEVEL;
	hBrush = ::CreateSolidBrush(STACKADDRESSCOLOUR);
	::SetBkColor(hDC, STACKADDRESSCOLOUR);
	StackDrawTopBevel(hDC, stackValuesX-BEVEL, y-BEVEL,
		(2*BEVEL)+(4*charSize.cx));
	StackDrawIndentedRect(hDC, stackValuesX-BEVEL, y,
		(2*BEVEL)+(4*charSize.cx), charSize.cy, hBrush);
	buffer[0] = hexChars[stackAddress>>12];
	buffer[1] = hexChars[(stackAddress>>8)&0xf];
	buffer[2] = hexChars[(stackAddress>>4)&0xf];
	buffer[3] = hexChars[stackAddress&0xf];
	buffer[4] = 0;
	::SetTextColor(hDC, NAMECOLOUR);
	::TextOut(hDC, stackValuesX, y, buffer, 4);
	StackDrawBottomBevel(hDC, stackValuesX-BEVEL,
		y+charSize.cy, (2*BEVEL)+(4*charSize.cx));
	::DeleteObject(hBrush);

	y += charSize.cy+3*BEVEL;
	hBrush = ::CreateSolidBrush(STACKPOINTERCOLOUR);
	::SetBkColor(hDC, STACKPOINTERCOLOUR);
	StackDrawTopBevel(hDC, stackValuesX-BEVEL, y-BEVEL,
		(2*BEVEL)+(4*charSize.cx));
	StackDrawIndentedRect(hDC, stackValuesX-BEVEL, y,
		(2*BEVEL)+(4*charSize.cx), charSize.cy, hBrush);
	buffer[0] = hexChars[stackPointer>>12];
	buffer[1] = hexChars[(stackPointer>>8)&0xf];
	buffer[2] = hexChars[(stackPointer>>4)&0xf];
	buffer[3] = hexChars[stackPointer&0xf];
	buffer[4] = 0;
	::SetTextColor(hDC, NAMECOLOUR);
	::TextOut(hDC, stackValuesX, y, buffer, 4);
	StackDrawBottomBevel(hDC, stackValuesX-BEVEL,
		y+charSize.cy, (2*BEVEL)+(4*charSize.cx));
	::DeleteObject(hBrush);

	rows = (stackClientH-(stackTopY+BEVEL+BORDER))/charSize.cy;
	pos = firstStackAddress;
	for (i=0; i<rows; i++)				// Each visible row
	{
		y = stackTopY+(i*charSize.cy);
		x = stackAddressX;

		newBrush = false;
		colour = WHITE;
		hBrush = (HBRUSH)::GetStockObject(WHITE_BRUSH);
		if (pos == stackAddress)
		{
			::SetBkColor(hDC, STACKADDRESSCOLOUR);
			hBrush = ::CreateSolidBrush(STACKADDRESSCOLOUR);
			colour = STACKADDRESSCOLOUR;
			newBrush = true;
		}
		else if (pos == stackPointer)
		{
			hBrush = ::CreateSolidBrush(STACKPOINTERCOLOUR);
			colour = STACKPOINTERCOLOUR;
			newBrush = true;
		}
		else if (pos > stackAddress || pos < stackPointer)
		{
			hBrush = notStackBrush;
			colour = NOTSTACKCOLOUR;
		}
		else if (pos > stackPointer &&
			pos <= stackPointer+project->debugStackBytes)
		{
			hBrush = debugStackBrush;
			colour = DEBUGSTACKCOLOUR;
		}
		::SetBkColor(hDC, colour);

		StackDrawIndentedRect(hDC, stackAddressX-BEVEL, y,
			(2*BEVEL)+(4*charSize.cx), charSize.cy, hBrush);
		StackDrawIndentedRect(hDC, stackHexX-BEVEL, y, (2*BEVEL)+
			(2*charSize.cx), charSize.cy, hBrush);
		StackDrawIndentedRect(hDC, stackAsciiX-BEVEL, y,
			(2*BEVEL)+charSize.cx, charSize.cy, hBrush);

		if (pos >= sramStart)
		{
			// Address
			buffer[0] = hexChars[pos>>12];
			buffer[1] = hexChars[(pos>>8)&0xf];
			buffer[2] = hexChars[(pos>>4)&0xf];
			buffer[3] = hexChars[pos&0xf];
			buffer[4] = 0;
			::SetTextColor(hDC, NAMECOLOUR);
			::TextOut(hDC, x, y, buffer, 4);

			::SetTextColor(hDC, VALUECOLOUR);
			buffer[2] = 0;

			value = sram[pos-sramStart];

			// Hex value
			x = stackHexX;
			if (sramMask[pos-sramStart])
			{
				buffer[0] = hexChars[value>>4];
				buffer[1] = hexChars[value&0xf];
			}
			else
			{
				buffer[0] = '?';
				buffer[1] = '?';
			}
			::TextOut(hDC, x, y, buffer, 2);

			// Ascii char
			x = stackAsciiX;
			if (sramMask[pos-sramStart])
			{
				if (value < 32)
					buffer[0] = '.';
				else
					buffer[0] = value;
			}
			else
				buffer[0] = '?';
			buffer[1] = 0;
			::TextOut(hDC, x, y, buffer, 1);
		}
		pos--;

		if (newBrush && i != rows-1)
			::DeleteObject(hBrush);
	}
	if (i*charSize.cy != stackClientH-(stackTopY+BEVEL+BORDER))
	{
		StackDrawIndentedRect(hDC, stackAddressX-BEVEL,
			stackTopY+(i*charSize.cy), (2*BEVEL)+(4*charSize.cx),
			stackClientH-(stackTopY+BEVEL+BORDER+(i*charSize.cy)),
			hBrush);
		StackDrawIndentedRect(hDC, stackHexX-BEVEL,
			stackTopY+(i*charSize.cy), (2*BEVEL)+(2*charSize.cx),
			stackClientH-(stackTopY+BEVEL+BORDER+(i*charSize.cy)),
			hBrush);
		StackDrawIndentedRect(hDC, stackAsciiX-BEVEL,
			stackTopY+(i*charSize.cy), (2*BEVEL)+charSize.cx,
			stackClientH-(stackTopY+BEVEL+BORDER+(i*charSize.cy)),
			hBrush);
	}
	if (newBrush)
		::DeleteObject(hBrush);

	ReleaseDC(stackWindowHandle, hDC);
	::DeleteObject(notStackBrush);
	::DeleteObject(debugStackBrush);
}


void StackDrawTopBevel(HDC hDC, int x, int y, int w)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x+w;
	rect.bottom = y+2;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	rect.right = rect.right-1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
	rect.left = rect.left+1;
	rect.top = rect.top+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	rect.right = rect.right-1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
}


void StackDrawIndentedRect(HDC hDC, int x, int y, int w, int h,
	HBRUSH bkBrush)
{
	RECT rect;

	rect.top = y;
	rect.bottom = y+h;
	rect.left = x;
	rect.right = rect.left+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
	rect.left = rect.left+1;
	rect.right = rect.left+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	rect.left = rect.left+1;
	rect.right = rect.left+1;
//	rect.right = x+(w-2);
	::FillRect(hDC, &rect, bkBrush);
	rect.left = x+(w-3);
	rect.right = rect.left+1;
	::FillRect(hDC, &rect, bkBrush);
	rect.left = x+(w-2);
	rect.right = rect.left+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	rect.left = rect.left+1;
	rect.right = rect.left+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
}


void StackDrawBottomBevel(HDC hDC, int x, int y, int w)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x+w;
	rect.bottom = y+2;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	rect.right = rect.right-1;
	rect.bottom = rect.bottom-1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
	rect.left = rect.left+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
}


void EnsureStackVisible()
{
	firstStackAddress = stackAddress;
	if (firstStackAddress != sramStart+(sramSize-1))
		firstStackAddress++;
	stackPos = (sramStart+(sramSize-1))-firstStackAddress;
	project->stackEndAddress = firstStackAddress+1;
	project->stackStartAddress = max(firstStackAddress-(stackPage-1),
		sramStart);
	stackScrollInfo.fMask = SIF_POS;
	::GetScrollInfo(stackScroller, SB_CTL, &stackScrollInfo);
	if (stackPos != stackScrollInfo.nPos)
	{
		stackScrollInfo.nPos = stackPos;
		::SetScrollInfo(stackScroller, SB_CTL, &stackScrollInfo, TRUE);
		::InvalidateRect(stackWindowHandle, NULL, FALSE);
		::UpdateWindow(stackWindowHandle);
	}
}
