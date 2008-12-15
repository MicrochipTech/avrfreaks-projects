// Memory.cpp

#define MEMORY

#define WIN32_LEAN_AND_MEAN				// No MFC, we want speed!!


// Includes ///////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>					// All the windows headers
#include <windowsx.h>					// Useful macros
#include <stdio.h>						// Standard IO
#include <io.h>							// _lseek()
#include <math.h>
#include <fstream.h>					// File IO

#include "Memory.h"
#include "Project.h"
#include "Registers.h"
#include "Comms.h"
#include "Resource.h"					// IDB_BLUECHARS
#include "Globals.h"


// DEFINES ////////////////////////////////////////////////////////////////////

#define SRAMWINDOW_CLASS_NAME "SRAMWINDOW_CLASS_NAME"
#define EEPROMWINDOW_CLASS_NAME "EEPROMWINDOW_CLASS_NAME"

#define NAMECOLOUR 0x00000000
#define VALUECOLOUR 0x00000099
#define VALUEBACKCOLOUR 0x00ffffff
#define BACKCOLOUR 0x00ffffff
#define DARKCOLOUR 0x00000000
#define LIGHTCOLOUR 0x00dfdfdf

#define WIDTH 353
#define HEIGHT 300
#define BORDER 5
#define BEVEL 3


// GLOBALS ////////////////////////////////////////////////////////////////////

HWND sramWindowHandle = NULL;
HWND sramScroller = NULL;
WNDPROC sramOldScroll = NULL;
HFONT sramNamesFont;
HFONT sramValuesFont;
int sramX, sramY, sramW, sramH;
bool sramWindowRegistered = false;
int sramBytesPerRow;
int topY;
int sramHexX;
int sramAsciiX;
int sramScrollX;
int sramClientW;
int sramClientH;
int sramMaxW;
int sramMinW;
int sramMinH;
SCROLLINFO sramScrollInfo;
int firstSramAddress = 0; //sramFirst;
int sramPos;
int sramPage;
int sramMax;

HWND eepromWindowHandle = NULL;
HWND eepromScroller = NULL;
WNDPROC eepromOldScroll = NULL;
HFONT eepromNamesFont;
HFONT eepromValuesFont;
int eepromX, eepromY, eepromW, eepromH;
bool eepromWindowRegistered = false;
int eepromBytesPerRow;
int eepromHexX;
int eepromAsciiX;
int eepromScrollX;
int eepromClientW;
int eepromClientH;
int eepromMaxW;
int eepromMinW;
int eepromMinH;
SCROLLINFO eepromScrollInfo;
int firstEepromAddress = 0; //eepromFirst;
int eepromPos;
int eepromPage;
int eepromMax;


// FUNCTIONS //////////////////////////////////////////////////////////////////

BOOL OpenSramWindow()
{
	WNDCLASSEX winClass;
	int min, max;

	sramWindowHandle = NULL;

	// Fill the window class structure
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_DBLCLKS | CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	winClass.lpfnWndProc = SramProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInstance;
	winClass.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SRAM));
	winClass.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SRAM));
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;		// I'm painting it
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = SRAMWINDOW_CLASS_NAME;

	// Register the class
	if (!sramWindowRegistered && !RegisterClassEx(&winClass))
		return FALSE;

	sramWindowRegistered = true;

	// Find the window's preferred position & size
	sramX = project->sramWindowInfo.x;
	sramY = project->sramWindowInfo.y;
	sramW = project->sramWindowInfo.width;
	sramH = project->sramWindowInfo.height;
	min = project->sramWindowInfo.minimized;
	max = project->sramWindowInfo.maximized;

	firstSramAddress = sramStart;

	// Create the window
	if (!(sramWindowHandle = CreateWindowEx(
		NULL,							// Extended Style
		SRAMWINDOW_CLASS_NAME,			// Class
		"SRAM",							// Title
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |WS_MINIMIZEBOX | WS_THICKFRAME, // Style
		sramX, sramY,					// X & Y
		sramW, sramH,					// Width & Height
		clientAreaHandle,				// Parent
		NULL,							// Menu
		hInstance,						// This Application
		NULL)))							// Extra parameters
	{
		return FALSE;
	}
	::SetParent(sramWindowHandle, clientAreaHandle);
	if (min)
		::ShowWindow(sramWindowHandle, SW_MINIMIZE);
	if (max)
		::ShowWindow(sramWindowHandle, SW_MAXIMIZE);
	project->sramWindowInfo.open = TRUE;

	return TRUE;
}


