#if !defined(AFX_PROJECTWINDOW_H__A88D2D47_17CC_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_PROJECTWINDOW_H__A88D2D47_17CC_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectWindow.h : header file
//

#include "CBTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectWindow frame

class CProjectWindow : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CProjectWindow)
protected:
	CProjectWindow();           // protected constructor used by dynamic creation

// Attributes
public:
	CCBTreeCtrl	tree;
	int selectedItem;
	CImageList imageList;

// Operations
public:
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectWindow)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CProjectWindow();

	int windowX, windowY, windowW, windowH, windowMin, windowMax;
	bool sizing;
	bool init;
	
	HTREEITEM root;

	// Generated message map functions
	//{{AFX_MSG(CProjectWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnContextVariableAddtowatch();
	afx_msg void OnContextVariableProperties();
	afx_msg void OnContextFileOpen();
	afx_msg void OnContextFileProperties();
	afx_msg void OnContextFunctionShow();
	afx_msg void OnContextFunctionProperties();
	afx_msg void OnContextVariableShow();
	afx_msg void OnContextFileExpand();
	afx_msg void OnContextFunctionExpand();
	afx_msg void OnContextProjectExpand();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTWINDOW_H__A88D2D47_17CC_11D6_B181_00E02950FE66__INCLUDED_)
