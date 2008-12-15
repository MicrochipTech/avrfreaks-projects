#if !defined(AFX_VARIABLEDIALOG_H__A88D2D48_17CC_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_VARIABLEDIALOG_H__A88D2D48_17CC_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VariableDialog.h : header file
//

// Forward declaration.
class FMSymbol;

/////////////////////////////////////////////////////////////////////////////
// CVariableDialog dialog

class CVariableDialog : public CDialog
{
// Construction
public:
	CVariableDialog(CWnd* pParent = NULL, FMSymbol * s = NULL);

// Dialog Data
	//{{AFX_DATA(CVariableDialog)
	enum { IDD = IDD_VARIABLE };
	CStatic	m_line;
	CStatic	m_functionText;
	CStatic	m_function;
	CStatic	m_dimensionsText;
	CStatic	m_dimensions;
	CStatic	m_declaration;
	CStatic	m_dataType;
	CButton	m_array;
	CButton	m_pointer;
	CStatic	m_scopeFrom;
	CStatic	m_scopeText;
	CStatic	m_scopeTo;
	CStatic	m_size;
	CStatic	m_value;
	CStatic	m_source;
	CButton	m_global;
	CStatic	m_address;
	CStatic	m_name;
	CButton	m_register;
	CButton	m_memory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVariableDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	FMSymbol * symbol;

	// Generated message map functions
	//{{AFX_MSG(CVariableDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VARIABLEDIALOG_H__A88D2D48_17CC_11D6_B181_00E02950FE66__INCLUDED_)
