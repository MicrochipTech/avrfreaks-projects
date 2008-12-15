#if !defined(AFX_SETTINGSDIALOG_H__4CCD5684_EFA7_11D5_B180_00E02950FE66__INCLUDED_)
#define AFX_SETTINGSDIALOG_H__4CCD5684_EFA7_11D5_B180_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDialog.h : header file
//

#include "CBListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog dialog

class CSettingsDialog : public CDialog
{
// Construction
public:
	CSettingsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDialog)
	enum { IDD = IDD_SETTINGS };
	CEdit	m_stackAddress;
	CStatic	m_externalMemStartText;
	CEdit	m_externalMemStart;
	CStatic	m_externalMemSizeText;
	CEdit	m_externalMemSize;
	CButton	m_externalMem;
	CButton	m_sfrsFlags;
	CComboBox	m_cpuPackage;
	CComboBox	m_cpuCPU;
	CComboBox	m_commsBaud;
	CComboBox	m_commsPort;
	CEdit	m_commsCustom;
	CButton	m_commsCheckBox;
	CCBListCtrl	m_list;
	CTabCtrl	m_tab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool sizing;
	int port;
	int baud;
	int chip;
	int package;
	int flags;
	int sfrNameLength;
	int numSFRs;
	int * sfrFlags;
	char * sfrNames;
	int * sfrMarkers;
	int externalMem;
	int externalMemStart;
	int externalMemSize;
	int stackAdr;

	// Generated message map functions
	//{{AFX_MSG(CSettingsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSettingsTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	virtual void OnOK();
	afx_msg void OnSettingsCommscheck();
	afx_msg void OnSelchangeSettingsCommsbaud();
	afx_msg void OnSelchangeSettingsCpucpu();
	afx_msg void OnSelchangeSettingsCpupackage();
	afx_msg void OnSettingsSfrsselect();
	afx_msg void OnSettingsSfrsclear();
	afx_msg void OnSettingsSfrsflags();
	afx_msg void OnDestroy();
	afx_msg void OnSettingsExternalmem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDIALOG_H__4CCD5684_EFA7_11D5_B180_00E02950FE66__INCLUDED_)
