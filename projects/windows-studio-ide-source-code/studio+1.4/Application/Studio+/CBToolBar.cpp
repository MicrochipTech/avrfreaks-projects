// CBToolBarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "CBToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCBToolBarCtrl

CCBToolBar::CCBToolBar()
{
}

CCBToolBar::~CCBToolBar()
{
}


BEGIN_MESSAGE_MAP(CCBToolBar, CToolBar)
	//{{AFX_MSG_MAP(CCBToolBar)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCBToolBar message handlers

BOOL CCBToolBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CToolBar::PreCreateWindow(cs))
		return FALSE;
	cs.style &= ~WS_SYSMENU;
	return TRUE;
	
//	return CToolBar::PreCreateWindow(cs);
}


void CCBToolBar::OnClose() 
{
}