LRESULT CALLBACK SramProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// This is the main message handler for the SRAM window
	PAINTSTRUCT ps;						// Used in WM_PAINT
	HDC hDC;							// Handle to a device context
	RECT rect;
	WINDOWPLACEMENT wndPlacement;
	int i, j;
	int x, y;
	char buffer[5];
	SIZE charSize;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	// Switch on message
	switch (msg)
	{
		case WM_CREATE:
		{
			sramScroller = ::CreateWindow(TEXT("scrollbar"), NULL,
				WS_CHILD|WS_VISIBLE|WS_TABSTOP|SBS_VERT, 0, 0, 0, 0,
				hWnd, (HMENU)IDC_SRAM_SCROLLER, hInstance, NULL);
			sramScrollInfo.cbSize = sizeof SCROLLINFO;
			sramScrollInfo.fMask = SIF_POS;
			sramPos = 0;
			::SetScrollPos(sramScroller, SB_CTL, sramPos, FALSE);
			sramOldScroll = (WNDPROC) ::SetWindowLong(sramScroller,
				GWL_WNDPROC, (LONG)SramScrollProc);

			sramNamesFont = ::CreateFont(14, 0, 0, 0, FW_BOLD, false, false, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");
			sramValuesFont = ::CreateFont(16, 0, 0, 0, FW_MEDIUM, false, false, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, FIXED_PITCH|FF_MODERN, "Courier New");

			return FALSE;
			break;
		}

		case WM_PAINT:
		{
			hDC = BeginPaint(hWnd, &ps);

			if (project->sramWindowInfo.minimized)
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
			vert[1].x = sramW+1;
			vert[1].y = sramH+1;
			vert[1].Red = (GRADIENTBOTTOM&0xff)<<8; // 0xff00;
			vert[1].Green = GRADIENTBOTTOM&0xff00; //0xff00;
			vert[1].Blue = (GRADIENTBOTTOM&0xff0000)>>8; //0xff00;
			vert[1].Alpha = 0x0000;
			gRect.UpperLeft = 0;
			gRect.LowerRight = 1;
			::GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

			// Write the labels
			::SelectObject(hDC, sramNamesFont);
			::SetTextColor(hDC, NAMECOLOUR);
			::SetBkColor(hDC, BACKCOLOUR);
			::SetBkMode(hDC, TRANSPARENT);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			rect.top = topY-(BEVEL+BORDER+BEVEL+charSize.cy);
			rect.bottom = topY+charSize.cy;
			rect.left = BORDER+BEVEL;
			rect.right = rect.left+(5*charSize.cx);
			::DrawText(hDC, "ADDR:", 5, &rect, DT_LEFT|DT_NOCLIP|DT_VCENTER);
			rect.left = sramAsciiX;
			rect.right = rect.left+(5*charSize.cx);
			::DrawText(hDC, "ASCII", 5, &rect, DT_LEFT|DT_NOCLIP|DT_VCENTER);

			::SelectObject(hDC, sramValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			y = topY-(BEVEL+BORDER+BEVEL+charSize.cy);
			MemDrawTopBevel(hDC, sramHexX-BEVEL, y-2,
				(2*BEVEL)+(sramBytesPerRow*2*charSize.cx)+
				((sramBytesPerRow-1)*BORDER));
			rect.top = y;
			rect.bottom = y+charSize.cy;
			rect.left = sramHexX;
			rect.right = rect.left+((sramBytesPerRow*2*charSize.cx)+
				((sramBytesPerRow-1)*BORDER));
			::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
			MemDrawIndentedRect(hDC, sramHexX-BEVEL, y, (2*BEVEL)+
				(sramBytesPerRow*2*charSize.cx)+
				((sramBytesPerRow-1)*BORDER), charSize.cy);
			::SetBkColor(hDC, VALUEBACKCOLOUR);
			::SetBkMode(hDC, OPAQUE);
			for (i=0; i<sramBytesPerRow; i++)
			{
				x = sramHexX+(i*((2*charSize.cx)+BORDER));
				buffer[0] = hexChars[i>>4];
				buffer[1] = hexChars[i&0xf];
				::TextOut(hDC, x, y, buffer, 2);
			}
			MemDrawBottomBevel(hDC, sramHexX-BEVEL, y+charSize.cy,
				(2*BEVEL)+(sramBytesPerRow*2*charSize.cx)+
				((sramBytesPerRow-1)*BORDER));

			// Write the data
			::SelectObject(hDC, sramValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			MemDrawTopBevel(hDC, BORDER, topY-2, (2*BEVEL)+(4*charSize.cx));
			MemDrawTopBevel(hDC, sramHexX-BEVEL, topY-2,
				(2*BEVEL)+(sramBytesPerRow*2*charSize.cx)+
				((sramBytesPerRow-1)*BORDER));
			MemDrawTopBevel(hDC, sramAsciiX-BEVEL, topY-2,
				(2*BEVEL)+(sramBytesPerRow*charSize.cx));
			int pos = firstSramAddress;
			for (i=0; i<(sramClientH-(topY+BEVEL+BORDER))/charSize.cy;
				i++)					// Each visible row
			{
				y = topY+(i*charSize.cy);
				x = BORDER+BEVEL;

				rect.top = y;
				rect.bottom = y+charSize.cy;
				rect.left = x;
				rect.right = rect.left+(4*charSize.cx);
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, x-BEVEL, y, (2*BEVEL)+(4*charSize.cx),
					charSize.cy);
				rect.left = sramHexX;
				rect.right = rect.left+((sramBytesPerRow*2*charSize.cx)+
					((sramBytesPerRow-1)*BORDER));
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, sramHexX-BEVEL, y, (2*BEVEL)+
					(sramBytesPerRow*2*charSize.cx)+
					((sramBytesPerRow-1)*BORDER), charSize.cy);
				rect.left = sramAsciiX;
				rect.right = rect.left+(sramBytesPerRow*charSize.cx);
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, sramAsciiX-BEVEL, y, (2*BEVEL)+
					(sramBytesPerRow*charSize.cx), charSize.cy);

				if (pos < sramStart+sramSize)
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
					for (j=0; j<sramBytesPerRow; j++) // For each value
					{
						if (pos+j < sramStart+sramSize)
						{
							int value = sram[(pos-sramStart)+j];

							// Hex value
							x = sramHexX+(j*((2*charSize.cx)+BORDER));
							if (sramMask[(pos-sramStart)+j])
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
							x = sramAsciiX+(j*charSize.cx);
							if (sramMask[(pos-sramStart)+j])
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
					}
				}
				pos += sramBytesPerRow;
			}
			if (i*charSize.cy != sramClientH-(topY+BEVEL+BORDER))
			{
				rect.top = topY+(i*charSize.cy);
				rect.bottom = sramClientH-(BEVEL+BORDER);
				rect.left = BORDER+BEVEL;
				rect.right = rect.left+(4*charSize.cx);
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, BORDER, topY+(i*charSize.cy),
					(2*BEVEL)+(4*charSize.cx),
					sramClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
				rect.left = sramHexX;
				rect.right = rect.left+((sramBytesPerRow*2*charSize.cx)+
					((sramBytesPerRow-1)*BORDER));
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, sramHexX-BEVEL, topY+(i*charSize.cy),
					(2*BEVEL)+(sramBytesPerRow*2*charSize.cx)+
					((sramBytesPerRow-1)*BORDER),
					sramClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
				rect.left = sramAsciiX;
				rect.right = rect.left+(sramBytesPerRow*charSize.cx);
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, sramAsciiX-BEVEL,
					topY+(i*charSize.cy), (2*BEVEL)+
					(sramBytesPerRow*charSize.cx),
					sramClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
			}

			MemDrawBottomBevel(hDC, BORDER, sramClientH-(BEVEL+BORDER),
				(2*BEVEL)+(4*charSize.cx));
			MemDrawBottomBevel(hDC, sramHexX-BEVEL, sramClientH-(BEVEL+BORDER),
				(2*BEVEL)+(sramBytesPerRow*2*charSize.cx)+
				((sramBytesPerRow-1)*BORDER));
			MemDrawBottomBevel(hDC, sramAsciiX-BEVEL,
				sramClientH-(BEVEL+BORDER),
				(2*BEVEL)+(sramBytesPerRow*charSize.cx));
			EndPaint(hWnd, &ps);
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
			if (!project->sramWindowInfo.minimized &&
				!project->sramWindowInfo.maximized)
			{
				::GetWindowPlacement(hWnd, &wndPlacement);
				sramX = wndPlacement.rcNormalPosition.left;
				sramY = wndPlacement.rcNormalPosition.top;
				project->sramWindowInfo.x = sramX;
				project->sramWindowInfo.y = sramY;
			}
			return FALSE;
			break;
		}

		case WM_SIZING:
		{
			RECT * size = (RECT *)lParam;
			if (size->right-size->left < sramMinW)
				size->right = size->left+sramMinW;
			if (size->right-size->left > sramMaxW)
				size->right = size->left+sramMaxW;
			if (size->bottom-size->top < sramMinH)
				size->bottom = size->top+sramMinH;
			if (endCondition)
				updateSram = true;
			return FALSE;
			break;
		}

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				project->sramWindowInfo.maximized = FALSE;
				project->sramWindowInfo.minimized = TRUE;
				return FALSE;
			}
			if (wParam == SIZE_MAXIMIZED)
			{
				project->sramWindowInfo.minimized = FALSE;
				project->sramWindowInfo.maximized = TRUE;
				return FALSE;
			}
			if (wParam != SIZE_RESTORED)
				return FALSE;
			project->sramWindowInfo.minimized = FALSE;
			project->sramWindowInfo.maximized = FALSE;
			::GetWindowPlacement(hWnd, &wndPlacement);
			project->sramWindowInfo.width =
				wndPlacement.rcNormalPosition.right-
				wndPlacement.rcNormalPosition.left;
			project->sramWindowInfo.height =
				wndPlacement.rcNormalPosition.bottom-
				wndPlacement.rcNormalPosition.top;

			if (firstSramAddress < sramStart) // If sramStart has changed
				firstSramAddress = sramStart; // Update it
			::GetWindowRect(hWnd, &rect);
			sramH = rect.bottom-rect.top;
			sramW = rect.right-rect.left;
			hDC = GetDC(hWnd);
			::SelectObject(hDC, sramValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			ReleaseDC(hWnd, hDC);
			sramClientW = LOWORD(lParam);
			sramClientH = HIWORD(lParam);
			topY = charSize.cy+(3*BEVEL)+(2*BORDER);
			sramMinW = (sramW-sramClientW)+(5+3)*BORDER+6*BEVEL+(4+8+4+2)*
				charSize.cx;
			sramMaxW = (sramW-sramClientW)+(5+15)*BORDER+6*BEVEL+
				(4+32+16+2)*charSize.cx;
			sramMinH = (sramH-sramClientH)+topY+BORDER+BEVEL+4*charSize.cy;
			sramBytesPerRow = sramClientW-(6*charSize.cx+4*BORDER+6*BEVEL);
			sramBytesPerRow /= (3*charSize.cx)+BORDER;
			sramHexX = BORDER+BEVEL+(4*charSize.cx)+BEVEL+BORDER+BEVEL;
			sramAsciiX = sramHexX+(sramBytesPerRow*2*charSize.cx)+
				((sramBytesPerRow-1)*BORDER)+BEVEL+BORDER+BEVEL;
			sramScrollX = sramAsciiX+(sramBytesPerRow*charSize.cx)+BEVEL+
				BORDER;
			::MoveWindow(sramScroller, sramScrollX, topY, 2*charSize.cx,
				(HIWORD(lParam))-(topY+BEVEL+BORDER), TRUE);
			sramPage = ((sramClientH-(topY+BEVEL+BORDER))/charSize.cy);
			sramMax = sramSize/sramBytesPerRow;
			if (sramMax*sramBytesPerRow != sramSize)
				sramMax++;
			sramMax--;
			project->sramEndAddress = min(firstSramAddress+
				(sramPage*sramBytesPerRow), sramStart+sramSize);
			sramScrollInfo.fMask = SIF_RANGE|SIF_PAGE;
			sramScrollInfo.nMin = 0;
			sramScrollInfo.nMax = sramMax;
			sramScrollInfo.nPage = sramPage;
			::SetScrollInfo(sramScroller, SB_CTL, &sramScrollInfo, TRUE);
			sramScrollInfo.fMask = SIF_POS;
			::GetScrollInfo(sramScroller, SB_CTL, &sramScrollInfo);
			if (sramPos != sramScrollInfo.nPos)
			{
				sramPos = sramScrollInfo.nPos;
				firstSramAddress = sramStart+(sramPos*sramBytesPerRow);
				project->sramStartAddress = firstSramAddress;
				project->sramEndAddress = min(firstSramAddress+
					(sramPage*sramBytesPerRow), sramStart+sramSize);
			}
//			::SetFocus(sramWindowHandle); // Stop thumb from flashing
			if (sramPage > sramMax)
				ShowWindow(sramScroller, SW_HIDE);
			else
				ShowWindow(sramScroller, SW_SHOW);
			return FALSE;
			break;
		}

		case WM_VSCROLL:
		{
			switch (LOWORD(wParam))
			{
				case SB_PAGEDOWN:
					sramPos += sramPage-1;

				case SB_LINEDOWN:
					sramPos = min(sramMax-(sramPage-1), sramPos+1);
					break;

				case SB_PAGEUP:
					sramPos -= sramPage-1;

				case SB_LINEUP:
					sramPos = max(0, sramPos-1);
					break;

				case SB_TOP:
					sramPos = 0;
					break;

				case SB_BOTTOM:
					sramPos = sramMax-(sramPage-1);
					break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:
					sramPos = HIWORD(wParam);
					break;

				default:
					break;
			}
			firstSramAddress = sramStart+(sramPos*sramBytesPerRow);
			project->sramStartAddress = firstSramAddress;
			project->sramEndAddress = min(firstSramAddress+
				(sramPage*sramBytesPerRow), sramStart+sramSize);
			if (endCondition)
				updateSram = true;
			DrawSramValues();
			::SetScrollPos(sramScroller, SB_CTL, sramPos, TRUE);
//			::SetFocus(sramWindowHandle);
			return FALSE;
		}

		case WM_CLOSE:
			project->sramWindowInfo.open = FALSE;
			break;

		case WM_DESTROY:
		{
			::DeleteObject(sramNamesFont);
			::DeleteObject(sramValuesFont);
			sramWindowHandle = NULL;
			break;
		}

		default:
			break;
	}

	// Process any messages that we didn't take care of
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT CALLBACK SramScrollProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam)
{
	int id = ::GetWindowLong(hWnd, GWL_ID);

	return ::CallWindowProcA(sramOldScroll, hWnd, msg, wParam, lParam);
}


