// ProjectWizard.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "ProjectWizard.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectWizard dialog


CProjectWizard::CProjectWizard(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectWizard::IDD, pParent)
{
	ideLogo = NULL;
	familyLogo = NULL;
	
	currentIDE = 0;
	currentFamily = 0;

	//{{AFX_DATA_INIT(CProjectWizard)
	//}}AFX_DATA_INIT
}


void CProjectWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectWizard)
	DDX_Control(pDX, IDC_PROJECTWIZARD_IDELOGO, m_ideLogo);
	DDX_Control(pDX, IDC_PROJECTWIZARD_FAMILYLOGO, m_familyLogo);
	DDX_Control(pDX, IDC_PROJECTWIZARD_FAMILY, m_family);
	DDX_Control(pDX, IDC_PROJECTWIZARD_IDE, m_ide);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectWizard, CDialog)
	//{{AFX_MSG_MAP(CProjectWizard)
	ON_CBN_SELCHANGE(IDC_PROJECTWIZARD_IDE, OnSelchangeProjectwizardIde)
	ON_CBN_SELCHANGE(IDC_PROJECTWIZARD_FAMILY, OnSelchangeProjectwizardFamily)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProjectWizard message handlers


BOOL CProjectWizard::OnInitDialog() 
{
	int i;

	CDialog::OnInitDialog();

	for (i=0; i<numberIDEs; i++)
		m_ide.AddString(ideNames[i*2]);
	m_ide.SetCurSel(0);
	OnSelchangeProjectwizardIde();	

	m_family.AddString("AVR");
	m_family.SetCurSel(0);
	OnSelchangeProjectwizardFamily();

	return TRUE;
}


void CProjectWizard::OnOK() 
{
	// Check to see if the selected IDE is registered
	if (!(theApp.OpenIdeLibrary(ideNames[m_ide.GetCurSel()*2])))
	{
		char buffer[1000];
		strcpy(buffer, "This copy of ");
		strcat(buffer, STRING_NAME);
		strcat(buffer, " has not been registered to support\r\n\
projects created using ");
		strcat(buffer, ideNames[m_ide.GetCurSel()*2]);
		strcat(buffer, ". Please contact Flash to register it.");
		::MessageBox(NULL, buffer, "Development Environment not Registered",
			MB_OK|MB_ICONSTOP);
		return;
	}

//	if ((ILGetIdeID()&globals.ides) == 0)
//	{
//		char buffer[1000];
//		strcpy(buffer, "This copy of ");
//		strcat(buffer, STRING_NAME);
//		strcat(buffer, " has not been registered to support\r\n\
//projects created using ");
//		strcat(buffer, ideNames[m_ide.GetCurSel()*2]);
//		strcat(buffer, ". Please contact Flash to register it.");
//		::MessageBox(NULL, buffer, "Development Environment not Registered",
//			MB_OK|MB_ICONSTOP);
//		return;
//	}
	CDialog::OnOK();
}


void CProjectWizard::OnSelchangeProjectwizardIde() 
{
	CString libPath;

	// Open the IDE Library
	if (hIdeDll)
		::FreeLibrary(hIdeDll);
	hIdeDll = NULL;

	libPath = instDir;
	libPath += "Plug-ins\\";
	libPath += ideNames[m_ide.GetCurSel()*2+1];
	hIdeDll = ::LoadLibrary(libPath);
	ILGetIdeID = (GETIDEID)GetProcAddress(hIdeDll, "GetIdeID");
	ILGetIdeLogo = (GETIDELOGO)GetProcAddress(hIdeDll, "GetIdeLogo");

	currentIDE = m_ide.GetCurSel();
	if (ideLogo)
		::DeleteObject(ideLogo);
	ideLogo = ILGetIdeLogo();
	m_ideLogo.SetBitmap(ideLogo);	
}


void CProjectWizard::OnSelchangeProjectwizardFamily() 
{
	// Open the AVR Family Library
	theApp.OpenFamilyLibrary("AVR");
	if (familyLogo)
		::DeleteObject(familyLogo);
	familyLogo = CLGetFamilyLogo();
	m_familyLogo.SetBitmap(familyLogo);	
}


void CProjectWizard::OnPaint() 
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

	HFONT hFont = ::CreateFont(18, 0, 0, 0, FW_BOLD, TRUE, FALSE, 0,
		ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, "Arial");

	::SetBkMode(dc.m_hDC, TRANSPARENT);

	HFONT hOldFont = (HFONT)::SelectObject(dc.m_hDC, hFont);
	::SetTextColor(dc.m_hDC, 0x00400000);

	GetClientRect(&rect);
	rect.left = 12;
	rect.top = 10;
	::DrawText(dc.m_hDC, "Project Development Environment",
		strlen("Project Development Environment"), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);

	rect.top = rect.top+115;
	::DrawText(dc.m_hDC, "Target CPU Family",
		strlen("Target CPU Family"), &rect,
		DT_NOCLIP|DT_LEFT|DT_TOP);

	::SelectObject(dc.m_hDC, hOldFont);
	::DeleteObject(hFont);
}


void CProjectWizard::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if (ideLogo)
		::DeleteObject(ideLogo);
	if (familyLogo)
		::DeleteObject(familyLogo);
}
