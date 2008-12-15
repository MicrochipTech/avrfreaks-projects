// WatchWindow.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "WatchWindow.h"
#include "Symbol.h"
#include "Function.h"
#include "VariableDialog.h"
#include "AsmVariableDialog.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchWindow

IMPLEMENT_DYNCREATE(CWatchWindow, CMDIChildWnd)

CWatchWindow::CWatchWindow()
{
	// Find the window's preferred dimensions
	windowX = project->watchWindowInfo.x; // Default values
	windowY = project->watchWindowInfo.y;
	windowW = project->watchWindowInfo.width;
	windowH = project->watchWindowInfo.height;
	windowMin = project->watchWindowInfo.minimized;
	windowMax = project->watchWindowInfo.maximized;

	sizing = false;
	init = true;
	focusedItem = -1;
}

CWatchWindow::~CWatchWindow()
{
}


void CWatchWindow::AddSymbol(FMSymbol * symbol)
{
	char buffer[10000];
	FMFunction * function = NULL;

	if (!symbol)
		return;

	if (symbol->GetScope())
		function = symbol->GetScope()->GetFunction();
	strcpy(buffer, symbol->GetName());
	if (function)
	{
		strcat(buffer, " (");
		strcat(buffer, function->GetName());
		strcat(buffer, ")");
	}

	int item = m_list.InsertItem(m_list.GetItemCount(), buffer);
	m_list.SetItemText(item, 1, symbol->DataTypeString());
	m_list.SetItemText(item, 2, symbol->GetAddressString());
	symbol->GetValueString(buffer, 10000);
	m_list.SetItemText(item, 3, buffer);
	m_list.SetItemData(item, (DWORD)symbol);
}


void CWatchWindow::Update()
{
	char buffer[10000];
	FMSymbol * symbol;
	FMFunction * function;

	for (int i=0; i<m_list.GetItemCount(); i++)
	{
		symbol = (FMSymbol *)m_list.GetItemData(i);
		if (symbol->GetScope())
			function = symbol->GetScope()->GetFunction();
		else
			function = NULL;
		strcpy(buffer, symbol->GetName());
		if (function)
		{
			strcat(buffer, " (");
			strcat(buffer, function->GetName());
			strcat(buffer, ")");
		}
		m_list.SetItemText(i, 0, buffer);
		m_list.SetItemText(i, 1, symbol->DataTypeString());
		m_list.SetItemText(i, 2, symbol->GetAddressString());
		symbol->GetValueString(buffer, 10000);
		m_list.SetItemText(i, 3, buffer);
	}
}


void CWatchWindow::SelectSymbol(FMSymbol * symbol)
{
	for (int i=0; i<m_list.GetItemCount(); i++)
	{
		if ((FMSymbol *)(m_list.GetItemData(i)) == symbol)
			m_list.SetItemState(i, LVIS_FOCUSED|LVIS_SELECTED, (UINT)-1);
		else
			m_list.SetItemState(i, 0, (UINT)-1);
	}
}


BEGIN_MESSAGE_MAP(CWatchWindow, CMDIChildWnd)
	//{{AFX_MSG_MAP(CWatchWindow)
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_WATCH_LIST, OnItemchangedWatchList)
	ON_NOTIFY(NM_DBLCLK, IDC_WATCH_LIST, OnDblclkWatchList)
	ON_WM_KEYDOWN()
	ON_COMMAND(IDM_CONTEXT_WATCHSRC_PROPERTIES, OnContextWatchSrcProperties)
	ON_COMMAND(IDM_CONTEXT_WATCHSRC_DELETE, OnContextWatchDelete)
	ON_COMMAND(IDM_CONTEXT_WATCHASM_EDIT, OnContextWatchAsmEdit)
	ON_COMMAND(IDM_CONTEXT_WATCHNEW_ADD, OnContextWatchNewAdd)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(IDM_CONTEXT_WATCHASM_DELETE, OnContextWatchDelete)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWatchWindow message handlers


BOOL CWatchWindow::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDR_WATCH));
	cs.lpszClass = AfxRegisterWndClass(0, NULL, NULL, hIcon);
	return TRUE;
}


int CWatchWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = rect.left+(windowW-10);
	rect.bottom = rect.top+(windowH-10);
	m_list.Create(LVS_REPORT | LVS_SINGLESEL | WS_TABSTOP,
		rect, this, IDC_WATCH_LIST);
	m_list.ModifyStyleEx(0, WS_EX_CLIENTEDGE, TRUE);

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_TWOCLICKACTIVATE);
	SendMessage(WM_SIZE);				// Set the size
	m_list.GetClientRect(&rect);
	m_list.InsertColumn(0, "Variable", LVCFMT_LEFT, 80);
	m_list.InsertColumn(1, "Type", LVCFMT_LEFT, 100);
	m_list.InsertColumn(2, "Address", LVCFMT_LEFT, 75);
	m_list.InsertColumn(3, "Value", LVCFMT_LEFT, rect.Width()-255);

	project->UpdateWatchSymbols();
	for (int i=0; i<project->GetNumWatchSymbols(); i++)
	{
		AddSymbol(project->GetWatchSymbol(i));

		if (projectWindowOpen &&
			!(project->GetWatchSymbol(i)->GetType()&VARIABLE_ASSEMBLERMASK))
		{
			TVITEM tItem;
			tItem.mask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
			tItem.hItem =
				(HTREEITEM)(project->GetWatchSymbol(i))->GetTreeItem();
			projectWindow->tree.GetItem(&tItem);
			tItem.iImage = 10;
			tItem.iSelectedImage = 10;
			projectWindow->tree.SetItem(&tItem);
		}
	}
	project->watchWindowInfo.open = TRUE;

	if (endCondition)					// If debugger is active
	{
		firstWatchSymbol = 0;			// Update their values
		lastWatchSymbol = project->GetNumWatchSymbols();
	}

	m_list.ShowWindow(SW_SHOWNORMAL);

	return 0;
}


void CWatchWindow::OnPaint() 
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


void CWatchWindow::OnMove(int x, int y) 
{
	WINDOWPLACEMENT wndPlacement;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);
	CMDIChildWnd::OnMove(x, y);
	if (!project->watchWindowInfo.minimized &&
		!project->watchWindowInfo.maximized)
	{
		::GetWindowPlacement(m_hWnd, &wndPlacement);
		windowX = wndPlacement.rcNormalPosition.left;
		windowY = wndPlacement.rcNormalPosition.top;
		project->watchWindowInfo.x = windowX;
		project->watchWindowInfo.y = windowY;
	}
}


void CWatchWindow::OnSize(UINT nType, int cx, int cy) 
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
		if (m_list)
		{
			GetWindowRect(&rect);
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
		project->watchWindowInfo.maximized = FALSE;
		project->watchWindowInfo.minimized = TRUE;
		return;
	}
	if (nType == SIZE_MAXIMIZED)
	{
		project->watchWindowInfo.minimized = FALSE;
		project->watchWindowInfo.maximized = TRUE;
		sizing = true;					// Re-sized
	}
	if (nType == SIZE_RESTORED)
	{
		project->watchWindowInfo.minimized = FALSE;
		project->watchWindowInfo.maximized = FALSE;
		GetWindowPlacement(&wndPlacement);
		project->watchWindowInfo.width =
			wndPlacement.rcNormalPosition.right-
			wndPlacement.rcNormalPosition.left;
		project->watchWindowInfo.height =
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


void CWatchWindow::OnClose() 
{
	project->watchWindowInfo.open = FALSE;
	watchWindowOpen = false;
	CMDIChildWnd::OnClose();
}


void CWatchWindow::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	watchWindowOpen = false;
}


void CWatchWindow::OnItemchangedWatchList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->uNewState&LVIS_FOCUSED)
		focusedItem = pNMListView->iItem;

	*pResult = 0;
}


void CWatchWindow::OnDblclkWatchList(NMHDR* pNMHDR, LRESULT* pResult)
{
	FMSymbol * symbol = NULL;
	if (focusedItem != -1)				// If an item is focused
	{
		symbol = (FMSymbol *)m_list.GetItemData(focusedItem);
		if (symbol->GetType()&VARIABLE_ASSEMBLERMASK)
		{
			*pResult = 0;
			return;
		}
		if (!projectWindowOpen)
			theApp.OnViewProject();	// Open project window
		projectWindow->tree.EnsureVisible(
			(HTREEITEM)symbol->GetTreeItem());
		projectWindow->tree.SelectItem((HTREEITEM)symbol->GetTreeItem());
		projectWindow->SetFocus();
	}
	else
	{
		CAsmVariableDialog dialog;
		if (dialog.DoModal() == IDOK)
			project->AddSymbolToWatch(dialog.GetSymbol(), true);
	}
	*pResult = 0;
}


void CWatchWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	LVITEM item;
	FMSymbol * symbol;
	int i;

	if (nChar != VK_DELETE)				// If not delete
	{
		CMDIChildWnd::OnKeyDown(nChar, nRepCnt, nFlags); // Let the window have it
		return;
	}

	item.mask = LVIF_STATE;
	item.stateMask = LVIS_SELECTED;
	item.iSubItem = 0;
	for (i=0; i<m_list.GetItemCount(); i++)
	{
		item.iItem = i;
		m_list.GetItem(&item);
		if (item.state&LVIS_SELECTED)	// If item is selected
		{
			symbol = (FMSymbol *)m_list.GetItemData(i);
			project->RemoveSymbolFromWatch(symbol);
			if ((symbol->GetType()&VARIABLE_ASSEMBLERMASK))
				delete symbol;
			project->Changed();
		}
	}
	m_list.DeleteAllItems();
	for (i=0; i<project->GetNumWatchSymbols(); i++)
		AddSymbol(project->GetWatchSymbol(i));
}


void CWatchWindow::OnContextWatchSrcProperties()
{
	if (focusedItem != -1)
	{
		FMSymbol * symbol = (FMSymbol *)m_list.GetItemData(focusedItem);
		if (symbol != NULL)
		{
			CVariableDialog * dialog = new CVariableDialog(NULL, symbol);
			dialog->DoModal();
		}
	}
}


void CWatchWindow::OnContextWatchDelete()
{
	FMSymbol * symbol;
	int i;

	if (focusedItem != -1)
	{
		symbol = (FMSymbol *)m_list.GetItemData(focusedItem);
		project->RemoveSymbolFromWatch(symbol);
		if ((symbol->GetType()&VARIABLE_ASSEMBLERMASK))
			delete symbol;
		m_list.DeleteAllItems();
		for (i=0; i<project->GetNumWatchSymbols(); i++)
			AddSymbol(project->GetWatchSymbol(i));
		project->Changed();
	}
}
	

void CWatchWindow::OnContextWatchAsmEdit()
{
	FMSymbol * symbol = (FMSymbol *)m_list.GetItemData(focusedItem);
	CAsmVariableDialog * dialog = new CAsmVariableDialog(symbol, this);
	if (dialog->DoModal() == IDOK)
	{
		Update();
		project->Changed();
	}
}


void CWatchWindow::OnContextWatchNewAdd()
{
	CAsmVariableDialog dialog;
	if (dialog.DoModal() == IDOK)
	{
		project->AddSymbolToWatch(dialog.GetSymbol(), true);
		project->Changed();
	}
}


void CWatchListCtrl::ShowPopupMenu(CPoint& point, int contextMenu)
{
	if (point.x == -1 && point.y == -1)
	{									// Keystroke invoked
		CRect rect;
		GetClientRect(&rect);
		ClientToScreen(&rect);
		point = rect.TopLeft();
		point.Offset(5, 5);
	}
	CMenu menu;
	menu.LoadMenu(contextMenu);
//	VERIFY(menu.LoadMenu(contextMenu));

	CMenu * pPopup = menu.GetSubMenu(0);
//	ASSERT(pPopup != NULL);
	CWnd * pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle()&WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}


int CWatchListCtrl::GetSelectedItem()
{
	return selectedItem;
}


BEGIN_MESSAGE_MAP(CWatchListCtrl, CCBListCtrl)
	//{{AFX_MSG_MAP(CWatchListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CWatchListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags;
	int item = HitTest(point, &uFlags);
	selectedItem = item;
	if ((item != -1) && (uFlags&TVHT_ONITEM))
		SetItemState(item, LVIS_FOCUSED|LVIS_SELECTED, (UINT)-1);
	CListCtrl::OnLButtonDown(nFlags, point);
}


void CWatchListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags;
	int item = HitTest(point, &uFlags);
	if (selectedItem != -1)
		SetItemState(selectedItem, 0, (UINT)-1);
	selectedItem = item;
	if ((item != -1) && (uFlags&TVHT_ONITEM))
		SetItemState(item, LVIS_FOCUSED|LVIS_SELECTED, (UINT)-1);
}


void CWatchListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	UINT uFlags;
	CPoint ptList = point;
	ScreenToClient(&ptList);
	int item = HitTest(ptList, &uFlags);
	if ((item != -1) && (uFlags&TVHT_ONITEM))
	{
		FMSymbol * symbol = (FMSymbol *)GetItemData(item);
		if (symbol->GetType()&VARIABLE_ASSEMBLERMASK)
			ShowPopupMenu(point, IDM_CONTEXT_WATCHASM);
		else
			ShowPopupMenu(point, IDM_CONTEXT_WATCHSRC);
	}
	else
		ShowPopupMenu(point, IDM_CONTEXT_WATCHNEW);
}


void CWatchWindow::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);
	theApp.SetTitle();	
}
