// ChipWindow.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "ChipWindow.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NAMECOLOUR 0x00000000
#define VALUEBACKCOLOUR 0x00ffffff

#define LOWCOLOUR 0x00800000
#define HIGHCOLOUR 0x00000080
#define UNKNOWNCOLOUR 0x00404040
#define ANALOGUECOLOUR 0x00008000
#define XTALCOLOUR 0x00008080

#define AREAW 250
#define AREAH 250
#define BORDER 5
#define BEVEL 2

/////////////////////////////////////////////////////////////////////////////
// CChipWindow

IMPLEMENT_DYNCREATE(CChipWindow, CMDIChildWnd)


CChipWindow::CChipWindow()
{
	// Find the window's preferred dimensions
	windowX = project->chipWindowInfo.x; // Default values
	windowY = project->chipWindowInfo.y;
	windowW = project->chipWindowInfo.width;
	windowH = project->chipWindowInfo.height;
	windowMin = project->chipWindowInfo.minimized;
	windowMax = project->chipWindowInfo.maximized;

	init = TRUE;
	pinFlags = NULL;
}


CChipWindow::~CChipWindow()
{
}


void CChipWindow::Refresh()
{
	char title[100];

	strcpy(title, "Chip - ");
	project->GetChipName(project->GetChip(), &title[strlen(title)],
		100-strlen(title));
	strcat(title, " (");
	project->GetPackageName(project->GetPackage(), &title[strlen(title)],
		100-strlen(title));
	strcat(title, ")");
	::SetWindowText(m_hWnd, title);

	if (pinFlags)
		delete pinFlags;
	pinFlags = new int[project->GetPins()];
	CLSetPinsToDefault(pinFlags, project->GetChip(), project->GetPackage());

	::InvalidateRect(m_hWnd, NULL, FALSE);
	Update();
}


void CChipWindow::Update()
{
	CLSetChipPortPins(pinFlags, sfrs, project->GetChip(), project->GetPackage());
	HDC hDC = ::GetDC(m_hWnd);
	DrawChipValues(hDC);
}


