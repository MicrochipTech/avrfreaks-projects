// FunctionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "FunctionDialog.h"
#include "SourceFile.h"
#include "Function.h"
#include "ScopeBlock.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFunctionDialog dialog


CFunctionDialog::CFunctionDialog(CWnd* pParent /*=NULL*/, FMFunction * f)
	: CDialog(CFunctionDialog::IDD, pParent)
{
	function = f;
	//{{AFX_DATA_INIT(CFunctionDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFunctionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFunctionDialog)
	DDX_Control(pDX, IDC_FUNCTION_TO, m_to);
	DDX_Control(pDX, IDC_FUNCTION_FROM, m_from);
	DDX_Control(pDX, IDC_FUNCTION_LINE, m_line);
	DDX_Control(pDX, IDC_FUNCTION_FILE, m_file);
	DDX_Control(pDX, IDC_FUNCTION_NAME, m_name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFunctionDialog, CDialog)
	//{{AFX_MSG_MAP(CFunctionDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFunctionDialog message handlers

BOOL CFunctionDialog::OnInitDialog() 
{
	int i;
	char buffer[1000];
	CDialog::OnInitDialog();
	if (function == NULL)
		return TRUE;
	m_name.SetWindowText(function->GetName());
	m_file.SetWindowText(function->GetFile()->GetFileName());
	strcpy(buffer, "Declared on line ");
	i = function->GetSourceLine()+1;
	itoa(i, &buffer[strlen(buffer)], 10);
	m_line.SetWindowText(buffer);
	FMScopeBlock * scope = function->GetScopeBlock(0);
	strcpy(buffer, "0x");
	itoh(scope->GetFirstAddress(), buffer, false, 4);
	m_from.SetWindowText(buffer);
	buffer[2] = 0;
	itoh(scope->GetLastAddress(), buffer, false, 4);
	m_to.SetWindowText(buffer);
	return TRUE;
}
