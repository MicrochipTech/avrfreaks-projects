#if !defined(AFX_WATCHWINDOW_H__A88D2D46_17CC_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_WATCHWINDOW_H__A88D2D46_17CC_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WatchWindow.h : header file
//

#include "CBListCtrl.h"

class FMSymbol;

/////////////////////////////////////////////////////////////////////////////
// CWatchListCtrl List Control

class CWatchListCtrl : public CCBListCtrl
{
	int GetSelectedItem();
protected:
	//{{AFX_MSG(CCBListCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void ShowPopupMenu(CPoint& point, int contextMenu);

	int selectedItem;
	int oldSel;
};


/////////////////////////////////////////////////////////////////////////////
// CWatchWindow frame

class CWatchWindow : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CWatchWindow)
protected:
	CWatchWindow();           // protected constructor used by dynamic creation

// Attributes
public:
	CWatchListCtrl m_list;

// Operations
public:
	void AddSymbol(FMSymbol * symbol);
	void Update();
	void SelectSymbol(FMSymbol * symbol);

// Overrides
	//{{AFX_VIRTUAL(CWatchWindow)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWatchWindow();

	int windowX, windowY, windowW, windowH, windowMin, windowMax;
	bool sizing;
	bool init;
	int focusedItem;

	// Generated message map functions
	//{{AFX_MSG(CWatchWindow)
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemchangedWatchList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkWatchList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextWatchSrcProperties();
	afx_msg void OnContextWatchDelete();
	afx_msg void OnContextWatchAsmEdit();
	afx_msg void OnContextWatchNewAdd();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHWINDOW_H__A88D2D46_17CC_11D6_B181_00E02950FE66__INCLUDED_)
