// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include <afxadv.h>						// For CDockState
#include "Studio.h"

#include "MainFrm.h"
#include "Registers.h"					// Registers window handle
#include "Memory.h"						// Sram & Eeprom window handles
#include "Stack.h"						// Stack window handle
#include "Comms.h"						// Comms functions
//#include "Resource.h"					// ToolBar resource IDs

#include "Globals.h"					// Global Variables


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FAMILY, OnUpdateFamily)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CPU, OnUpdateCPU)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_PACKAGE, OnUpdatePackage)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_PC, OnUpdatePC)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COMPORT, OnUpdateComport)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_BAUD, OnUpdateBaud)
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_COMMAND(ID_SINGLESTEP, OnSinglestep)
	ON_UPDATE_COMMAND_UI(ID_SINGLESTEP, OnUpdateSinglestep)
	ON_UPDATE_COMMAND_UI(ID_CSTEP, OnUpdateCstep)
	ON_COMMAND(ID_CSTEP, OnCstep)
	ON_COMMAND(ID_CSTEPINTO, OnCstepinto)
	ON_UPDATE_COMMAND_UI(ID_CSTEPINTO, OnUpdateCstepinto)
	ON_COMMAND(ID_CSTEPOVER, OnCstepover)
	ON_UPDATE_COMMAND_UI(ID_CSTEPOVER, OnUpdateCstepover)
	ON_COMMAND(ID_CSTEPOUT, OnCstepout)
	ON_UPDATE_COMMAND_UI(ID_CSTEPOUT, OnUpdateCstepout)
	ON_COMMAND(ID_STEPTOCURSOR, OnSteptocursor)
	ON_UPDATE_COMMAND_UI(ID_STEPTOCURSOR, OnUpdateSteptocursor)
	ON_COMMAND(ID_ANIMATE, OnAnimate)
	ON_UPDATE_COMMAND_UI(ID_ANIMATE, OnUpdateAnimate)
	ON_COMMAND(ID_TOSOFTBP, OnTosoftbp)
	ON_UPDATE_COMMAND_UI(ID_TOSOFTBP, OnUpdateTosoftbp)
	ON_COMMAND(ID_COMPLEXBP, OnComplexbp)
	ON_UPDATE_COMMAND_UI(ID_COMPLEXBP, OnUpdateComplexbp)
	ON_COMMAND(ID_TOHARDBP, OnTohardbp)
	ON_UPDATE_COMMAND_UI(ID_TOHARDBP, OnUpdateTohardbp)
	ON_COMMAND(ID_SNAPSHOT, OnSnapshot)
	ON_UPDATE_COMMAND_UI(ID_SNAPSHOT, OnUpdateSnapshot)
	ON_COMMAND(ID_PAUSETARGET, OnPausetarget)
	ON_UPDATE_COMMAND_UI(ID_PAUSETARGET, OnUpdatePausetarget)
	ON_COMMAND(ID_RESETTARGET, OnResettarget)
	ON_UPDATE_COMMAND_UI(ID_RESETTARGET, OnUpdateResettarget)
	ON_BN_CLICKED(IDC_PROJECTBAR_OPEN, OnProjectbaropen)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_OPEN, OnUpdateProjectbaropen)
	ON_BN_CLICKED(IDC_PROJECTBAR_SAVE, OnProjectbarsave)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_SAVE, OnUpdateProjectbarsave)
	ON_BN_CLICKED(IDC_PROJECTBAR_SETTINGS, OnProjectbarsettings)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_SETTINGS, OnUpdateProjectbarsettings)
	ON_COMMAND(IDC_PROJECTBAR_REFRESH, OnProjectbarRefresh)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_REFRESH, OnUpdateProjectbarRefresh)
	ON_BN_CLICKED(IDC_PROJECTBAR_COMMS, OnProjectbarcomms)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_COMMS, OnUpdateProjectbarcomms)
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWPROJECT, OnProjectbarviewproject)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWPROJECT, OnUpdateProjectbarviewproject)
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWREGISTERS, OnProjectbarviewregisters)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWREGISTERS, OnUpdateProjectbarviewregisters)
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWSFRS, OnProjectbarviewsfrs)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWSFRS, OnUpdateProjectbarviewsfrs)
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWSRAM, OnProjectbarviewsram)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWSRAM, OnUpdateProjectbarviewsram)
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWEEPROM, OnProjectbarvieweeprom)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWEEPROM, OnUpdateProjectbarvieweeprom)
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWWATCH, OnProjectbarviewwatch)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWWATCH, OnUpdateProjectbarviewwatch)
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWSTACK, OnProjectbarviewstack)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWSTACK, OnUpdateProjectbarviewstack)
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWCHIP, OnProjectbarviewchip)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWCHIP, OnUpdateProjectbarviewchip)
	ON_COMMAND(IDC_PROJECTBAR_SYNTAXHI, OnProjectbarSyntaxhi)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_SYNTAXHI, OnUpdateProjectbarSyntaxhi)
	ON_COMMAND(IDC_PROJECTBAR_TRACKPC, OnProjectbarTrackpc)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_TRACKPC, OnUpdateProjectbarTrackpc)
	ON_COMMAND(IDC_PROJECTBAR_ASSEMBLER, OnProjectbarAssembler)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_ASSEMBLER, OnUpdateProjectbarAssembler)
	ON_COMMAND(IDC_PROJECTBAR_ADDRESSES, OnProjectbarAddresses)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_ADDRESSES, OnUpdateProjectbarAddresses)
	ON_COMMAND(IDC_PROJECTBAR_MACHINECODE, OnProjectbarMachinecode)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_MACHINECODE, OnUpdateProjectbarMachinecode)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PROJECTBAR_VIEWSTACK, OnProjectbarviewwatch)
	ON_UPDATE_COMMAND_UI(IDC_PROJECTBAR_VIEWSTACK, OnUpdateProjectbarviewwatch)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,						// status line indicator
	ID_INDICATOR_FAMILY,
	ID_INDICATOR_CPU,
	ID_INDICATOR_PACKAGE,
	ID_INDICATOR_PC,
	ID_INDICATOR_COMPORT,
	ID_INDICATOR_BAUD
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction


