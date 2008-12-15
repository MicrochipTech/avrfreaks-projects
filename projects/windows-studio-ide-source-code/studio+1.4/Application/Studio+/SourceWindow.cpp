// SourceWindow.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "SourceWindow.h"
#include "SourceFile.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Source window breakpoint bar colour
#define BREAKPOINTBARCOLOUR 0x00eebbaa	// Light purple

// Default (fixed) tab size
//#define DEFAULTTABCHARS 4

// Breakpoint bar width
#define BREAKPOINTBARW 20

// Source window "paper" colours
#define CURSORCOLOUR 0x00ffc0a0			// Light Purple
#define PCCOLOUR 0x0080ffff				// Light Yellow
#define SOFTBPCOLOUR 0x0080ff80			// Pale Green
#define HARDBPCOLOUR 0x00FFc0c0			// Pale Blue
#define SOURCECOLOUR 0x00ffffff			// White
#define CODECOLOUR 0x00d0d0d0			// Light Gray

/////////////////////////////////////////////////////////////////////////////
// CSourceWindow

IMPLEMENT_DYNCREATE(CSourceWindow, CMDIChildWnd)

CSourceWindow::CSourceWindow()
{
	sourceFile = NULL;

	sizing = false;
	init = true;

	charSize.cx = charSize.cy = 15;
	hScrollMin = 0;
	hScrollMax = 0;
	hScrollPos = 0;
	vScrollMin = 0;
	vScrollMax = 0;
	vScrollPos = 0;

	textFont = ::CreateFont(16, 0, 0, 0, FW_MEDIUM, false, false, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FIXED_PITCH|FF_MODERN, "Courier");// New");
//	textFont.CreateFont(16, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, 0,
//		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
//		FIXED_PITCH|FF_MODERN, "Courier");
	pcBrush = ::CreateSolidBrush(PCCOLOUR);
//	pcBrush.CreateSolidBrush(PCCOLOUR);
	bpBarBrush = ::CreateSolidBrush(BREAKPOINTBARCOLOUR);
//	bpBarBrush.CreateSolidBrush(BREAKPOINTBARCOLOUR);
	cursorBrush = ::CreateSolidBrush(CURSORCOLOUR);
//	cursorBrush.CreateSolidBrush(CURSORCOLOUR);
	softBPBrush = ::CreateSolidBrush(SOFTBPCOLOUR);
//	softBPBrush.CreateSolidBrush(SOFTBPCOLOUR);
	hardBPBrush = ::CreateSolidBrush(HARDBPCOLOUR);
//	hardBPBrush.CreateSolidBrush(HARDBPCOLOUR);
	sourceBrush = ::CreateSolidBrush(SOURCECOLOUR);
//	sourceBrush.CreateSolidBrush(SOURCECOLOUR);
	codeBrush = ::CreateSolidBrush(CODECOLOUR);
//	codeBrush.CreateSolidBrush(CODECOLOUR);
	pcBitmap = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PC));
//	pcBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_PC));
	cursorBitmap = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_CURSOR));
//	cursorBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_CURSOR));
	softBPBitmap = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SOFTBP));
//	softBPBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_SOFTBP));
	hardBPBitmap = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HARDBP));
//	hardBPBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_HARDBP));

//	sizeBox = NULL;
//	hScroll = NULL;
//	vScroll = NULL;
}


