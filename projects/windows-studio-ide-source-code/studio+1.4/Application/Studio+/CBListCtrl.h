// CBListCtrl.h

#ifndef CBLISTCTRL_H
#define CBLISTCTRL_H

class CCBListCtrl : public CListCtrl
{
protected:
	//{{AFX_MSG(CCBListCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
};

#endif
