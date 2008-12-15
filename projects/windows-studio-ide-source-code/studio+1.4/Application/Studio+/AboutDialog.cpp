// AboutDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Studio.h"
#include "AboutDialog.h"

#include "Globals.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog


CAboutDialog::CAboutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDialog)
	DDX_Control(pDX, IDC_ABOUTANIM, m_Animation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
	//{{AFX_MSG_MAP(CAboutDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAboutDialog message handlers


BOOL CAboutDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Animation.Open(IDA_ABOUT);
	return TRUE;
}


void CAboutDialog::OnPaint() 
{
	CRect rect;
	CRect cRect;
	SIZE fontSize;
	char buffer[500];

	CPaintDC dc(this); // device context for painting
	
	GetClientRect(&cRect);
	GetWindowRect(&rect);
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

	HFONT studioFont = ::CreateFont(36, 0, 0, 0, FW_BOLD, TRUE, FALSE, 0,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");
	HFONT textFont = ::CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");
	HFONT gotoFont = ::CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");
	HFONT wwwFont = ::CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");

	::SetBkMode(dc.m_hDC, TRANSPARENT);

	int x = 10+6+150+6+10;
	
	HFONT hOldFont = (HFONT)::SelectObject(dc.m_hDC, studioFont);
	::GetTextExtentPoint32(dc.m_hDC, STRING_NAME, strlen(STRING_NAME),
		&fontSize);
	int studioH = fontSize.cy;
	::SetTextColor(dc.m_hDC, 0x00FFFFFF);
	rect.left = x+(((cRect.Width()-x)/2)-fontSize.cx/2)+3;
	rect.top = 6+3;
	rect.right = rect.left+fontSize.cx;
	rect.bottom = rect.top+fontSize.cy;
	::DrawText(dc.m_hDC, STRING_NAME, strlen(STRING_NAME), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);
	::SetTextColor(dc.m_hDC, 0x00FF0000);
	rect.left = x+((cRect.Width()-x)/2)-fontSize.cx/2;
	rect.top = 6;
	rect.right = rect.left+fontSize.cx;
	rect.bottom = rect.top+fontSize.cy;
	::DrawText(dc.m_hDC, STRING_NAME, strlen(STRING_NAME), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);

	::SelectObject(dc.m_hDC, wwwFont);
	::GetTextExtentPoint32(dc.m_hDC, STRING_NAME, strlen(STRING_NAME),
		&fontSize);
	::SetTextColor(dc.m_hDC, 0x00000000);

	rect.left = x;
	rect.top = 6+studioH+6;
	rect.right = cRect.right;
	rect.bottom = cRect.bottom;
	::DrawText(dc.m_hDC, STRING_VERSION, strlen(STRING_VERSION), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);
	rect.top += fontSize.cy;
	::DrawText(dc.m_hDC, STRING_COPYRIGHT, strlen(STRING_COPYRIGHT), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);

	if (globals.registered)
	{
		rect.top += 3*fontSize.cy;
		strcpy(buffer, "Registered");
		::SetTextColor(dc.m_hDC, 0x00000080);
		::DrawText(dc.m_hDC, buffer, strlen(buffer), &rect,
			DT_NOCLIP|DT_LEFT|DT_TOP);
	}
	else
	{
		rect.top += (3*fontSize.cy)/2;
		strcpy(buffer, "Unregistered");
		::SetTextColor(dc.m_hDC, 0x00000080);
		::DrawText(dc.m_hDC, buffer, strlen(buffer), &rect,
			DT_NOCLIP|DT_LEFT|DT_TOP);
		rect.top += (3*fontSize.cy)/2;

		::SelectObject(dc.m_hDC, textFont);
		strcpy(buffer,
"This unregistered trial version will only work for 30 \
days.  Also, the project code size is limited to 2K. \
You will need to register to debug projects larger than 2K.");
		::GetTextExtentPoint32(dc.m_hDC, buffer, strlen(buffer), &fontSize);
		rect.right = cRect.right-16;
		::SetTextColor(dc.m_hDC, 0x00000000);
		::DrawText(dc.m_hDC, buffer, strlen(buffer), &rect,
			DT_NOCLIP|DT_LEFT|DT_TOP|DT_WORDBREAK|DT_CALCRECT);
		::DrawText(dc.m_hDC, buffer, strlen(buffer), &rect,
			DT_NOCLIP|DT_LEFT|DT_TOP|DT_WORDBREAK);
	}

	::SelectObject(dc.m_hDC, gotoFont);
	strcpy(buffer, "For more information on ");
	strcat(buffer, STRING_NAME);
	strcat(buffer, ", visit: ");
	::GetTextExtentPoint32(dc.m_hDC, buffer, strlen(buffer), &fontSize);
	rect.top = 10+6+150+6+10;
	rect.bottom = rect.top+fontSize.cy;
	rect.left = 10;
	rect.right = cRect.right-16;
	::DrawText(dc.m_hDC, buffer, strlen(buffer), &rect,
		DT_NOCLIP|DT_LEFT|DT_BOTTOM);
	rect.top = rect.bottom;
	rect.bottom = rect.top+fontSize.cy;
	strcpy(buffer, "     ");
	strcat(buffer, STRING_FLASHURL);
	::SelectObject(dc.m_hDC, wwwFont);
	::SetTextColor(dc.m_hDC, 0x00FF0000);
	::GetTextExtentPoint32(dc.m_hDC, buffer, strlen(buffer), &fontSize);
	::DrawText(dc.m_hDC, buffer, strlen(buffer), &rect,
		DT_NOCLIP|DT_LEFT|DT_BOTTOM|DT_WORDBREAK);
	rect.left += fontSize.cx;

	strcpy(buffer, __DATE__);
	::SelectObject(dc.m_hDC, textFont);
	::SetTextColor(dc.m_hDC, 0x00000000);
	::GetTextExtentPoint32(dc.m_hDC, buffer, strlen(buffer), &fontSize);
	rect.left = 10;
	rect.top = cRect.bottom-(10+fontSize.cy);
	rect.bottom = cRect.bottom;
	::DrawText(dc.m_hDC, buffer, strlen(buffer), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);

	::SelectObject(dc.m_hDC, hOldFont);
	::DeleteObject(studioFont);
	::DeleteObject(textFont);
	::DeleteObject(gotoFont);
	::DeleteObject(wwwFont);
}
