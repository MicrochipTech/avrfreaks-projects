// ProjectWindow.cpp : implementation file
//

#include "stdafx.h"
#include <afxcmn.h>						// CImageList
#include "studio.h"
#include "ProjectWindow.h"				// Header
#include "Project.h"					// FDProject class
#include "SourceFile.h"					// FMSourceFile class
#include "ScopeBlock.h"					// FMScopeBlock class
#include "Function.h"					// FMFunction class
#include "SourceFileDialog.h"			// CSourceFileDialog class
#include "FunctionDialog.h"				// CFunctionDialog class
#include "VariableDialog.h"				// CVariableDialog class
#include "SourceWindow.h"				// CSourceWindow class

#include "Symbol.h"

#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectWindow

IMPLEMENT_DYNCREATE(CProjectWindow, CMDIChildWnd)

CProjectWindow::CProjectWindow()
{
	// Find the window's preferred dimensions
	windowX = project->projectWindowInfo.x; // Default values
	windowY = project->projectWindowInfo.y;
	windowW = project->projectWindowInfo.width;
	windowH = project->projectWindowInfo.height;
	windowMin = project->projectWindowInfo.minimized;
	windowMax = project->projectWindowInfo.maximized;

	sizing = false;
	init = true;
}


CProjectWindow::~CProjectWindow()
{
}


void CProjectWindow::Update()
{
	int i, j, k, l;
	FMScopeBlock * scopeBlock;
	FMSymbol * symbol;
	CString symbolName;

	tree.DeleteAllItems();

	CString rootName = project->GetName();
	CString filesName = rootName + " Files";

	root = tree.InsertItem(rootName, 0, 0);
	project->SetTreeItem((int)root);

	HTREEITEM files = tree.InsertItem(filesName, 1, 2, root, root);

	selectedItem = 0;

	HTREEITEM fileitem = NULL;
	for (i=0; i<project->GetNumFiles(); i++)
	{
		FMSourceFile * file = project->GetFile(i);
		fileitem = tree.InsertItem(file->GetFileName(), 7, 7, files, fileitem);
		file->SetTreeItem((int)fileitem);

		HTREEITEM functions = tree.InsertItem("Functions", 3, 4, fileitem, NULL);
		HTREEITEM functionitem = NULL;
		for (j=0; j<file->GetNumFunctions(); j++)
		{
			FMFunction * function = file->GetFunction(j);
			functionitem = tree.InsertItem(function->GetName(), 8, 8, functions, functionitem);
			function->SetTreeItem((int)functionitem);

			bool first = true;
			HTREEITEM symbols = NULL;
			HTREEITEM symbolitem = NULL;
			for (k=0; k<(int)function->GetNumScopeBlocks(); k++)
			{
				scopeBlock = function->GetScopeBlock(k);
				for (l=0; l<scopeBlock->GetNumSymbols(); l++)
				{
					symbol = scopeBlock->GetSymbol(l);
					if (first)
					{
						first = false;
						symbols = tree.InsertItem("Variables", 5, 6,
							functionitem, NULL);
					}
					symbolName = symbol->GetName();
					symbolName += " (";
					symbolName += symbol->DataTypeString();
					symbolName += ")";
					symbolitem = tree.InsertItem(symbolName, 9, 9,
						symbols, symbolitem);
					symbol->SetTreeItem((int)symbolitem);
				}
			}
		}
		HTREEITEM symbols = tree.InsertItem("Global Variables", 5, 6, fileitem, NULL);
		HTREEITEM symbolitem = NULL;
		for (j=0; j<file->GetNumSymbols(); j++)
		{
			FMSymbol * symbol = file->GetSymbol(j);
			symbolName = symbol->GetName();
			symbolName += " (";
			symbolName += symbol->DataTypeString();
			symbolName += ")";
			symbolitem = tree.InsertItem(symbolName, 9, 9, symbols, symbolitem);
			symbol->SetTreeItem((int)symbolitem);
		}
	}

	tree.Expand(root, TVE_EXPAND);		// Expand the project item
}