void DrawSramValues()
{
	char buffer[10];
	SIZE charSize;
	int pos, value;
	int i, j;
	int x, y;
	HDC hDC = GetDC(sramWindowHandle);

	::SelectObject(hDC, sramValuesFont);
	::GetTextExtentPoint32(hDC, "E", 1, &charSize);

	// Write the data
	::SetBkColor(hDC, VALUEBACKCOLOUR);
	::SetBkMode(hDC, OPAQUE);
	pos = firstSramAddress;
	for (i=0; i<(sramClientH-(topY+BEVEL+BORDER))/charSize.cy; i++)					// Each visible row
	{
		y = topY+(i*charSize.cy);
		x = BORDER+BEVEL;
		MemDrawIndentedRect(hDC, x-BEVEL, y, (2*BEVEL)+(4*charSize.cx),
			charSize.cy);
		MemDrawIndentedRect(hDC, sramHexX-BEVEL, y, (2*BEVEL)+
			(sramBytesPerRow*2*charSize.cx)+
			((sramBytesPerRow-1)*BORDER), charSize.cy);
		MemDrawIndentedRect(hDC, sramAsciiX-BEVEL, y, (2*BEVEL)+
			(sramBytesPerRow*charSize.cx), charSize.cy);

		if (pos < sramStart+sramSize)
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
			for (j=0; j<sramBytesPerRow; j++) // For each value
			{
				if (pos+j < sramStart+sramSize)
				{
					value = sram[(pos-sramStart)+j];

					// Hex value
					x = sramHexX+(j*((2*charSize.cx)+BORDER));
					if (sramMask[(pos-sramStart)+j])
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
					x = sramAsciiX+(j*charSize.cx);
					if (sramMask[(pos-sramStart)+j])
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
			}
		}
		pos += sramBytesPerRow;
	}
	if (i*charSize.cy != sramClientH-(topY+BEVEL+BORDER))
	{
		MemDrawIndentedRect(hDC, BORDER, topY+(i*charSize.cy),
			(2*BEVEL)+(4*charSize.cx),
			sramClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
		MemDrawIndentedRect(hDC, sramHexX-BEVEL, topY+(i*charSize.cy),
			(2*BEVEL)+(sramBytesPerRow*2*charSize.cx)+
			((sramBytesPerRow-1)*BORDER),
			sramClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
		MemDrawIndentedRect(hDC, sramAsciiX-BEVEL,
			topY+(i*charSize.cy), (2*BEVEL)+(sramBytesPerRow*charSize.cx),
			sramClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
	}

	ReleaseDC(sramWindowHandle, hDC);
}


BOOL OpenEepromWindow()
{
	WNDCLASSEX winClass;
	int min, max;

	eepromWindowHandle = NULL;

	// Fill the window class structure
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_DBLCLKS | CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	winClass.lpfnWndProc = EepromProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInstance;
	winClass.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EEPROM));
	winClass.hIconSm = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EEPROM));
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;		// I'm painting it
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = EEPROMWINDOW_CLASS_NAME;

	// Register the class
	if (!eepromWindowRegistered && !RegisterClassEx(&winClass))
		return FALSE;
	eepromWindowRegistered = true;

	// Find the window's preferred position & size
	eepromX = project->eepromWindowInfo.x;
	eepromY = project->eepromWindowInfo.y;
	eepromW = project->eepromWindowInfo.width;
	eepromH = project->eepromWindowInfo.height;
	min = project->eepromWindowInfo.minimized;
	max = project->eepromWindowInfo.maximized;

	// Create the window
	if (!(eepromWindowHandle = CreateWindowEx(
		NULL,							// Extended Style
		EEPROMWINDOW_CLASS_NAME,		// Class
		"EEPROM",						// Title
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU |WS_MINIMIZEBOX | WS_THICKFRAME, // Style
		eepromX, eepromY,				// X & Y
		eepromW, eepromH,				// Width & Height
		clientAreaHandle,				// Parent
		NULL,							// Menu
		hInstance,						// This Application
		NULL)))							// Extra parameters
	{
		return FALSE;
	}
	::SetParent(eepromWindowHandle, clientAreaHandle);
	if (min)
		::ShowWindow(eepromWindowHandle, SW_MINIMIZE);
	if (max)
		::ShowWindow(eepromWindowHandle, SW_MAXIMIZE);
	project->eepromWindowInfo.open = TRUE;

	return TRUE;
}


