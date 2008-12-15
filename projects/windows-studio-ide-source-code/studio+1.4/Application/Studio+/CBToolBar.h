#if !defined(AFX_CBTOOLBAR_H__4DAC3C64_23BE_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_CBTOOLBAR_H__4DAC3C64_23BE_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CBToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCBToolBar window

class CCBToolBar : public CToolBar
{
// Construction
public:
	CCBToolBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCBToolBar)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCBToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCBToolBar)
	afx_msg void OnClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CBTOOLBAR_H__4DAC3C64_23BE_11D6_B181_00E02950FE66__INCLUDED_)