CSourceWindow::~CSourceWindow()
{
	if (textFont)
		::DeleteObject(textFont);
	textFont = NULL;
	if (pcBrush)
		::DeleteObject(pcBrush);
	pcBrush = NULL;
	if (bpBarBrush)
		::DeleteObject(bpBarBrush);
	bpBarBrush = NULL;
	if (cursorBrush)
		::DeleteObject(cursorBrush);
	cursorBrush = NULL;
	if (softBPBrush)
		::DeleteObject(softBPBrush);
	softBPBrush = NULL;
	if (hardBPBrush)
		::DeleteObject(hardBPBrush);
	hardBPBrush = NULL;
	if (sourceBrush)
		::DeleteObject(sourceBrush);
	sourceBrush = NULL;
	if (codeBrush)
		::DeleteObject(codeBrush);
	codeBrush = NULL;
	if (pcBitmap)
		::DeleteObject(pcBitmap);
	pcBitmap = NULL;
	if (cursorBitmap)
		::DeleteObject(cursorBitmap);
	cursorBitmap = NULL;
	if (softBPBitmap)
		::DeleteObject(softBPBitmap);
	softBPBitmap = NULL;
	if (hardBPBitmap)
		::DeleteObject(hardBPBitmap);
	hardBPBitmap = NULL;
/*
	textFont.DeleteObject();
	pcBrush.DeleteObject();
	bpBarBrush.DeleteObject();
	cursorBrush.DeleteObject();
	softBPBrush.DeleteObject();
	hardBPBrush.DeleteObject();
	sourceBrush.DeleteObject();
	codeBrush.DeleteObject();
	pcBitmap.DeleteObject();
	cursorBitmap.DeleteObject();
	softBPBitmap.DeleteObject();
	hardBPBitmap.DeleteObject();
*/
}


FMSourceFile * CSourceWindow::GetSourceFile()
{
	return sourceFile;
}


void CSourceWindow::SetSourceFile(FMSourceFile * sf)
{
	CRect rect;
	
	sourceFile = sf;

	HDC hDC = ::GetDC(m_hWnd);
	::SelectObject(hDC, textFont);
	::GetTextExtentPoint32(hDC, "E", 1, &charSize);
	::ReleaseDC(m_hWnd, hDC);
	sizing = TRUE;

	sourceFile->GetWindowRect(&rect);
	MoveWindow(&rect, TRUE);

	SetWindowText(sourceFile->GetFileName());

	GetClientRect(&rect);
	hScrollMax = sourceFile->longestLineLength-1;
	vScrollMax = sourceFile->GetNumLines()-1;
	hScrollPage = (rect.Width()-(BREAKPOINTBARW/*+vScrollExtra*/))/charSize.cx;
	vScrollPage = (rect.Height()/*-hScrollExtra*/)/charSize.cy;

	hScrollInfo.cbSize = sizeof SCROLLINFO;
	hScrollInfo.fMask = SIF_POS|SIF_RANGE|SIF_PAGE;
	hScrollInfo.nMin = hScrollMin;
	hScrollInfo.nMax = hScrollMax;
	hScrollInfo.nPos = hScrollPos;
	hScrollInfo.nPage = hScrollPage;
	SetScrollInfo(SB_HORZ, &hScrollInfo, FALSE);

	vScrollInfo.cbSize = sizeof SCROLLINFO;
	vScrollInfo.fMask = SIF_POS|SIF_RANGE|SIF_PAGE;
	vScrollInfo.nMin = vScrollMin;
	vScrollInfo.nMax = vScrollMax;
	vScrollInfo.nPos = vScrollPos;
	vScrollInfo.nPage = vScrollPage;
	SetScrollInfo(SB_VERT, &vScrollInfo, FALSE);

	char buffer[10000];
	int max = 10000;
	if (sourceFile->IsC())
	{
		ILGetCComment(buffer, &max);
		comment = buffer;
		ILGetCCommentStart(buffer, &max);
		commentStart = buffer;
		ILGetCCommentEnd(buffer, &max);
		commentEnd = buffer;
		stringChar = ILGetCStringChar();
		charChar = ILGetCCharChar();
		escapeChar = ILGetCEscapeChar();
		directiveChar = ILGetCDirectiveChar();
		ILGetCDirectives(buffer, &max);
		directives = buffer;
		ILGetCKeywordChars(buffer, &max);
		keywordChars = buffer;
		ILGetCKeywords(buffer, &max);
		keywords = buffer;
		sourceFile->FormatCSourceText(comment, commentStart, commentEnd,
			stringChar, charChar, escapeChar,
			directiveChar, directives, keywordChars, keywords);
	}
	else
	{
		ILGetAsmComment(buffer, &max);
		comment = buffer;
		stringChar = ILGetAsmStringChar();
		charChar = ILGetAsmCharChar();
		escapeChar = ILGetAsmEscapeChar();
		directiveChar = ILGetAsmDirectiveChar();
		ILGetAsmDirectives(buffer, &max);
		directives = buffer;
		ILGetAsmKeywordChars(buffer, &max);
		keywordChars = buffer;
		ILGetAsmKeywords(buffer, &max);
		keywords = buffer;
		sourceFile->FormatAsmSourceText(comment,
			stringChar, charChar, escapeChar,
			directiveChar, directives, keywordChars, keywords);
	}
}


