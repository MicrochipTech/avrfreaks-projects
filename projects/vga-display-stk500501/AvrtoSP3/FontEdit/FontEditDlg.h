// FontEditDlg.h : header file
//

#if !defined(AFX_FONTEDITDLG_H__B162C156_0F0A_4A41_9B27_2F6E1F5EFCC8__INCLUDED_)
#define AFX_FONTEDITDLG_H__B162C156_0F0A_4A41_9B27_2F6E1F5EFCC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FontDisp.h"
#include "HorzScrollBar.h"
#include "NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CFontEditDlg dialog

class CFontEditDlg : public CDialog
{
// Construction
public:
	int m_NumFontBytes;
	char * FontBuffer;
	CFontEditDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFontEditDlg)
	enum { IDD = IDD_FONTEDIT_DIALOG };
	CNumEdit	m_Edit_Dest;
	CNumEdit	m_Edit_Count;
	CNumEdit	m_Edit_Source;
	CStatic	m_Static_CharSel;
	CStatic	m_Static_Debug;
	CHorzScrollBar	m_ScrollBar_CharSel;
	CFontDisp	m_Static_FontDisp;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	LRESULT OnNextFont(WPARAM wparam, LPARAM lparam);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFontEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonAbout();
	afx_msg void OnButtonMove();
	afx_msg void OnButtonInvert();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonSwap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTEDITDLG_H__B162C156_0F0A_4A41_9B27_2F6E1F5EFCC8__INCLUDED_)