void CChipWindow::DrawChipValues(HDC hDC)
{
	HFONT hOldFont;
	int i;
	int x, y;
	SIZE charSize;
	int gap;
	int sideTopGap;
	int sideSideGap;
	int topTopGap;
	int topSideGap;
	int pinIndex;

	hOldFont = (HFONT)::SelectObject(hDC, chipValuesFont);
	::SetBkColor(hDC, VALUEBACKCOLOUR);
	::SetBkMode(hDC, OPAQUE);
	::GetTextExtentPoint32(hDC, "8", 1, &charSize);
	switch (project->GetPackage())
	{
		case 2: //PACKAGE_DIP:
		case 4: //PACKAGE_SOIC:
		case 5: //PACKAGE_SSOP:
		{
			gap = BORDER-1;
			sideTopGap = (AREAH-((project->GetPins()/2)*charSize.cy-
				(project->GetPins()/2)))/2;
			sideSideGap = gap;
			for (i=0; i<project->GetPins()/2; i++)
			{
				x = 2*BORDER+sideSideGap+2*charSize.cx+gap;
				y = 2*BORDER+sideTopGap+i*(charSize.cy-1);
				DrawPinFlag(hDC, i+1, x, y);
				
				x = (2*BORDER+AREAW)-(sideSideGap+3*charSize.cx+gap);
				y = (2*BORDER+AREAH)-(sideTopGap+i*(charSize.cy-1)+charSize.cy);
				DrawPinFlag(hDC, i+1+project->GetPins()/2, x, y);
			}
			break;
		}

		case 1: //PACKAGE_PLCC:
		{
			gap = BORDER-1;
			sideTopGap = (AREAH-((project->GetPins()/4)*charSize.cy-
				(project->GetPins()/4)))/2;
			sideSideGap = gap;
			topTopGap = gap;
			topSideGap = (AREAW-((project->GetPins()/4-1)*gap+
				(project->GetPins()/4)*2*charSize.cx))/2;

			for (i=0; i<project->GetPins()/4; i++)
			{
				x = (2*BORDER+AREAW+charSize.cx/2)-(topSideGap+
					i*(gap+2*charSize.cx)+2*charSize.cx);
				y = 2*BORDER+gap+charSize.cy;
				pinIndex = i+1+(project->GetPins()/4)*3+
					project->GetPins()/8+1;
				if (pinIndex > project->GetPins())
					pinIndex -= project->GetPins();
				DrawPinFlag(hDC, pinIndex, x, y);

				x = 2*BORDER+sideSideGap+2*charSize.cx+gap;
				y = 2*BORDER+sideTopGap+i*(charSize.cy-1);
				pinIndex = i+1+project->GetPins()/8+1;
				DrawPinFlag(hDC, pinIndex, x, y);

				x = 2*BORDER+topSideGap+i*(gap+2*charSize.cx)+(charSize.cx/2);
				y = (2*BORDER+AREAH)-(gap+charSize.cy+charSize.cy);
				pinIndex = i+1+project->GetPins()/4+
					project->GetPins()/8+1;
				DrawPinFlag(hDC, pinIndex, x, y);

				x = (2*BORDER+AREAW)-(sideSideGap+3*charSize.cx+gap);
				y = (2*BORDER+AREAH)-(sideTopGap+i*(charSize.cy-1)+
					charSize.cy);
				pinIndex = i+1+project->GetPins()/2+
					project->GetPins()/8+1;
				DrawPinFlag(hDC, pinIndex, x, y);
			}
			break;
		}
		case 0: //PACKAGE_TQFP:
		case 3: //PACKAGE_MLF:
		{
			gap = BORDER-1;
			sideTopGap = (AREAH-((project->GetPins()/4)*charSize.cy-
				(project->GetPins()/4)))/2;
			sideSideGap = gap;
			topTopGap = gap;
			topSideGap = (AREAW-((project->GetPins()/4-1)*gap+
						(project->GetPins()/4)*2*charSize.cx))/2;

			for (i=0; i<project->GetPins()/4; i++)
			{
				x = 2*BORDER+sideSideGap+2*charSize.cx+gap;
				y = 2*BORDER+sideTopGap+i*(charSize.cy-1);
				DrawPinFlag(hDC, i+1, x, y);

				x = 2*BORDER+topSideGap+i*(gap+2*charSize.cx)+charSize.cx/2;
				y = (2*BORDER+AREAH)-(gap+2*charSize.cy);
				DrawPinFlag(hDC, i+1+project->GetPins()/4, x, y);
				
				x = (2*BORDER+AREAW)-(sideSideGap+3*charSize.cx+gap);
				y = (2*BORDER+AREAH)-(sideTopGap+i*(charSize.cy-1)+
					charSize.cy);
				DrawPinFlag(hDC, i+1+project->GetPins()/2, x, y);

				x = (2*BORDER+AREAW+charSize.cx/2)-(topSideGap+
					i*(gap+2*charSize.cx)+2*charSize.cx);
				y = 2*BORDER+gap+charSize.cy;
				DrawPinFlag(hDC, i+1+(project->GetPins()/4)*3, x, y);
			}
			break;
		}
	}
	::SelectObject(hDC, hOldFont);
}


void CChipWindow::DrawPinFlag(HDC hDC, int pin, int x, int y)
{
	RECT rect;
	SIZE charSize;

	if (!pinFlags)
		return;

	char buffer[2];
	buffer[1] = 0;

	switch (pinFlags[pin-1])
	{
		case PS_LOW:
			::SetTextColor(hDC, LOWCOLOUR);
			buffer[0] = 'L';
			break;

		case PS_HIGH:
			::SetTextColor(hDC, HIGHCOLOUR);
			buffer[0] = 'H';
			break;

		case PS_UNKNOWN:
			::SetTextColor(hDC, UNKNOWNCOLOUR);
			buffer[0] = '?';
			break;

		case PS_ANALOGUE:
			::SetTextColor(hDC, ANALOGUECOLOUR);
			buffer[0] = 'A';
			break;

		case PS_XTAL:
			::SetTextColor(hDC, XTALCOLOUR);
			buffer[0] = 'X';
			break;

		default:
			::SetTextColor(hDC, UNKNOWNCOLOUR);
			buffer[0] = '-';
			break;
	}

	::GetTextExtentPoint32(hDC, "H", 1, &charSize);
	rect.left = x;
	rect.right = x+charSize.cx;
	rect.top = y;
	rect.bottom = y+charSize.cy;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	::TextOut(hDC, x, y, buffer, 1);
}


