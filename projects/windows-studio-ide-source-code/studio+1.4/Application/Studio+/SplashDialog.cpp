// SplashDialog.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "SplashDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CSplashDialog dialog


CSplashDialog::CSplashDialog(CWnd* pParent)
	: CDialog(CSplashDialog::IDD, pParent)
{
	CDialog::Create(IDD_SPLASH);

	//{{AFX_DATA_INIT(CSplashDialog)
	//}}AFX_DATA_INIT
}


void CSplashDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashDialog, CDialog)
	//{{AFX_MSG_MAP(CSplashDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashDialog message handlers

