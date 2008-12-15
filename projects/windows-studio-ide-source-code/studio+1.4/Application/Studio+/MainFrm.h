// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__A3A4BC37_DB70_11D5_B180_00E02950FE66__INCLUDED_)
#define AFX_MAINFRM_H__A3A4BC37_DB70_11D5_B180_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CBToolBar.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	int windowX, windowY, windowW, windowH;
	CCBToolBar m_wndProjectBar;
	CCBToolBar m_wndTControlBar;

// Operations
public:
	void SaveWindowSettings();
	void ShowTargetControlBar(int flag);
	int CreateProjectBar();
	int CreateTargetControlBar();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnUpdateFrameTitle(BOOL addToTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();

protected:  // control bar embedded members
	CStatusBar m_wndStatusBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateFamily(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCPU(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePackage(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePC(CCmdUI *pCmdUI);
	afx_msg void OnUpdateComport(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBaud(CCmdUI *pCmdUI);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSinglestep();
	afx_msg void OnUpdateSinglestep(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCstep(CCmdUI* pCmdUI);
	afx_msg void OnCstep();
	afx_msg void OnCstepinto();
	afx_msg void OnUpdateCstepinto(CCmdUI* pCmdUI);
	afx_msg void OnCstepover();
	afx_msg void OnUpdateCstepover(CCmdUI* pCmdUI);
	afx_msg void OnCstepout();
	afx_msg void OnUpdateCstepout(CCmdUI* pCmdUI);
	afx_msg void OnSteptocursor();
	afx_msg void OnUpdateSteptocursor(CCmdUI* pCmdUI);
	afx_msg void OnAnimate();
	afx_msg void OnUpdateAnimate(CCmdUI* pCmdUI);
	afx_msg void OnTosoftbp();
	afx_msg void OnUpdateTosoftbp(CCmdUI* pCmdUI);
	afx_msg void OnComplexbp();
	afx_msg void OnUpdateComplexbp(CCmdUI* pCmdUI);
	afx_msg void OnTohardbp();
	afx_msg void OnUpdateTohardbp(CCmdUI* pCmdUI);
	afx_msg void OnSnapshot();
	afx_msg void OnUpdateSnapshot(CCmdUI* pCmdUI);
	afx_msg void OnPausetarget();
	afx_msg void OnUpdatePausetarget(CCmdUI* pCmdUI);
	afx_msg void OnResettarget();
	afx_msg void OnUpdateResettarget(CCmdUI* pCmdUI);
	afx_msg void OnProjectbaropen();
	afx_msg void OnUpdateProjectbaropen(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarsave();
	afx_msg void OnUpdateProjectbarsave(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarsettings();
	afx_msg void OnUpdateProjectbarsettings(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarRefresh();
	afx_msg void OnUpdateProjectbarRefresh(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarcomms();
	afx_msg void OnUpdateProjectbarcomms(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarviewproject();
	afx_msg void OnUpdateProjectbarviewproject(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarviewregisters();
	afx_msg void OnUpdateProjectbarviewregisters(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarviewsfrs();
	afx_msg void OnUpdateProjectbarviewsfrs(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarviewsram();
	afx_msg void OnUpdateProjectbarviewsram(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarvieweeprom();
	afx_msg void OnUpdateProjectbarvieweeprom(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarviewwatch();
	afx_msg void OnUpdateProjectbarviewwatch(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarviewstack();
	afx_msg void OnUpdateProjectbarviewstack(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarviewchip();
	afx_msg void OnUpdateProjectbarviewchip(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarSyntaxhi();
	afx_msg void OnUpdateProjectbarSyntaxhi(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarTrackpc();
	afx_msg void OnUpdateProjectbarTrackpc(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarAssembler();
	afx_msg void OnUpdateProjectbarAssembler(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarAddresses();
	afx_msg void OnUpdateProjectbarAddresses(CCmdUI* pCmdUI);
	afx_msg void OnProjectbarMachinecode();
	afx_msg void OnUpdateProjectbarMachinecode(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__A3A4BC37_DB70_11D5_B180_00E02950FE66__INCLUDED_)