LRESULT CALLBACK EepromProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// This is the main message handler for the EEPROM window
	PAINTSTRUCT ps;						// Used in WM_PAINT
	HDC hDC;							// Handle to a device context
	RECT rect;
	WINDOWPLACEMENT wndPlacement;
	int i, j;
	int x, y;
	char buffer[5];
	SIZE charSize;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	// Switch on message
	switch (msg)
	{
		case WM_CREATE:
		{
			eepromScroller = ::CreateWindow(TEXT("scrollbar"), NULL,
				WS_CHILD|WS_VISIBLE|WS_TABSTOP|SBS_VERT, 0, 0, 0, 0,
				hWnd, (HMENU)IDC_EEPROM_SCROLLER, hInstance, NULL);
			eepromScrollInfo.cbSize = sizeof SCROLLINFO;
			eepromScrollInfo.fMask = SIF_POS;
			eepromPos = 0;
			::SetScrollPos(eepromScroller, SB_CTL, eepromStart, FALSE);
			eepromOldScroll = (WNDPROC) ::SetWindowLong(eepromScroller,
				GWL_WNDPROC, (LONG)EepromScrollProc);

			eepromNamesFont = ::CreateFont(14, 0, 0, 0, FW_BOLD, false, false, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");
			eepromValuesFont = ::CreateFont(16, 0, 0, 0, FW_MEDIUM, false, false, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, FIXED_PITCH|FF_MODERN, "Courier New");

			return FALSE;
			break;
		}

		case WM_PAINT:
		{
			hDC = BeginPaint(hWnd, &ps);

			if (project->eepromWindowInfo.minimized)
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
			vert[1].x = eepromW+1;
			vert[1].y = eepromH+1;
			vert[1].Red = (GRADIENTBOTTOM&0xff)<<8; // 0xff00;
			vert[1].Green = GRADIENTBOTTOM&0xff00; //0xff00;
			vert[1].Blue = (GRADIENTBOTTOM&0xff0000)>>8; //0xff00;
			vert[1].Alpha = 0x0000;
			gRect.UpperLeft = 0;
			gRect.LowerRight = 1;
			::GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

			// Write the labels
			::SelectObject(hDC, eepromNamesFont);
			::SetTextColor(hDC, NAMECOLOUR);
			::SetBkColor(hDC, BACKCOLOUR);
			::SetBkMode(hDC, TRANSPARENT);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			rect.top = topY-(BEVEL+BORDER+BEVEL+charSize.cy);
			rect.bottom = topY+charSize.cy;
			rect.left = BORDER+BEVEL;
			rect.right = rect.left+(5*charSize.cx);
			::DrawText(hDC, "ADDR:", 5, &rect, DT_LEFT|DT_NOCLIP|DT_VCENTER);
			rect.left = eepromAsciiX;
			rect.right = rect.left+(5*charSize.cx);
			::DrawText(hDC, "ASCII", 5, &rect, DT_LEFT|DT_NOCLIP|DT_VCENTER);

			::SelectObject(hDC, eepromValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			y = topY-(BEVEL+BORDER+BEVEL+charSize.cy);
			MemDrawTopBevel(hDC, eepromHexX-BEVEL, y-2,
				(2*BEVEL)+(eepromBytesPerRow*2*charSize.cx)+
				((eepromBytesPerRow-1)*BORDER));
			rect.top = y;
			rect.bottom = y+charSize.cy;
			rect.left = eepromHexX;
			rect.right = rect.left+((eepromBytesPerRow*2*charSize.cx)+
				((eepromBytesPerRow-1)*BORDER));
			::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
			MemDrawIndentedRect(hDC, eepromHexX-BEVEL, y, (2*BEVEL)+
				(eepromBytesPerRow*2*charSize.cx)+
				((eepromBytesPerRow-1)*BORDER), charSize.cy);
			::SetBkColor(hDC, VALUEBACKCOLOUR);
			::SetBkMode(hDC, OPAQUE);
			for (i=0; i<eepromBytesPerRow; i++)
			{
				x = eepromHexX+(i*((2*charSize.cx)+BORDER));
				buffer[0] = hexChars[i>>4];
				buffer[1] = hexChars[i&0xf];
				::TextOut(hDC, x, y, buffer, 2);
			}
			MemDrawBottomBevel(hDC, eepromHexX-BEVEL, y+charSize.cy,
				(2*BEVEL)+(eepromBytesPerRow*2*charSize.cx)+
				((eepromBytesPerRow-1)*BORDER));

			// Write the data
			::SelectObject(hDC, eepromValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			MemDrawTopBevel(hDC, BORDER, topY-2, (2*BEVEL)+(4*charSize.cx));
			MemDrawTopBevel(hDC, eepromHexX-BEVEL, topY-2,
				(2*BEVEL)+(eepromBytesPerRow*2*charSize.cx)+
				((eepromBytesPerRow-1)*BORDER));
			MemDrawTopBevel(hDC, eepromAsciiX-BEVEL, topY-2,
				(2*BEVEL)+(eepromBytesPerRow*charSize.cx));
			int pos = firstEepromAddress;
			for (i=0; i<(eepromClientH-(topY+BEVEL+BORDER))/charSize.cy; i++)
			{							// Each visible row
				y = topY+(i*charSize.cy);
				x = BORDER+BEVEL;

				rect.top = y;
				rect.bottom = y+charSize.cy;
				rect.left = x;
				rect.right = rect.left+(4*charSize.cx);
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, x-BEVEL, y, (2*BEVEL)+(4*charSize.cx),
					charSize.cy);
				rect.left = sramHexX;
				rect.right = rect.left+((eepromBytesPerRow*2*charSize.cx)+
					((sramBytesPerRow-1)*BORDER));
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, eepromHexX-BEVEL, y, (2*BEVEL)+
					(eepromBytesPerRow*2*charSize.cx)+
					((eepromBytesPerRow-1)*BORDER), charSize.cy);
				rect.left = eepromAsciiX;
				rect.right = rect.left+(eepromBytesPerRow*charSize.cx);
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, eepromAsciiX-BEVEL, y, (2*BEVEL)+
					(eepromBytesPerRow*charSize.cx), charSize.cy);

				if (pos < eepromStart+eepromSize)
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
					for (j=0; j<eepromBytesPerRow; j++) // For each value
					{
						if (pos+j < eepromStart+eepromSize)
						{
							int value = eeprom[(pos-eepromStart)+j];

							// Hex value
							x = eepromHexX+(j*((2*charSize.cx)+5));
							if (eepromMask[(pos-eepromStart)+j])
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
							x = eepromAsciiX+(j*charSize.cx);
							if (eepromMask[(pos-eepromStart)+j])
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
					}
				}
				pos += eepromBytesPerRow;
			}
			if (i*charSize.cy != eepromClientH-(topY+BEVEL+BORDER))
			{
				rect.top = topY+(i*charSize.cy);
				rect.bottom = eepromClientH-(BEVEL+BORDER);
				rect.left = BORDER+BEVEL;
				rect.right = rect.left+(4*charSize.cx);
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, BORDER, topY+(i*charSize.cy),
					(2*BEVEL)+(4*charSize.cx),
					eepromClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
				rect.left = eepromHexX;
				rect.right = rect.left+((eepromBytesPerRow*2*charSize.cx)+
					((eepromBytesPerRow-1)*BORDER));
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, eepromHexX-BEVEL,
					topY+(i*charSize.cy),
					(2*BEVEL)+(eepromBytesPerRow*2*charSize.cx)+
					((eepromBytesPerRow-1)*BORDER),
					eepromClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
				rect.left = eepromAsciiX;
				rect.right = rect.left+(eepromBytesPerRow*charSize.cx);
				::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
				MemDrawIndentedRect(hDC, eepromAsciiX-BEVEL,
					topY+(i*charSize.cy), (2*BEVEL)+
					(eepromBytesPerRow*charSize.cx),
					eepromClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
			}
			MemDrawBottomBevel(hDC, BORDER, eepromClientH-(BEVEL+BORDER),
				(2*BEVEL)+(4*charSize.cx));
			MemDrawBottomBevel(hDC, eepromHexX-BEVEL,
				eepromClientH-(BEVEL+BORDER),
				(2*BEVEL)+(eepromBytesPerRow*2*charSize.cx)+
				((eepromBytesPerRow-1)*BORDER));
			MemDrawBottomBevel(hDC, eepromAsciiX-BEVEL,
				eepromClientH-(BEVEL+BORDER),
				(2*BEVEL)+(eepromBytesPerRow*charSize.cx));

			EndPaint(hWnd, &ps);
			return FALSE;
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
			if (!project->eepromWindowInfo.minimized &&
				!project->eepromWindowInfo.maximized)
			{
				::GetWindowPlacement(hWnd, &wndPlacement);
				eepromX = wndPlacement.rcNormalPosition.left;
				eepromY = wndPlacement.rcNormalPosition.top;
				project->eepromWindowInfo.x = eepromX;
				project->eepromWindowInfo.y = eepromY;
			}
			return FALSE;
			break;
		}

		case WM_SIZING:
		{
			RECT * size = (RECT *)lParam;
			if (size->right-size->left < eepromMinW)
				size->right = size->left+eepromMinW;
			if (size->right-size->left > eepromMaxW)
				size->right = size->left+eepromMaxW;
			if (size->bottom-size->top < eepromMinH)
				size->bottom = size->top+eepromMinH;
			if (endCondition)
				updateEeprom = true;
			return FALSE;
			break;
		}

		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
			{
				project->eepromWindowInfo.maximized = FALSE;
				project->eepromWindowInfo.minimized = TRUE;
				return FALSE;
			}
			if (wParam == SIZE_MAXIMIZED)
			{
				project->eepromWindowInfo.minimized = FALSE;
				project->eepromWindowInfo.maximized = TRUE;
				return FALSE;
			}
			if (wParam != SIZE_RESTORED)
				return FALSE;
			project->eepromWindowInfo.minimized = FALSE;
			project->eepromWindowInfo.maximized = FALSE;
			::GetWindowPlacement(hWnd, &wndPlacement);
			project->eepromWindowInfo.width =
				wndPlacement.rcNormalPosition.right-
				wndPlacement.rcNormalPosition.left;
			project->eepromWindowInfo.height =
				wndPlacement.rcNormalPosition.bottom-
				wndPlacement.rcNormalPosition.top;
			
			if (firstEepromAddress < eepromStart) // If eepromStart has changed
				firstEepromAddress = eepromStart; // Update it
			::GetWindowRect(hWnd, &rect);
			eepromH = rect.bottom-rect.top;
			eepromW = rect.right-rect.left;
			hDC = GetDC(hWnd);
			::SelectObject(hDC, eepromValuesFont);
			::GetTextExtentPoint32(hDC, "E", 1, &charSize);
			ReleaseDC(hWnd, hDC);
			eepromClientW = LOWORD(lParam);
			eepromClientH = HIWORD(lParam);
			topY = charSize.cy+(3*BEVEL)+(2*BORDER);
			eepromMinW = (eepromW-eepromClientW)+(5+3)*BORDER+6*BEVEL+
				(4+8+4+2)*charSize.cx;
			eepromMaxW = (eepromW-eepromClientW)+(5+15)*BORDER+6*BEVEL+
				(4+32+16+2)*charSize.cx;
			eepromMinH = (eepromH-eepromClientH)+topY+BORDER+BEVEL+
				4*charSize.cy;
			eepromBytesPerRow = eepromClientW-(6*charSize.cx+4*BORDER+6*BEVEL);
			eepromBytesPerRow /= (3*charSize.cx)+BORDER;
			eepromHexX = BORDER+BEVEL+(4*charSize.cx)+BEVEL+BORDER+BEVEL;
			eepromAsciiX = eepromHexX+(eepromBytesPerRow*2*charSize.cx)+
				((eepromBytesPerRow-1)*BORDER)+BEVEL+BORDER+BEVEL;
			eepromScrollX = eepromAsciiX+(eepromBytesPerRow*charSize.cx)+
				BEVEL+BORDER;
			::MoveWindow(eepromScroller, eepromScrollX, topY, 2*charSize.cx,
				(HIWORD(lParam))-(topY+BEVEL+BORDER), TRUE);

			eepromPage = ((eepromClientH-(topY+BEVEL+BORDER))/charSize.cy);
			eepromMax = eepromSize/eepromBytesPerRow;
			if (eepromMax*eepromBytesPerRow != eepromSize)
				eepromMax++;
			eepromMax--;
			project->eepromEndAddress = min(firstEepromAddress+
				(eepromPage*eepromBytesPerRow), eepromStart+eepromSize);
			eepromScrollInfo.fMask = SIF_RANGE|SIF_PAGE;
			eepromScrollInfo.nMin = 0;
			eepromScrollInfo.nMax = eepromMax;
			eepromScrollInfo.nPage = eepromPage;
			::SetScrollInfo(eepromScroller, SB_CTL, &eepromScrollInfo, TRUE);
			eepromScrollInfo.fMask = SIF_POS;
			::GetScrollInfo(eepromScroller, SB_CTL, &eepromScrollInfo);
			if (eepromPos != eepromScrollInfo.nPos)
			{
				eepromPos = eepromScrollInfo.nPos;
				firstEepromAddress = eepromStart+(eepromPos*eepromBytesPerRow);
				project->eepromStartAddress = firstEepromAddress;
				project->eepromEndAddress = min(firstEepromAddress+
					(eepromPage*eepromBytesPerRow), eepromStart+eepromSize);
			}
//			::SetFocus(eepromWindowHandle); // Stop thumb from flashing
			if (eepromPage > eepromMax)
				ShowWindow(eepromScroller, SW_HIDE);
			else
				ShowWindow(eepromScroller, SW_SHOW);

			return FALSE;
			break;
		}

		case WM_VSCROLL:
		{
			switch (LOWORD(wParam))
			{
				case SB_PAGEDOWN:
					eepromPos += eepromPage-1;

				case SB_LINEDOWN:
					eepromPos = min(eepromMax-(eepromPage-1), eepromPos+1);
					break;

				case SB_PAGEUP:
					eepromPos -= eepromPage-1;

				case SB_LINEUP:
					eepromPos = max(0, eepromPos-1);
					break;

				case SB_TOP:
					eepromPos = 0;
					break;

				case SB_BOTTOM:
					eepromPos = eepromMax-(eepromPage-1);
					break;

				case SB_THUMBPOSITION:
				case SB_THUMBTRACK:
					eepromPos = HIWORD(wParam);
					break;

				default:
					break;
			}
			firstEepromAddress = eepromStart+(eepromPos*eepromBytesPerRow);
			project->eepromStartAddress = firstEepromAddress;
			project->eepromEndAddress = min(firstEepromAddress+
				(eepromPage*eepromBytesPerRow), eepromStart+eepromSize);
			if (endCondition)
				updateEeprom = true;
			DrawEepromValues();
			::SetScrollPos(eepromScroller, SB_CTL, eepromPos, TRUE);
//			::SetFocus(eepromWindowHandle);
			return FALSE;
		}

		case WM_CLOSE:
			project->eepromWindowInfo.open = FALSE;
			break;

		case WM_DESTROY:
		{
			::DeleteObject(eepromNamesFont);
			::DeleteObject(eepromValuesFont);
			eepromWindowHandle = NULL;
			break;
		}

		default:
			break;
	}

	// Process any messages that we didn't take care of
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT CALLBACK EepromScrollProc(HWND hWnd, UINT msg, WPARAM wParam,
	LPARAM lParam)
{
	int id = ::GetWindowLong(hWnd, GWL_ID);

	return ::CallWindowProcA(eepromOldScroll, hWnd, msg, wParam, lParam);
}


