// LicenseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Studio.h"

#include "LicenseDialog.h"

#include "Globals.h"

//#pragma warning(disable: 4228)
#include <mapi.h>
//#pragma warning(default: 4228)

#ifdef DEBUG
#include "fstream.h"
extern ofstream debug;
#endif


/////////////////////////////////////////////////////////////////////////////
// CLicenseDialog dialog


CLicenseDialog::CLicenseDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLicenseDialog::IDD, pParent)
{
	newOptions = 0;
	newCPUs = 0;
	newIDEs = 0;
	//{{AFX_DATA_INIT(CLicenseDialog)
	//}}AFX_DATA_INIT
}


void CLicenseDialog::UpdateOptions()
{
	if (globals.options < SS_ICESUPPORT)
	{
		if (newOptions >= SS_ICESUPPORT) // Want "ICE Support"
			m_ICEWant.SetCheck(1);
		else
			m_ICEWant.SetCheck(0);
	}
	else
		m_ICEWant.SetCheck(1);

	if (globals.options < SS_ENTERPRISE)
	{
		if (newOptions >= SS_ENTERPRISE) // Want "Enterprise"
			m_EntWant.SetCheck(1);
		else
			m_EntWant.SetCheck(0);
	}
	else
		m_EntWant.SetCheck(1);

	if (globals.options < SS_PROFESSIONAL)
	{
		if (newOptions >= SS_PROFESSIONAL) // Want "Professional"
			m_ProfWant.SetCheck(1);
		else
			m_ProfWant.SetCheck(0);
	}
	else
		m_ProfWant.SetCheck(1);

	if (globals.options < SS_BASIC)
	{
		if (newOptions >= SS_BASIC)		// Want "Basic"
			m_BasicWant.SetCheck(1);
		else
			m_BasicWant.SetCheck(0);
	}
	else
		m_BasicWant.SetCheck(1);
}


void CLicenseDialog::UpdateCPUs()
{
//	if (newCPUs&0x100)					// All CPUs
//		m_CPUs.SelItemRange(TRUE, 0, m_CPUs.GetCount()-1);
//	else								// Selected CPUs
//	{
//		for (int i=0; i<m_CPUs.GetCount(); i++)
//		{
//			if ((newCPUs == i+1) || (globals.cpus == i+1))
//				m_CPUs.SetSel(i, TRUE);
//			else
//				m_CPUs.SetSel(i, FALSE);
//		}
//	}
	if (!(newCPUs&0x100))				// If not universal
		m_CPUs.SetCurSel(newCPUs-1);
}


void CLicenseDialog::UpdateIDEs()
{
	for (int i=0; i<m_IDEs.GetCount(); i++) // For all IDE list items
	{
		if ((newIDEs&m_IDEs.GetItemData(i)) || // If item selected
			(globals.ides&m_IDEs.GetItemData(i))) // or IDE is registered
		{
			m_IDEs.SetSel(i, TRUE);		// Select it
		}
		else							// If item not selected or IDE not reg
			m_IDEs.SetSel(i, FALSE);	// Deselect it
	}
}


void CLicenseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseDialog)
	DDX_Control(pDX, IDC_LICENSE_ANIM, m_animation);
	DDX_Control(pDX, IDC_LICENSE_CODE, m_code);
	DDX_Control(pDX, IDC_LICENSE_IN, m_unlockCode);
	DDX_Control(pDX, IDC_LICENSE_CPUS, m_CPUs);
	DDX_Control(pDX, IDC_LICENSE_IDES, m_IDEs);
	DDX_Control(pDX, IDC_LICENSE_BASIC, m_Basic);
	DDX_Control(pDX, IDC_LICENSE_BASICWANT, m_BasicWant);
	DDX_Control(pDX, IDC_LICENSE_PROF, m_Prof);
	DDX_Control(pDX, IDC_LICENSE_PROFWANT, m_ProfWant);
	DDX_Control(pDX, IDC_LICENSE_ENT, m_Ent);
	DDX_Control(pDX, IDC_LICENSE_ENTWANT, m_EntWant);
	DDX_Control(pDX, IDC_LICENSE_ICE, m_ICE);
	DDX_Control(pDX, IDC_LICENSE_ICEWANT, m_ICEWant);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLicenseDialog, CDialog)
	//{{AFX_MSG_MAP(CLicenseDialog)
	ON_BN_CLICKED(IDC_LICENSE_LICENSE, OnLicenseLicense)
	ON_BN_CLICKED(IDCLOSE, OnClose)
	ON_BN_CLICKED(IDC_LICENSE_FROMDISK, OnLicenseFromdisk)
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(IDC_LICENSE_CPUS, OnSelchangeLicenseCpus)
	ON_LBN_SELCHANGE(IDC_LICENSE_IDES, OnSelchangeLicenseIdes)
	ON_BN_CLICKED(IDC_LICENSE_EMAIL, OnLicenseEmail)
	ON_BN_CLICKED(IDC_LICENSE_BASICWANT, OnLicenseBasicwant)
	ON_BN_CLICKED(IDC_LICENSE_PROFWANT, OnLicenseProfwant)
	ON_BN_CLICKED(IDC_LICENSE_ENTWANT, OnLicenseEntwant)
	ON_BN_CLICKED(IDC_LICENSE_ICEWANT, OnLicenseICEwant)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseDialog message handlers

