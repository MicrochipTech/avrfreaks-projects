// CBTreeCtrl.cpp

// A "Right Clickable" tree control.

#include "stdafx.h"
#include "Studio.h"
#include "CBTreeCtrl.h"
#include "SourceFile.h"
#include "Function.h"
#include "Symbol.h"
#include "Globals.h"


/////////////////////////////////////////////////////////////////////////////
// CCBTreeCtrl

void CCBTreeCtrl::ShowPopupMenu(CPoint& point, int contextMenu)
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
	VERIFY(menu.LoadMenu(contextMenu));

	CMenu * pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd * pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle()&WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}


BEGIN_MESSAGE_MAP(CCBTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CCBTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCBTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags;
	HTREEITEM htItem = HitTest(point, &uFlags);
	if ((htItem != NULL) && (uFlags&TVHT_ONITEM))
	{
		selectedItem = (int)htItem;
		Select(htItem, TVGN_DROPHILITE);
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
}


void CCBTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	FMSourceFile * file;
	FMFunction * function;
	FMSymbol * symbol;
	int index;

	UINT uFlags;
	HTREEITEM htItem = HitTest(point, &uFlags);
	if ((htItem != NULL) && (uFlags&TVHT_ONITEM))
	{
		file = project->GetSourceFileFromTreeItem((int)htItem);
		if (file != NULL)
		{
			index = file->GetSourceFileIndex();
			if (!file->IsOpen())		// If not open already
				theApp.OpenSourceFile(index);
			else
				sourceWindows[index]->BringWindowToTop();
			return;
		}
		function = project->GetFunctionFromTreeItem((int)htItem);
		if (function != NULL)
		{
			file = function->GetFile();
			index = file->GetSourceFileIndex();
			if (!file->IsOpen())		// If not open already
				theApp.OpenSourceFile(index);
			else
				sourceWindows[index]->BringWindowToTop();
			cursorLine = function->GetSourceLine();
			cursorFile = file;
			sourceWindows[index]->ShowLine(cursorLine);
			return;
		}
		symbol = project->GetSymbolFromTreeItem((int)htItem);
		if (symbol != NULL)
		{
			file = symbol->GetFile();
			index = file->GetSourceFileIndex();
			if (!file->IsOpen())		// If not open already
				theApp.OpenSourceFile(index);
			else
				sourceWindows[index]->BringWindowToTop();
			cursorLine = symbol->GetSourceLine();
			cursorFile = file;
			sourceWindows[index]->ShowLine(cursorLine);
			return;
		}
	}
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}


void CCBTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags;
	HTREEITEM htItem = HitTest(point, &uFlags);
	if ((htItem != NULL) && (uFlags&TVHT_ONITEM))
	{
		m_pOldSel = GetSelectedItem();
		selectedItem = (int)htItem;
		Select(htItem, TVGN_CARET);
	}
}


void CCBTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	UINT uFlags;
	int menu;
	CPoint ptTree = point;
	ScreenToClient(&ptTree);
	HTREEITEM htItem = HitTest(ptTree, &uFlags);
	if ((htItem != NULL) && (uFlags&TVHT_ONITEM))
	{
		if (project->GetContextMenu((int)htItem, &menu))
		{
			if (menu != -1)
				ShowPopupMenu(point, menu);
		}
	}
	else
		CTreeCtrl::OnContextMenu(pWnd, point);
}