void CChipWindow::ChipDrawTopBevel(HDC hDC, int x, int y, int w, BOOL in)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x+w;
	rect.bottom = y+2;
	if (in)							// Bevel goes in
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	else							// Bevel goes out
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	rect.right = rect.right-1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	rect.left = rect.left+1;
	rect.top = rect.top+1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
	rect.right = rect.right-1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
}


void CChipWindow::ChipDrawSideBevels(HDC hDC, int x, int y, int w, int h, BOOL in)
{
	RECT rect;

	rect.top = y;
	rect.bottom = y+h;
	rect.left = x;
	rect.right = rect.left+1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	rect.left = rect.left+1;
	rect.right = rect.left+1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	rect.left = x+(w-2);
	rect.right = rect.left+1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
	rect.left = rect.left+1;
	rect.right = rect.left+1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
}


void CChipWindow::ChipDrawBottomBevel(HDC hDC, int x, int y, int w, BOOL in)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x+w;
	rect.bottom = y+2;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	rect.right = rect.right-1;
	rect.bottom = rect.bottom-1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	rect.left = rect.left+1;
	if (in)
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));
	else
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(GRAY_BRUSH));
}


BEGIN_MESSAGE_MAP(CChipWindow, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChipWindow)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChipWindow message handlers


BOOL CChipWindow::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= ~WS_THICKFRAME;
	return TRUE;
}


int CChipWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	chipNamesFont = ::CreateFont(12, 0, 0, 0, FW_NORMAL, false, false, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");

	chipValuesFont = ::CreateFont(12, 0, 0, 0, FW_NORMAL, false, false, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, FIXED_PITCH|FF_MODERN, "Arial");

	windowW = AREAW+2*BEVEL+4*BORDER+2*GetSystemMetrics(SM_CXFIXEDFRAME);
	project->chipWindowInfo.width = windowW;
	windowH = AREAH+2*BEVEL+4*BORDER+2*GetSystemMetrics(SM_CYFIXEDFRAME)+
		GetSystemMetrics(SM_CYCAPTION);
	project->chipWindowInfo.height = windowH;
	
	SendMessage(WM_SIZE);				// Set the size
	project->chipWindowInfo.open = TRUE;
	Refresh();
	return 0;
}


void CChipWindow::OnUpdateFrameTitle(BOOL addToTitle)
{
	char title[100];

	strcpy(title, "Chip - ");
	project->GetChipName(project->GetChip(), &title[strlen(title)],
		100-strlen(title));
	strcat(title, " (");
	project->GetPackageName(project->GetPackage(), &title[strlen(title)],
		100-strlen(title));
	strcat(title, ")");
	::SetWindowText(m_hWnd, title);
}


BOOL CChipWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; //CMDIChildWnd::OnEraseBkgnd(pDC);
}


