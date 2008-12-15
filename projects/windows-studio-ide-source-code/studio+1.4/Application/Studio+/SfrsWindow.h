#if !defined(AFX_SFRSWINDOW_H__A88D2D44_17CC_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_SFRSWINDOW_H__A88D2D44_17CC_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Sfrs.h : header file
//

#include "CBListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSfrs window

class CSfrsWindow : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSfrsWindow)
// Construction
public:
	CSfrsWindow();

// Attributes
public:
	CCBListCtrl m_list;

// Operations
public:
	void Update();
	void Refresh();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSfrsWindow)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSfrsWindow();

protected:
	int windowX, windowY, windowW, windowH, windowMin, windowMax;
	bool sizing;
	bool init;
	int focusedItem;

	// Generated message map functions
	//{{AFX_MSG(CSfrsWindow)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SFRSWINDOW_H__A88D2D44_17CC_11D6_B181_00E02950FE66__INCLUDED_)
