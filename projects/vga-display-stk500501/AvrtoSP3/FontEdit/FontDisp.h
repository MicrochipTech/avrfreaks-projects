#if !defined(AFX_FONTDISP_H__E2FFB8D7_CFF6_47CB_9DDB_CA8B251FB8E3__INCLUDED_)
#define AFX_FONTDISP_H__E2FFB8D7_CFF6_47CB_9DDB_CA8B251FB8E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FontDisp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFontDisp window

class CFontDisp : public CStatic
{
// Construction
public:
	CFontDisp();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontDisp)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetChar(char *pC);
	char * pChar;
	int clicked;
	CPen BlackPen;
	CStatic * pDBDisp;
	void SetDebugDisplay(CStatic *pDB);
	virtual ~CFontDisp();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontDisp)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTDISP_H__E2FFB8D7_CFF6_47CB_9DDB_CA8B251FB8E3__INCLUDED_)
