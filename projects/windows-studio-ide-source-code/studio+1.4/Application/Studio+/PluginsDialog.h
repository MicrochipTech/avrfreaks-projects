#if !defined(AFX_PLUGINSDIALOG_H__5071C661_32A3_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_PLUGINSDIALOG_H__5071C661_32A3_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PluginsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPluginsDialog dialog

class CPluginsDialog : public CDialog
{
// Construction
public:
	CPluginsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPluginsDialog)
	enum { IDD = IDD_PLUGINS };
	CListBox	m_familys;
	CListBox	m_ides;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluginsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPluginsDialog)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINSDIALOG_H__5071C661_32A3_11D6_B181_00E02950FE66__INCLUDED_)
