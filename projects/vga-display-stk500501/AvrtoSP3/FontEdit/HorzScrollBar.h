#if !defined(AFX_HORZSCROLLBAR_H__BF0E46E0_EBA4_4A7A_A18D_E1ED7CADE32D__INCLUDED_)
#define AFX_HORZSCROLLBAR_H__BF0E46E0_EBA4_4A7A_A18D_E1ED7CADE32D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HorzScrollBar.h : header file
//

#include "NumEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CHorzScrollBar window

class CHorzScrollBar : public CScrollBar
{
	void (*FormatFunction)(void *p,char *s,int l,int val);
	void (*EditFormatFunction)(void *p,char *s,int l,int val);
	char *pS;
	CStatic *Static_TextDisplay;
	int max;
	int min;
	CStatic * StaticDebugDisplay;
	CEdit * CEdit_DisplayText;
	CNumEdit * CNumEdit_DisplayText;
// Construction
public:
	CHorzScrollBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHorzScrollBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	HWND m_Parrent;
	UINT ActionMessage;
	void SetActionMessage(HWND h,UINT m);
	int Modified;
	void *pParam;
	int pos;
	int lineinc;
	int inc;
	long *LongMasterValue;
	int *IntMasterValue;
	int IsModified(void);
	void ClearModify(void);
	void SetFormatParam(void *p);
	void SetLongMasterValue(long *v);
	void SetIntMasterValue(int *v);
	void Update(void);
	void SetNumEditDisplay(CNumEdit *pNE);
	void SetScrollRange(int minval, int maxval);
	void SetDebugDisplay(CStatic *d);
	void SetEditDisplayFormatFunction(void (__cdecl *pfunc)(void *p,char *s,int l,int val));
	void SetDisplayEdit(CEdit *pE);
	void SetDisplayFormatFunction(void (__cdecl *pfunc)(void *p, char *,int,int));
	void SetDisplayText(CStatic *pCStatic);
	void SetScrollPos(int v);
	virtual ~CHorzScrollBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHorzScrollBar)
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HORZSCROLLBAR_H__BF0E46E0_EBA4_4A7A_A18D_E1ED7CADE32D__INCLUDED_)
