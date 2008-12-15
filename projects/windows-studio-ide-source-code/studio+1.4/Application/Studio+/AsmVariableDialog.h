#if !defined(AFX_ASMVARIABLEDIALOG_H__691F5504_0A54_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_ASMVARIABLEDIALOG_H__691F5504_0A54_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AsmVariableDialog.h : header file
//

#include "Symbol.h"
#include "ScopeBlock.h"


/////////////////////////////////////////////////////////////////////////////
// CAsmVariableDialog dialog

class CAsmVariableDialog : public CDialog
{
// Construction
public:
	CAsmVariableDialog(CWnd* pParent = NULL); // standard constructor
	CAsmVariableDialog(FMSymbol * s, CWnd* pParent = NULL); // Edit constructor
	~CAsmVariableDialog();

	FMSymbol * GetSymbol();

// Dialog Data
	//{{AFX_DATA(CAsmVariableDialog)
	enum { IDD = IDD_ASMVARIABLE };
	CEdit	m_name;
	CEdit	m_address;
	CButton	m_register;
	CButton	m_memory;
	CButton	m_global;
	CEdit	m_scopeFrom;
	CStatic	m_scopeText;
	CEdit	m_scopeTo;
	CButton	m_pointer;
	CButton	m_array;
	CStatic	m_dimensionsText;
	CEdit	m_dimensions;
	CComboBox	m_dataType;
	CStatic	m_size;
	CStatic	m_value;
	CStatic	m_declaration;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAsmVariableDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	FMSymbol * symbol;
	FMSymbol * backupSymbol;
	FMScopeBlock * scope;
	FMScopeBlock * backupScope;

	void SetSymbol();
	bool BuildDataTypeString(char * buffer, int * max);

	// Generated message map functions
	//{{AFX_MSG(CAsmVariableDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAsmvariableGlobal();
	afx_msg void OnAsmvariableArray();
	afx_msg void OnAsmvariableRegister();
	afx_msg void OnAsmvariableMemory();
	afx_msg void OnSelchangeAsmvariableDatatype();
	virtual void OnCancel();
	afx_msg void OnAsmvariablePointer();
	afx_msg void OnChangeAsmvariableAddress();
	afx_msg void OnChangeAsmvariableDimensions();
	afx_msg void OnChangeAsmvariableName();
	afx_msg void OnKillfocusAsmvariableScopefrom();
	afx_msg void OnKillfocusAsmvariableScopeto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASMVARIABLEDIALOG_H__691F5504_0A54_11D6_B181_00E02950FE66__INCLUDED_)
