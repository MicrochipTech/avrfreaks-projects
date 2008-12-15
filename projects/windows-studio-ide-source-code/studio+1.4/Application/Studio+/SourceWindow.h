#if !defined(AFX_SOURCEWINDOW_H__AA19D9E4_1BC0_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_SOURCEWINDOW_H__AA19D9E4_1BC0_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SourceWindow.h : header file
//

class FMSourceFile;

/////////////////////////////////////////////////////////////////////////////
// CSourceWindow frame

class CSourceWindow : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSourceWindow)
protected:
	CSourceWindow(); // protected constructor used by dynamic creation
	void DrawSource();

// Attributes
public:

// Operations
public:

	FMSourceFile * GetSourceFile();
	void SetSourceFile(FMSourceFile * sf);
	void ShowLine(int line);
	void Update();
	void RedrawWindow();

// Overrides
	//{{AFX_VIRTUAL(CSourceWindow)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdateFrameTitle(BOOL addToTitle);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSourceWindow();
	void PaintSource(HDC hDC);

	FMSourceFile * sourceFile;

	BOOL sizing;
	BOOL init;

	SIZE charSize;

	HFONT textFont;
	HBRUSH pcBrush;
	HBRUSH bpBarBrush;
	HBRUSH cursorBrush;
	HBRUSH softBPBrush;
	HBRUSH hardBPBrush;
	HBRUSH sourceBrush;
	HBRUSH codeBrush;
	HBITMAP pcBitmap;
	HBITMAP cursorBitmap;
	HBITMAP softBPBitmap;
	HBITMAP hardBPBitmap;
/*	CFont textFont;
	CBrush pcBrush;
	CBrush bpBarBrush;
	CBrush cursorBrush;
	CBrush softBPBrush;
	CBrush hardBPBrush;
	CBrush sourceBrush;
	CBrush codeBrush;
	CBitmap pcBitmap;
	CBitmap cursorBitmap;
	CBitmap softBPBitmap;
	CBitmap hardBPBitmap;
*/
	SCROLLINFO hScrollInfo;
	SCROLLINFO vScrollInfo;
	int hScrollPos;
	int hScrollPage;
	int hScrollMin;
	int hScrollMax;
	int vScrollPos;
	int vScrollPage;
	int vScrollMin;
	int vScrollMax;

	CString comment;
	CString commentStart;
	CString commentEnd;
	char stringChar;
	char charChar;
	char escapeChar;
	char directiveChar;
	CString directives;
	CString keywordChars;
	CString keywords;

	// Generated message map functions
	//{{AFX_MSG(CSourceWindow)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOURCEWINDOW_H__AA19D9E4_1BC0_11D6_B181_00E02950FE66__INCLUDED_)
