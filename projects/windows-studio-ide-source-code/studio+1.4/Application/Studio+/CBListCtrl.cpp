// CBListCtrl.cpp

// A List Control that sends it's parent key messages for the Delete key.

#include "stdafx.h"
#include "Studio.h"
#include "CBListCtrl.h"
#include "Globals.h"


/////////////////////////////////////////////////////////////////////////////
// CCBListCtrl


BEGIN_MESSAGE_MAP(CCBListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCBListCtrl)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCBListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DELETE)				// If delete key pressed
	{
		GetParent()->PostMessage(WM_KEYDOWN, nChar,
			MAKELPARAM(nRepCnt, nFlags));
	}
	else
		CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