void CSourceWindow::ShowLine(int line)
{
	if (line < 0 || line >= sourceFile->GetNumLines())
		return;							// No can do!!
	
	hScrollPos = 0;						// Start of line
	hScrollInfo.fMask = SIF_POS;
	hScrollInfo.nPos = hScrollPos;
	SetScrollInfo(SB_HORZ, &hScrollInfo, TRUE);

	vScrollPos = line-1;
	if (vScrollPos < 0)
		vScrollPos = 0;
	if (vScrollPos > vScrollMax-(vScrollPage-1))
		vScrollPos = vScrollMax-(vScrollPage-1);
	vScrollInfo.fMask = SIF_POS;
	vScrollInfo.nPos = vScrollPos;
	SetScrollInfo(SB_VERT, &vScrollInfo, TRUE);

	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}


void CSourceWindow::Update()
{
	if (!sourceFile || !sourceFile->open)
		return;							// Not open!!

	if (globals.pcTracking && pcFile == sourceFile) // If PC tracking enabled
	{
										// Set the scrollers
		hScrollPos = 0;					// Start of line
		hScrollInfo.fMask = SIF_POS;
		hScrollInfo.nPos = hScrollPos;
		SetScrollInfo(SB_HORZ, &hScrollInfo, TRUE);

		vScrollPos = pcLine-1; //(vScrollPage/2);
		if (vScrollPos < 0)
			vScrollPos = 0;
		if (vScrollPos > vScrollMax-(vScrollPage-1))
			vScrollPos = vScrollMax-(vScrollPage-1);
		vScrollInfo.fMask = SIF_POS;
		vScrollInfo.nPos = vScrollPos;
		SetScrollInfo(SB_VERT, &vScrollInfo, TRUE);
	}

	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}


void CSourceWindow::DrawSource()
{
	HDC hDC = ::GetDC(m_hWnd);
	PaintSource(hDC);
	::ReleaseDC(m_hWnd, hDC);
}


void CSourceWindow::RedrawWindow()
{
	if (!sourceFile || !sourceFile->open)
		return;							// Not open!!
	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}


BEGIN_MESSAGE_MAP(CSourceWindow, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSourceWindow)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSourceWindow message handlers


BOOL CSourceWindow::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;
	cs.style |= WS_HSCROLL|WS_VSCROLL;
	cs.style &= ~(LONG)FWS_ADDTOTITLE;
	return TRUE;
}


int CSourceWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SendMessage(WM_SIZE); //, 0, MAKELPARAM(rect.right-rect.left,
	return 0;
}


void CSourceWindow::OnUpdateFrameTitle(BOOL addToTitle)
{
	if (sourceFile != NULL)
		::SetWindowText(m_hWnd, sourceFile->GetFileName());
}


BOOL CSourceWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