void CChipWindow::OnPaint() 
{
	CRect rect;
	HDC hChipDC;
	HFONT hOldFont;
	HBITMAP hOldBitmap;
	HBITMAP hChipBitmap;
	SIZE charSize;
	int picW, picH;
	int sideTopGap;
	int sideSideGap;
	int topTopGap;
	int topSideGap;
	int gap;
	int x, y;
	int pinIndex;
	int i;
	char buffer[5];

	CPaintDC dc(this); // device context for painting

	HDC hDC = dc.m_hDC;

	if (project->chipWindowInfo.minimized)
		return;

	// First paint the background
	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	vert[0].x = 0;
	vert[0].y = 0;
	vert[0].Red = (GRADIENTTOP&0xff)<<8; // 0x5000;
	vert[0].Green = GRADIENTTOP&0xff00; //0x5000;
	vert[0].Blue = (GRADIENTTOP&0xff0000)>>8; //0xff00;
	vert[0].Alpha = 0x0000;
	vert[1].x = windowW+1;
	vert[1].y = windowH+1;
	vert[1].Red = (GRADIENTBOTTOM&0xff)<<8; // 0xff00;
	vert[1].Green = GRADIENTBOTTOM&0xff00; //0xff00;
	vert[1].Blue = (GRADIENTBOTTOM&0xff0000)>>8; //0xff00;
	vert[1].Alpha = 0x0000;
	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;
	::GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

	// Draw the values area
	ChipDrawTopBevel(hDC, 2*BORDER-BEVEL, 2*BORDER-BEVEL,
		chipClientW-(4*BORDER-2*BEVEL), TRUE);
	ChipDrawSideBevels(hDC, 2*BORDER-BEVEL, 2*BORDER,
		chipClientW-(4*BORDER-2*BEVEL), chipClientH-(4*BORDER), TRUE);
	ChipDrawBottomBevel(hDC, 2*BORDER-BEVEL, chipClientH-(2*BORDER),
		chipClientW-(4*BORDER-2*BEVEL), TRUE);
	rect.left = 2*BORDER;
	rect.right = chipClientW-2*BORDER;
	rect.top = 2*BORDER;
	rect.bottom = chipClientH-2*BORDER;
	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));

	hOldFont = (HFONT)::SelectObject(hDC, chipNamesFont);
	::SetTextColor(hDC, NAMECOLOUR);
	::SetBkColor(hDC, VALUEBACKCOLOUR);
	::SetBkMode(hDC, OPAQUE);
	::GetTextExtentPoint32(hDC, "8", 1, &charSize);

	switch (project->GetPackage())
	{
		case 2: //PACKAGE_DIP:
		case 4: //PACKAGE_SOIC:
		case 5: //PACKAGE_SSOP:
		{
			sideTopGap = (AREAH-((project->GetPins()/2)*charSize.cy-
				(project->GetPins()/2)))/2;
			sideSideGap = 4;
			for (i=0; i<project->GetPins()/2; i++)
			{
				x = 2*BORDER+sideSideGap;
				y = 2*BORDER+sideTopGap+i*(charSize.cy-1);
				itoa(i+1, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));
		
				x = (2*BORDER+AREAW)-(sideSideGap+2*charSize.cx);
				y = (2*BORDER+AREAH)-(sideTopGap+i*(charSize.cy-1)+charSize.cy);
				itoa(i+1+project->GetPins()/2, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));
			}
			break;
		}
		case 1: //PACKAGE_PLCC:
		{
			gap = 4;
			sideTopGap = (AREAH-((project->GetPins()/4)*charSize.cy-
				(project->GetPins()/4)))/2;
			sideSideGap = 4;
			topTopGap = 4;
			topSideGap = (AREAW-((project->GetPins()/4-1)*gap+
				(project->GetPins()/4)*2*charSize.cx))/2;

			for (i=0; i<project->GetPins()/4; i++)
			{
				x = (2*BORDER+AREAW)-(topSideGap+i*(gap+2*charSize.cx)+
					2*charSize.cx);
				y = 2*BORDER+gap;
				pinIndex = i+1+(project->GetPins()/4)*3+
					project->GetPins()/8+1;
				if (pinIndex > project->GetPins())
					pinIndex -= project->GetPins();
				itoa(pinIndex, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));

				x = 2*BORDER+sideSideGap;
				y = 2*BORDER+sideTopGap+i*(charSize.cy-1);
				pinIndex = i+1+project->GetPins()/8+1;
				itoa(pinIndex, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));

				x = 2*BORDER+topSideGap+i*(gap+2*charSize.cx);
				y = (2*BORDER+AREAH)-(gap+charSize.cy);
				pinIndex = i+1+project->GetPins()/4+
					project->GetPins()/8+1;
				itoa(pinIndex, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));

				x = (2*BORDER+AREAW)-(sideSideGap+2*charSize.cx);
				y = (2*BORDER+AREAH)-(sideTopGap+i*(charSize.cy-1)+
					charSize.cy);
				pinIndex = i+1+project->GetPins()/2+
					project->GetPins()/8+1;
				itoa(pinIndex, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));
			}
			break;
		}
		case 0: //PACKAGE_TQFP:
		case 3: //PACKAGE_MLF:
		{
			gap = 4;
			sideTopGap = (AREAH-((project->GetPins()/4)*charSize.cy-
				(project->GetPins()/4)))/2;
			sideSideGap = 4;
			topTopGap = 4;
			topSideGap = (AREAW-((project->GetPins()/4-1)*gap+
				(project->GetPins()/4)*2*charSize.cx))/2;

			for (i=0; i<project->GetPins()/4; i++)
			{
				x = 2*BORDER+sideSideGap;
				y = 2*BORDER+sideTopGap+i*(charSize.cy-1);
				itoa(i+1, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));

				x = 2*BORDER+topSideGap+i*(gap+2*charSize.cx);
				y = (2*BORDER+AREAH)-(gap+charSize.cy);
				itoa(i+1+project->GetPins()/4, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));
		
				x = (2*BORDER+AREAW)-(sideSideGap+2*charSize.cx);
				y = (2*BORDER+AREAH)-(sideTopGap+i*(charSize.cy-1)+
					charSize.cy);
				itoa(i+1+project->GetPins()/2, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));

				x = (2*BORDER+AREAW)-(topSideGap+i*(gap+2*charSize.cx)+
					2*charSize.cx);
				y = 2*BORDER+gap;
				itoa(i+1+(project->GetPins()/4)*3, buffer, 10);
				::TextOut(hDC, x, y, buffer, strlen(buffer));
			}
			break;
		}
	}
	::SelectObject(hDC, hOldFont);

	// Draw the chip picture
	CLGetChipImageSize(project->GetChip(), project->GetPackage(),
		&picW, &picH);
	ChipDrawTopBevel(hDC, chipClientW/2-(picW/2+BEVEL),
		chipClientH/2-(picH/2+BEVEL), picW+2*BEVEL, FALSE);
	ChipDrawSideBevels(hDC, chipClientW/2-(picW/2+BEVEL),
		chipClientH/2-picH/2, picW+2*BEVEL,	picH, FALSE);
	ChipDrawBottomBevel(hDC, chipClientW/2-(picW/2+BEVEL),
		chipClientH/2+picH/2, picW+2*BEVEL, FALSE);

	hChipBitmap = CLGetChipImage(project->GetChip(),
		project->GetPackage());
	hChipDC = ::CreateCompatibleDC(hDC);
	hOldBitmap = (HBITMAP)::SelectObject(hChipDC, hChipBitmap);
	::BitBlt(hDC, chipClientW/2-picW/2, chipClientH/2-picH/2,
		picW, picH, hChipDC, 0, 0, SRCCOPY);
	::SelectObject(hChipDC, hOldBitmap);
	::DeleteObject(hChipBitmap);
	::DeleteDC(hChipDC);

	// Write the values
	DrawChipValues(hDC);
}