BEGIN_MESSAGE_MAP(CProjectWindow, CMDIChildWnd)
	//{{AFX_MSG_MAP(CProjectWindow)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(IDM_CONTEXT_VARIABLE_ADDTOWATCH, OnContextVariableAddtowatch)
	ON_COMMAND(IDM_CONTEXT_VARIABLE_PROPERTIES, OnContextVariableProperties)
	ON_COMMAND(IDM_CONTEXT_FILE_OPEN, OnContextFileOpen)
	ON_COMMAND(IDM_CONTEXT_FILE_PROPERTIES, OnContextFileProperties)
	ON_COMMAND(IDM_CONTEXT_FUNCTION_SHOW, OnContextFunctionShow)
	ON_COMMAND(IDM_CONTEXT_FUNCTION_PROPERTIES, OnContextFunctionProperties)
	ON_COMMAND(IDM_CONTEXT_VARIABLE_SHOW, OnContextVariableShow)
	ON_COMMAND(IDM_CONTEXT_FILE_EXPAND, OnContextFileExpand)
	ON_COMMAND(IDM_CONTEXT_FUNCTION_EXPAND, OnContextFunctionExpand)
	ON_COMMAND(IDM_CONTEXT_PROJECT_EXPAND, OnContextProjectExpand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProjectWindow message handlers

BOOL CProjectWindow::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	HICON hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDR_PROJECT));
	cs.lpszClass = AfxRegisterWndClass(0, NULL, NULL, hIcon);
	return TRUE;
}


int CProjectWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CBitmap bitmap;

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	rect.left = 10;
	rect.top = 10;
	rect.right = rect.left+(windowW-10);
	rect.bottom = rect.top+(windowH-10);
	tree.Create(TVS_HASBUTTONS | TVS_HASLINES | WS_TABSTOP,
		rect, this, IDC_PROJECT_TREE);
	tree.ModifyStyleEx(0, WS_EX_CLIENTEDGE, TRUE);

	imageList.Create(16, 16, ILC_MASK, 11, 4);
	for (UINT nID = IDB_TREE_FIRST; nID <= IDB_TREE_LAST; nID++) // Load BMPs
	{
		bitmap.LoadBitmap(nID);
		imageList.Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
	}
	tree.SetImageList(&imageList, TVSIL_NORMAL);
	Update();
	SendMessage(WM_SIZE);				// Set the size

	project->projectWindowInfo.open = TRUE;

	tree.ShowWindow(SW_SHOWNORMAL);

	return 0;
}


void CProjectWindow::OnPaint() 
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
		tree.UpdateWindow();
	}
}


void CProjectWindow::OnMove(int x, int y) 
{
	WINDOWPLACEMENT wndPlacement;
	wndPlacement.length = sizeof(WINDOWPLACEMENT);
	CMDIChildWnd::OnMove(x, y);
	if (!project->projectWindowInfo.minimized &&
		!project->projectWindowInfo.maximized)
	{
		::GetWindowPlacement(m_hWnd, &wndPlacement);
		windowX = wndPlacement.rcNormalPosition.left;
		windowY = wndPlacement.rcNormalPosition.top;
		project->projectWindowInfo.x = windowX;
		project->projectWindowInfo.y = windowY;
	}
}


