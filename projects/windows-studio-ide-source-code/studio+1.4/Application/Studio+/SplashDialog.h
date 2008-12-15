#if !defined(AFX_SPLASHDIALOG_H__0055D840_FD4E_11D5_B180_00E02950FE66__INCLUDED_)
#define AFX_SPLASHDIALOG_H__0055D840_FD4E_11D5_B180_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplashDialog dialog

class CSplashDialog : public CDialog
{
// Construction
public:
	CSplashDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSplashDialog)
	enum { IDD = IDD_SPLASH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSplashDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHDIALOG_H__0055D840_FD4E_11D5_B180_00E02950FE66__INCLUDED_)
