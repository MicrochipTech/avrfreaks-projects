// SettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "SettingsDialog.h"
#include "Comms.h"
#include "Globals.h"


/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog dialog


CSettingsDialog::CSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDialog::IDD, pParent)
{
	sfrFlags = NULL;
	sfrNames = NULL;
	sfrMarkers = NULL;
	//{{AFX_DATA_INIT(CSettingsDialog)
	//}}AFX_DATA_INIT
}


void CSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDialog)
	DDX_Control(pDX, IDC_SETTINGS_STACK, m_stackAddress);
	DDX_Control(pDX, IDC_SETTINGS_EXMEMSTARTTEXT, m_externalMemStartText);
	DDX_Control(pDX, IDC_SETTINGS_EXMEMSTART, m_externalMemStart);
	DDX_Control(pDX, IDC_SETTINGS_EXMEMSIZETEXT, m_externalMemSizeText);
	DDX_Control(pDX, IDC_SETTINGS_EXMEMSIZE, m_externalMemSize);
	DDX_Control(pDX, IDC_SETTINGS_EXTERNALMEM, m_externalMem);
	DDX_Control(pDX, IDC_SETTINGS_SFRSFLAGS, m_sfrsFlags);
	DDX_Control(pDX, IDC_SETTINGS_CPUPACKAGE, m_cpuPackage);
	DDX_Control(pDX, IDC_SETTINGS_CPUCPU, m_cpuCPU);
	DDX_Control(pDX, IDC_SETTINGS_COMMSBAUD, m_commsBaud);
	DDX_Control(pDX, IDC_SETTINGS_COMMSPORT, m_commsPort);
	DDX_Control(pDX, IDC_SETTINGS_COMMSCUSTOM, m_commsCustom);
	DDX_Control(pDX, IDC_SETTINGS_COMMSCHECK, m_commsCheckBox);
	DDX_Control(pDX, IDC_SETTINGS_SFRSLIST, m_list);
	DDX_Control(pDX, IDC_SETTINGS_TAB, m_tab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CSettingsDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SETTINGS_TAB, OnSelchangeSettingsTab)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_SETTINGS_COMMSCHECK, OnSettingsCommscheck)
	ON_CBN_SELCHANGE(IDC_SETTINGS_COMMSBAUD, OnSelchangeSettingsCommsbaud)
	ON_CBN_SELCHANGE(IDC_SETTINGS_CPUCPU, OnSelchangeSettingsCpucpu)
	ON_CBN_SELCHANGE(IDC_SETTINGS_CPUPACKAGE, OnSelchangeSettingsCpupackage)
	ON_BN_CLICKED(IDC_SETTINGS_SFRSSELECT, OnSettingsSfrsselect)
	ON_BN_CLICKED(IDC_SETTINGS_SFRSCLEAR, OnSettingsSfrsclear)
	ON_BN_CLICKED(IDC_SETTINGS_SFRSFLAGS, OnSettingsSfrsflags)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SETTINGS_EXTERNALMEM, OnSettingsExternalmem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog message handlers