void CProjectWindow::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;
	WINDOWPLACEMENT wndPlacement;
	int tx, ty, tw, th;

	wndPlacement.length = sizeof(WINDOWPLACEMENT);
	CMDIChildWnd::OnSize(nType, cx, cy);

	if (init)							// Fix the size
	{
		sizing = true;
		MoveWindow(windowX, windowY, windowW, windowH, TRUE);
		if (tree)
		{
			GetWindowRect(&rect);
			CRect tRect;
			tree.GetWindowRect(&tRect);
			tx = (tRect.left-rect.left)-GetSystemMetrics(SM_CXSIZEFRAME);
			ty = (tRect.top-rect.top)-(GetSystemMetrics(SM_CYCAPTION)+
				GetSystemMetrics(SM_CYSIZEFRAME));
			tw = windowW-(2*(GetSystemMetrics(SM_CXSIZEFRAME)+tx));
			th = windowH-(GetSystemMetrics(SM_CYCAPTION)+
				(2*(tx+GetSystemMetrics(SM_CXSIZEFRAME))));
			tree.MoveWindow(tx, ty, tw, th, TRUE);
		}
		init = false;
		return;
	}

	if (nType == SIZE_MINIMIZED)
	{
		project->projectWindowInfo.maximized = FALSE;
		project->projectWindowInfo.minimized = TRUE;
		return;
	}
	if (nType == SIZE_MAXIMIZED)
	{
		project->projectWindowInfo.minimized = FALSE;
		project->projectWindowInfo.maximized = TRUE;
		sizing = true;					// Re-sized
	}
	if (nType == SIZE_RESTORED)
	{
		project->projectWindowInfo.minimized = FALSE;
		project->projectWindowInfo.maximized = FALSE;
		GetWindowPlacement(&wndPlacement);
		project->projectWindowInfo.width =
			wndPlacement.rcNormalPosition.right-
			wndPlacement.rcNormalPosition.left;
		project->projectWindowInfo.height =
			wndPlacement.rcNormalPosition.bottom-
			wndPlacement.rcNormalPosition.top;
		sizing = true;					// Re-sized
	}

	if (tree && sizing)
	{
		GetWindowRect(&rect);
		windowW = rect.Width();
		windowH = rect.Height();
		CRect cRect;
		GetClientRect(&cRect);
		CRect tRect;
		tree.GetWindowRect(&tRect);
		tx = (tRect.left-rect.left)-GetSystemMetrics(SM_CXSIZEFRAME);
		ty = (tRect.top-rect.top)-(GetSystemMetrics(SM_CYCAPTION)+
			GetSystemMetrics(SM_CYSIZEFRAME));
		tw = windowW-(2*(GetSystemMetrics(SM_CXSIZEFRAME)+tx));
		th = windowH-(GetSystemMetrics(SM_CYCAPTION)+
			(2*(tx+GetSystemMetrics(SM_CYSIZEFRAME))));
		tree.MoveWindow(tx, ty, tw, th);
	}
}


void CProjectWindow::OnClose() 
{
	project->projectWindowInfo.open = FALSE;
	projectWindowOpen = false;
	CMDIChildWnd::OnClose();
}


void CProjectWindow::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	projectWindowOpen = false;
}


// Context menu message handlers


void CProjectWindow::OnContextProjectExpand()
{
	HTREEITEM top = tree.GetSelectedItem();
	HTREEITEM item = top;
	tree.Expand(top, TVE_EXPAND);
	if (tree.ItemHasChildren(top))
	{
		HTREEITEM item = tree.GetChildItem(top);
		do
		{
			tree.Expand(item, TVE_EXPAND);
			if (tree.ItemHasChildren(item))
				item = tree.GetChildItem(item);
			else if (tree.GetNextSiblingItem(item))
				item = tree.GetNextSiblingItem(item);
			else
			{
				do
				{
					item = tree.GetParentItem(item);
					if (item != top &&
						tree.GetNextSiblingItem(item))
					{
						item = tree.GetNextSiblingItem(item);
						break;
					}
				}
				while (item != top);
			}
		}
		while (item != top);
	}
}


void CProjectWindow::OnContextFileOpen()
{
	FMSourceFile * file =
		project->GetSourceFileFromTreeItem((int)tree.GetSelectedItem());
	if (file != NULL)
	{
		if (!file->IsOpen())
		{
			int i = 0;
			do
			{
				if (project->GetFile(i) == file)
				{
					theApp.OpenSourceFile(i);
					break;
				}
				else
					i++;
			}
			while (i < project->GetNumFiles());
		}
	}
}


void CProjectWindow::OnContextFileProperties()
{
	FMSourceFile * file =
		project->GetSourceFileFromTreeItem((int)tree.GetSelectedItem());
	if (file != NULL)
	{
		CSourceFileDialog * dialog = new CSourceFileDialog(NULL, file);
		dialog->DoModal();
		delete dialog;
	}
}