void CSourceWindow::OnPaint() 
{
	CRect rect;
	CPaintDC dc(this); // device context for painting

	if (sourceFile && !sourceFile->IsWindowMinimized())
	{
		if (sizing)
			Invalidate(FALSE);
		PaintSource(dc.m_hDC);
/*		GetClientRect(&rect);
		TRIVERTEX vert[2];
		GRADIENT_RECT gRect;
		vert[0].x = 0;
		vert[0].y = 0;
		vert[0].Red = (GRADIENTTOP&0xff)<<8; // 0x5000;
		vert[0].Green = GRADIENTTOP&0xff00; //0x5000;
		vert[0].Blue = (GRADIENTTOP&0xff0000)>>8; //0xff00;
		vert[0].Alpha = 0x0000;
		vert[1].x = rect.Width()+1;
		vert[1].y = rect.Height()+1;
		vert[1].Red = (GRADIENTBOTTOM&0xff)<<8; // 0xff00;
		vert[1].Green = GRADIENTBOTTOM&0xff00; //0xff00;
		vert[1].Blue = (GRADIENTBOTTOM&0xff0000)>>8; //0xff00;
		vert[1].Alpha = 0x0000;
		gRect.UpperLeft = 0;
		gRect.LowerRight = 1;
		::GradientFill(dc.m_hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
*/		
		if (sizing)
			sizing = false;
	}
}