CMainFrame::CMainFrame()
{
//	pcIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PC));
//	comIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COM));
//	baudIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BAUD));
}


CMainFrame::~CMainFrame()
{
//	::DeleteObject(pcIcon);
//	::DeleteObject(comIcon);
//	::DeleteObject(baudIcon);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifdef MFMDEBUG
debug<<"CMainFrame::OnCreate():"<<endl;
#endif

	lpCreateStruct->style |= WS_HSCROLL | WS_VSCROLL;
	
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);

	if (!CreateProjectBar())
		return -1;
	if (!CreateTargetControlBar())
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators, 7))
	{
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_FAMILY, SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_CPU, SBPS_NORMAL, 70);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_PACKAGE, SBPS_NORMAL, 40);
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_PC, SBPS_NORMAL, 150);
	m_wndStatusBar.SetPaneInfo(5, ID_INDICATOR_COMPORT, SBPS_NORMAL, 50);
	m_wndStatusBar.SetPaneInfo(6, ID_INDICATOR_BAUD, SBPS_NORMAL, 50);

	// ToolBars and StatusBar have been created - restore them
	CString key = VERSIONKEY;
	key += "\\ToolBars";
	LoadBarState(key);

	mainWindowHandle = m_hWnd;
	clientAreaHandle = m_hWndMDIClient;

	return 0;
}


int CMainFrame::CreateProjectBar()
{
	CRect rect;
	rect.left = 1;
	rect.right = 1;
	rect.top = 1;
	rect.bottom = 1;
	if (!m_wndProjectBar.CreateEx(this,
		TBSTYLE_WRAPABLE | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT,
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS |
		CBRS_FLYBY | CBRS_SIZE_DYNAMIC, &rect, IDT_PROJECTBAR) ||
		!m_wndProjectBar.LoadToolBar(IDT_PROJECTBAR))
	{
		return FALSE;					// fail to create
	}
	m_wndProjectBar.SetWindowText("Options & Controls");
	m_wndProjectBar.SetBarStyle(m_wndProjectBar.GetBarStyle()|
		CBRS_BORDER_TOP|CBRS_BORDER_BOTTOM|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT);
	m_wndProjectBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndProjectBar, AFX_IDW_DOCKBAR_TOP);
	return TRUE;
}


