// PluginsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "PluginsDialog.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPluginsDialog dialog


CPluginsDialog::CPluginsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPluginsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPluginsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPluginsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPluginsDialog)
	DDX_Control(pDX, IDC_PLUGINS_FAMILYS, m_familys);
	DDX_Control(pDX, IDC_PLUGINS_IDES, m_ides);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPluginsDialog, CDialog)
	//{{AFX_MSG_MAP(CPluginsDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPluginsDialog message handlers


BOOL CPluginsDialog::OnInitDialog() 
{
	int i;
	CString str;

	CDialog::OnInitDialog();

	for (i=0; i<numberFamilys; i++)
	{
		str = familyNames[i*2];
		str += " v";
		str += familyVersions[i];
		m_familys.AddString(str);
	}
	
	for (i=0; i<numberIDEs; i++)
	{
		str = ideNames[i*2];
		str += " v";
		str += ideVersions[i];
		m_ides.AddString(str);
	}

	return TRUE;
}


void CPluginsDialog::OnPaint() 
{
	CRect rect;
	CPaintDC dc(this); // device context for painting
	
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

	HFONT hFont = ::CreateFont(16, 0, 0, 0, FW_BOLD, TRUE, FALSE, 0,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");

	::SetBkMode(dc.m_hDC, TRANSPARENT);

	HFONT hOldFont = (HFONT)::SelectObject(dc.m_hDC, hFont);
	::SetTextColor(dc.m_hDC, 0x00400000);

	GetClientRect(&rect);
	int width = rect.Width();
	rect.left = 10;
	rect.top = 10;
	::DrawText(dc.m_hDC, "CPU Familys", strlen("CPU Familys"), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);

	rect.left = width/2+6;
	::DrawText(dc.m_hDC, "Development Environments",
		strlen("Development Environments"), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);

	::SelectObject(dc.m_hDC, hOldFont);
	::DeleteObject(hFont);
}