void CSourceWindow::PaintSource(HDC hDC)
{
	HBRUSH brush = NULL;
//	CBrush * brush = NULL;
	CRect rect;
	char * string;
	int length;
	int line;
	BOOL newBrush;
	BOOL merge;
	int colour = 0;
	int breakpoint;
	int x;
	int i, j;

	::SelectObject(hDC, textFont);
//	dc.SelectObject(&textFont);
	::SetTextColor(hDC, TEXTCOLOUR);
//	dc.SetTextColor(TEXTCOLOUR);
	::SetBkColor(hDC, WHITE);
//	dc.SetBkColor(WHITE);
	::SetBkMode(hDC, OPAQUE);
//	dc.SetBkMode(OPAQUE);
	GetClientRect(&rect);

	HDC pcHDC = ::CreateCompatibleDC(NULL);
	::SelectObject(pcHDC, pcBitmap);
//	CDC pcDC; 
//	pcDC.CreateCompatibleDC(&pcDC);
//	pcDC.SelectObject(&pcBitmap);
	HDC cursorHDC = ::CreateCompatibleDC(NULL);
	::SelectObject(cursorHDC, cursorBitmap);
//	CDC cursorDC; 
//	cursorDC.CreateCompatibleDC(&cursorDC);
//	cursorDC.SelectObject(&cursorBitmap);
	HDC softBPHDC = ::CreateCompatibleDC(NULL);
	::SelectObject(softBPHDC, softBPBitmap);
//	CDC softBPDC; 
//	softBPDC.CreateCompatibleDC(&softBPDC);
//	softBPDC.SelectObject(&softBPBitmap);
	HDC hardBPHDC = ::CreateCompatibleDC(NULL);
	::SelectObject(hardBPHDC, hardBPBitmap);
//	CDC hardBPDC; 
//	hardBPDC.CreateCompatibleDC(&hardBPDC);
//	hardBPDC.SelectObject(&hardBPBitmap);

	GetClientRect(&rect);
	int clientH = rect.Height();
	int clientW = rect.Width();
//	if (hScrollPage < hScrollMax)
//		clientH -= GetSystemMetrics(SM_CYHSCROLL);
	int rows = clientH/charSize.cy;
	for (i=0; i<rows+1; i++)			// Extra 1 for the part-showing line
	{
		// White-out the line
		rect.top = i*charSize.cy;
		rect.bottom = rect.top+charSize.cy;

		if (vScrollPage >= sourceFile->GetNumLines())
			line = i;
		else
			line = vScrollPos+i;			// Line number

		rect.left = 0;
		rect.right = BREAKPOINTBARW;
		::FillRect(hDC, &rect, bpBarBrush);
//		dc.FillRect(&rect, &bpBarBrush);

		rect.left = BREAKPOINTBARW;
		rect.right = clientW;

		colour = SOURCECOLOUR;
		brush = sourceBrush;
		newBrush = false;
		merge = false;
		breakpoint = sourceFile->GetLineBreakpoint(line);
		if (breakpoint == SOFTBREAKPOINT)
		{
			::BitBlt(hDC, BREAKPOINTBARW-18, rect.top-((charSize.cy-15)/2),
				18, 15, softBPHDC, 0, 0, SRCCOPY);
//			dc.BitBlt(BREAKPOINTBARW-18, rect.top-((charSize.cy-15)/2),
//				18, 15, &softBPDC, 0, 0, SRCCOPY);
			colour = SOFTBPCOLOUR;
			brush = softBPBrush;
			merge = true;
		}
		else if (breakpoint == HARDBREAKPOINT)
		{
			::BitBlt(hDC, BREAKPOINTBARW-18, rect.top-((charSize.cy-15)/2),
				18, 15, hardBPHDC, 0, 0, SRCCOPY);
//			dc.BitBlt(BREAKPOINTBARW-18, rect.top-((charSize.cy-15)/2),
//				18, 15, &hardBPDC, 0, 0, SRCCOPY);
			colour = HARDBPCOLOUR;
			brush = hardBPBrush;
			merge = true;
		}
		if (sourceFile == pcFile && line == pcLine)
		{
			::BitBlt(hDC, BREAKPOINTBARW-18, rect.top-((charSize.cy-15)/2),
				18, 15, pcHDC, 0, 0, SRCCOPY);
//			dc.BitBlt(BREAKPOINTBARW-18, rect.top-((charSize.cy-15)/2),
//				18, 15, &pcDC, 0, 0, SRCCOPY);
			if (merge)
			{
				colour = (((colour&0xff)+(PCCOLOUR&0xff))/2)|
					(((colour&0xff00)+(PCCOLOUR&0xff00))/2)|
					(((colour&0xff0000)+(PCCOLOUR&0xff0000))/2);
				brush = ::CreateSolidBrush(colour);
//				brush = new CBrush();
//				brush->CreateSolidBrush(colour);
				newBrush = true;
			}
			else
			{
				colour = PCCOLOUR;
				brush = pcBrush;
				merge = true;
			}
		}
		if (sourceFile == cursorFile && line == cursorLine)
		{
			::BitBlt(hDC, BREAKPOINTBARW-18, rect.top-((charSize.cy-15)/2),
				18, 15, cursorHDC, 0, 0, SRCCOPY);
//			dc.BitBlt(BREAKPOINTBARW-18, rect.top-((charSize.cy-15)/2),
//				18, 15, &cursorDC, 0, 0, SRCCOPY);
//			if (merge)
//			{
//				colour = (((colour&0xff)+(CURSORCOLOUR&0xff))/2)|
//					(((colour&0xff00)+(CURSORCOLOUR&0xff00))/2)|
//					(((colour&0xff0000)+(CURSORCOLOUR&0xff0000))/2);
//				if (newBrush)
//					::DeleteObject(brush);
//				brush = ::CreateSolidBrush(colour);
//				newBrush = true;
//			}
//			else
//			{
//				colour = CURSORCOLOUR;
//				brush = cursorBrush;
//			}
		}
		::SetBkColor(hDC, colour);
//		dc.SetBkColor(colour);
		::FillRect(hDC, &rect, brush);
//		dc.FillRect(&rect, brush);
		if (newBrush)
			::DeleteObject(brush);
//			brush->DeleteObject();

		// If past last line then stop
		if (line >= sourceFile->GetNumLines())
			break;

		// Don't draw text for a Hardware Breakpoint (ask Dave!!)
		if (breakpoint != HARDBREAKPOINT)
		{
			if (sourceFile->GetLine(line)->length > hScrollPos) // If part line visible
			{
				if (globals.syntaxHiLighting) // If context Hi-Lighting
				{
					for (j=0; j<sourceFile->GetLine(line)->numSegments; j++)
					{					// For each segment
						if (sourceFile->GetLine(line)->segments[j].start+
							(sourceFile->GetLine(line)->segments[j].length-1) >= hScrollPos)
						{				// If some of segment is visible
							x = BREAKPOINTBARW;	// First char position
							if (sourceFile->GetLine(line)->segments[j].start >= hScrollPos)
							{			// If all of segment is visible
										// Calc X position
								x += (sourceFile->GetLine(line)->segments[j].start-
									hScrollPos)*charSize.cx;
										// First char is first char of segment
								string = &(sourceFile->GetLine(line)->line[
									sourceFile->GetLine(line)->segments[j].start]);
										// Length is length of segment
								length = sourceFile->GetLine(line)->segments[j].length;
							}
							else		// If part of segment is visible
							{
										// First char is "Horiz-Scroll-Pos"th
								string = &(sourceFile->GetLine(line)->line[hScrollPos]);
										// Length is Segs Last - H Scroll Pos
								length = (sourceFile->GetLine(line)->segments[j].start+
									sourceFile->GetLine(line)->segments[j].length)-
									hScrollPos;
							}
							::SetTextColor(hDC,
								sourceFile->GetLine(line)->segments[j].colour);
//							dc.SetTextColor(
//								sourceFile->GetLine(line)->segments[j].colour);
							::TextOut(hDC, x, i*charSize.cy, string, length);
//							dc.TextOut(x, i*charSize.cy, string, length);
						}
					}
				}
				else					// No Hi-Lighting
				{
					::SetTextColor(hDC, BLACK);
//					dc.SetTextColor(BLACK);
					::TextOut(hDC, BREAKPOINTBARW, i*charSize.cy,
						&(sourceFile->GetLine(line)->line[hScrollPos]),
						sourceFile->GetLine(line)->length-hScrollPos);
//					dc.TextOut(BREAKPOINTBARW, i*charSize.cy,
//						&(sourceFile->GetLine(line)->line[hScrollPos]),
//						sourceFile->GetLine(line)->length-hScrollPos);
				}
			}
		}
	}
	// If a gap at the bottom, fill it.
	if (i*charSize.cy < clientH)
	{
		rect.top = i*charSize.cy;
		rect.bottom = clientH;
		rect.left = 0;
		rect.right = BREAKPOINTBARW;
		::FillRect(hDC, &rect, bpBarBrush);
//		dc.FillRect(&rect, &bpBarBrush);
		rect.left = BREAKPOINTBARW;
		rect.right = clientW;
		::FillRect(hDC, &rect, sourceBrush);
//		dc.FillRect(&rect, &sourceBrush);
	}
	::DeleteDC(pcHDC);
//	pcDC.DeleteDC();
	::DeleteDC(cursorHDC);
//	cursorDC.DeleteDC();
	::DeleteDC(softBPHDC);
//	softBPDC.DeleteDC();
	::DeleteDC(hardBPHDC);
//	hardBPDC.DeleteDC();
}


