#if !defined(AFX_CHIPWINDOW_H__8DAECA04_2060_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_CHIPWINDOW_H__8DAECA04_2060_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChipWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChipWindow frame

class CChipWindow : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChipWindow)
protected:
	CChipWindow();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void Refresh();
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChipWindow)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdateFrameTitle(BOOL addToTitle);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CChipWindow();
	void DrawChipValues(HDC hDC);
	void DrawPinFlag(HDC hDC, int pin, int x, int y);
	void ChipDrawTopBevel(HDC hDC, int x, int y, int w, BOOL in);
	void ChipDrawSideBevels(HDC hDC, int x, int y, int w, int h, BOOL in);
	void ChipDrawBottomBevel(HDC hDC, int x, int y, int w, BOOL in);

	int windowX, windowY, windowW, windowH, windowMin, windowMax;
	BOOL init;

	HFONT chipNamesFont;
	HFONT chipValuesFont;
	int chipX, chipY, chipW, chipH;
	int chipClientW;
	int chipClientH;
	int * pinFlags;

	// Generated message map functions
	//{{AFX_MSG(CChipWindow)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHIPWINDOW_H__8DAECA04_2060_11D6_B181_00E02950FE66__INCLUDED_)
