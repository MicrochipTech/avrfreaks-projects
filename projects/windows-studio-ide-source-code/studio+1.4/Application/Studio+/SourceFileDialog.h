#if !defined(AFX_SOURCEFILEDIALOG_H__A88D2D4A_17CC_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_SOURCEFILEDIALOG_H__A88D2D4A_17CC_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SourceFileDialog.h : header file
//

// Forward declaration.
class FMSourceFile;

/////////////////////////////////////////////////////////////////////////////
// CSourceFileDialog dialog

class CSourceFileDialog : public CDialog
{
// Construction
public:
	CSourceFileDialog(CWnd* pParent = NULL, FMSourceFile * s = NULL);

// Dialog Data
	//{{AFX_DATA(CSourceFileDialog)
	enum { IDD = IDD_SOURCEFILE };
	CStatic	m_lines;
	CStatic	m_path;
	CStatic	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSourceFileDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	FMSourceFile * sourceFile;

	// Generated message map functions
	//{{AFX_MSG(CSourceFileDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOURCEFILEDIALOG_H__A88D2D4A_17CC_11D6_B181_00E02950FE66__INCLUDED_)