void CSourceWindow::OnMove(int x, int y) 
{
	WINDOWPLACEMENT wndPlacement;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	CMDIChildWnd::OnMove(x, y);

	if (sourceFile && !sourceFile->IsWindowMinimized() &&
		!sourceFile->IsWindowMaximized())
	{
		GetWindowPlacement(&wndPlacement);
		sourceFile->SetWindowRect(&(wndPlacement.rcNormalPosition));
	}
}


void CSourceWindow::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;
	WINDOWPLACEMENT wndPlacement;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	CMDIChildWnd::OnSize(nType, cx, cy);

	if (!sourceFile)
		return;

	GetClientRect(&rect);
	int clientW = rect.Width();
	int clientH = rect.Height();

	if (init)							// Set the size
	{
		sizing = TRUE;
		sourceFile->GetWindowRect(&rect);
		MoveWindow(&rect, TRUE);
		init = FALSE;
		return;
	}

	if (nType == SIZE_MINIMIZED)
	{
		sourceFile->SetMinimized(TRUE);
		sourceFile->SetMaximized(FALSE);
		return;
	}
	if (nType == SIZE_MAXIMIZED)
	{
		sourceFile->SetMaximized(TRUE);
		sourceFile->SetMinimized(FALSE);
		sizing = TRUE;					// Re-sized
	}
	if (nType == SIZE_RESTORED)
	{
		sourceFile->SetMaximized(FALSE);
		sourceFile->SetMinimized(FALSE);
		GetWindowPlacement(&wndPlacement);
		sourceFile->SetWindowRect(&(wndPlacement.rcNormalPosition));
		sizing = TRUE;					// Re-sized
	}

	if (sizing)
	{
		// Do we need the scrollers?
		BOOL needHScroller = FALSE;
		BOOL needVScroller = FALSE;
		int vScrollExtra = 0;
		int hScrollExtra = 0;
		if (clientH/charSize.cy < sourceFile->GetNumLines())
		{
			vScrollExtra = GetSystemMetrics(SM_CXVSCROLL);
			needVScroller = true;
		}
		if ((clientW-BREAKPOINTBARW)/charSize.cx <
			sourceFile->longestLineLength)
		{
			hScrollExtra = GetSystemMetrics(SM_CYHSCROLL);
			needHScroller = true;
		}
		if (!needVScroller && needHScroller) // Horiz scroller might mean
		{								// we need vert scroller
			if ((clientH-hScrollExtra)/charSize.cy < sourceFile->GetNumLines())
			{
				vScrollExtra = GetSystemMetrics(SM_CXVSCROLL);
				needVScroller = true;
			}
		}
		if (!needHScroller && needVScroller) // Vert scroller might mean
		{								// we need horiz scroller
			if ((clientW-(BREAKPOINTBARW+vScrollExtra))/charSize.cx <
				sourceFile->longestLineLength)
			{
				hScrollExtra = GetSystemMetrics(SM_CYHSCROLL);
				needHScroller = true;
			}
		}
		hScrollPage = (clientW-(BREAKPOINTBARW+vScrollExtra))/charSize.cx;
		vScrollPage = (clientH-hScrollExtra)/charSize.cy;
		
		// Move the scrollers
//		hScrollBar.MoveWindow(0, clientH-GetSystemMetrics(SM_CYHSCROLL),
//			clientW-vScrollExtra, GetSystemMetrics(SM_CYHSCROLL), FALSE);
//		::MoveWindow(vScroll, clientW-GetSystemMetrics(SM_CXVSCROLL), 0,
//			GetSystemMetrics(SM_CXVSCROLL),	clientH-hScrollExtra, FALSE);
//		::MoveWindow(sizeBox, clientW-GetSystemMetrics(SM_CXVSCROLL),
//			clientH-GetSystemMetrics(SM_CYHSCROLL),
//			GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CYHSCROLL),
//			FALSE);

//		// Do we need the sizeBox?
//		if (needHScroller && needVScroller)
//			::ShowWindow(sizeBox, SW_SHOW);
//		else
//			::ShowWindow(sizeBox, SW_HIDE);

		// Set the draw positions from the scrollers
		GetScrollInfo(SB_HORZ, &hScrollInfo);
		if (hScrollPos != hScrollInfo.nPos)
			hScrollPos = hScrollInfo.nPos;

		GetScrollInfo(SB_VERT, &vScrollInfo);
		if (vScrollPos != vScrollInfo.nPos)
			vScrollPos = vScrollInfo.nPos;

		// Set the scrollers' Page sizes
		hScrollInfo.fMask = SIF_PAGE;
		hScrollInfo.nPage = hScrollPage;
		SetScrollInfo(SB_HORZ, &hScrollInfo, TRUE);
//		if (needHScroller)
//			hScrollBar.ShowWindow(SW_SHOW);
//		else
//			hScrollBar.ShowWindow(SW_HIDE);

		vScrollInfo.fMask = SIF_PAGE;
		vScrollInfo.nPage = vScrollPage;
		SetScrollInfo(SB_VERT, &vScrollInfo, TRUE);
//		if (needVScroller)
//			::ShowWindow(vScroll, SW_SHOW);
//		else
//			::ShowWindow(vScroll, SW_HIDE);
	}
}