BOOL CLicenseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_animation.Open(IDA_ABOUT);

	customerCode = safeSerial->CustomerCode();
	m_code.SetWindowText(customerCode);
	m_code.SetSel(0, -1, FALSE);
	m_code.Copy();
	m_code.SetSel(-1, 0, FALSE);

	if (globals.universal)				// Enable everything
	{
		m_Basic.SetCheck(1);
		m_BasicWant.SetCheck(1);
		m_BasicWant.EnableWindow(FALSE);
		m_Prof.SetCheck(1);
		m_ProfWant.SetCheck(1);
		m_ProfWant.EnableWindow(FALSE);
		m_Ent.SetCheck(1);
		m_EntWant.SetCheck(1);
		m_EntWant.EnableWindow(FALSE);
		m_ICE.SetCheck(1);
		m_ICEWant.SetCheck(1);
		m_ICEWant.EnableWindow(FALSE);

		m_CPUs.EnableWindow(FALSE);
		m_IDEs.EnableWindow(FALSE);
	}
	else
	{
		newOptions = globals.options;
		newCPUs = globals.cpus;
		newIDEs = globals.ides;

		if (newOptions >= SS_ICESUPPORT) // Have "ICE Support"
		{
			m_ICE.SetCheck(1);
			m_ICEWant.EnableWindow(FALSE);
		}
		else
			m_ICE.SetCheck(0);
		if (newOptions >= SS_ENTERPRISE) // Have "Enterprise"
		{
			m_Ent.SetCheck(1);
			m_EntWant.EnableWindow(FALSE);
		}
		else
			m_Ent.SetCheck(0);
		if (newOptions >= SS_PROFESSIONAL) // Have "Professional"
		{
			m_Prof.SetCheck(1);
			m_ProfWant.EnableWindow(FALSE);
		}
		else
			m_Prof.SetCheck(0);
		if (newOptions >= SS_BASIC)		// Have "Basic"
		{
			m_Basic.SetCheck(1);
			m_BasicWant.EnableWindow(FALSE);
		}
		else
			m_Basic.SetCheck(0);
		UpdateOptions();

		m_CPUs.InsertString(0, "Atmel AVR");
		UpdateCPUs();

		for (int i=0; i<numberIDEs; i++)
		{
			m_IDEs.InsertString(i, ideNames[i*2]);
			m_IDEs.SetItemData(i, ideIDs[i]);
		}
//		m_IDEs.InsertString(0, "Atmel Studio (asm)");
//		m_IDEs.InsertString(1, "ImageCraft");
//		m_IDEs.InsertString(2, "GNU AVR GCC");
//		m_IDEs.InsertString(3, "CodeVision");
//		m_IDEs.InsertString(4, "IAR Embedded Workbench");
		UpdateIDEs();
	}

	return TRUE;
}


void CLicenseDialog::OnLicenseLicense()
{
	CString unlockCode;
	char buffer[1000];

	CWaitCursor * wait = new CWaitCursor();
	m_unlockCode.GetWindowText(unlockCode);
	CString confCode = safeSerial->ValidateCode(unlockCode);
	delete wait;
	int error = safeSerial->ResetError();
	if (error == 0)						// Success
	{
		strcpy(buffer,
"Thankyou for registering this copy of Studio+.\r\n\
Everybody at Flash welcomes you as a valued\r\n\
customer and hopes Studio+ will give you all\r\n\
the help it can with your projects.\r\n\r\n");
		strcat(buffer, "Your Confirmation Code is: ");
		strcat(buffer, confCode);
		strcat(buffer, "\r\n\
Please make a note of it if you have been\r\n\
asked to supply it.\r\n\r\n\
All the best,\r\nThe Flash Team");
		::MessageBox(NULL, buffer, "Thank you for registering",
			MB_OK|MB_ICONINFORMATION);
	}
	else if (error == 10)
	{
		::MessageBox(NULL, "You haven't entered an Unlock Code!!",
			"Missing Code!", MB_OK|MB_ICONEXCLAMATION);
	}
	else if (error == 11 || error == 12)
	{
		::MessageBox(NULL, "The Unlock-Code you entered is bad.\r\n\r\n\
Possible reasons are:\r\n\
 * It may have been mis-typed.\r\n\
 * You have reinstalled Studio+ after you sent\r\n\
    your Customer Code (displayed in red) to Flash.\r\n\
 * It may be for another application.",
		"Bad Code", MB_OK|MB_ICONEXCLAMATION);
	}
	if (error == 0)
		CDialog::OnOK();
}


