#if !defined(AFX_LICENSEDIALOG_H__DD102C00_B4E6_11D5_A0B4_E6B4C08D770D__INCLUDED_)
#define AFX_LICENSEDIALOG_H__DD102C00_B4E6_11D5_A0B4_E6B4C08D770D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LicenseDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLicenseDialog dialog

class CLicenseDialog : public CDialog
{
// Construction
public:
	CLicenseDialog(CWnd* pParent = NULL); // Standard constructor

// Dialog Data
	//{{AFX_DATA(CLicenseDialog)
	enum { IDD = IDD_LICENSE };
	CAnimateCtrl	m_animation;
	CEdit	m_code;
	CEdit	m_unlockCode;
	CListBox	m_CPUs;
	CListBox	m_IDEs;
	CButton	m_Basic;
	CButton	m_BasicWant;
	CButton	m_Prof;
	CButton	m_ProfWant;
	CButton	m_Ent;
	CButton	m_EntWant;
	CButton	m_ICE;
	CButton	m_ICEWant;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateOptions();
	void UpdateCPUs();
	void UpdateIDEs();

	CString customerCode;
	int newOptions;
	int newCPUs;
	int newIDEs;

	// Generated message map functions
	//{{AFX_MSG(CLicenseDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnLicenseLicense();
	afx_msg void OnClose();
	afx_msg void OnLicenseFromdisk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLicenseEmail();
	afx_msg void OnLicenseBasicwant();
	afx_msg void OnLicenseProfwant();
	afx_msg void OnLicenseEntwant();
	afx_msg void OnLicenseICEwant();
	afx_msg void OnSelchangeLicenseCpus();
	afx_msg void OnSelchangeLicenseIdes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSEDIALOG_H__DD102C00_B4E6_11D5_A0B4_E6B4C08D770D__INCLUDED_)