BOOL CSettingsDialog::OnInitDialog() 
{
	int i;
	int item;
	CDialog::OnInitDialog();
	char buffer[10];
	LRESULT dummy;

	port = project->selectedComPort;
	baud = project->selectedBaud;
	chip = project->GetChip();
	package = project->GetPackage();
	flags = project->SfrFlagsVisible();
	numSFRs = CLGetNumberSFRs(chip);
	sfrFlags = new int[numSFRs];
	for (i=0; i<numSFRs; i++)
		sfrFlags[i] = project->GetSfrFlag(i);
	sfrNameLength = CLGetSfrNameLength();
	i = 0;
	CLGetSfrNames(NULL, chip, &i);
	sfrNames = new char[i];
	CLGetSfrNames(sfrNames, chip, &i);
	sfrMarkers = new int[numSFRs];
	CLGetSfrMarkers(sfrMarkers, chip, &numSFRs);
	externalMem = project->IsUsingExternalMem();
	externalMemStart = project->GetExternalMemStart();
	externalMemSize = project->GetExternalMemSize();
	stackAdr = stackAddress;
	sizing = true;

	// Set the Tabs
	m_tab.InsertItem(0, "Comms");
	m_tab.InsertItem(1, "CPU");
	m_tab.InsertItem(2, "SFRs");
	m_tab.InsertItem(3, "Memory");
	m_tab.InsertItem(4, "Stack");

	m_tab.SetCurSel(0);					// First Tab
	OnSelchangeSettingsTab(NULL, &dummy);

	// Set the Comms controls
	strcpy(buffer, "COM");
	for (i=0; i<numComPorts; i++)
	{
		itoa(comPortIndexes[i]+1, &buffer[3], 10);
		m_commsPort.AddString(buffer);
	}
	itoa(port, &buffer[3], 10);
	m_commsPort.SelectString(-1, buffer);

	m_commsBaud.AddString("9600");
	m_commsBaud.AddString("14400");
	m_commsBaud.AddString("19200");
	m_commsBaud.AddString("28800");
	m_commsBaud.AddString("38400");
	m_commsBaud.AddString("56000");
	m_commsBaud.AddString("57600");
	m_commsBaud.AddString("115200");
	m_commsBaud.AddString("128000");
	m_commsBaud.AddString("230400");
	m_commsBaud.AddString("256000");
	m_commsBaud.SetCurSel(7);
	m_commsCheckBox.SetCheck(0);
	m_commsCustom.EnableWindow(FALSE);
	switch(baud)
	{
		case 9600:
			m_commsBaud.SetCurSel(0);
			break;
		case 14400:
			m_commsBaud.SetCurSel(1);
			break;
		case 19200:
			m_commsBaud.SetCurSel(2);
			break;
		case 28800:
			m_commsBaud.SetCurSel(3);
			break;
		case 38400:
			m_commsBaud.SetCurSel(4);
			break;
		case 56000:
			m_commsBaud.SetCurSel(5);
			break;
		case 57600:
			m_commsBaud.SetCurSel(6);
			break;
		case 115200:
			m_commsBaud.SetCurSel(7);
			break;
		case 128000:
			m_commsBaud.SetCurSel(8);
			break;
		case 230400:
			m_commsBaud.SetCurSel(9);
			break;
		case 256000:
			m_commsBaud.SetCurSel(10);
			break;
		default:						// Custom baud
			m_commsCheckBox.SetCheck(1);
			m_commsCustom.EnableWindow(TRUE);
			itoa(baud, buffer, 10);
			m_commsCustom.SetWindowText(buffer);
			break;
	}

	// Set the CPU controls
	for (i=0; i<numberChips; i++)
		m_cpuCPU.AddString((char *)&chipNames[chipIDs[i]*chipNameLength]);
	m_cpuCPU.SetCurSel(CLGetChipIdIndex(chip));

	// Set the SFRs controls
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES|
		LVS_EX_TWOCLICKACTIVATE);
	m_list.InsertColumn(0, "Show", LVCFMT_CENTER, 40);
	m_list.InsertColumn(1, "SFR", LVCFMT_LEFT, 60);
	m_list.InsertColumn(2, "Address", LVCFMT_CENTER, 50);

	if (project->SfrFlagsVisible())
		m_sfrsFlags.SetCheck(1);
	else
		m_sfrsFlags.SetCheck(0);
	for (i=0; i<numSFRs; i++)
	{
		if (sfrMarkers[i])
		{
			item = m_list.InsertItem(m_list.GetItemCount(), "");
			m_list.SetCheck(item, sfrFlags[i]);
			m_list.SetItemText(item, 1, &sfrNames[i*sfrNameLength]);
			strcpy(buffer, "$");
			itoh(i, buffer, false, 2);
			m_list.SetItemText(item, 2, buffer);
			m_list.SetItemData(item, (DWORD)i);
		}
	}

	// Now set the CPU (needed the SFRs set first)
	OnSelchangeSettingsCpucpu();		// Set the package options
	package = project->GetPackage();
	m_cpuPackage.SetCurSel(CLGetPackageIndex(chip, package));

	// Set the Memory controls
	if (externalMem)
	{
		m_externalMem.SetCheck(1);
		strcpy(buffer, "0x");
		itoh(externalMemStart, buffer, false, 4);
		m_externalMemStart.SetWindowText(buffer);
		buffer[2] = 0;
		itoh(externalMemSize, buffer, false, 4);
		m_externalMemSize.SetWindowText(buffer);
	}
	else
		m_externalMem.SetCheck(0);

	// Set the Stack control
	strcpy(buffer, "0x");
	itoh(stackAdr, buffer, false, 4);
	m_stackAddress.SetWindowText(buffer);

	return TRUE;
}


