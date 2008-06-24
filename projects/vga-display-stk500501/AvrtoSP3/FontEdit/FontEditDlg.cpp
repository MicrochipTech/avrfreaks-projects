// FontEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontEdit.h"
#include "FontEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_NEXTFONT		WM_USER+1

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontEditDlg dialog

CFontEditDlg::CFontEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFontEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	FontBuffer = NULL;
}

void CFontEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontEditDlg)
	DDX_Control(pDX, IDC_EDIT_DEST, m_Edit_Dest);
	DDX_Control(pDX, IDC_EDIT_COUNT, m_Edit_Count);
	DDX_Control(pDX, IDC_EDIT_SOURCE, m_Edit_Source);
	DDX_Control(pDX, IDC_STATIC_FONT_SEL, m_Static_CharSel);
	DDX_Control(pDX, IDC_STATIC_DEBUG, m_Static_Debug);
	DDX_Control(pDX, IDC_SCROLLBAR_FONT_SEL, m_ScrollBar_CharSel);
	DDX_Control(pDX, IDC_STATIC_FONTDISP, m_Static_FontDisp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFontEditDlg, CDialog)
	//{{AFX_MSG_MAP(CFontEditDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, OnButtonAbout)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, OnButtonMove)
	ON_BN_CLICKED(IDC_BUTTON_INVERT, OnButtonInvert)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SWAP, OnButtonSwap)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEXTFONT,OnNextFont)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontEditDlg message handlers

BOOL CFontEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	m_Static_FontDisp.SetDebugDisplay(&m_Static_Debug);

	m_ScrollBar_CharSel.SetScrollRange(0,1023);
	m_ScrollBar_CharSel.SetActionMessage(m_hWnd,WM_NEXTFONT);
	m_ScrollBar_CharSel.SetScrollPos(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFontEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFontEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFontEditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFontEditDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CWnd *wC;

	wC = GetFocus();		//get handle of control that gen message
	int id = wC->GetDlgCtrlID();	//get ID of control
	switch(id)						//parse the id
	{
		case IDOK:						//the OK button was really the one
			{
				if(FontBuffer) delete[] FontBuffer;
			}
			CDialog::OnOK();
			break;
		default:
			wC = GetNextDlgTabItem(wC);	//reset the focus here
			wC->SetFocus();
			break;
	}
}

void CFontEditDlg::OnButtonLoad() 
{
	// TODO: Add your control notification handler code here
	CFileDialog nOpenDialog(true,"bin","characters",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL,this);
	int id;
	char *path = new char[256];
	CString p;

	id = nOpenDialog.DoModal();
	if(id == IDOK)
	{
		p = nOpenDialog.GetPathName();
		strcpy(path,p.GetBuffer(256));
		m_Static_Debug.SetWindowText(path);
		FILE *nIn;

		if((nIn = fopen(path,"rb")) == NULL)
		{
			char *s = new char[256];
			sprintf(s,"Could not open\n%s\nFor Input",path);
			MessageBox(s,"ERROR");
			delete[] s;
		}
		else
		{
			if(FontBuffer)
			{
				delete[] FontBuffer;
			}
			FontBuffer = new char[8192];

			m_NumFontBytes = fread(FontBuffer,1,8192,nIn);	//get font data from file
			m_ScrollBar_CharSel.SetScrollPos(0);
		}
	}
	delete[] path;
	m_Static_FontDisp.Invalidate();
}

void CFontEditDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	CFileDialog nOpenDialog(false,"bin","characters",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL,this);
	int id;
	char *path = new char[256];
	CString p;

	id = nOpenDialog.DoModal();
	if(id == IDOK)
	{
		p = nOpenDialog.GetPathName();
		strcpy(path,p.GetBuffer(256));
		m_Static_Debug.SetWindowText(path);
		FILE *nOut;
	
		if((nOut = fopen(path,"wb")) == NULL)
		{
			char *s = new char[256];
			sprintf(s,"Could not open\n%s\nfor output",path);
			MessageBox(s,"ERROR");
			delete[] s;
		}
		else
		{
			if(FontBuffer)
			{
				fwrite(FontBuffer,1,8192,nOut);
				fclose(nOut);
			}
		}
	}
}



LRESULT CFontEditDlg::OnNextFont(WPARAM wparam, LPARAM lparam)
{
	//**********************************
	// Font index is in wparam
	//**********************************
	if(FontBuffer)
	{
		char *s = new char[256];
		m_Static_FontDisp.SetChar(&FontBuffer[wparam * 8]);
		m_Static_FontDisp.Invalidate();
		sprintf(s,"%d  0x%03x",wparam,wparam&0x03ff);
		m_Static_CharSel.SetWindowText(s);
		delete[] s;
	}
	return 0;
}

void CFontEditDlg::OnButtonAbout() 
{
	// TODO: Add your control notification handler code here
	CAboutDlg AboutDialog;

	AboutDialog.DoModal();
}

void CFontEditDlg::OnButtonMove() 
{
	// TODO: Add your control notification handler code here
	//----------------------------------------
	// move the characters around in the font
	// buffer...this is actuall copy
	//----------------------------------------
	char *Source,*Dest;
	int Src,Dst,Cnt;

	char *s = new char[256];
	m_Edit_Source.GetWindowText(s,256);
	Src = atoi(s) * 8;
	m_Edit_Dest.GetWindowText(s,256);
	Dst = atoi(s) * 8;
	m_Edit_Count.GetWindowText(s,256);
	Cnt = atoi(s) * 8;

	char *t = new char[Cnt];

	Dest = &FontBuffer[Dst];
	Source = &FontBuffer[Src];

	memcpy(t,Dest,Cnt);
	memcpy(Dest,Source,Cnt);

	delete[] t;
	delete[] s;
	m_Static_FontDisp.Invalidate();
}

void CFontEditDlg::OnButtonInvert() 
{
	// TODO: Add your control notification handler code here
	char *Source;
	int Src,Cnt,i;

	char *s = new char[256];
	m_Edit_Source.GetWindowText(s,256);
	Src = atoi(s) * 8;
	m_Edit_Count.GetWindowText(s,256);
	Cnt = atoi(s) * 8;

	Source = &FontBuffer[Src];
	for(i=0;i<Cnt;++i)
		*Source++ ^= 0xff;
	delete[] s;
	m_Static_FontDisp.Invalidate();
}

void CFontEditDlg::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	
	char *Source;
	int Src,Cnt,i;

	char *s = new char[256];
	m_Edit_Source.GetWindowText(s,256);
	Src = atoi(s) * 8;
	m_Edit_Count.GetWindowText(s,256);
	Cnt = atoi(s) * 8;

	Source = &FontBuffer[Src];
	for(i=0;i<Cnt;++i)
		*Source++ = 0;
	delete[] s;
	m_Static_FontDisp.Invalidate();
}

void CFontEditDlg::OnButtonSwap() 
{
	// TODO: Add your control notification handler code here
	char *Source,*Dest;
	int Src,Dst,Cnt;

	char *s = new char[256];
	m_Edit_Source.GetWindowText(s,256);
	Src = atoi(s) * 8;
	m_Edit_Dest.GetWindowText(s,256);
	Dst = atoi(s) * 8;
	m_Edit_Count.GetWindowText(s,256);
	Cnt = 8 * atoi(s);

	char *t = new char[Cnt];

	Dest = &FontBuffer[Dst];
	Source = &FontBuffer[Src];

	memcpy(t,Dest,Cnt);
	memcpy(Dest,Source,Cnt);
	memcpy(Source,t,Cnt);

	delete[] t;
	delete[] s;
	m_Static_FontDisp.Invalidate();
}