void DrawEepromValues()
{
	char buffer[10];
	SIZE charSize;
	int pos, value;
	int i, j;
	int x, y;
	HDC hDC = GetDC(eepromWindowHandle);
	::SelectObject(hDC, eepromValuesFont);
	::GetTextExtentPoint32(hDC, "E", 1, &charSize);
	::SetBkColor(hDC, VALUEBACKCOLOUR);
	::SetBkMode(hDC, OPAQUE);
	pos = firstEepromAddress;
	for (i=0; i<(eepromClientH-(topY+BEVEL+BORDER))/charSize.cy; i++)					// Each visible row
	{
		y = topY+(i*charSize.cy);
		x = BORDER+BEVEL;
		MemDrawIndentedRect(hDC, x-BEVEL, y, (2*BEVEL)+(4*charSize.cx),
			charSize.cy);
		MemDrawIndentedRect(hDC, eepromHexX-BEVEL, y, (2*BEVEL)+
			(eepromBytesPerRow*2*charSize.cx)+
			((eepromBytesPerRow-1)*BORDER), charSize.cy);
		MemDrawIndentedRect(hDC, eepromAsciiX-BEVEL, y, (2*BEVEL)+
			(eepromBytesPerRow*charSize.cx), charSize.cy);

		if (pos < eepromStart+eepromSize)
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
			for (j=0; j<eepromBytesPerRow; j++) // For each value
			{
				if (pos+j < eepromStart+eepromSize)
				{
					value = eeprom[(pos-eepromStart)+j];

					// Hex value
					x = eepromHexX+(j*((2*charSize.cx)+BORDER));
					if (eepromMask[(pos-eepromStart)+j])
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
					x = eepromAsciiX+(j*charSize.cx);
					if (eepromMask[(pos-eepromStart)+j])
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
			}
		}
		pos += eepromBytesPerRow;
	}
	if (i*charSize.cy != eepromClientH-(topY+BEVEL+BORDER))
	{
		MemDrawIndentedRect(hDC, BORDER, topY+(i*charSize.cy),
			(2*BEVEL)+(4*charSize.cx),
			eepromClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
		MemDrawIndentedRect(hDC, sramHexX-BEVEL, topY+(i*charSize.cy),
			(2*BEVEL)+(eepromBytesPerRow*2*charSize.cx)+
			((eepromBytesPerRow-1)*BORDER),
			eepromClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
		MemDrawIndentedRect(hDC, eepromAsciiX-BEVEL,
			topY+(i*charSize.cy), (2*BEVEL)+(eepromBytesPerRow*charSize.cx),
			eepromClientH-(topY+BEVEL+BORDER+(i*charSize.cy)));
	}

	ReleaseDC(eepromWindowHandle, hDC);
}


void MemDrawTopBevel(HDC hDC, int x, int y, int w)
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


void MemDrawIndentedRect(HDC hDC, int x, int y, int w, int h)
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
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	rect.left = x+(w-3);
	rect.right = rect.left+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	rect.left = x+(w-2);
	rect.right = rect.left+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	rect.left = rect.left+1;
	rect.right = rect.left+1;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
}


void MemDrawBottomBevel(HDC hDC, int x, int y, int w)
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