int CMainFrame::CreateTargetControlBar()
{
	CRect rect;
	rect.left = 1;
	rect.right = 1;
	rect.top = 1;
	rect.bottom = 1;
	if (!m_wndTControlBar.CreateEx(this,
		TBSTYLE_WRAPABLE | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT,
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS |
		CBRS_FLYBY | CBRS_SIZE_DYNAMIC, &rect, IDT_TARGETCONTROLBAR) ||
		!m_wndTControlBar.LoadToolBar(IDT_TARGETCONTROLBAR))
	{
		return FALSE;					// fail to create
	}
	m_wndTControlBar.SetWindowText("Target Control");
	m_wndTControlBar.SetBarStyle(m_wndTControlBar.GetBarStyle()|
		CBRS_BORDER_TOP|CBRS_BORDER_BOTTOM|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT);
	m_wndTControlBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndTControlBar, AFX_IDW_DOCKBAR_TOP);
	return TRUE;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


void CMainFrame::SaveWindowSettings()
{
#ifdef MFMDEBUG
debug<<"CMainFrame::SaveWindowSettings():"<<endl;
#endif

	HKEY localmachine;
	HKEY software;
	HKEY company;
	HKEY product;
	HKEY version;
	unsigned long length;
	long regError;

	// Save the Window's X,Y, Width & Height in the Registry
	if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ,
		&localmachine) == ERROR_SUCCESS)
	{
		if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0,
			KEY_READ, &software) == ERROR_SUCCESS)
		{
			if (regError = RegOpenKeyEx(software, COMPANYKEY, 0, KEY_READ,
				&company) == ERROR_SUCCESS)
			{
				if (regError = RegOpenKeyEx(company, PRODUCTKEY, 0,
					KEY_READ, &product) == ERROR_SUCCESS)
				{
					if (regError = RegOpenKeyEx(product, VERSIONKEY, 0,
						KEY_READ|KEY_WRITE, &version) == ERROR_SUCCESS)
					{
						length = 4;
						int pos = (windowX | (windowY<<16));
						int size = (windowW | (windowH<<16));
						RegSetValueEx(version, "Window Position", NULL,
							REG_DWORD, (CONST BYTE *)&pos, length);
						RegSetValueEx(version, "Window Size", NULL,
							REG_DWORD, (CONST BYTE *)&size, length);
						RegCloseKey(version);
					}
					RegCloseKey(product);
				}
				RegCloseKey(company);
			}
			RegCloseKey(software);
		}
		RegCloseKey(localmachine);
	}
}