void CProjectWindow::OnContextFileExpand()
{
	HTREEITEM top = tree.GetSelectedItem();
	HTREEITEM item = top;
	tree.Expand(top, TVE_EXPAND);
	if (tree.ItemHasChildren(top))
	{
		HTREEITEM item = tree.GetChildItem(top);
		do
		{
			tree.Expand(item, TVE_EXPAND);
			if (tree.ItemHasChildren(item))
				item = tree.GetChildItem(item);
			else if (tree.GetNextSiblingItem(item))
				item = tree.GetNextSiblingItem(item);
			else
			{
				do
				{
					item = tree.GetParentItem(item);
					if (item != top &&
						tree.GetNextSiblingItem(item))
					{
						item = tree.GetNextSiblingItem(item);
						break;
					}
				}
				while (item != top);
			}
		}
		while (item != top);
	}
}


void CProjectWindow::OnContextFunctionShow()
{
	FMFunction * function =
		project->GetFunctionFromTreeItem((int)tree.GetSelectedItem());
	if (function != NULL)
	{
		if (function->GetSourceLine() != -1) // If we know the source line
		{
			cursorLine = function->GetSourceLine();
			cursorFile = function->GetFile();
			int index = cursorFile->GetSourceFileIndex();
			if (!cursorFile->open)
				theApp.OpenSourceFile(index);
			sourceWindows[index]->ShowLine(cursorLine);
		}
	}
}


void CProjectWindow::OnContextFunctionProperties()
{
	FMFunction * function =
		project->GetFunctionFromTreeItem((int)tree.GetSelectedItem());
	if (function != NULL)
	{
		CFunctionDialog * dialog = new CFunctionDialog(NULL, function);
		dialog->DoModal();
		delete dialog;
	}
}


void CProjectWindow::OnContextFunctionExpand()
{
	HTREEITEM top = tree.GetSelectedItem();
	HTREEITEM item = top;
	tree.Expand(top, TVE_EXPAND);
	if (tree.ItemHasChildren(top))
	{
		HTREEITEM item = tree.GetChildItem(top);
		do
		{
			tree.Expand(item, TVE_EXPAND);
			if (tree.ItemHasChildren(item))
				item = tree.GetChildItem(item);
			else if (tree.GetNextSiblingItem(item))
				item = tree.GetNextSiblingItem(item);
			else
			{
				do
				{
					item = tree.GetParentItem(item);
					if (item != top &&
						tree.GetNextSiblingItem(item))
					{
						item = tree.GetNextSiblingItem(item);
						break;
					}
				}
				while (item != top);
			}
		}
		while (item != top);
	}
}


void CProjectWindow::OnContextVariableShow()
{
	FMSymbol * symbol =
		project->GetSymbolFromTreeItem((int)tree.GetSelectedItem());
	if (symbol != NULL)
	{
		if (symbol->GetSourceLine() != -1) // If we know the source line
		{
			cursorLine = symbol->GetSourceLine();
			cursorFile = symbol->GetFile();
			int index = cursorFile->GetSourceFileIndex();
			if (!cursorFile->open)
				theApp.OpenSourceFile(index);
			sourceWindows[index]->ShowLine(cursorLine);
		}
	}
}


void CProjectWindow::OnContextVariableAddtowatch()
{
	FMSymbol * symbol =
		project->GetSymbolFromTreeItem((int)tree.GetSelectedItem());
	if (symbol != NULL)
	{
		project->AddSymbolToWatch(symbol, true);
		project->Changed();
	}
}


void CProjectWindow::OnContextVariableProperties()
{
	FMSymbol * symbol =
		project->GetSymbolFromTreeItem((int)tree.GetSelectedItem());
	if (symbol != NULL)
	{
		CVariableDialog * dialog = new CVariableDialog(NULL, symbol);
		dialog->DoModal();
		delete dialog;
	}
}
