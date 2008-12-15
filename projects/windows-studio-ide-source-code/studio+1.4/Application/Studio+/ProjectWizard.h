#if !defined(AFX_PROJECTWIZARD_H__F68F7601_21E6_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_PROJECTWIZARD_H__F68F7601_21E6_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectWizard.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectWizard dialog

class CProjectWizard : public CDialog
{
// Construction
public:
	CProjectWizard(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectWizard)
	enum { IDD = IDD_PROJECTWIZARD };
	CStatic	m_ideLogo;
	CStatic	m_familyLogo;
	CComboBox	m_family;
	CComboBox	m_ide;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectWizard)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HBITMAP ideLogo;
	HBITMAP familyLogo;

	int currentIDE;
	int currentFamily;

	// Generated message map functions
	//{{AFX_MSG(CProjectWizard)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeProjectwizardIde();
	afx_msg void OnSelchangeProjectwizardFamily();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTWIZARD_H__F68F7601_21E6_11D6_B181_00E02950FE66__INCLUDED_)