void CLicenseDialog::OnClose()
{
	CDialog::OnCancel();
}


void CLicenseDialog::OnLicenseFromdisk() 
{
	char buffer[1000];
	char title[1000];
	int type;

	CWaitCursor * wait = new CWaitCursor();
	safeSerial->SafeImport();
	delete wait;

	int error = safeSerial->ResetError();
	if (error == 0)						// Success
	{
		strcpy(title, "License imported");
		strcpy(buffer,
"Your license has been transfered from disk\r\n\
to this computer. After Studio+ has closed,\r\n\
restart it to access the registered options.");
		type = MB_OK;
	}
	else if (error == 201)				// Invalid License
	{
		strcpy(title, "Invalid License");
		strcpy(buffer,
"The license on this disk is invalid.\r\n\
Studio+ uses a secure method of transfering\r\n\
it's license to and from disks. These disks\r\n\
must not be used for any other purpose\r\n\
while they have a license on them.");
		type = MB_OK|MB_ICONSTOP;
	}
	else if (error == 904)				// No access
	{
		strcpy(title, "Bad Disk");
		strcpy(buffer,
"Studio+ cannot read it's license from this disk.\r\n\
please make sure the disk is in the drive and\r\n\
that it is NOT write protected."); 
		type = MB_OK|MB_ICONSTOP;
	}
	if (error == 0 || error == 201 || error == 904)
		::MessageBox(NULL, buffer, title, type);
	if (error == 0)
		CDialog::OnOK();
}


HBRUSH CLicenseDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	CString windowText;

	pWnd->GetWindowText(windowText);
	if (windowText.Compare("Send Email") == 0)
		pDC->SetTextColor(0x00FF0000);	
	if (windowText.Compare(customerCode) == 0)
		pDC->SetTextColor(0x000000FF);	
	
	return hbr;
}


