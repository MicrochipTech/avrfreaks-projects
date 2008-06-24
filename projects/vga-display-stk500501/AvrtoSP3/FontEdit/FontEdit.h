// FontEdit.h : main header file for the FONTEDIT application
//

#if !defined(AFX_FONTEDIT_H__F04E1F46_D2AB_4254_81D0_34FB06960E07__INCLUDED_)
#define AFX_FONTEDIT_H__F04E1F46_D2AB_4254_81D0_34FB06960E07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFontEditApp:
// See FontEdit.cpp for the implementation of this class
//

class CFontEditApp : public CWinApp
{
public:
	CFontEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontEditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFontEditApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTEDIT_H__F04E1F46_D2AB_4254_81D0_34FB06960E07__INCLUDED_)