void CChipWindow::OnMove(int x, int y) 
{
	WINDOWPLACEMENT wndPlacement;
	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	CMDIChildWnd::OnMove(x, y);
	
	if (!project->chipWindowInfo.minimized &&
		!project->chipWindowInfo.maximized)
	{
		GetWindowPlacement(&wndPlacement);
		windowX = wndPlacement.rcNormalPosition.left;
		windowY = wndPlacement.rcNormalPosition.top;
		project->chipWindowInfo.x = windowX;
		project->chipWindowInfo.y = windowY;
	}
}


void CChipWindow::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;
	WINDOWPLACEMENT wndPlacement;
	wndPlacement.length = sizeof(WINDOWPLACEMENT);
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	if (init)
	{
		init = FALSE;
		MoveWindow(windowX, windowY, windowW, windowH, TRUE);
		return;
	}

	if (nType == SIZE_MINIMIZED)
	{
		project->chipWindowInfo.maximized = FALSE;
		project->chipWindowInfo.minimized = TRUE;
		return;
	}
	if (nType == SIZE_MAXIMIZED)
	{
		project->chipWindowInfo.minimized = FALSE;
		project->chipWindowInfo.maximized = TRUE;
		return;
	}
	if (nType == SIZE_RESTORED)
	{
		project->chipWindowInfo.minimized = FALSE;
		project->chipWindowInfo.maximized = FALSE;
		GetClientRect(&rect);
		chipClientW = rect.Width();
		chipClientH = rect.Height();
	}
}


void CChipWindow::OnClose() 
{
	project->chipWindowInfo.open = FALSE;
	chipWindowOpen = false;
	CMDIChildWnd::OnClose();
}


void CChipWindow::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	chipWindowOpen = false;
	::DeleteObject(chipNamesFont);
	::DeleteObject(chipValuesFont);
	delete pinFlags;
}
