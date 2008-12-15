// CBTreeCtrl.h

#ifndef CBTREECTRL_H
#define CBTREECTRL_H

class CCBTreeCtrl : public CTreeCtrl
{
protected:
	//{{AFX_MSG(CCBTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	void ShowPopupMenu(CPoint& point, int contextMenu);

	int selectedItem;
	HTREEITEM m_pOldSel;
};

#endif
