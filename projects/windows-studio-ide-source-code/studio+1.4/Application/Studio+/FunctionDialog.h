#if !defined(AFX_FUNCTIONDIALOG_H__A88D2D49_17CC_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_FUNCTIONDIALOG_H__A88D2D49_17CC_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FunctionDialog.h : header file
//

// Forward declaration.
class FMFunction;

/////////////////////////////////////////////////////////////////////////////
// CFunctionDialog dialog

class CFunctionDialog : public CDialog
{
// Construction
public:
	CFunctionDialog(CWnd* pParent = NULL, FMFunction * f = NULL);

// Dialog Data
	//{{AFX_DATA(CFunctionDialog)
	enum { IDD = IDD_FUNCTION };
	CStatic	m_to;
	CStatic	m_from;
	CStatic	m_line;
	CStatic	m_file;
	CStatic	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFunctionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	FMFunction * function;
	// Generated message map functions
	//{{AFX_MSG(CFunctionDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FUNCTIONDIALOG_H__A88D2D49_17CC_11D6_B181_00E02950FE66__INCLUDED_)
