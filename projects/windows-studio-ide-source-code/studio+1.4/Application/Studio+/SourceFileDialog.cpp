// SourceFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Shlwapi.h"					// Path functions
#include "studio.h"
#include "SourceFileDialog.h"
#include "SourceFile.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSourceFileDialog dialog


CSourceFileDialog::CSourceFileDialog(CWnd* pParent /*=NULL*/, FMSourceFile * s)
	: CDialog(CSourceFileDialog::IDD, pParent)
{
	sourceFile = s;

	//{{AFX_DATA_INIT(CSourceFileDialog)
	//}}AFX_DATA_INIT
}


void CSourceFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSourceFileDialog)
	DDX_Control(pDX, IDC_SOURCEFILE_LINES, m_lines);
	DDX_Control(pDX, IDC_SOURCEFILE_PATH, m_path);
	DDX_Control(pDX, IDC_SOURCEFILE_NAME, m_name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSourceFileDialog, CDialog)
	//{{AFX_MSG_MAP(CSourceFileDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSourceFileDialog message handlers


BOOL CSourceFileDialog::OnInitDialog() 
{
	char buffer[1000];
	CDialog::OnInitDialog();
	if (sourceFile == NULL)
		return TRUE;
	m_name.SetWindowText(sourceFile->GetFileName());
	strcpy(buffer, sourceFile->GetAbsolutePath());
	while (buffer[strlen(buffer)-1] == '\\')
		buffer[strlen(buffer)-1] = 0;
	m_path.SetWindowText(buffer);
	int lines = sourceFile->GetNumLines();
	itoa(lines, buffer, 10);
	m_lines.SetWindowText(buffer);
	return TRUE;
}