//void CMainFrame::ShowTargetControlBar(int flag)
//{
//	if (flag == SW_SHOWNORMAL || flag == SW_SHOW)
//	{
//		m_wndTControlBar.GetToolBarCtrl().ShowWindow(flag);
//		DockControlBar(&m_wndTControlBar, IDT_TCONTROLBAR);
//	}
//	else
//	{
////		FloatControlBar(&m_wndTControlBar, IDT_TCONTROLBAR);
//		m_wndTControlBar.GetToolBarCtrl().ShowWindow(flag);
//	}
//}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnUpdateFamily(CCmdUI *pCmdUI)
{
	char buffer[100];
	int max = 100;

	if (project && project->IsOpen())
	{
		pCmdUI->Enable();
		CLGetFamilyName(buffer, &max);
		pCmdUI->SetText(buffer);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnUpdateCPU(CCmdUI *pCmdUI)
{
	char buffer[100];

	if (project && project->IsOpen())
	{
		pCmdUI->Enable();
		project->GetChipName(project->GetChip(), buffer, 100);
		pCmdUI->SetText(buffer);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnUpdatePackage(CCmdUI *pCmdUI)
{
	char buffer[100];

	if (project && project->IsOpen())
	{
		pCmdUI->Enable();
		project->GetPackageName(project->GetPackage(), buffer, 100);
		pCmdUI->SetText(buffer);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnUpdatePC(CCmdUI *pCmdUI)
{
	if (project && project->IsOpen() && pcFile && pcLine != -1)
	{
		pCmdUI->Enable();
//		if (m_wndStatusBar.GetStatusBarCtrl().IsSimple())
//			m_wndStatusBar.GetStatusBarCtrl().SetSimple(FALSE);
//		m_wndStatusBar.GetStatusBarCtrl().SetIcon(1, pcIcon);
		CString str;
		str.Format("%s, Line %d", pcFile->GetFileName(), pcLine);
		pCmdUI->SetText(str);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnUpdateComport(CCmdUI *pCmdUI)
{
	if (project && project->IsOpen())
	{
		pCmdUI->Enable();
		CString str;
		str.Format("COM%d", project->selectedComPort);
		pCmdUI->SetText(str);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnUpdateBaud(CCmdUI *pCmdUI)
{
	if (project && project->IsOpen())
	{
		pCmdUI->Enable();
		CString str = "Closed";
		if (ComPortOpen())
			str.Format("%d", project->selectedBaud);
		pCmdUI->SetText(str);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnMove(int x, int y)
{
	CMDIFrameWnd::OnMove(x, y);
	CRect rect;
	GetWindowRect(&rect);
	windowX = rect.left;
	windowY = rect.top;
	SaveWindowSettings();
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);
	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXIMIZED ||
		nType == SIZE_MAXSHOW || nType == SIZE_MINIMIZED)
		return;
	CRect rect;
	GetWindowRect(&rect);
	windowW = rect.Width();
	windowH = rect.Height();
	SaveWindowSettings();
}


void CMainFrame::OnSinglestep()
{
	cStepFile = pcFile;
	cStepLine = pcLine;
	debugMode = DM_SINGLESTEP;
	controlChar = 'S';
	debugRunning = true;
}


void CMainFrame::OnUpdateSinglestep(CCmdUI* pCmdUI)
{
	if (ComPortOpen() && project && project->IsOpen())
		pCmdUI->Enable(!debugRunning && !gettingData);
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnCstep()
{
	cStepFile = pcFile;
	cStepLine = pcLine;
	debugMode = DM_CSTEP;
	controlChar = 'S';
	debugRunning = true;
}


void CMainFrame::OnUpdateCstep(CCmdUI* pCmdUI)
{
	// Project must be open and PC must be in a C file
	if (ComPortOpen() && project && project->IsOpen() &&
		pcFile && pcFile->IsC())
	{
		pCmdUI->Enable(!debugRunning && !gettingData);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnCstepinto()
{
}


void CMainFrame::OnUpdateCstepinto(CCmdUI* pCmdUI)
{
//	if (ComPortOpen() && project && project->IsOpen() &&
//		pcFile && pcFile->IsC())
//	{
//		pCmdUI->Enable(!debugRunning && !gettingData);
//	}
//	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnCstepover()
{

}


void CMainFrame::OnUpdateCstepover(CCmdUI* pCmdUI)
{
//	if (ComPortOpen() && project && project->IsOpen() &&
//		pcFile && pcFile->IsC())
//	{
//		pCmdUI->Enable(!debugRunning && !gettingData);
//	}
//	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnCstepout()
{

}


void CMainFrame::OnUpdateCstepout(CCmdUI* pCmdUI)
{
//	if (ComPortOpen() && project && project->IsOpen() &&
//		pcFile && pcFile->IsC())
//	{
//		pCmdUI->Enable(!debugRunning && !gettingData);
//	}
//	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnSteptocursor()
{
	debugMode = DM_STEPTOCURSOR;
	controlChar = 'S';
	debugRunning = true;
}


void CMainFrame::OnUpdateSteptocursor(CCmdUI* pCmdUI)
{
	if (ComPortOpen() && project && project->IsOpen() &&
		cursorFile != NULL && cursorLine != -1 &&
		cursorFile->GetLine(cursorLine)->code)
	{
		pCmdUI->Enable(!debugRunning && !gettingData);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnAnimate()
{
	controlChar = 'S';
	debugMode = DM_ANIMATE;
	debugRunning = true;
}


void CMainFrame::OnUpdateAnimate(CCmdUI* pCmdUI)
{
	if (ComPortOpen() && project && project->IsOpen())
		pCmdUI->Enable(!debugRunning && !gettingData);
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnTosoftbp()
{
	controlChar = 'S';
	debugMode = DM_TOSOFTBP;
	debugRunning = true;
}


void CMainFrame::OnUpdateTosoftbp(CCmdUI* pCmdUI)
{
	if (ComPortOpen() && project && project->IsOpen() &&
		project->GetNumSoftBPs())
	{
		pCmdUI->Enable(!debugRunning && !gettingData);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnComplexbp()
{
}


void CMainFrame::OnUpdateComplexbp(CCmdUI* pCmdUI)
{
//	if (ComPortOpen() && project && project->IsOpen())
//		pCmdUI->Enable(!debugRunning && !gettingData);
//	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnTohardbp()
{
	debugMode = DM_TOHARDBP;
	controlChar = 'C';
	debugRunning = true;
}


void CMainFrame::OnUpdateTohardbp(CCmdUI* pCmdUI)
{
	if (ComPortOpen() && project && project->IsOpen() &&
		project->GetNumHardBPs())
	{
		pCmdUI->Enable(!debugRunning && !gettingData);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnSnapshot()
{
	controlChar = 'C';
	debugMode = DM_SNAPSHOT;
	debugRunning = true;
}


void CMainFrame::OnUpdateSnapshot(CCmdUI* pCmdUI)
{
	if (ComPortOpen() && project && project->IsOpen())
		pCmdUI->Enable(!debugRunning && !gettingData);
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnPausetarget()
{
	debugMode = DM_STOP;
}


void CMainFrame::OnUpdatePausetarget(CCmdUI* pCmdUI)
{
	if (ComPortOpen() && project && project->IsOpen())
		pCmdUI->Enable(debugRunning);
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnResettarget()
{
	ClearDTR();
	Sleep(10);
	SetDTR();

	debugRunning = true;
	needInfoBlock = true;
	gettingData = false;
	waitingForD = true;
	endCondition = false;
	updateRegisters = false;
	updateSfrs = false;
	updateSram = false;
	updateEeprom = false;
	updateStack = false;
	firstWatchSymbol = 0;
	lastWatchSymbol = 0;
	debugMode = DM_STOP;
	askedFor = SENDPC;
}


void CMainFrame::OnUpdateResettarget(CCmdUI* pCmdUI)
{
	if (ComPortOpen() && project && project->IsOpen())
		pCmdUI->Enable(TRUE); //!debugRunning && !gettingData);
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnProjectbaropen()
{
	theApp.OnFileOpenproject();
}


void CMainFrame::OnUpdateProjectbaropen(CCmdUI* pCmdUI)
{
	theApp.OnUpdateFileOpenproject(pCmdUI);
}


void CMainFrame::OnProjectbarsave()
{
	theApp.OnFileSave();
}


void CMainFrame::OnUpdateProjectbarsave(CCmdUI* pCmdUI)
{
	theApp.OnUpdateFileSave(pCmdUI);
}


void CMainFrame::OnProjectbarsettings()
{
	theApp.OnProjectSettings();
}


void CMainFrame::OnUpdateProjectbarsettings(CCmdUI* pCmdUI)
{
	theApp.OnUpdateProjectSettings(pCmdUI);
}


void CMainFrame::OnProjectbarRefresh() 
{
	theApp.OnProjectRefresh();
}


void CMainFrame::OnUpdateProjectbarRefresh(CCmdUI* pCmdUI) 
{
	if (project && project->IsOpen())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}


void CMainFrame::OnProjectbarcomms()
{
	if (ComPortOpen())
		CloseComPort();
	else
	{
		OpenComPort();
		if (project->firstContact)
		{
			project->firstContact = FALSE;
			OnResettarget();
		}
	}
}


void CMainFrame::OnUpdateProjectbarcomms(CCmdUI* pCmdUI)
{
	if (project && project->IsOpen() && !debugRunning)
	{
		pCmdUI->Enable(TRUE);
		if (ComPortOpen())
			pCmdUI->SetCheck(1);
//			m_wndProjectBar.SetButtonInfo(2, IDC_PROJECTBAR_COMMS,
//				TBBS_BUTTON, 1);
		else
			pCmdUI->SetCheck(0);
//			m_wndProjectBar.SetButtonInfo(2, IDC_PROJECTBAR_COMMS,
//				TBBS_BUTTON, 2);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
	}
}


void CMainFrame::OnProjectbarviewproject()
{
	if (projectWindowOpen)
		::SendMessage(projectWindow->m_hWnd, WM_CLOSE, 0, 0);
	else
		theApp.OnViewProject();
}


void CMainFrame::OnUpdateProjectbarviewproject(CCmdUI* pCmdUI)
{
	if (project != 0)
	{
		pCmdUI->Enable(TRUE);
		if (projectWindowOpen)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnProjectbarviewregisters()
{
	if (registersWindowHandle)
		::SendMessage(registersWindowHandle, WM_CLOSE, 0, 0);
	else
		theApp.OnViewRegisters();
}


void CMainFrame::OnUpdateProjectbarviewregisters(CCmdUI* pCmdUI)
{
	if (project != 0)
	{
		pCmdUI->Enable(TRUE);
		if (registersWindowHandle)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnProjectbarviewsfrs()
{
	if (sfrsWindowOpen)
		::SendMessage(sfrsWindow->m_hWnd, WM_CLOSE, 0, 0);
	else
		theApp.OnViewMemorySfrs();
}


void CMainFrame::OnUpdateProjectbarviewsfrs(CCmdUI* pCmdUI)
{
	if (project != 0)
	{
		pCmdUI->Enable(TRUE);
		if (sfrsWindowOpen)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnProjectbarviewsram()
{
	if (sramWindowHandle)
		::SendMessage(sramWindowHandle, WM_CLOSE, 0, 0);
	else
		theApp.OnViewMemorySram();
}


void CMainFrame::OnUpdateProjectbarviewsram(CCmdUI* pCmdUI)
{
	if (project != 0 && sramSize)
	{
		pCmdUI->Enable(TRUE);
		if (sramWindowHandle)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnProjectbarvieweeprom()
{
	if (eepromWindowHandle)
		::SendMessage(eepromWindowHandle, WM_CLOSE, 0, 0);
	else
		theApp.OnViewMemoryEeprom();
}


void CMainFrame::OnUpdateProjectbarvieweeprom(CCmdUI* pCmdUI)
{
	if (project != 0 && eepromSize)
	{
		pCmdUI->Enable(TRUE);
		if (eepromWindowHandle)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnProjectbarviewwatch()
{
	if (watchWindowOpen)
		::SendMessage(watchWindow->m_hWnd, WM_CLOSE, 0, 0);
	else
		theApp.OnViewWatch();
}


void CMainFrame::OnUpdateProjectbarviewwatch(CCmdUI* pCmdUI)
{
	if (project != 0)
	{
		pCmdUI->Enable(TRUE);
		if (watchWindowOpen)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnProjectbarviewstack()
{
	if (stackWindowHandle)
		::SendMessage(stackWindowHandle, WM_CLOSE, 0, 0);
	else
		theApp.OnViewStack();
}


void CMainFrame::OnUpdateProjectbarviewstack(CCmdUI* pCmdUI)
{
	if (project != 0 && sramSize)
	{
		pCmdUI->Enable(TRUE);
		if (stackWindowHandle)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnProjectbarviewchip()
{
	if (chipWindowOpen)
		::SendMessage(chipWindow->m_hWnd, WM_CLOSE, 0, 0);
	else
		theApp.OnViewChip();
}


void CMainFrame::OnUpdateProjectbarviewchip(CCmdUI* pCmdUI)
{
	if (project != 0)
	{
		pCmdUI->Enable(TRUE);
		if (chipWindowOpen)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnProjectbarSyntaxhi() 
{
	theApp.OnPrefsSourceSyntaxhi();
}


void CMainFrame::OnUpdateProjectbarSyntaxhi(CCmdUI* pCmdUI) 
{
	theApp.OnUpdatePrefsSourceSyntaxhi(pCmdUI);
}


void CMainFrame::OnProjectbarTrackpc() 
{
	theApp.OnPrefsSourceTrackpc();	
}


void CMainFrame::OnUpdateProjectbarTrackpc(CCmdUI* pCmdUI) 
{
	theApp.OnUpdatePrefsSourceTrackpc(pCmdUI);
}


void CMainFrame::OnProjectbarAssembler() 
{
	theApp.OnPrefsSourceAsm();
}


void CMainFrame::OnUpdateProjectbarAssembler(CCmdUI* pCmdUI) 
{
	theApp.OnUpdatePrefsSourceAsm(pCmdUI);
}


void CMainFrame::OnProjectbarAddresses() 
{
	theApp.OnPrefsSourceAddress();
}


void CMainFrame::OnUpdateProjectbarAddresses(CCmdUI* pCmdUI) 
{
	theApp.OnUpdatePrefsSourceAddress(pCmdUI);
}


void CMainFrame::OnProjectbarMachinecode() 
{
	theApp.OnPrefsSourceCode();
}


void CMainFrame::OnUpdateProjectbarMachinecode(CCmdUI* pCmdUI) 
{
	theApp.OnUpdatePrefsSourceCode(pCmdUI);
}


void CMainFrame::OnClose() 
{
//	CDockState dockState;
	CString key;

//	GetDockState(dockState);
//	key = "HKEY_LOCAL_MACHINE\\Software\\";
//	key += COMPANYKEY;
//	key += "\\";
//	key += PRODUCTKEY;
//	key += "\\";
	key = VERSIONKEY;
	key += "\\ToolBars";
	// Save the ToolBars' position's in the Registry
//	dockState.SaveState(key);

	SaveBarState(key);

	CMDIFrameWnd::OnClose();
}


// This is a very dirty hack to stop MFC from screwing with the
// main app's title.
void CMainFrame::OnUpdateFrameTitle(BOOL addToTitle)
{
	char buffer[1000];
	int max = 1000;
	theApp.GetTitle(buffer, &max);
	::SetWindowText(m_hWnd, buffer);
}
