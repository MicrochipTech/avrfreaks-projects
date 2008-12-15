#if !defined(AFX_COMMSTIMEOUTDIALOG_H__CB9BBC41_35A7_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_COMMSTIMEOUTDIALOG_H__CB9BBC41_35A7_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommsTimeoutDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCommsTimeoutDialog dialog

class CCommsTimeoutDialog : public CDialog
{
// Construction
public:
	CCommsTimeoutDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCommsTimeoutDialog)
	enum { IDD = IDD_COMMSTIMEOUT };
	CEdit	m_period;
	CStatic	m_ms;
	CStatic	m_text;
	CButton	m_enable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommsTimeoutDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCommsTimeoutDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCommstimeoutEnable();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMSTIMEOUTDIALOG_H__CB9BBC41_35A7_11D6_B181_00E02950FE66__INCLUDED_)