void CSettingsDialog::OnSelchangeSettingsTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	CWnd * control;
	int i, last;

	switch(m_tab.GetCurSel())
	{
		case 0:							// Comms
		{
			for (i=0; i<(IDC_SETTINGS_CPULAST-IDC_SETTINGS_CPUFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_CPUFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_SFRSLAST-IDC_SETTINGS_SFRSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_SFRSFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_MEMLAST-IDC_SETTINGS_MEMFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_MEMFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_STACKLAST-IDC_SETTINGS_STACKFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_STACKFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_COMMSLAST-IDC_SETTINGS_COMMSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_COMMSFIRST+i);
				if (control)
					control->ShowWindow(SW_SHOW);
			}
			break;
		}

		case 1:							// CPU
		{
			for (i=0; i<(IDC_SETTINGS_COMMSLAST-IDC_SETTINGS_COMMSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_COMMSFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_SFRSLAST-IDC_SETTINGS_SFRSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_SFRSFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_MEMLAST-IDC_SETTINGS_MEMFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_MEMFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_STACKLAST-IDC_SETTINGS_STACKFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_STACKFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_CPULAST-IDC_SETTINGS_CPUFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_CPUFIRST+i);
				if (control)
					control->ShowWindow(SW_SHOW);
			}
			break;
		}

		case 2:							// SFRs
		{
			for (i=0; i<(IDC_SETTINGS_COMMSLAST-IDC_SETTINGS_COMMSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_COMMSFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_CPULAST-IDC_SETTINGS_CPUFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_CPUFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_MEMLAST-IDC_SETTINGS_MEMFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_MEMFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_STACKLAST-IDC_SETTINGS_STACKFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_STACKFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_SFRSLAST-IDC_SETTINGS_SFRSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_SFRSFIRST+i);
				if (control)
					control->ShowWindow(SW_SHOW);
			}
			break;
		}

		case 3:							// Memory
		{
			for (i=0; i<(IDC_SETTINGS_COMMSLAST-IDC_SETTINGS_COMMSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_COMMSFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_CPULAST-IDC_SETTINGS_CPUFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_CPUFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_SFRSLAST-IDC_SETTINGS_SFRSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_SFRSFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_STACKLAST-IDC_SETTINGS_STACKFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_STACKFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			if (externalMem)
				last = IDC_SETTINGS_MEMLAST;
			else
				last = IDC_SETTINGS_MEMFIRST;
			for (i=0; i<(last-IDC_SETTINGS_MEMFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_MEMFIRST+i);
				if (control)
					control->ShowWindow(SW_SHOW);
			}
			break;
		}
		case 4:							// Stack
		{
			for (i=0; i<(IDC_SETTINGS_COMMSLAST-IDC_SETTINGS_COMMSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_COMMSFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_CPULAST-IDC_SETTINGS_CPUFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_CPUFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_MEMLAST-IDC_SETTINGS_MEMFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_MEMFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_SFRSLAST-IDC_SETTINGS_SFRSFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_SFRSFIRST+i);
				if (control)
					control->ShowWindow(SW_HIDE);
			}
			for (i=0; i<(IDC_SETTINGS_STACKLAST-IDC_SETTINGS_STACKFIRST)+1; i++)
			{
				control = (CWnd *)GetDlgItem(IDC_SETTINGS_STACKFIRST+i);
				if (control)
					control->ShowWindow(SW_SHOW);
			}
			break;
		}
	}

	*pResult = 0;
}

void CSettingsDialog::OnPaint() 
{
//	CRect rect;
	CPaintDC dc(this); // device context for painting

/*	if (sizing)
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
		m_list.UpdateWindow();
	}
*/
}


void CSettingsDialog::OnSettingsCommscheck() 
{
	if (m_commsCheckBox.GetCheck())
		m_commsCustom.EnableWindow(TRUE);
	else
		m_commsCustom.EnableWindow(FALSE);
}


void CSettingsDialog::OnSelchangeSettingsCommsbaud() 
{
	m_commsCheckBox.SetCheck(0);
	m_commsCustom.EnableWindow(FALSE);
}


void CSettingsDialog::OnOK() 
{
	char buffer[500];
	int i;
	int index;

	// Get comms selections
	port = comPortIndexes[m_commsPort.GetCurSel()]+1;
	m_commsBaud.GetWindowText(buffer, 100);
	if (m_commsCheckBox.GetCheck())
		m_commsCustom.GetWindowText(buffer, 100);
	baud = atoi(buffer);
	
	// If they have changed then re-open the port
	if (project->selectedComPort != port ||
		project->selectedBaud != baud)
	{
		project->selectedComPort = port;
		project->selectedBaud = baud;
		if (ComPortOpen())
		{
			CloseComPort();
			OpenComPort();
		}
	}

	if (project->GetChip() != chip ||
		project->GetPackage() != package)
	{
		if (targetCPU != -1 && targetCPU != chip)
		{
			strcpy(buffer, "Your target is not a");
			project->GetChipName(chip, &buffer[400], 100);
			if (buffer[400] == 'A' || buffer[400] == 'E' ||
				buffer[400] == 'I' || buffer[400] == 'O' ||
				buffer[400] == 'U')
			{
				strcat(buffer, "n");
			}
			strcat(buffer, " ");
			strcat(buffer, &buffer[400]);
			strcat(buffer, ". Continue?");
			if (AfxMessageBox(buffer, MB_OKCANCEL|MB_ICONQUESTION, 0) !=
				IDOK)
			{
				return;
			}
		}
		project->SetChip(chip);
		project->SetPackage(package);
		if (chipWindowOpen)
			chipWindow->Refresh();
	}

	for (i=0; i<m_list.GetItemCount(); i++)
	{
		index = m_list.GetItemData(i);
		if (m_list.GetCheck(i))
			sfrFlags[index] = TRUE;
		else
			sfrFlags[index] = FALSE;
	}
	for (i=0; i<numSFRs; i++)
		project->SetSfrFlag(i, sfrFlags[i]);
	if (project->SfrFlagsVisible() != flags)
		project->ShowSfrFlags(flags);
	if (sfrsWindowOpen)
		sfrsWindow->Refresh();

	if (sfrFlags)
		delete sfrFlags;
	sfrFlags = NULL;
	if (sfrNames)
		delete sfrNames;
	sfrNames = NULL;
	if (sfrMarkers)
		delete sfrMarkers;
	sfrMarkers = NULL;

	m_externalMemStart.GetWindowText(buffer, 100);
	int start = ahtoi(buffer);
	m_externalMemSize.GetWindowText(buffer, 100);
	int size = ahtoi(buffer);
	if (!externalMem)
	{
		start = 0;
		size = 0;
	}
	if (project->IsUsingExternalMem() && externalMem)
	{
		if (start != project->GetExternalMemStart() ||
			size != project->GetExternalMemSize())
		{
			project->SetExternalMem(start, size);
		}
	}
	if (project->IsUsingExternalMem() != externalMem)
		project->SetExternalMem(start, size);

	m_stackAddress.GetWindowText(buffer, 100);
	stackAddress = ahtoi(buffer);

	project->Changed();

	CDialog::OnOK();
}


void CSettingsDialog::OnSelchangeSettingsCpucpu() 
{
	int i, j;
	int index;
	int item;
	char buffer[200];
	int oldChip = chip;

	for (i=0; i<m_list.GetItemCount(); i++) // For each list item
	{
		index = m_list.GetItemData(i);	// Get SFR index
		if (m_list.GetCheck(i))			// If its checked
			sfrFlags[index] = TRUE;		// Select it!
		else
			sfrFlags[index] = FALSE;
	}

	chip = chipIDs[m_cpuCPU.GetCurSel()]; // New chip ID

	int * packages = new int[numberPackages];
	CLGetAvailablePackages(packages, &numberPackages, chip); // Avail packages 
	m_cpuPackage.ResetContent();
	for (i=0; i<numberPackages; i++)
	{									// Add the package names to the list
		if (packages[i] != -1)
			m_cpuPackage.AddString(
				(char *)&packageNames[packages[i]*packageNameLength]);
	}
	delete packages;
	package = CLGetDefaultPackage(chip);
	m_cpuPackage.SetCurSel(CLGetPackageIndex(chip, package));

	m_list.DeleteAllItems();			// Clear the SFR list
	int numOldSFRs = numSFRs;
	int * oldSfrFlags = sfrFlags;
	numSFRs = CLGetNumberSFRs(chip);	// Number SFR flags in new chip
	sfrFlags = new int[numSFRs];
	for (i=0; i<numSFRs; i++)
		sfrFlags[i] = FALSE;			// Not selected is default

	char * oldSfrNames = sfrNames;
	i = 0;
	CLGetSfrNames(NULL, chip, &i);		// Get needed size
	sfrNames = new char[i];
	CLGetSfrNames(sfrNames, chip, &i);	// Get names

	for (i=0; i<numOldSFRs; i++)		// For each old SFR
	{
		if (oldSfrFlags[i])				// If SFR was selected
		{
			for (j=0; j<numSFRs; j++)	// For each new SFR
			{
				if (strcmp((char *)&oldSfrNames[i*sfrNameLength],
					(char *)&sfrNames[j*sfrNameLength]) == 0) // If same SFR
				{
					sfrFlags[j] = TRUE;	// Select it
				}
			}
		}
	}

	if (oldSfrFlags)
		delete oldSfrFlags;
	if (oldSfrNames)
		delete oldSfrNames;
	
	if (sfrMarkers)
		delete sfrMarkers;
	sfrMarkers = new int[numSFRs];
	CLGetSfrMarkers(sfrMarkers, chip, &numSFRs);
	for (i=0; i<numSFRs; i++)
	{
		if (sfrMarkers[i])
		{
			item = m_list.InsertItem(m_list.GetItemCount(), "");
			m_list.SetCheck(item, sfrFlags[i]);
			m_list.SetItemText(item, 1, &sfrNames[i*sfrNameLength]);
			strcpy(buffer, "$");
			itoh(i, buffer, false, 2);
			m_list.SetItemText(item, 2, buffer);
			m_list.SetItemData(item, (DWORD)i);
		}
	}
}


void CSettingsDialog::OnSelchangeSettingsCpupackage() 
{
	int * packages = new int[numberPackages];
	CLGetAvailablePackages(packages, &numberPackages, chip); // Avail packages
	package = packages[m_cpuPackage.GetCurSel()];
	delete packages;
}


void CSettingsDialog::OnSettingsSfrsselect() 
{
	int i;

	for (i=0; i<numSFRs; i++)
		sfrFlags[i] = TRUE;
	for (i=0; i<m_list.GetItemCount(); i++)
		m_list.SetCheck(i, TRUE);
}


void CSettingsDialog::OnSettingsSfrsclear() 
{
	int i;

	for (i=0; i<numSFRs; i++)
		sfrFlags[i] = FALSE;
	for (i=0; i<m_list.GetItemCount(); i++)
		m_list.SetCheck(i, FALSE);
}


void CSettingsDialog::OnSettingsSfrsflags() 
{
	if (m_sfrsFlags.GetCheck() && !project->SfrFlagsVisible())
		flags = TRUE;
	if (!m_sfrsFlags.GetCheck() && project->SfrFlagsVisible())
		flags = FALSE;
}


void CSettingsDialog::OnSettingsExternalmem() 
{
	char buffer[100];

	if (m_externalMem.GetCheck())
	{
		externalMem = TRUE;
		strcpy(buffer, "0x");
		itoh(externalMemStart, buffer, false, 4);
		m_externalMemStart.SetWindowText(buffer);
		buffer[2] = 0;
		itoh(externalMemSize, buffer, false, 4);
		m_externalMemSize.SetWindowText(buffer);
		m_externalMemStartText.ShowWindow(SW_SHOW);
		m_externalMemStart.ShowWindow(SW_SHOW);
		m_externalMemSizeText.ShowWindow(SW_SHOW);
		m_externalMemSize.ShowWindow(SW_SHOW);
	}
	else
	{
		externalMem = FALSE;
		m_externalMemStartText.ShowWindow(SW_HIDE);
		m_externalMemStart.ShowWindow(SW_HIDE);
		m_externalMemSizeText.ShowWindow(SW_HIDE);
		m_externalMemSize.ShowWindow(SW_HIDE);
	}
}


void CSettingsDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	if (sfrFlags)
		delete sfrFlags;
	if (sfrNames)
		delete sfrNames;
	if (sfrMarkers)
		delete sfrMarkers;
}
