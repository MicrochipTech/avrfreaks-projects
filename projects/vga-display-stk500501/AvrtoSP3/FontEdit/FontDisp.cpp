// FontDisp.cpp : implementation file
//

#include "stdafx.h"
#include "FontEdit.h"
#include "FontDisp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontDisp

CFontDisp::CFontDisp()
{
	pDBDisp = NULL;	//no debug display
	BlackPen.CreatePen(PS_SOLID,1,COLORREF(0));	//black pen for drawing grid
	clicked = 0;		//number of times this has been clicked
	pChar = NULL;		//pointer to character to display/edit
}

CFontDisp::~CFontDisp()
{
}


BEGIN_MESSAGE_MAP(CFontDisp, CStatic)
	//{{AFX_MSG_MAP(CFontDisp)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()	//message handler for clicking on display
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontDisp message handlers

void CFontDisp::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CStatic::OnPaint() for painting messages

	RECT clrect;
	int X,Y;
	GetClientRect(&clrect);
	X = clrect.right;
	Y = clrect.bottom;
	CBitmap BitmapFont;
	BitmapFont.CreateCompatibleBitmap(&dc,X,Y);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBrush brush (RGB(255,255,0));
	CBitmap * pOldBitmap = dcMem.SelectObject(&BitmapFont);
	dcMem.SetBkColor(RGB(255,255,0));
	dcMem.FillRect(CRect(0,0,X,Y),&brush);
	CPen *pOldPen = dcMem.SelectObject(&BlackPen);
	int i,j;
	//-----------------------------------------
	// Draw Grid
	//-----------------------------------------
	for(i=0;i<8;++i)
	{
		if(i)
		{
			dcMem.MoveTo(i*12,0);
			dcMem.LineTo(i*12,Y);
			dcMem.MoveTo(0,i*12);
			dcMem.LineTo(X,i*12);
		}
	}
	CBrush gbrush (RGB(0,255,0));
	if(pChar)	//is there anything to display?
	{
		for(i=0;i<8;++i)	//select byte
		{
			for(j=0;j<8;++j)	//select bit
			{
				if(pChar[i] & (1 << j))
					dcMem.FillRect(CRect( (7-j)*12+1,i*12+1,(7-j)*12+11,i*12+11 ),&gbrush);
				else
					dcMem.FillRect(CRect((7-j)*12+1,i*12+1,(7-j)*12+11,i*12+11),&brush);
			}
		}
	}
	dc.BitBlt(0,0,X,Y,&dcMem,0,0,SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
}

void CFontDisp::SetDebugDisplay(CStatic *pDB)
{
	pDBDisp = pDB;
}


void CFontDisp::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//------------------------------------------------------
	// This is where we handle a double click message for
	// changing the bits in the display
	//------------------------------------------------------
	CStatic::OnLButtonDblClk(nFlags, point);

	int X,Y;	//x holds the BIT, Y hold byte number

	X = point.x / 12;
	Y = point.y / 12;

	//---------------------------------------
	// OK, we need to toggle the appropriate
	// Bit in the character
	//---------------------------------------
	if(pChar)
	{
		pChar[Y] ^= 1 << (7-X);
		Invalidate();
	}
	if(pDBDisp)
	{
		++clicked;
		char *s = new char[256];
		sprintf(s,"X = %d  Y = %d\nClicked %d",X,Y,clicked);
		pDBDisp->SetWindowText(s);
		delete[] s;
	}
}

void CFontDisp::SetChar(char *pC)
{
	//-------------------------------------
	// Set pointer to character to display
	//-------------------------------------
	pChar = pC;
}
