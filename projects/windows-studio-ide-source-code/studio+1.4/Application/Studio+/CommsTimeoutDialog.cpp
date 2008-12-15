// CommsTimeoutDialog.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "CommsTimeoutDialog.h"

#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommsTimeoutDialog dialog


CCommsTimeoutDialog::CCommsTimeoutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCommsTimeoutDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCommsTimeoutDialog)
	//}}AFX_DATA_INIT
}


void CCommsTimeoutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommsTimeoutDialog)
	DDX_Control(pDX, IDC_COMMSTIMEOUT_PERIOD, m_period);
	DDX_Control(pDX, IDC_COMMSTIMEOUT_MS, m_ms);
	DDX_Control(pDX, IDC_COMMSTIMEOUT_TEXT, m_text);
	DDX_Control(pDX, IDC_COMMSTIMEOUT_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommsTimeoutDialog, CDialog)
	//{{AFX_MSG_MAP(CCommsTimeoutDialog)
	ON_BN_CLICKED(IDC_COMMSTIMEOUT_ENABLE, OnCommstimeoutEnable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCommsTimeoutDialog message handlers


BOOL CCommsTimeoutDialog::OnInitDialog() 
{
	char buffer[100];

	CDialog::OnInitDialog();
	
	itoa(globals.commsTimeout, buffer, 10);
	m_period.SetWindowText(buffer);

	if (globals.commsTimeout != 0)		// If Time-out enabled
	{
		m_enable.SetCheck(1);
		m_text.ShowWindow(SW_SHOWNORMAL);
		m_period.ShowWindow(SW_SHOWNORMAL);
		m_ms.ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		m_enable.SetCheck(0);
		m_text.ShowWindow(SW_HIDE);
		m_period.ShowWindow(SW_HIDE);
		m_ms.ShowWindow(SW_HIDE);
	}
	return TRUE;
}


void CCommsTimeoutDialog::OnCommstimeoutEnable() 
{
	if (m_enable.GetCheck())			// If Time-out enabled
	{
		m_text.ShowWindow(SW_SHOWNORMAL);
		m_period.ShowWindow(SW_SHOWNORMAL);
		m_ms.ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		m_text.ShowWindow(SW_HIDE);
		m_period.ShowWindow(SW_HIDE);
		m_ms.ShowWindow(SW_HIDE);
	}
}


void CCommsTimeoutDialog::OnOK() 
{
	if (m_enable.GetCheck())			// If Time-out enabled
	{
		CString period;
		m_period.GetWindowText(period);
		globals.commsTimeout = atoi(period);
	}
	else
		globals.commsTimeout = 0;		// Disable Comms Time-out
	
	CDialog::OnOK();
}
