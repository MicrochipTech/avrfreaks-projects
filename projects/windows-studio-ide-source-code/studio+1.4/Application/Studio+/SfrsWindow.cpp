// SfrsWindow.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "SfrsWindow.h"
#include "Globals.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSfrsWindow

CSfrsWindow::CSfrsWindow()
{
	// Find the window's preferred dimensions
	windowX = project->sfrsWindowInfo.x; // Default values
	windowY = project->sfrsWindowInfo.y;
	windowW = project->sfrsWindowInfo.width;
	windowH = project->sfrsWindowInfo.height;
	windowMin = project->sfrsWindowInfo.minimized;
	windowMax = project->sfrsWindowInfo.maximized;

	sizing = false;
	init = true;
	focusedItem = -1;
}


CSfrsWindow::~CSfrsWindow()
{
}


void CSfrsWindow::Update()
{
	int i, j;
	int index;
	char buffer[10];
	CString str;

	for (i=0; i<m_list.GetItemCount(); i++)
	{
		index = (int)m_list.GetItemData(i);
		if (sfrsMask[index])
		{
			strcpy(buffer, "$");
			itoh((int)sfrs[index], buffer, false, 2);
		}
		else
			strcpy(buffer, "$??");
		m_list.SetItemText(i, 2, buffer);
		if (project->SfrFlagsVisible())
		{
			for (j=0; j<8; j++)
			{
				if (project->GetSfrFlagMarkers(index)[j])
				{
					str = project->GetSfrFlagName(index, j);
					str += " = ";
					if (sfrsMask[index])
					{
						if (sfrs[index]&(1<<(7-j)))
							str += "1";
						else
							str += "0";
					}
					else
						str += "?";
					m_list.SetItemText(i, 3+j, LPCTSTR(str));
				}
				else
					m_list.SetItemText(i, 3+j, "");

			}
		}
	}
}


void CSfrsWindow::Refresh()
{
	int i;
	char buffer[10];
	int item;

	m_list.DeleteAllItems();
	for (i=0; i<8; i++)
		m_list.DeleteColumn(3);
	if (project->SfrFlagsVisible())
	{
		strcpy(buffer, "Flag  ");
		for (i=0; i<8; i++)
		{
			buffer[5] = '0'+(char)(7-i);
			m_list.InsertColumn(3+i, buffer, LVCFMT_LEFT, 75, 3+i);
		}
	}

	for (i=0; i<project->GetNumSfrs(); i++)
	{
		if (project->GetSfrFlag(i))		// If user wants this SFR
		{
			item = m_list.InsertItem(m_list.GetItemCount(),
				project->GetSfrName(i));
			strcpy(buffer, "$");
			itoh(i, buffer, false, 2);
			m_list.SetItemText(item, 1, buffer);
			m_list.SetItemData(item, (DWORD)i);
		}
	}

	Update();
}


IMPLEMENT_DYNCREATE(CSfrsWindow, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CSfrsWindow, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSfrsWindow)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSfrs message handlers


BOOL CSfrsWindow::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDR_SFRS));
	cs.lpszClass = AfxRegisterWndClass(0, NULL, NULL, hIcon);
	return TRUE;
}


int CSfrsWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = rect.left+(windowW-10);
	rect.bottom = rect.top+(windowH-10);
	m_list.Create(LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
		rect, this, IDC_SFRS_LIST);
	m_list.ModifyStyleEx(0, WS_EX_CLIENTEDGE, TRUE);
	
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_TWOCLICKACTIVATE);
	m_list.InsertColumn(0, "SFR", LVCFMT_LEFT, 75, 0);
	m_list.InsertColumn(1, "Address", LVCFMT_LEFT, 50, 1);
	m_list.InsertColumn(2, "Value", LVCFMT_LEFT, 50, 2);

	SendMessage(WM_SIZE);				// Set the size

	m_list.ShowWindow(SW_SHOWNORMAL);

	project->sfrsWindowInfo.open = TRUE;
	
	Refresh();

	return 0;
}