void CSourceWindow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (!sourceFile)
	{
		CMDIChildWnd::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	switch (nSBCode)
	{
		case SB_PAGEDOWN:
			hScrollPos += hScrollPage-1;

		case SB_LINEDOWN:
			hScrollPos = min(hScrollMax-(hScrollPage-1), hScrollPos+1);
			break;

		case SB_PAGEUP:
			hScrollPos -= hScrollPage-1;

		case SB_LINEUP:
			hScrollPos = max(hScrollMin, hScrollPos-1);
			break;

		case SB_TOP:
			hScrollPos = hScrollMin;
			break;

		case SB_BOTTOM:
			hScrollPos = hScrollMax-(hScrollPage-1);
			break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			hScrollPos = nPos;
			break;

		default:
			break;
	}
	DrawSource();
	SetScrollPos(SB_HORZ, hScrollPos, TRUE);
	CMDIChildWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSourceWindow::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (!sourceFile)
	{
		CMDIChildWnd::OnVScroll(nSBCode, nPos, pScrollBar);
		return;
	}
	switch (nSBCode)
	{
		case SB_PAGEDOWN:
			vScrollPos += vScrollPage-1;

		case SB_LINEDOWN:
			vScrollPos = min(vScrollMax-(vScrollPage-1), vScrollPos+1);
			break;

		case SB_PAGEUP:
			vScrollPos -= vScrollPage-1;

		case SB_LINEUP:
			vScrollPos = max(vScrollMin, vScrollPos-1);
			break;

		case SB_TOP:
			vScrollPos = vScrollMin;
			break;

		case SB_BOTTOM:
			vScrollPos = vScrollMax-(vScrollPage-1);
			break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			vScrollPos = nPos;
			break;

		default:
			break;
	}
	DrawSource();
	SetScrollPos(SB_VERT, vScrollPos, TRUE);
	CMDIChildWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CSourceWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!sourceFile)
		return;
	int x = point.x;
	int y = point.y;
	int line = y/charSize.cy;
	if (vScrollPage < sourceFile->GetNumLines())
		line += vScrollPos;
	if (line >= sourceFile->GetNumLines())
		return;							// Click was beyond EOF
	if (x < BREAKPOINTBARW)				// If click within Breakpoint Bar
	{
		if (sourceFile->GetLine(line)->code) // If a code line
		{
			if (!sourceFile->SetSoftBP(line)) // Try to set
				sourceFile->ClearSoftBP(line); // If set, clear it
			RedrawWindow();
			project->UpdateBreakpoints();
		}
	}
	else								// Move cursor
	{
		if (cursorFile != NULL && cursorFile != sourceFile)
		{
			cursorLine = -1;
			x = cursorFile->GetSourceFileIndex();
			if (sourceWindows[x] != NULL)
				sourceWindows[x]->RedrawWindow();
		}
		cursorLine = line;
		cursorFile = sourceFile;
		RedrawWindow();
	}
	
	CMDIChildWnd::OnLButtonDown(nFlags, point);
}


void CSourceWindow::OnClose() 
{
	if (sourceFile != NULL)
	{
		// Delete the allocated memory in the source File
		SourceLine * line;
		for (int i=0; i<sourceFile->GetNumLines(); i++)
		{
			line = sourceFile->GetLine(i);
			if (line->numSegments)
				delete [] line->segments;
			line->segments = NULL;
			line->numSegments = 0;
		}
		// Close it
		sourceFile->open = FALSE;
		sourceWindows[sourceFile->GetSourceFileIndex()] = NULL;
		sourceFile = NULL;
	}
	CMDIChildWnd::OnClose();
}


void CSourceWindow::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	if (sourceFile != NULL)
		sourceWindows[sourceFile->GetSourceFileIndex()] = NULL;
}
