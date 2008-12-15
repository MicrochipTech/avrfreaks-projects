// Studio.h

#if !defined(AFX_STUDIO_H__A3A4BC33_DB70_11D5_B180_00E02950FE66__INCLUDED_)
#define AFX_STUDIO_H__A3A4BC33_DB70_11D5_B180_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// CStudioApp:
// See Studio.cpp for the implementation of this class
//

class CStudioApp : public CWinApp
{
public:
	CStudioApp();
	void SetTitle();
	BOOL GetTitle(char * title, int * max);
	void SetSourceMenu();
	void SetProjectMenu();

	int OpenIdeLibrary(char * name);
	int OpenFamilyLibrary(char * name);

	int OpenSourceFile(int index);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStudioApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:
//	HMENU m_hMDIMenu;
//	HACCEL m_hMDIAccel;

private:

	void UpdateSFRs(unsigned char * values, int offset, int count);
	void UpdateSram(unsigned char * values, int offset, int count);
	void UpdateEeprom(unsigned char * values, int offset, int count);

	void GetInfoBlock();
	void CalculateExpected();
	void SaveWindowPreferences();

	DWORD commsTimer;
	int sramFirst, sramLast;
	int eepromFirst, eepromLast;


public:
	//{{AFX_MSG(CStudioApp)
	afx_msg void OnFileNew();
	afx_msg void OnPrefsSourceSyntaxhi();
	afx_msg void OnUpdatePrefsSourceSyntaxhi(CCmdUI* pCmdUI);
	afx_msg void OnPrefsSourceTrackpc();
	afx_msg void OnUpdatePrefsSourceTrackpc(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenproject();
	afx_msg void OnUpdateFileOpenproject(CCmdUI* pCmdUI);
	afx_msg void OnFileCloseproject();
	afx_msg void OnUpdateFileCloseproject(CCmdUI* pCmdUI);
	afx_msg void OnViewProject();
	afx_msg void OnUpdateViewProject(CCmdUI* pCmdUI);
	afx_msg void OnViewRegisters();
	afx_msg void OnUpdateViewRegisters(CCmdUI* pCmdUI);
	afx_msg void OnViewMemorySram();
	afx_msg void OnUpdateViewMemorySram(CCmdUI* pCmdUI);
	afx_msg void OnViewMemoryEeprom();
	afx_msg void OnUpdateViewMemoryEeprom(CCmdUI* pCmdUI);
	afx_msg void OnProjectSettings();
	afx_msg void OnUpdateProjectSettings(CCmdUI* pCmdUI);
	afx_msg void OnWindowCascade();
	afx_msg void OnUpdateWindowCascade(CCmdUI* pCmdUI);
	afx_msg void OnProjectRefresh();
	afx_msg void OnUpdateProjectRefresh(CCmdUI* pCmdUI);
	afx_msg void OnPrefsSourceCode();
	afx_msg void OnUpdatePrefsSourceCode(CCmdUI* pCmdUI);
	afx_msg void OnViewWatch();
	afx_msg void OnUpdateViewWatch(CCmdUI* pCmdUI);
	afx_msg void OnViewMemorySfrs();
	afx_msg void OnUpdateViewMemorySfrs(CCmdUI* pCmdUI);
	afx_msg void OnViewStack();
	afx_msg void OnUpdateViewStack(CCmdUI* pCmdUI);
	afx_msg void OnViewChip();
	afx_msg void OnUpdateViewChip(CCmdUI* pCmdUI);
	afx_msg void OnHelpAbout();
	afx_msg void OnUpdateHelpAbout(CCmdUI* pCmdUI);
	afx_msg void OnHelpRegister();
	afx_msg void OnUpdateHelpRegister(CCmdUI* pCmdUI);
	afx_msg void OnHelpExport();
	afx_msg void OnUpdateHelpExport(CCmdUI* pCmdUI);
	afx_msg void OnPrefsSourceAsm();
	afx_msg void OnUpdatePrefsSourceAsm(CCmdUI* pCmdUI);
	afx_msg void OnPrefsSourceAddress();
	afx_msg void OnUpdatePrefsSourceAddress(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveas();
	afx_msg void OnUpdateFileSaveas(CCmdUI* pCmdUI);
	afx_msg void OnFileNewproject();
	afx_msg void OnUpdateFileNewproject(CCmdUI* pCmdUI);
	afx_msg void OnHelpHelptopics();
	afx_msg void OnUpdateHelpHelptopics(CCmdUI* pCmdUI);
	afx_msg void OnViewMaintoolbar();
	afx_msg void OnUpdateViewMaintoolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewTargetcontrolbar();
	afx_msg void OnUpdateViewTargetcontrolbar(CCmdUI* pCmdUI);
	afx_msg void OnHelpPlugins();
	afx_msg void OnUpdateHelpPlugins(CCmdUI* pCmdUI);
	afx_msg void OnPrefsCommstimeout();
	afx_msg void OnUpdatePrefsCommstimeout(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STUDIO_H__A3A4BC33_DB70_11D5_B180_00E02950FE66__INCLUDED_)