void CSfrsWindow::OnPaint() 
{
	CRect rect;
	CPaintDC dc(this); // device context for painting

	if (sizing)
		Invalidate(FALSE);

	GetWindowRect(&rect);
	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	vert[0].x = 0;
	vert[0].y = 0;
	vert[0].Red = (GRADIENTTOP&0xff)<<8; // 0x5000;
	vert[0].Green = GRADIENTTOP&0xff00; //0x5000;
	vert[0].Blue = (GRADIENTTOP&0xff0000)>>8; //0xff00;
	vert[0].Alpha = 0x0000;
	vert[1].x = rect.Width()+1;
	vert[1].y = rect.Height()+1;
	vert[1].Red = (GRADIENTBOTTOM&0xff)<<8; // 0xff00;
	vert[1].Green = GRADIENTBOTTOM&0xff00; //0xff00;
	vert[1].Blue = (GRADIENTBOTTOM&0xff0000)>>8; //0xff00;
	vert[1].Alpha = 0x0000;
	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;
	::GradientFill(dc.m_hDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);

	if (sizing)
	{
		sizing = false;
		m_list.UpdateWindow();
	}
}


void CSfrsWindow::OnMove(int x, int y) 
{
	WINDOWPLACEMENT wndPlacement;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	CMDIChildWnd::OnMove(x, y);

	if (!project->sfrsWindowInfo.minimized &&
		!project->sfrsWindowInfo.maximized)
	{
		::GetWindowPlacement(m_hWnd, &wndPlacement);
		windowX = wndPlacement.rcNormalPosition.left;
		windowY = wndPlacement.rcNormalPosition.top;
		project->sfrsWindowInfo.x = windowX;
		project->sfrsWindowInfo.y = windowY;
	}
}


void CSfrsWindow::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;
	WINDOWPLACEMENT wndPlacement;
	int lx, ly, lw, lh;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);

	CMDIChildWnd::OnSize(nType, cx, cy);
	
	if (init)							// Fix the size
	{
		sizing = true;
		MoveWindow(windowX, windowY, windowW, windowH, TRUE);
		GetWindowRect(&rect);
		if (m_list)
		{
			CRect lRect;
			m_list.GetWindowRect(&lRect);
			lx = (lRect.left-rect.left)-GetSystemMetrics(SM_CXSIZEFRAME);
			ly = (lRect.top-rect.top)-(GetSystemMetrics(SM_CYCAPTION)+
				GetSystemMetrics(SM_CYSIZEFRAME));
			lw = windowW-(2*(GetSystemMetrics(SM_CXSIZEFRAME)+lx));
			lh = windowH-(GetSystemMetrics(SM_CYCAPTION)+
				(2*(lx+GetSystemMetrics(SM_CXSIZEFRAME))));
			m_list.MoveWindow(lx, ly, lw, lh, TRUE);
		}
		init = false;
		return;
	}

	if (nType == SIZE_MINIMIZED)
	{
		project->sfrsWindowInfo.maximized = FALSE;
		project->sfrsWindowInfo.minimized = TRUE;
		return;
	}
	if (nType == SIZE_MAXIMIZED)
	{
		project->sfrsWindowInfo.minimized = FALSE;
		project->sfrsWindowInfo.maximized = TRUE;
		sizing = true;					// Re-sized
	}
	if (nType == SIZE_RESTORED)
	{
		project->sfrsWindowInfo.minimized = FALSE;
		project->sfrsWindowInfo.maximized = FALSE;
		GetWindowPlacement(&wndPlacement);
		project->sfrsWindowInfo.width =
			wndPlacement.rcNormalPosition.right-
			wndPlacement.rcNormalPosition.left;
		project->sfrsWindowInfo.height =
			wndPlacement.rcNormalPosition.bottom-
			wndPlacement.rcNormalPosition.top;
		sizing = true;					// Re-sized
	}

	if (m_list && sizing)
	{
		GetWindowRect(&rect);
		windowW = rect.Width();
		windowH = rect.Height();
		CRect cRect;
		GetClientRect(&cRect);
		CRect lRect;
		m_list.GetWindowRect(&lRect);
		lx = (lRect.left-rect.left)-GetSystemMetrics(SM_CXSIZEFRAME);
		ly = (lRect.top-rect.top)-(GetSystemMetrics(SM_CYCAPTION)+
			GetSystemMetrics(SM_CYSIZEFRAME));
		lw = windowW-(2*(GetSystemMetrics(SM_CXSIZEFRAME)+lx));
		lh = windowH-(GetSystemMetrics(SM_CYCAPTION)+
			(2*(lx+GetSystemMetrics(SM_CYSIZEFRAME))));
		m_list.MoveWindow(lx, ly, lw, lh);
	}	
}


void CSfrsWindow::OnClose() 
{
	project->sfrsWindowInfo.open = FALSE;
	sfrsWindowOpen = false;
	CMDIChildWnd::OnClose();
}


void CSfrsWindow::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	sfrsWindowOpen = false;
}


void CSfrsWindow::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);
	theApp.SetTitle();	
}