void CLicenseDialog::OnLicenseEmail() 
{
	CWaitCursor wait;

	HINSTANCE dll = ::LoadLibraryA("MAPI32.DLL"); // Get dll
	if (dll == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
		return;
	}
	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)lpfnSendMail = GetProcAddress(dll, "MAPISendMail");
	if (lpfnSendMail == NULL)
	{
		AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
		return;
	}

	// prepare the message
	char recipientName[100] = "Flash Microsystems";
	char recipientAddress[100] = "SMTP:";
	strcat(recipientAddress, STRING_FLASHEMAIL);
	char subject[100] = "Studio+ Registration";
	char noteText[2000];
	DWORD length;
	strcpy(noteText, "<HTML><BODY>");
	strcat(noteText, "My Name is: ");
	length = 2000-strlen(noteText);
	::GetUserName(&noteText[strlen(noteText)], &length);
	strcat(noteText, ".<BR>");
	strcat(noteText, "My Company is: \"Your Company\".<BR>");
	strcat(noteText, "My Phone Number is: \"Your Number\".<BR>");
	strcat(noteText, "My email address is: \"Email Address, if not sender's\".<BR>");
	strcat(noteText, "My Code is: \"");
	m_code.GetWindowText(&noteText[strlen(noteText)], 2000-strlen(noteText));
	strcat(noteText, "\".<BR>");

	strcat(noteText, "<P>My desired Option is:<BR>");
	if (newOptions >= SS_ICESUPPORT)
		strcat(noteText, "&nbsp&nbsp&nbsp ICE Support.<BR>");
	else if (newOptions >= SS_ENTERPRISE)
		strcat(noteText, "&nbsp&nbsp&nbsp Enterprise.<BR>");
	else if (newOptions >= SS_PROFESSIONAL)
		strcat(noteText, "&nbsp&nbsp&nbsp Professional.<BR>");
	else if (newOptions >= SS_BASIC)
		strcat(noteText, "&nbsp&nbsp&nbsp Basic.<BR>");
	strcat(noteText, "</P>");

	strcat(noteText, "<P>My desired CPU Family is:<BR>");
	if (newCPUs&0x100)
		strcat(noteText, "&nbsp&nbsp&nbsp All Familys.<BR>");
	else
	{
		if (newCPUs == 1)
			strcat(noteText, "&nbsp&nbsp&nbsp Atmel AVR.<BR>");
	}
	strcat(noteText, "</P>");

	strcat(noteText, "<P>My desired IDE support is:<BR>");
	for (int i=0; i<numberIDEs; i++)
	{
		if (newIDEs&ideIDs[i])
		{
			strcat(noteText, "&nbsp&nbsp&nbsp ");
			strcat(noteText, ideNames[i*2]);
			strcat(noteText, ".<BR>");
		}
	}
	strcat(noteText, "</P>");

	strcat(noteText, "<P>(For Flash use: ");
	int lt = newOptions|newCPUs;
	itoa(lt, &noteText[strlen(noteText)], 10);
	strcat(noteText, "/");
	itoa(newIDEs, &noteText[strlen(noteText)], 10);
	strcat(noteText, ")</P>");

	strcat(noteText, "</BODY></HTML>");

	MapiRecipDesc originator;
	memset(&originator, 0, sizeof(originator));
	originator.ulRecipClass = MAPI_ORIG;

	MapiRecipDesc recipient;
	memset(&recipient, 0, sizeof(recipient));
	recipient.ulRecipClass = MAPI_TO;
	recipient.lpszName = recipientName;
	recipient.lpszAddress = recipientAddress;

	MapiMessage message;
	memset(&message, 0, sizeof(message));
	message.lpOriginator = &originator;
	message.lpszSubject = subject;
	message.lpszNoteText = noteText;
	message.nFileCount = 0;
	message.lpFiles = NULL;
	message.nRecipCount = 1;
	message.lpRecips = &recipient;
	message.flFlags = MAPI_RECEIPT_REQUESTED;

	// prepare for modal dialog box
	AfxGetApp()->EnableModeless(FALSE);
	HWND hWndTop;
	CWnd* pParentWnd = CWnd::GetSafeOwner(NULL, &hWndTop);

	// some extra precautions are required to use MAPISendMail as it
	// tends to enable the parent window in between dialogs (after
	// the login dialog, but before the send note dialog).
	pParentWnd->SetCapture();
	::SetFocus(NULL);
	pParentWnd->m_nFlags |= WF_STAYDISABLED;

	int nError = lpfnSendMail(0, 0 /*(ULONG)pParentWnd->GetSafeHwnd()*/,
		&message, MAPI_LOGON_UI|MAPI_DIALOG|MAPI_NEW_SESSION, 0);

	// after returning from the MAPISendMail call, the window must
	// be re-enabled and focus returned to the frame to undo the workaround
	// done before the MAPI call.
	::ReleaseCapture();
	pParentWnd->m_nFlags &= ~WF_STAYDISABLED;

	pParentWnd->EnableWindow(TRUE);
	::SetActiveWindow(NULL);
	pParentWnd->SetActiveWindow();
	pParentWnd->SetFocus();
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
	AfxGetApp()->EnableModeless(TRUE);

	if (nError != SUCCESS_SUCCESS &&
		nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
	}
}


void CLicenseDialog::OnLicenseBasicwant() 
{
	if (m_BasicWant.GetCheck())
		newOptions = SS_BASIC;
	else
		newOptions = 0;
	UpdateOptions();
}


void CLicenseDialog::OnLicenseProfwant() 
{
	if (m_ProfWant.GetCheck())
		newOptions = SS_PROFESSIONAL;
	else
		newOptions = 0;
	UpdateOptions();
}


void CLicenseDialog::OnLicenseEntwant() 
{
	if (m_EntWant.GetCheck())
		newOptions = SS_ENTERPRISE;
	else
		newOptions = 0;
	UpdateOptions();
}


void CLicenseDialog::OnLicenseICEwant() 
{
	if (m_ICEWant.GetCheck())
		newOptions = SS_ICESUPPORT;
	else
		newOptions = 0;
	UpdateOptions();
}


void CLicenseDialog::OnSelchangeLicenseCpus() 
{
	newCPUs = 0;
	for (int i=0; i<m_CPUs.GetCount(); i++)
	{
		if (m_CPUs.GetSel(i))
			newCPUs = i+1;
	}
	UpdateCPUs();
}


void CLicenseDialog::OnSelchangeLicenseIdes() 
{
	for (int i=0; i<m_IDEs.GetCount(); i++)
	{
		if (m_IDEs.GetSel(i))			// If IDE's list item selected
		{								// Select the IDE
			newIDEs |= m_IDEs.GetItemData(i);
		}
		else							// If list item not selected
		{								// and it's not already registered
			if (!(globals.ides&m_IDEs.GetItemData(i)))
				newIDEs &= ~(m_IDEs.GetItemData(i)); // Deselect the IDE
		}
	}
	UpdateIDEs();
}
