// Studio.cpp

#include "stdafx.h"
#include "Studio.h"
#include <direct.h>						// _getcwd()

#include "MainFrm.h"
#include "ChildFrm.h"
#include "Comms.h"						// Comms functions
#include "SourceWindow.h"				// CSourceWindow class
#include "Registers.h"
#include "Memory.h"
#include "Stack.h"
#include "ProjectWizard.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"
#include "LicenseDialog.h"
#include "PluginsDialog.h"
#include "CommsTimeoutDialog.h"
#include "Symbol.h"

#define STUDIO
#include "Globals.h"					// Global Definitions


/////////////////////////////////////////////////////////////////////////////
// CStudioApp

BEGIN_MESSAGE_MAP(CStudioApp, CWinApp)
	//{{AFX_MSG_MAP(CStudioApp)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_PREFS_SOURCE_SYNTAXHI, OnPrefsSourceSyntaxhi)
	ON_UPDATE_COMMAND_UI(ID_PREFS_SOURCE_SYNTAXHI, OnUpdatePrefsSourceSyntaxhi)
	ON_COMMAND(ID_PREFS_SOURCE_TRACKPC, OnPrefsSourceTrackpc)
	ON_UPDATE_COMMAND_UI(ID_PREFS_SOURCE_TRACKPC, OnUpdatePrefsSourceTrackpc)
	ON_COMMAND(ID_FILE_OPENPROJECT, OnFileOpenproject)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENPROJECT, OnUpdateFileOpenproject)
	ON_COMMAND(ID_FILE_CLOSEPROJECT, OnFileCloseproject)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSEPROJECT, OnUpdateFileCloseproject)
	ON_COMMAND(ID_VIEW_PROJECT, OnViewProject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, OnUpdateViewProject)
	ON_COMMAND(ID_VIEW_REGISTERS, OnViewRegisters)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REGISTERS, OnUpdateViewRegisters)
	ON_COMMAND(ID_VIEW_MEMORY_SRAM, OnViewMemorySram)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MEMORY_SRAM, OnUpdateViewMemorySram)
	ON_COMMAND(ID_VIEW_MEMORY_EEPROM, OnViewMemoryEeprom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MEMORY_EEPROM, OnUpdateViewMemoryEeprom)
	ON_COMMAND(ID_PROJECT_SETTINGS, OnProjectSettings)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_SETTINGS, OnUpdateProjectSettings)
	ON_COMMAND(IDM_WINDOW_CASCADE, OnWindowCascade)
	ON_UPDATE_COMMAND_UI(IDM_WINDOW_CASCADE, OnUpdateWindowCascade)
	ON_COMMAND(ID_PROJECT_REFRESH, OnProjectRefresh)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_REFRESH, OnUpdateProjectRefresh)
	ON_COMMAND(ID_PREFS_SOURCE_CODE, OnPrefsSourceCode)
	ON_UPDATE_COMMAND_UI(ID_PREFS_SOURCE_CODE, OnUpdatePrefsSourceCode)
	ON_COMMAND(ID_VIEW_WATCH, OnViewWatch)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCH, OnUpdateViewWatch)
	ON_COMMAND(ID_VIEW_MEMORY_SFRS, OnViewMemorySfrs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MEMORY_SFRS, OnUpdateViewMemorySfrs)
	ON_COMMAND(ID_VIEW_STACK, OnViewStack)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STACK, OnUpdateViewStack)
	ON_COMMAND(IDM_VIEW_CHIP, OnViewChip)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_CHIP, OnUpdateViewChip)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_UPDATE_COMMAND_UI(ID_HELP_ABOUT, OnUpdateHelpAbout)
	ON_COMMAND(ID_HELP_REGISTER, OnHelpRegister)
	ON_UPDATE_COMMAND_UI(ID_HELP_REGISTER, OnUpdateHelpRegister)
	ON_COMMAND(IDM_HELP_EXPORT, OnHelpExport)
	ON_UPDATE_COMMAND_UI(IDM_HELP_EXPORT, OnUpdateHelpExport)
	ON_COMMAND(IDM_PREFS_SOURCE_ASM, OnPrefsSourceAsm)
	ON_UPDATE_COMMAND_UI(IDM_PREFS_SOURCE_ASM, OnUpdatePrefsSourceAsm)
	ON_COMMAND(IDM_PREFS_SOURCE_ADDRESS, OnPrefsSourceAddress)
	ON_UPDATE_COMMAND_UI(IDM_PREFS_SOURCE_ADDRESS, OnUpdatePrefsSourceAddress)
	ON_COMMAND(IDM_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(IDM_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(IDM_FILE_SAVEAS, OnFileSaveas)
	ON_UPDATE_COMMAND_UI(IDM_FILE_SAVEAS, OnUpdateFileSaveas)
	ON_COMMAND(ID_FILE_NEWPROJECT, OnFileNewproject)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEWPROJECT, OnUpdateFileNewproject)
	ON_COMMAND(ID_HELP_HELPTOPICS, OnHelpHelptopics)
	ON_UPDATE_COMMAND_UI(ID_HELP_HELPTOPICS, OnUpdateHelpHelptopics)
	ON_COMMAND(IDM_VIEW_MAINTOOLBAR, OnViewMaintoolbar)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_MAINTOOLBAR, OnUpdateViewMaintoolbar)
	ON_COMMAND(IDM_VIEW_TARGETCONTROLBAR, OnViewTargetcontrolbar)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_TARGETCONTROLBAR, OnUpdateViewTargetcontrolbar)
	ON_COMMAND(IDM_HELP_PLUGINS, OnHelpPlugins)
	ON_UPDATE_COMMAND_UI(IDM_HELP_PLUGINS, OnUpdateHelpPlugins)
	ON_COMMAND(IDM_PREFS_COMMSTIMEOUT, OnPrefsCommstimeout)
	ON_UPDATE_COMMAND_UI(IDM_PREFS_COMMSTIMEOUT, OnUpdatePrefsCommstimeout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only CStudioApp object

CStudioApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CStudioApp construction

CStudioApp::CStudioApp()
{
	HKEY localmachine;
	HKEY software;
	HKEY company;
	HKEY product;
	HKEY version;
	long regError;
	unsigned long length;
	int i;

#ifdef DEBUG
debug.open("debug.txt", ios::out);
debug<<"** Studio+ Debug file **"<<endl;
debug<<endl;
debug<<"Running on:"<<endl;
debug<<"   Windows Major Version: "<<_winmajor<<endl;
debug<<"   Windows Minor Version: "<<_winminor<<endl;
#endif

	numberIDEs = 0;
	numberFamilys = 0;
//	ideFamilyTable = new int[MAX_IDES*MAX_FAMILYS];

	familyIDs = NULL; //new int[MAX_FAMILYS];
	familyNames = NULL; //new char *[MAX_FAMILYS];
	familyVersions = NULL;
//	for (i=0; i<MAX_FAMILYES; i++)
//		familyNames[i] = NULL;

	ideIDs = NULL; //new int[MAX_IDES];
	ideNames = NULL; //new char *[MAX_IDES*2];
	ideVersions = NULL;
//	for (i=0; i<MAX_IDES*2; i++)
//		ideNames[i] = NULL;

	safeSerial = NULL;

	errorCode = SPERROR_SUCCESS;		// No errors!!
	errorMessage = new char[1000];
	errorMessage[0] = 0;

	// Global settings
	globals.workingDrive = _getdrive();
	globals.workingDirectory = new char[_MAX_PATH];
	_getcwd(globals.workingDirectory, _MAX_PATH);
	if (globals.workingDirectory[strlen(globals.workingDirectory)-1] != '\\')
		strcat(globals.workingDirectory, "\\");
//	globals.usersDrive = globals.workingDrive;
//	globals.usersDirectory = new char[_MAX_PATH];
//	strcpy(globals.usersDirectory, globals.workingDirectory);

	// License
	globals.registered = FALSE;
	globals.universal = FALSE;
	globals.options = 0;
	globals.cpus = 0;
	globals.ides = 0;

	// Global default preferences
	globals.syntaxHiLighting = TRUE;
	globals.pcTracking = TRUE;
	globals.assembler = FALSE;
	globals.addresses = FALSE;
	globals.machineCode = FALSE;

	globals.commsTimeout = 5000;		// 5 seconds

	// Initialise IDE info
	hIdeDll = NULL;

	// Initialise family info
	hFamilyDll = NULL;
	numberChips = 0;
	chipIDs = NULL;
	chipNames = NULL;
	numberPackages = 0;
	packageNames = NULL;

	// Set the current project to NULL
	project = NULL;

	// Init PC info
	pcFile = NULL;
	oldPCFile = NULL;
	pcLine = -1;

	// Set Stack info
	stackAddress = 0;
	stackPointer = 0;

	// Set debug state to dormant
	debugRunning = false;
	gettingData = false;
	endCondition = false;
	updateRegisters = false;
	updateSram = false;
	updateEeprom = false;
	updateStack = false;
	firstWatchSymbol = 0;
	lastWatchSymbol = 0;

	// Set default PC size (8 bit CPUs)
	pcSize = 2;

	// Initialise the data buffers
	registersSize = 0; //MAXREGISTERSSIZE;
	sfrsSize = 0; //MAXSFRSSIZE;
	sramStart = 0;
	sramSize = 0; //MAXSRAMSIZE;
	eepromStart = 0;
	eepromSize = 0; //MAXEEPROMSIZE;
	registers = NULL; //new unsigned char[registersSize];
	sfrs = NULL; //new unsigned char[sfrsSize];
	sram = NULL; //new unsigned char[sramSize];
	eeprom = NULL; //new unsigned char[eepromSize];
	registersMask = NULL; //new unsigned char[registersSize];
	sfrsMask = NULL; //new unsigned char[sfrsSize];
	sramMask = NULL; //new unsigned char[sramSize];
	eepromMask = NULL; //new unsigned char[eepromSize];

	// Get the install directory
	instDir = NULL;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ, &localmachine);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0, KEY_READ, &software);
	if (RegOpenKeyEx(software, COMPANYKEY, 0, KEY_READ, &company) !=
		ERROR_SUCCESS)
	{
		errorCode = SPERROR_NOTINSTALLED;
		ErrorMessage();
		RegCloseKey(software);
		RegCloseKey(localmachine);
		return;
	}
	if (RegOpenKeyEx(company, PRODUCTKEY, 0, KEY_READ, &product) !=
		ERROR_SUCCESS)
	{
		errorCode = SPERROR_NOTINSTALLED;
		ErrorMessage();
		RegCloseKey(company);
		RegCloseKey(software);
		RegCloseKey(localmachine);
		return;
	}
	if (RegOpenKeyEx(product, VERSIONKEY, 0, KEY_READ, &version) !=
		ERROR_SUCCESS)
	{
		errorCode = SPERROR_NOTINSTALLED;
		ErrorMessage();
		RegCloseKey(product);
		RegCloseKey(company);
		RegCloseKey(software);
		RegCloseKey(localmachine);
		return;
	}
	length = _MAX_PATH;
	instDir = new char[_MAX_PATH];
	RegQueryValueEx(version, "Install Directory", NULL, NULL,
		(unsigned char *)instDir, &length);
	RegCloseKey(version);
	RegCloseKey(product);
	RegCloseKey(company);
	RegCloseKey(software);
	RegCloseKey(localmachine);
	if (instDir[strlen(instDir)-1] != '\\')
		strcat(instDir, "\\");

	// Get the settings
	if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ,
		&localmachine) == ERROR_SUCCESS)
	{
		if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0,
			KEY_READ, &software) == ERROR_SUCCESS)
		{
			if (regError = RegOpenKeyEx(software, COMPANYKEY, 0,
				KEY_READ, &company) == ERROR_SUCCESS)
			{
				if (regError = RegOpenKeyEx(company, PRODUCTKEY, 0,
					KEY_READ, &product) == ERROR_SUCCESS)
				{
					if (regError = RegOpenKeyEx(product, VERSIONKEY, 0,
						KEY_READ|KEY_WRITE, &version) == ERROR_SUCCESS)
					{
						length = 4;
						i = globals.syntaxHiLighting;
						RegQueryValueEx(version, "Syntax HiLighting",
							NULL, NULL,	(unsigned char *)&i, &length);
						globals.syntaxHiLighting = i;
						i = globals.pcTracking;
						RegQueryValueEx(version, "PC Tracking",
							NULL, NULL,	(unsigned char *)&i, &length);
						globals.pcTracking = i;
						i = globals.assembler;
						RegQueryValueEx(version, "Assembler",
							NULL, NULL,	(unsigned char *)&i, &length);
						globals.assembler = i;
						i = globals.addresses;
						RegQueryValueEx(version, "Addresses",
							NULL, NULL,	(unsigned char *)&i, &length);
						globals.addresses = i;
						i = globals.machineCode;
						RegQueryValueEx(version, "MachineCode",
							NULL, NULL,	(unsigned char *)&i, &length);
						globals.machineCode = i;
						i = globals.commsTimeout;
						RegQueryValueEx(version, "CommsTimeout",
							NULL, NULL,	(unsigned char *)&i, &length);
						globals.commsTimeout = i;
						RegCloseKey(version);
					}
					RegCloseKey(product);
				}
				RegCloseKey(company);
			}
			RegCloseKey(software);
		}
		RegCloseKey(localmachine);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStudioApp initialization

BOOL CStudioApp::InitInstance()
{
	HKEY localmachine;
	HKEY software;
	HKEY company;
	HKEY product;
	HKEY version;
	HKEY ides;
	HKEY ide;
	long regError;
	unsigned long length;
	int i, j;
	CRect rect;
	CString message;
	char buffer[1000];
	char * idesBuffer;
//	char * familysBuffer;
	CString libPath;
	int max;

	hInstance = AfxGetInstanceHandle();

	AfxEnableControlContainer();

	// Show the splash dialog
	CSplashDialog splashDialog; // CSplashDialog();
	splashDialog.ShowWindow(SW_SHOWNORMAL);
	splashDialog.UpdateWindow();
	DWORD startTime = ::GetTickCount();
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CString settingsKey;
//	settingsKey = "HKEY_CURRENT_USER\\Software\\";
	settingsKey = COMPANYKEY;
//	settingsKey += "\\";
//	settingsKey += PRODUCTKEY;
//	settingsKey += "\\";
//	settingsKey += VERSIONKEY;
	SetRegistryKey(settingsKey);

	// All IDE librarys only support the AVR family so
	// I'm not bothering with the Family stuff
	// at the moment (Time!!)

	numberFamilys = 1;

	familyIDs = new int[1];
	familyIDs[0] = 0x01;

	familyNames = new char *[2];
	familyNames[0] = new char[strlen("AVR")+1];
	strcpy(familyNames[0], "AVR");
	familyNames[1] = new char[strlen("AVR.dll")+1];
	strcpy(familyNames[1], "AVR.dll");

	familyVersions = new char *[1];
	libPath = instDir;
	libPath += "Plug-ins\\";
	libPath += familyNames[1];
	hFamilyDll = ::LoadLibrary(libPath);
	if (hFamilyDll)
	{
		CLGetFamilyVersion = (GETFAMILYVERSION)GetProcAddress(hFamilyDll,
			"GetFamilyVersion");
		max = 0;
		CLGetFamilyVersion(NULL, &max);
		familyVersions[0] = new char[max];
		CLGetFamilyVersion(familyVersions[0], &max);
	}
	else
		familyVersions[0] = NULL;

	// Determine available IDEs

	idesBuffer = NULL;
//	familysBuffer = NULL;
	if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ,
		&localmachine) == ERROR_SUCCESS)
	{
		if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0,
			KEY_READ, &software) == ERROR_SUCCESS)
		{
			if (regError = RegOpenKeyEx(software, COMPANYKEY, 0,
				KEY_READ, &company) == ERROR_SUCCESS)
			{
				if (regError = RegOpenKeyEx(company, PRODUCTKEY, 0,
					KEY_READ, &product) == ERROR_SUCCESS)
				{
					if (regError = RegOpenKeyEx(product, VERSIONKEY, 0,
						KEY_READ|KEY_WRITE, &version) == ERROR_SUCCESS)
					{
						length = 0;
						regError = RegQueryValueEx(version, "IDEs", NULL,
							NULL, NULL, &length);
						if (regError == ERROR_SUCCESS)
						{
							idesBuffer = new char[length];
							RegQueryValueEx(version, "IDEs", NULL,
								NULL, (unsigned char *)idesBuffer, &length);
						}
//						length = 0;
//						regError = RegQueryValueEx(version, "Familys", NULL,
//							NULL, NULL, &length);
//						if (regError == ERROR_SUCCESS)
//						{
//							familysBuffer = new char[length];
//							RegQueryValueEx(version, "Familys", NULL,
//								NULL, (unsigned char *)familysBuffer, &length);
//						}
						RegCloseKey(version);
					}
					RegCloseKey(product);
				}
				RegCloseKey(company);
			}
			RegCloseKey(software);
		}
		RegCloseKey(localmachine);
	}
	if (!idesBuffer || idesBuffer[0] == 0) // No available IDE librarys
	{
		if (idesBuffer != NULL)
			delete idesBuffer;
//		if (familysBuffer != NULL)
//			delete familysBuffer;
		strcpy(buffer, STRING_NAME);
		strcat(buffer, " could not find any installed IDE librarys.\r\n\
Please reinstall ");
		strcat(buffer, STRING_NAME);
		strcat(buffer, ".");
		::MessageBox(NULL, buffer, "No IDE librarys", MB_OK|MB_ICONSTOP);
		return FALSE;
	}
//	if (!familysBuffer || familysBuffer[0] == 0) // No Family librarys
//	{
//		if (idesBuffer != NULL)
//			delete idesBuffer;
//		if (familysBuffer != NULL)
//			delete familysBuffer;
//		strcpy(buffer, STRING_NAME);
//		strcat(buffer, " could not find any installed CPU Family librarys.\r\n\
//Please reinstall ");
//		strcat(buffer, STRING_NAME);
//		strcat(buffer, ".");
//		::MessageBox(NULL, buffer, "No CPU Family librarys", MB_OK|MB_ICONSTOP);
//		return FALSE;
//	}

	// Parse "idesBuffer" for IDE names.
	// They may appear more than once!!
	CString idesStr = idesBuffer;
	CString goodIDEs = "";
	CString ideName;
	delete idesBuffer;
	numberIDEs = 0;
	idesStr.TrimRight(',');
	do
	{
		idesStr.TrimLeft(',');
		if (idesStr.GetLength() != 0)	// If an IDE name
		{
			if (idesStr.Find(',', 0) != -1)
			{
				ideName = idesStr.Mid(0, idesStr.Find(','));
				idesStr = idesStr.Mid(idesStr.Find(',')+1);
			}
			else						// Last name!
			{
				ideName = idesStr;
				idesStr = "";
			}
			ideName += ",";
			if (goodIDEs.Find(ideName) == -1) // A new name
				goodIDEs += ideName;
		}
	}
	while (idesStr.GetLength() != 0);
	goodIDEs.TrimRight(',');			// Remove "search" comma
	// "goodIDEs" now holds a comma delimited list of IDE plug-ins

	// It's clean, so put it back in the registry
	if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ,
		&localmachine) == ERROR_SUCCESS)
	{
		if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0,
			KEY_READ, &software) == ERROR_SUCCESS)
		{
			if (regError = RegOpenKeyEx(software, COMPANYKEY, 0,
				KEY_READ, &company) == ERROR_SUCCESS)
			{
				if (regError = RegOpenKeyEx(company, PRODUCTKEY, 0,
					KEY_READ, &product) == ERROR_SUCCESS)
				{
					if (regError = RegOpenKeyEx(product, VERSIONKEY, 0,
						KEY_READ|KEY_WRITE, &version) == ERROR_SUCCESS)
					{
						idesBuffer = new char[goodIDEs.GetLength()+1];
						strcpy(idesBuffer, goodIDEs);
						length = goodIDEs.GetLength()+1;
						RegSetValueEx(version, "IDEs", NULL, REG_SZ,
							(CONST BYTE *)idesBuffer, length);
						delete idesBuffer;						
						RegCloseKey(version);
					}
					RegCloseKey(product);
				}
				RegCloseKey(company);
			}
			RegCloseKey(software);
		}
		RegCloseKey(localmachine);
	}

	// Extract the IDE info from the list
	idesStr = goodIDEs;
	for (i=0; i<idesStr.GetLength(); i++) // Count the IDEs
	{
		if (idesStr.GetAt(i) == ',')
			numberIDEs++;
	}
	numberIDEs++;
	ideNames = new char *[numberIDEs*2]; // Get the IDE's names
	for (i=0; i<numberIDEs; i++)
	{
		ideNames[i*2] = NULL;
		ideNames[i*2+1] = NULL;
		if (idesStr.Find(',') != -1)
		{
			ideNames[i*2] = new char[idesStr.Find(',')+1];
			strcpy(ideNames[i*2], idesStr.Mid(0, idesStr.Find(',')));
			idesStr = idesStr.Mid(idesStr.Find(',')+1);
		}
		else
		{
			ideNames[i*2] = new char[idesStr.GetLength()+1];
			strcpy(ideNames[i*2], idesStr);
		}
	}

//	CString familysStr = familysBuffer;
//	delete familysBuffer;
//	numberFamilys = 0;
//	for (i=0; i<familysStr.GetLength(); i++) // Count the CPU familys
//	{
//		if (familysStr.GetAt(i) == ',')
//			numberFamilys++;
//	}
//	numberFamilys++;
//	familyNames = new char *[numberFamilys*2]; // Get the CPU Family's names
//	for (i=0; i<numberFamilys; i++)
//	{
//		familyNames[i*2] = NULL;
//		familyNames[i*2+1] = NULL;
//		if (familysStr.Find(',') != -1)
//		{
//			familyNames[i*2] = new char[familysStr.Find(',')+1];
//			strcpy(familyNames[i*2], familysStr.Mid(0, familysStr.Find(',')));
//			familysStr = familysStr.Mid(familysStr.Find(',')+1);
//		}
//		else
//		{
//			familyNames[i*2] = new char[familysStr.GetLength()+1];
//			strcpy(familyNames[i*2], familysStr);
//		}
//	}

//	ideFamilyTable = new int[numberIDEs*numberFamilys]; // Get the lookup table
//	for (i=0; i<numberIDEs*numberFamilys; i++)
//		ideFamilyTable[i] = 0;			// Initialise table

	ideIDs = new int[numberIDEs];		// Get the IDE's IDs
	ideVersions = new char *[numberIDEs];
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ, &localmachine);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0, KEY_READ, &software);
	if (regError = RegOpenKeyEx(software, COMPANYKEY, 0, KEY_READ, &company)
		!= ERROR_SUCCESS)
	{
		errorCode = SPERROR_NOTINSTALLED;
		ErrorMessage();
		return FALSE;
	}
	if (regError = RegOpenKeyEx(company, PRODUCTKEY, 0, KEY_READ, &product)
		!= ERROR_SUCCESS)
	{
		errorCode = SPERROR_NOTINSTALLED;
		ErrorMessage();
		return FALSE;
	}
	if (regError = RegOpenKeyEx(product, VERSIONKEY, 0, KEY_READ|KEY_WRITE,
		&version) != ERROR_SUCCESS)
	{
		errorCode = SPERROR_NOTINSTALLED;
		ErrorMessage();
		return FALSE;
	}
	if (regError = RegOpenKeyEx(version, "IDEs", 0, KEY_READ|KEY_WRITE,
		&ides) != ERROR_SUCCESS)
	{
		errorCode = SPERROR_NOTINSTALLED;
		ErrorMessage();
		return FALSE;
	}

//	int * supportedFamilys;
	for (i=0; i<numberIDEs; i++)
	{
		if (regError = RegOpenKeyEx(ides, ideNames[i*2], 0, KEY_READ|KEY_WRITE,
			&ide) != ERROR_SUCCESS)
		{
			continue;
		}
		length = 0;
		if (regError = RegQueryValueEx(ide, "Name", NULL, NULL, NULL, &length)
			!= ERROR_SUCCESS)
		{
			continue;
		}
		ideNames[i*2+1] = new char[length];
		RegQueryValueEx(ide, "Name", NULL, NULL, (BYTE *)ideNames[i*2+1],
			&length);

		ideIDs[i] = 0;
		libPath = instDir;
		libPath += "Plug-ins\\";
		libPath += ideNames[i*2+1];
		hIdeDll = ::LoadLibrary(libPath);
		if (!hIdeDll)
			continue;

		ILGetIdeID = (GETIDEID)GetProcAddress(hIdeDll, "GetIdeID");
		ideIDs[i] = ILGetIdeID(); // Get ID

		ILGetIdeVersion = (GETIDEVERSION)GetProcAddress(hIdeDll,
			"GetIdeVersion");
		max = 0;
		ILGetIdeVersion(NULL, &max);
		ideVersions[i] = new char[max];
		ILGetIdeVersion(ideVersions[i], &max);

//		ILGetSupportedFamilys = (GETSUPPORTEDFAMILYS)GetProcAddress(hIdeDll,
//			"GetSupportedFamilys");
//		ILGetFamilySupportID = (GETFAMILYSUPPORTID)GetProcAddress(hIdeDll,
//			"GetFamilySupportID");
//		length = 0;
//		ILGetSupportedFamilys(NULL, &length);
//		supportedFamilys = new int[length];
//		ILGetSupportedFamilys[supportedFamilys, &length];

		::FreeLibrary(hIdeDll);
		hIdeDll = NULL;

		RegCloseKey(ide);
	}
	RegCloseKey(ides);
	RegCloseKey(version);
	RegCloseKey(product);
	RegCloseKey(company);
	RegCloseKey(software);
	RegCloseKey(localmachine);

	ideFlags = 0;
	for (i=0; i<numberIDEs; i++)
		ideFlags |= ideIDs[i];

	projectWindow = NULL;
	projectWindowOpen = false;
	sourceWindows = NULL;
	watchWindow = NULL;
	watchWindowOpen = false;
	sfrsWindow = NULL;
	sfrsWindowOpen = false;
	chipWindow = NULL;
	chipWindowOpen = false;

	// Comms
	rxBuffer = new unsigned char[37+65536+65536];
	dataIndex = 0;
	rxCount = 0;

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame * pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	pFrame->windowX = 0;
	pFrame->windowY = 0;
	pFrame->windowW = GetSystemMetrics(SM_CXFULLSCREEN);
	pFrame->windowH = GetSystemMetrics(SM_CYFULLSCREEN);
	// Find the main window's preferred size
	if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ,
		&localmachine) == ERROR_SUCCESS)
	{
		if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0,
			KEY_READ, &software) == ERROR_SUCCESS)
		{
			if (regError = RegOpenKeyEx(software, COMPANYKEY, 0, KEY_READ,
				&company) == ERROR_SUCCESS)
			{
				if (regError = RegOpenKeyEx(company, PRODUCTKEY, 0,
					KEY_READ, &product) == ERROR_SUCCESS)
				{
					if (regError = RegOpenKeyEx(product, VERSIONKEY, 0,
						KEY_READ, &version) == ERROR_SUCCESS)
					{
						length = 4;
						int pos = (pFrame->windowX | (pFrame->windowY<<16));
						int size = (pFrame->windowW | (pFrame->windowH<<16));
						RegQueryValueEx(version, "Window Position",
							NULL, NULL,	(unsigned char *)&pos, &length);
						RegQueryValueEx(version, "Window Size",
							NULL, NULL,	(unsigned char *)&size, &length);
						RegCloseKey(version);
						pFrame->windowX = (pos&0xFFFF);
						pFrame->windowY = (pos>>16);
						pFrame->windowW = (size&0xFFFF);
						pFrame->windowH = (size>>16);
					}
					RegCloseKey(product);
				}
				RegCloseKey(company);
			}
			RegCloseKey(software);
		}
		RegCloseKey(localmachine);
	}
	if (pFrame->windowW > GetSystemMetrics(SM_CXFULLSCREEN))
		pFrame->windowW = GetSystemMetrics(SM_CXFULLSCREEN);
	if (pFrame->windowH > GetSystemMetrics(SM_CYFULLSCREEN))
		pFrame->windowH = GetSystemMetrics(SM_CYFULLSCREEN);
	if (pFrame->windowX < 0)
		pFrame->windowX = 0;
	if (pFrame->windowY < 0)
		pFrame->windowY = 0;
	if (pFrame->windowX+pFrame->windowW > GetSystemMetrics(SM_CXFULLSCREEN))
		pFrame->windowX = GetSystemMetrics(SM_CXFULLSCREEN)-pFrame->windowW;
	if (pFrame->windowY+pFrame->windowH > GetSystemMetrics(SM_CYFULLSCREEN))
		pFrame->windowY = GetSystemMetrics(SM_CYFULLSCREEN)-pFrame->windowH;

	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	safeSerial = new CSafeSerial();
	safeSerial->Create("SafeSerial", 0, rect, m_pMainWnd, -1, NULL, FALSE,
		NULL);
	CString ssPath = instDir;
	ssPath += "license";
	safeSerial->SetSSPath(ssPath);
	if (safeSerial->GetVersion() < 238)
	{
		::MessageBox(NULL, "Studio+ has been corrupted, please reinstall it.",
			"Internal Error", MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	// Product info
	safeSerial->SetProductName(STRING_NAME);
	safeSerial->SetCompanyName(STRING_FLASH);
	safeSerial->SetAdressLine1(STRING_FLASHURL);
	safeSerial->SetAdressLine2(STRING_FLASHEMAIL);

	// Default settings
	safeSerial->SetEncodeNum1(SS_CODE1);
	safeSerial->SetEncodeNum2(SS_CODE2);
	safeSerial->SetEncodeNum3(SS_CODE3);
	safeSerial->SetEncodeNum4(SS_CODE4);
	safeSerial->SetDaysAllowed(30);
	safeSerial->SetMaxExecs(255); //30);
	safeSerial->SetNetwork(FALSE);		// Every user needs to register
	safeSerial->SetLimitUsers(FALSE);
	i = SS_PROFESSIONAL|1;
	safeSerial->SetLicenseType(i);		// Professional & AVR
	safeSerial->SetCounter(0xff);		// All IDEs
	safeSerial->SetDefaultMessage(FALSE);
	safeSerial->SetAutoTerminate(FALSE);

	safeSerial->Execution();

	int licenseType = safeSerial->ReadLicenseType();
	int counter = safeSerial->readcounter();
	globals.universal = FALSE;
	if (licenseType&SS_UNIVERSALMASK)
	{
		globals.universal = TRUE;
		globals.options = SS_ICESUPPORT;
		globals.cpus = 0x01FF;
		globals.ides = 0xff;
	}
	else
	{
		globals.options = licenseType&SS_OPTIONMASK;
		if (globals.options < SS_BASIC)
			globals.options = SS_BASIC;	// Everybody gets the basic option
		globals.cpus = licenseType&SS_CPUMASK;
		if (globals.cpus < 1)
			globals.cpus = 1;			// Everybody gets AVR support
		globals.ides = counter;
		globals.ides |= 0x05;			// Everybody gets AVR Studio (3.53 & 4)
	}

	if (safeSerial->Checklicense() != SS_CODE1+SS_CODE2+SS_CODE3+SS_CODE4)
	{
		if (::MessageBox(NULL,
"Your Studio+ trial has ended,\r\n\
would you like to register now?\r\n\
(or import a license from a disk)", "Please Register",
			MB_YESNO|MB_ICONQUESTION) == IDYES)
		{
			splashDialog.DestroyWindow();
			OnHelpRegister();
		}
		else
			splashDialog.DestroyWindow();
		return FALSE;
	}

	if (!safeSerial->IsRegistered())
	{
		char buffer[100];
		message = "You are on day ";
		itoa(30-(safeSerial->GetDaysLeft()-1),	buffer, 10);
		message += buffer;
		message += " of your 30 day trial period."; //\r\n";
//		int execs = safeSerial->ReadMaxExecs()-safeSerial->ReadExecs();
//		if (execs == 0)
//			message += "You are on your last trial run!!";
//		else if (execs == 1)
//			message += "You have 1 trial run left.";
//		else
//		{
//			message += "You have ";
//			itoa(execs, buffer, 10);
//			message += buffer;
//			message += " trial runs left.";
//		}
		::MessageBox(NULL, message, "Trial Period", MB_OK|MB_ICONEXCLAMATION);
		globals.registered = FALSE;
	}
	else
		globals.registered = TRUE;

	// Build the Help menu "Dev. Environments" sub menu
	CMenu * menu;
	menu = m_pMainWnd->GetMenu();
	menu = menu->GetSubMenu(5);			// Help Menu
	menu->EnableMenuItem(2, MF_BYPOSITION|MF_ENABLED);
	menu = menu->GetSubMenu(2);			// "Help->Dev. Environments" Menu
	while (menu->DeleteMenu(0, MF_BYPOSITION)); // Delete all items
	for (i=0; i<numberIDEs; i++)
	{
		menu->AppendMenu(MF_ENABLED | MF_STRING, IDM_HELP_IDE+i,
			ideNames[i*2]);
	}
	m_pMainWnd->DrawMenuBar();			// Update menu bar

	// Find all available COM ports
	availableComPorts = new int[256];
	comPortIndexes = new int[256];
	j = 0;
	for (i=0; i<256; i++)
	{
		if (ComPortExists(i+1))
		{
			availableComPorts[i] = TRUE;
			comPortIndexes[j++] = i;
		}
		else
		{
			availableComPorts[i] = FALSE;
		}
	}
	numComPorts = j;

	while (::GetTickCount() - startTime < 3000)
		Sleep(100);
	splashDialog.DestroyWindow();
//	delete splashDialog;
//	splashDialog = NULL;

//	m_hMDIMenu  = ::LoadMenu(hInstance, MAKEINTRESOURCE(IDR_STUDIOTYPE));
//	m_hMDIAccel = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_STUDIOTYPE));

	SetSourceMenu();
	SetProjectMenu();

	pFrame->MoveWindow(pFrame->windowX, pFrame->windowY, pFrame->windowW,
		pFrame->windowH);
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	if (m_lpCmdLine[0] != _T('\0'))
	{

#ifdef MAINDEBUG
debug<<"CStudioApp::InitInstance():"<<endl;
debug<<"   CommandLine invoked: "<<m_lpCmdLine<<endl;
#endif
		
		// Open a file passed as the first command line parameter.
		project = new FDProject();
		if (!project->Open(m_lpCmdLine))
		{
			ErrorMessage();
			delete project;
			project = NULL;
			return TRUE;
		}
		SetTitle();
	}   

	return TRUE;
}


int CStudioApp::Run()
{
	int i;
	MSG msg;
	DWORD timeStampTimer = 0;
	DWORD projectTime = 0;
	LONG idleCount = 0;
	unsigned int needed;
	unsigned char tx;
	FMSymbol * symbol;
	FDProject * currentProject = NULL;
	int fileIndex;

	commsTimer = 0;
	gettingData = false;
	firstWatchSymbol = 0;
	lastWatchSymbol = 0;
	updateSram = false;
	updateEeprom = false;
	updateStack = false;

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)	// If "QUIT" then quit!!
				break;

			if (!PreTranslateMessage(&msg)) // Give MFC a chance to do stuff
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else							// No messages
		{
			idleCount = 0;
			while (OnIdle(idleCount++)); // Do Idle processing
		}

		// Main loop processing
		if (!project)
		{
			commsTimer = 0;
			currentProject = NULL;
			timeStampTimer = 0;
			continue;
		}

		if (currentProject != project)
		{
			currentProject = project;
			timeStampTimer = ::GetTickCount();
			projectTime = project->GetTimeStamp();			
		}
		else
		{
			if (::GetTickCount()-timeStampTimer > 1000)
			{
				timeStampTimer = ::GetTickCount();
				if (projectTime != project->GetTimeStamp())
				{
					projectTime = project->GetTimeStamp();
					if (::MessageBox(NULL,
"Your project has been updated by another application.\r\n\
Would you like to reload it? (recommended)", "Project Updated",
						MB_YESNO|MB_ICONEXCLAMATION) == IDYES)
					{
						OnProjectRefresh();
						continue;
					}
				}
			}
		}

		if (!ComPortOpen())
			continue;

		if (!debugRunning && !gettingData &&
			firstWatchSymbol == lastWatchSymbol &&
			!updateRegisters && !updateSfrs && !updateSram && !updateEeprom &&
			!updateStack)
		{
			continue;					// Got all the data we need!!
		}

		if (!commsTimer)				// If first time in debugger
			commsTimer = ::GetTickCount();

		if (updateRegisters && !gettingData)
		{								// User opened the Registers window
			updateRegisters = false;
			askedFor = SENDREGISTERS;
			gettingData = true;
		}

		if (updateSfrs && !gettingData && !updateRegisters)
		{								// User opened the Registers window
			updateSfrs = false;
			for (i=0; i<sfrsSize; i++)
			{
				if (!sfrsMask[i])
				{						// Only get SFRs if we need them
					askedFor = SENDSFRS;
					gettingData = true;
					break;
				}
			}
		}

		if (updateSram && !gettingData && !updateSfrs && !updateRegisters)
		{								// User fiddled with SRAM window 
			updateSram = false;
			sramFirst = project->sramStartAddress;
			sramLast = project->sramEndAddress;
			while (sramMask[sramFirst-sramStart] != 0 && sramFirst < sramLast)
				sramFirst++;
			if (sramFirst == sramLast)
				continue;				// Already got the data
			sramLast--;
			while (sramMask[sramLast-sramStart] != 0)
				sramLast--;
			sramLast++;					// Last address isn't inclusive
			askedFor = SENDSRAM;
			gettingData = true;
		}

		if (updateEeprom && !gettingData && !updateSfrs && !updateRegisters &&
			!updateSram)				// User fiddled with EEPROM window
		{
			updateEeprom = false;
			eepromFirst = project->eepromStartAddress;
			eepromLast = project->eepromEndAddress;
			while (eepromMask[eepromFirst-eepromStart] != 0 &&
				eepromFirst < eepromLast)
			{
				eepromFirst++;
			}
			if (eepromFirst == eepromLast)
				continue;				// Already got the data
			eepromLast--;
			while (eepromMask[eepromLast-eepromStart] != 0)
				eepromLast--;
			eepromLast++;				// Last address isn't inclusive
			askedFor = SENDEEPROM;
			gettingData = true;
		}

		if (updateStack && !gettingData && !updateSfrs && !updateRegisters &&
			!updateSram && !updateEeprom) // User fiddled with Stack window 
		{
			updateStack = false;
			sramFirst = project->stackStartAddress;
			sramLast = project->stackEndAddress;
			while (sramMask[sramFirst-sramStart] != 0 && sramFirst < sramLast)
				sramFirst++;
			if (sramFirst == sramLast)
				continue;				// Already got the data
			sramLast--;
			while (sramMask[sramLast-sramStart] != 0)
				sramLast--;
			sramLast++;					// Last address isn't inclusive
			askedFor = SENDSRAM;
			gettingData = true;
		}

		if (endCondition)				// If in the End Condition
		{
			endCondition = false;

			PurgeBuffers();
			
			while (!TxChar(controlChar));
			commsTimer = ::GetTickCount();
			waitingForD = true;

//			while (!Rx(1))				// Wait till option echoed back
//			{
//				if (::GetTickCount()-commsTimer > RXTIMEOUT) // Half a second
//				{
//					while (!Tx(controlChar)); // Transmit option
//					commsTimer = ::GetTickCount();
//				}
//			}
			continue;
		}

		if (waitingForD)
		{
			dataIndex = 0;				// Reset receive buffer index
			Rx(0);						// Try to receive bytes
			if (rxCount)				// If bytes were received
			{
				commsTimer = ::GetTickCount(); // Remember when

				if (rxBuffer[rxCount-1] == 'D') // If we got a 'D'
				{

					PurgeBuffers();

					CalculateExpected(); // Calculate number of bytes expected
					if (needInfoBlock)	// If we need an Info-Block
						GetInfoBlock();	// Get it (will also send request code)
					else
						while (!TxChar(askedFor)); // Send data request code
					commsTimer = ::GetTickCount(); // Remember when
//					while (!Rx(1))		// Wait for code to be echoed back
//					{
//						if (::GetTickCount()-commsTimer > 100)
//						{
//							while (!Tx(askedFor)); // Transmit code
//							commsTimer = ::GetTickCount();
//						}
//					}

					if (askedFor == SENDSRAM) // Send the addresses
					{
						tx = (sramFirst&0xff);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = ((sramFirst>>8)&0xff);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = (sramFirst>>16);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = (sramLast&0xff);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = ((sramLast>>8)&0xff);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = (sramLast>>16);
						while (!TxChar(tx));
						while (!Rx(1));
					}

					if (askedFor == SENDEEPROM) // Send the addresses
					{
						tx = (eepromFirst&0xff);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = ((eepromFirst>>8)&0xff);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = (eepromFirst>>16);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = (eepromLast&0xff);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = ((eepromLast>>8)&0xff);
						while (!TxChar(tx));
						while (!Rx(1));
						tx = (eepromLast>>16);
						while (!TxChar(tx));
						while (!Rx(1));
					}

					dataIndex = 0;		// Reset buffer index
					waitingForD = false; // Not waiting for 'D'
					if (askedFor == SENDPC)
					{
//						pcFile = NULL;	// Reset PC source code file
						pcLine = -1;	// Reset PC source code line
					}
					continue;
				}
				if (rxBuffer[rxCount-1] == 'E') // End condition
				{						// Maybe the PC paused for a long time
										// and we missed loads of bytes, this
										// is more likely during the data
					waitingForD = false; // Receiving stage (Dave's Laptop did
					endCondition = true; // this). I'd be very suprised if
					continue;			// this bit of code ever gets executed.
				}

				// Got data but not a 'D' or 'E'
				if (globals.commsTimeout &&
					::GetTickCount()-commsTimer > (DWORD)globals.commsTimeout)
				{
					int result = ::MessageBox(NULL,
				"Studio+ cannot detect your Target,\r\nPlease reset it.",
						"Comms Time-Out",
						MB_OKCANCEL|MB_ICONEXCLAMATION);
					if (result != IDOK)
						debugRunning = false;
					commsTimer = ::GetTickCount();
#ifdef MAINDEBUG
debug<<"CStudioApp::Run()"<<endl;
debug<<"   Waiting for D (got data), no target after 3 seconds"<<endl;
#endif
				}
			}
			else						// If no bytes were recieved
			{
				if (globals.commsTimeout &&
					::GetTickCount()-commsTimer > (DWORD)globals.commsTimeout)
				{
					int result = ::MessageBox(NULL,
				"Studio+ cannot detect your Target,\r\nPlease reset it.",
						"Comms Time-Out",
						MB_OKCANCEL|MB_ICONEXCLAMATION);
					if (result != IDOK)
						debugRunning = false;
					commsTimer = ::GetTickCount();
#ifdef MAINDEBUG
debug<<"CStudioApp::Run()"<<endl;
debug<<"   Waiting for D (no data), no target after 3 seconds"<<endl;
#endif
				}
			}
			continue;
		}

										// If not in End-Condition &
										// not waiting for the 'D' character
										// then must be Receiving Data
		if (Rx(0))						// If received bytes
		{
			commsTimer = ::GetTickCount(); // Remember when

			if (askedFor == SENDPC)		// Just PC
			{
				if (dataIndex+rxCount >= 2) // PC just updated
				{
					pc = rxBuffer[0];
					pc <<= 8;
					pc += rxBuffer[1];
					pc *= 2;			// AVR PC is divided by 2
					project->SetPC();
				}
			}

			else if (askedFor == SENDREGISTERS) // Registers
				UpdateRegisters(rxBuffer, dataIndex, rxCount);

			else if (askedFor == SENDSFRS)
				UpdateSFRs(rxBuffer, dataIndex, rxCount);

			else if (askedFor == SENDSRAM)
				UpdateSram(rxBuffer, dataIndex, rxCount);

			else if (askedFor == SENDEEPROM)
				UpdateEeprom(rxBuffer, dataIndex, rxCount);

			dataIndex += rxCount;
			if (dataIndex > expected)	// If all the expected data is in
			{
				if (rxBuffer[dataIndex-1] == 'E') // End Condition
				{
					switch (debugMode)	// Switch on Debug Mode
					{
						case DM_STOP:
						case DM_TOHARDBP:
							debugMode = DM_STOP;
							debugRunning = false;
							break;

						case DM_SINGLESTEP:
							if (pcFile != cStepFile && pcFile != NULL)
							{			// Different File
								int index = pcFile->GetSourceFileIndex();
								if (!pcFile->IsOpen())
									OpenSourceFile(index);
								else
									sourceWindows[index]->BringWindowToTop();
							}
							debugMode = DM_STOP;
							debugRunning = false;
							break;

						case DM_CSTEP:
							if (pcLine != -1)
							{
								if (pcFile == cStepFile)
								{
									if (pcLine != cStepLine)
									{
										debugMode = DM_STOP;
										debugRunning = false;
									}
								}
								else
								{
									if (pcFile != NULL)
									{	// Different file
										int index =
											pcFile->GetSourceFileIndex();
										if (!pcFile->IsOpen())
											OpenSourceFile(index);
										else
											sourceWindows[index]->
												BringWindowToTop();
										debugMode = DM_STOP;
										debugRunning = false;
									}
								}
							}
							break;

						case DM_STEPTOCURSOR:
							if (pcFile == cursorFile &&
								pcLine == cursorLine)
							{
								debugMode = DM_STOP;
								debugRunning = false;
							}
							break;

						case DM_TOSOFTBP:
							if (pcFile && pcLine != -1 &&
								pcFile->GetLineBreakpoint(pcLine) ==
								SOFTBREAKPOINT)
							{
								debugMode = DM_STOP;
								debugRunning = false;
							}
							break;

						case DM_ANIMATE:
						case DM_SNAPSHOT:
							break;

						default:
							debugMode = DM_STOP;
							debugRunning = false;
							break;
					}

					if (debugMode == DM_STOP || // If need to update displays
						debugMode == DM_ANIMATE ||
						debugMode == DM_SNAPSHOT)
					{
						if (askedFor == SENDPC) // Do this once per PC address
						{
							if (pcFile)
							{
								fileIndex = pcFile->GetSourceFileIndex();
								if (pcFile->IsOpen())
									sourceWindows[fileIndex]->Update();
							}
							if (oldPCFile && oldPCFile != pcFile)
							{
								fileIndex = oldPCFile->GetSourceFileIndex();
								if (oldPCFile->IsOpen())
									sourceWindows[fileIndex]->Update();
							}
							oldPCFile = pcFile;

										// Reset the data masks
							for (i=0; i<registersSize; i++)
								registersMask[i] = 0;
							for (i=0; i<sfrsSize; i++)
								sfrsMask[i] = 0;
							for (i=0; i<sramSize; i++)
								sramMask[i] = 0;
							for (i=0; i<eepromSize; i++)
								eepromMask[i] = 0;

							needed = 0;
							if (registersWindowHandle)
							{
								updateRegisters = false;
								needed |= NEEDREGISTERS;
							}
							if (watchWindowOpen)
							{
								updateRegisters = false;
								needed |= NEEDREGISTERS;
								firstWatchSymbol = 0;
								lastWatchSymbol =
									project->GetNumWatchSymbols();
							}
							else
							{
								firstWatchSymbol = 0;
								lastWatchSymbol = 0;
							}
							if (sfrsWindowOpen || chipWindowOpen)
							{
								updateSfrs = false;
								needed |= NEEDSFRS;
							}
							if (sramWindowHandle)
							{
								updateSram = false;
								needed |= NEEDSRAM;
							}
							if (eepromWindowHandle)
							{
								updateEeprom = false;
								needed |= NEEDEEPROM;
							}
							if (stackWindowHandle)
							{
								updateStack = false;
								updateRegisters = false;
								needed |= NEEDSTACK|NEEDREGISTERS;
							}

							if (needed || firstWatchSymbol != lastWatchSymbol)
								gettingData = true;

							askedFor = 0;
						}

						if (askedFor == SENDREGISTERS)
						{
							stackPointer = registers[34];
							stackPointer <<= 8;
							stackPointer += registers[33];

							framePointer = registers[29];
							framePointer <<= 8;
							framePointer += registers[28];
							askedFor = 0;
						}

						if (askedFor == SENDSFRS)
							askedFor = 0;

						if (askedFor == SENDSRAM)
							askedFor = 0;
						
						if (askedFor == SENDEEPROM)
							askedFor = 0;
						
						endCondition = true;
						controlChar = 'R'; // Get more data

						if (needed&NEEDREGISTERS)
						{
							needed &= ~NEEDREGISTERS;
							askedFor = SENDREGISTERS;
							continue;
						}

						if (needed&NEEDSFRS)
						{
							needed &= ~NEEDSFRS;
							askedFor = SENDSFRS;
							continue;
						}

						if (needed&NEEDSRAM)
						{
							needed &= ~NEEDSRAM;
							askedFor = SENDSRAM;
							sramFirst = project->sramStartAddress;
							sramLast = project->sramEndAddress;
							continue;
						}
						
						if (needed&NEEDSTACK)
						{
							needed &= ~NEEDSTACK;
							askedFor = SENDSRAM;
							sramFirst = project->stackStartAddress;
							sramLast = project->stackEndAddress;
							continue;
						}

						if (needed&NEEDEEPROM)
						{
							needed &= ~NEEDEEPROM;
							askedFor = SENDEEPROM;
							eepromFirst = project->eepromStartAddress;
							eepromLast = project->eepromEndAddress;
							continue;
						}
						
						while (firstWatchSymbol != lastWatchSymbol)
						{
							symbol = project->GetWatchSymbol(
								firstWatchSymbol++);
							if ((symbol->GetType()&VARIABLE_TYPEMASK) ==
								VARIABLE_REGISTER)
							{			// Already have it's data
								continue;
							}
							else if ((symbol->GetType()&VARIABLE_TYPEMASK) ==
								VARIABLE_STATIC)
							{
								sramFirst = symbol->GetAddress();
								sramLast = sramFirst+symbol->GetValueSize();
							}
							else if (symbol->GetType() == VARIABLE_LOCAL)
							{
								sramFirst = framePointer+symbol->GetAddress();
								sramLast = sramFirst+symbol->GetValueSize();
							}
							else		// Ignore if not Register or Memory
								continue;

							while (sramMask[sramFirst-sramStart] != 0 &&
								sramFirst < sramLast)
							{
								sramFirst++;
							}
							if (sramFirst == sramLast)
								continue; // Already got the data
							sramLast--;
							while (sramMask[sramLast-sramStart] != 0)
								sramLast--;
							sramLast++;	// Last address isn't inclusive
							askedFor = SENDSRAM;
							break;
						}
						if (askedFor == SENDSRAM)
							continue;

						if (registersWindowHandle)
							DrawRegisterValues();

						if (watchWindowOpen)
						{
							project->UpdateWatchSymbols();
							watchWindow->Update();
						}

						if (sfrsWindowOpen)
							sfrsWindow->Update();

						if (sramWindowHandle)
							DrawSramValues();

						if (eepromWindowHandle)
							DrawEepromValues();

						if (stackWindowHandle)
							DrawStackValues();

						if (chipWindowOpen)
							chipWindow->Update();

						if (debugMode == DM_SNAPSHOT)
							controlChar = 'C';
						else if (debugMode == DM_ANIMATE)
							controlChar = 'S';
										// All other modes have stopped
										// so wait for a new mode or
										// a request for more data (sram etc).
						gettingData = false;
						askedFor = SENDPC;
					}

					endCondition = true;
					continue;
				}
				else					// Comms error
				{
					endCondition = true;
					debugMode = DM_STOP;
					controlChar = 'R';
					askedFor = SENDPC;
					continue;
			
				
				
				}
			}
		}
		else							// If no bytes received
		{
			if (::GetTickCount()-commsTimer > RXTIMEOUT)
			{							// If data stream has stopped
										// assume the target was reset
				CalculateExpected();	// Calculate number of bytes expected
				while (!TxChar(askedFor)); // Send data request code
				commsTimer = ::GetTickCount();
//				while (!Rx(1))			// Wait for request code to be echoed
//				{
//					if (::GetTickCount()-commsTimer > RXTIMEOUT)
//					{
//						while (!Tx(askedFor)); // Transmit code
//						commsTimer = ::GetTickCount();
//					}
//				}
				dataIndex = 0;			// Reset buffer index
//				pcFile = NULL;			// Reset PC source code file
				pcLine = -1;			// Reset PC source code line
			}
		}
	} // end while

	ExitInstance();

	return msg.wParam;
}


void CStudioApp::UpdateSFRs(unsigned char * values, int offset, int count)
{
	int pos = offset;
	if (pos >= sfrsSize)
		return;
	do
	{
		sfrs[pos] = values[pos];
		sfrsMask[pos] = 1;
		pos++;
	}
	while (pos < offset+count && pos < sfrsSize);
}


void CStudioApp::UpdateSram(unsigned char * values, int offset, int count)
{
	int pos;
	int address;

	pos = offset;
	address = sramFirst+pos;
	do
	{
		if (address < sramStart+sramSize)
		{
			sram[address-sramStart] = values[pos];
			sramMask[address-sramStart] = 1;
		}
		pos++;
		address++;
	}
	while (pos < offset+count && address < sramLast &&
		address < sramStart+sramSize);
}


void CStudioApp::UpdateEeprom(unsigned char * values, int offset, int count)
{
	int pos;
	int address;

	pos = offset;
	address = eepromFirst+pos;
	do
	{
		if (address < eepromStart+eepromSize)
		{
			eeprom[address-eepromStart] = values[pos];
			eepromMask[address-eepromStart] = 1;
		}
		pos++;
		address++;
	}
	while (pos < offset+count && address < eepromLast &&
		address < eepromStart+eepromSize);
}


void CStudioApp::GetInfoBlock()
{
	DWORD time;
	BOOL oldISD = FALSE;
	int length;
	int i;

#ifdef MAINDEBUG
debug<<"CStudioApp::GetInfoBlock()"<<endl;
#endif

	while (!TxChar((unsigned char)'I')); // Want Info-Block
	while (!TxChar(askedFor));			// Send data request code
	time = ::GetTickCount();			// Remember time

	while (!Rx(1))						// While no bytes received
	{
		if (::GetTickCount()-time > 100)
		{								// If more than 100ms since last byte
										// Try again
			while (!TxChar((unsigned char)'I')); // Info-Block
			while (!TxChar(askedFor));	// Transmit data option
			time = ::GetTickCount();	// Remember time
			dataIndex = 0;				// Reset buffer pointer
		}
	}
	dataIndex++;						// Got the block length
	
	length = rxBuffer[0];				// Block length	

	while (dataIndex != length)
	{									// Get Info-Block
		while (!Rx(1))					// While no bytes received
		{
			if (::GetTickCount()-time > 100)
			{							// If more than 100ms since last byte
										// Try again
				while (!TxChar((unsigned char)'I')); // Info-Block
				while (!TxChar(askedFor));	// Transmit data option
				time = ::GetTickCount(); // Remember time
				dataIndex = 0;			// Reset buffer pointer
			}
		}
		dataIndex++;					// Got an info byte

		if (dataIndex == 2)				// Just got version
		{
			if (rxBuffer[1] < 0x16 || rxBuffer[1] == '0')
			{							// If ISD's version is earlier than 1.5
				length = OLDINFOBLOCKSIZE;
				char buffer[1000];
				strcpy(buffer,
"Your Target is using an old ISD. Please use the latest version\r\n\
of the ISD for your chosen target CPU. You are strongly recommended\r\n\
to close this project immediatly and change the ISD. The earliest\r\n\
supported ISD is version 1.6.");
				::MessageBox(NULL, buffer, "Old In-System Debugger!",
					MB_OK|MB_ICONSTOP);
			}
		}
	}

	targetFamily = rxBuffer[2];			// Target
	targetCPU = rxBuffer[3];
	if (project->GetFamily() != targetFamily)
	{
		char buffer[1000];
		strcpy(buffer,
"Your Target CPU appears to belong to an unknown CPU family.\r\n\
You are strongly recommended to close this project immediatly\r\n\
and check your target to make sure you have the right one\r\n\
connected.\r\n\r\n\
Only the Atmel AVR CPU family is supported at this time.");
		::MessageBox(NULL, buffer, "Target CPU Family Mismatch!",
			MB_OK|MB_ICONSTOP);
	}

	if (project->GetChip() != targetCPU)
	{
		char buffer[1000];
		strcpy(buffer,
"Your Target CPU appears to be a");
		strcpy(&buffer[900], &chipNames[targetCPU*chipNameLength]);
		char firstChar = buffer[900];
		if (firstChar == 'A' || firstChar =='a' ||
			firstChar == 'E' || firstChar =='e' ||
			firstChar == 'I' || firstChar =='i' ||
			firstChar == 'O' || firstChar =='o' ||
			firstChar == 'U' || firstChar =='u')
		{
			strcat(buffer, "n");
		}
		strcat(buffer, " ");
		strcat(buffer, &buffer[900]);
		strcat(buffer, ".\r\n\
This does not match the one specified in your project's\r\n\
settings which is a");
		strcpy(&buffer[900], &chipNames[project->GetChip()*chipNameLength]);
		firstChar = buffer[900];
		if (firstChar == 'A' || firstChar =='a' ||
			firstChar == 'E' || firstChar =='e' ||
			firstChar == 'I' || firstChar =='i' ||
			firstChar == 'O' || firstChar =='o' ||
			firstChar == 'U' || firstChar =='u')
		{
			strcat(buffer, "n");
		}
		strcat(buffer, " ");
		strcat(buffer, &buffer[900]);
		strcat(buffer, ".\r\n\
Would you like it changed to match your target? (Strongly Recommended)\r\n\r\n\
Also, make sure you have the correct target project connected!!");
		if (::MessageBox(NULL, buffer, "Target Mismatch!",
			MB_YESNO|MB_ICONQUESTION) == IDYES)
		{
			project->SetChip(targetCPU);
		}
	}

	if (rxBuffer[4])					// If custom SRAM & EEPROM sizes
	{
		if (sramSize)
		{
			delete sram;
			delete sramMask;
		}
		sram = NULL;
		sramMask = NULL;
		if (eepromSize)
		{
			delete eeprom;
			delete eepromMask;
		}
		eeprom = NULL;
		eepromMask = NULL;

		sramSize = rxBuffer[7];
		sramSize <<= 8;
		sramSize += rxBuffer[6];
		sramSize <<= 8;
		sramSize += rxBuffer[5];
		eepromSize = rxBuffer[10];
		eepromSize <<= 8;
		eepromSize += rxBuffer[9];
		eepromSize <<= 8;
		eepromSize += rxBuffer[8];

		if (project->externalMem)
		{
			if (project->externalMemStart < sramStart)
			{
				sramSize += sramStart-project->externalMemStart;
				sramStart = project->externalMemStart;
			}
			if (project->externalMemStart+project->externalMemSize >
				sramStart+sramSize)
			{
				sramSize = (project->externalMemStart+
					project->externalMemSize)-sramStart;
			}
		}
		if (sramSize)
		{
			sram = new unsigned char[sramSize];
			sramMask = new unsigned char[sramSize];
			for (i=0; i<sramSize; i++)
				sramMask[i] = 0;
			if (sramWindowHandle)		// If Sram window open
			{							// Get it to recalculate itself
				::SendMessage(sramWindowHandle, WM_SIZE, 0,
					MAKELPARAM(sramClientW, sramClientH));
				::InvalidateRect(sramWindowHandle, NULL,
					FALSE);
			}
		}
		else
		{
			if (sramWindowHandle)
				::SendMessage(sramWindowHandle, WM_CLOSE, 0, 0);
			if (stackWindowHandle)
				::SendMessage(stackWindowHandle, WM_CLOSE, 0, 0);
		}
		if (eepromSize)
		{
			eeprom = new unsigned char[eepromSize];
			eepromMask = new unsigned char[eepromSize];
			for (i=0; i<eepromSize; i++)
				eepromMask[i] = 0;
			if (eepromWindowHandle)		// If Eeprom window open
			{							// Get it to recalculate itself
				::SendMessage(eepromWindowHandle, WM_SIZE, 0,
					MAKELPARAM(eepromClientW, eepromClientH));
				::InvalidateRect(eepromWindowHandle, NULL,
					FALSE);
			}
		}
		else
		{
			if (eepromWindowHandle)
				::SendMessage(eepromWindowHandle, WM_CLOSE, 0, 0);
		}

		project->debugStackBytes = rxBuffer[11];
	}
	else								// Using default SRAM & EEPROM sizes
		project->debugStackBytes = rxBuffer[5];

	if (sramWindowHandle)				// If Sram window open
	{									// Get it to recalculate itself
		::SendMessage(sramWindowHandle, WM_SIZE, 0,
			MAKELPARAM(sramClientW, sramClientH));
		::InvalidateRect(sramWindowHandle, NULL,
			FALSE);
	}
	if (eepromWindowHandle)				// If Eeprom window open
	{									// Get it to recalculate itself
		::SendMessage(eepromWindowHandle, WM_SIZE, 0,
			MAKELPARAM(eepromClientW, eepromClientH));
		::InvalidateRect(eepromWindowHandle, NULL,
			FALSE);
	}
	if (stackWindowHandle)				// If Stack window open
	{									// Get it to recalculate itself
		::SendMessage(stackWindowHandle, WM_SIZE, 0,
			MAKELPARAM(stackClientW, stackClientH));
		EnsureStackVisible();
		::InvalidateRect(stackWindowHandle, NULL,
			FALSE);
	}
	needInfoBlock = false;				// Don't need it now!!
}


void CStudioApp::CalculateExpected()
{
	if (askedFor == SENDPC)
		expected = pcSize;
	else if (askedFor == SENDREGISTERS)
		expected = registersSize;
	else if (askedFor == SENDSFRS)
		expected = sfrsSize;
	else if (askedFor == SENDSRAM)
		expected = sramLast-sramFirst;
	else if (askedFor == SENDEEPROM)
		expected = eepromLast-eepromFirst;
}


int CStudioApp::ExitInstance()
{
	int i;

	if (project)						// First, close the project
	{
		project->Close();
		delete project;
		project = NULL;
	}

	if (sourceWindows)
		delete sourceWindows;
	sourceWindows = NULL;

	if (registers)						// Delete the data buffers
		delete registers;
	if (sfrs)
		delete sfrs;
	if (sram)
		delete sram;
	if (eeprom)
		delete eeprom;
	if (registersMask)					// Delete the data masks
		delete registersMask;
	if (sfrsMask)
		delete sfrsMask;
	if (sramMask)
		delete sramMask;
	if (eepromMask)
		delete eepromMask;

	if (availableComPorts)				// Delete com port info
		delete availableComPorts;
	if (comPortIndexes)
		delete comPortIndexes;
	if (rxBuffer)						// Delete the Comms buffer
		delete rxBuffer;

										// Delete the Global Settings
	if (globals.workingDirectory)
		delete globals.workingDirectory;
//	if (globals.usersDirectory)
//		delete globals.usersDirectory;

	delete errorMessage;

	delete safeSerial;

	if (familyVersions)
	{
		for (i=0; i<numberFamilys; i++)
		{
			if (familyVersions[i])
				delete familyVersions[i];
			familyVersions[i] = NULL;
		}
		delete familyVersions;
	}
	familyVersions = NULL;	
	if (familyNames)
	{
		for (i=0; i<numberFamilys; i++)
		{
			if (familyNames[i*2])
				delete familyNames[i*2];
			familyNames[i*2] = NULL;
			if (familyNames[i*2+1])
				delete familyNames[i*2+1];
			familyNames[i*2+1] = NULL;
		}
		delete familyNames;
	}
	familyNames = NULL;
	if (familyIDs)
		delete familyIDs;
	familyIDs = NULL;
	numberFamilys = 0;
	
	if (ideVersions)
	{
		for (i=0; i<numberIDEs; i++)
		{
			if (ideVersions[i])
				delete ideVersions[i];
			ideVersions[i] = NULL;
		}
		delete ideVersions;
	}
	ideVersions = NULL;	
	if (ideNames)
	{
		for (i=0; i<numberIDEs; i++)
		{
			if (ideNames[i*2])
				delete ideNames[i*2];
			ideNames[i*2] = NULL;
			if (ideNames[i*2+1])
				delete ideNames[i*2+1];
			ideNames[i*2+1] = NULL;
		}
		delete ideNames;
	}
	ideNames = NULL;
	if (ideIDs)
		delete ideIDs;
	ideIDs = NULL;
	numberIDEs = 0;

//	if (ideFamilyTable)
//		delete ideFamilyTable;
//	ideFamilyLookupTable = NULL;

	if (instDir)
		delete [] instDir;
	instDir = NULL;

	if (hIdeDll)						// If an IDE library is open
		::FreeLibrary(hIdeDll);			// Close it
	hIdeDll = NULL;
	
	if (hFamilyDll)						// If a Family library is open
		::FreeLibrary(hFamilyDll);		// Close it
	hFamilyDll = NULL;
	numberChips = 0;
	if (chipIDs)
		delete chipIDs;
	chipIDs = NULL;
	if (chipNames)
		delete chipNames;
	chipNames = NULL;
	numberPackages = 0;
	if (packageNames)
		delete packageNames;
	packageNames = NULL;

#ifdef DEBUG
debug<<"CStudioApp::ExitInstance(): Closing Debug file"<<endl;
debug.close();
#endif

	return CWinApp::ExitInstance();
}


void CStudioApp::SetTitle()
{
	char buffer[1000];
	int max = 1000;
	GetTitle(buffer, &max);
	m_pMainWnd->SetWindowText(buffer);
}


BOOL CStudioApp::GetTitle(char * title, int * max)
{
	CString str = STRING_NAME;
	if (project != NULL)
	{
		char buffer[1000];
		str += " - ";
		str += project->GetSPPName();
		str += " (";
		int m = 1000;
		project->GetFamilyName(buffer, &m);
		str += buffer;
		str += " & ";
		m = 1000;
		project->GetIDEName(buffer, &m);
		str += buffer;
		str += ")";
	}
	if (title && *max > str.GetLength())
	{
		strcpy(title, str);
		return TRUE;
	}
	*max = str.GetLength()+1;
	if (!title)
		return TRUE;
	return FALSE;
}


void CStudioApp::SetSourceMenu()
{
	int i;
	CMenu * menu;
	if (project && project->IsOpen())
	{
		menu = m_pMainWnd->GetMenu();
		menu = menu->GetSubMenu(1);		// View Menu
		menu->EnableMenuItem(5, MF_BYPOSITION|MF_ENABLED);
		menu = menu->GetSubMenu(5);		// View->Source Menu
		while (menu->DeleteMenu(0, MF_BYPOSITION)); // Delete all items
		for (i=0; i<project->GetNumFiles(); i++)
		{
			menu->AppendMenu(MF_ENABLED | MF_STRING, ID_VIEW_SOURCE+i,
				project->GetFile(i)->GetFileName());
		}
		m_pMainWnd->DrawMenuBar();		// Update menu bar
	}
	else
	{
		if (!m_pMainWnd)
			return;
		menu = m_pMainWnd->GetMenu();
		menu = menu->GetSubMenu(1);		// View menu
		menu->EnableMenuItem(5, MF_BYPOSITION|MF_GRAYED); // Gray Source popup
		m_pMainWnd->DrawMenuBar();		// Update menu bar
	}
}


void CStudioApp::SetProjectMenu()
{
	CMenu * menu;
	HKEY localmachine;
	HKEY software;
	HKEY company;
	HKEY product;
	HKEY version;
	HKEY projects;
	unsigned long length;
	char buffer[10000];
	long regError;

	// Find the recent project list
	if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ,
		&localmachine) == ERROR_SUCCESS)
	{
		if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0,
			KEY_READ, &software) == ERROR_SUCCESS)
		{
			if (regError = RegOpenKeyEx(software, COMPANYKEY, 0, KEY_READ,
				&company) == ERROR_SUCCESS)
			{
				if (regError = RegOpenKeyEx(company, PRODUCTKEY, 0,
					KEY_READ, &product) == ERROR_SUCCESS)
				{
					if (regError = RegOpenKeyEx(product, VERSIONKEY, 0,
						KEY_READ, &version) == ERROR_SUCCESS)
					{
						if (regError = RegOpenKeyEx(version, PROJECTSKEY, 0,
							KEY_READ, &projects) == ERROR_SUCCESS)
						{
							if (!m_pMainWnd)
								return;
							menu = m_pMainWnd->GetMenu();
							menu = menu->GetSubMenu(0); // File menu
							while (menu->GetMenuItemID(6) != ID_APP_EXIT)
								menu->DeleteMenu(6, MF_BYPOSITION);
							int nextPos = 6;
							char itemText[1000];
							CString key = "Recent 1";
							bool done = false;
							do
							{
								key.Insert(7, (char)('1'+(nextPos-6)));
								key = key.Left(8);
								length = 10000;
								regError = RegQueryValueEx(projects,
									LPCTSTR(key), NULL, NULL,
									(unsigned char *)buffer, &length);
								if (regError == ERROR_SUCCESS)
								{
									itemText[0] = '&';
									itemText[1] = (char)('1'+(nextPos-6));
									itemText[2] = 0;
									strcat(itemText, " ");
									strcat(itemText, buffer);
									menu->InsertMenu(nextPos,
										MF_BYPOSITION|MF_STRING,
										ID_FILE_RECENTPROJECT+(nextPos-6),
										itemText);
//									menu->SetMenuItemBitmaps(nextPos,
//										MF_BYPOSITION, projectMenuBitmap,
//										projectMenuBitmap);
									nextPos++;
								}
								else
									done = true;
							}
							while (!done && nextPos < 6+4);

							// If menu options added, add a separator
							if (nextPos != 6)
							{
								menu->InsertMenu(nextPos,
									MF_BYPOSITION|MF_SEPARATOR);
							}
							RegCloseKey(version);
						}
						RegCloseKey(projects);
					}
					RegCloseKey(product);
				}
				RegCloseKey(company);
			}
			RegCloseKey(software);
		}
		RegCloseKey(localmachine);
	}

//	menu = m_pMainWnd->GetMenu();
//	menu = menu->GetSubMenu(2);		// Project Menu
//	if (project && project->IsOpen())
//		menu->EnableMenuItem(3, MF_BYPOSITION|MF_ENABLED);
//	else
//		menu->EnableMenuItem(3, MF_BYPOSITION|MF_GRAYED); // Gray Target popup
	m_pMainWnd->DrawMenuBar();		// Update menu bar
}


int CStudioApp::OpenIdeLibrary(char * libName)
{
	int i;
	CString libPath;

	if (hIdeDll)
		::FreeLibrary(hIdeDll);
	hIdeDll = NULL;

	i = 0;
	do
	{
		if (stricmp(libName, ideNames[i*2]) == 0)
		{
			libPath = instDir;
			libPath += "Plug-ins\\";
			libPath += ideNames[i*2+1];
			hIdeDll = ::LoadLibrary(libPath);
			if (!hIdeDll)
			{							// If failed to load library
				errorCode = SPERROR_MISSINGIDE;
				strcpy(errorMessage, "IDE Library: ");
				strcat(errorMessage, ideNames[i*2]);
				strcat(errorMessage, "\r\nIDE Library File: ");
				strcat(errorMessage, ideNames[i*2+1]);
				strcat(errorMessage, "\r\n");
				return FALSE;
			}
			ILGetIdeID = (GETIDEID)GetProcAddress(hIdeDll, "GetIdeID");
			int id = ILGetIdeID();
			if (!(globals.ides&id))
			{				// If not registered
				::FreeLibrary(hIdeDll);
				hIdeDll = NULL;
				errorCode = SPERROR_IDENOTLICENSED;
				strcpy(errorMessage, "IDE Library: ");
				strcat(errorMessage, ideNames[i*2]);
				strcat(errorMessage, "\r\n");
				return FALSE;
			}
		}
		i++;
	}
	while (hIdeDll == NULL && i < numberIDEs);

	if (!hIdeDll)
	{
		errorCode = SPERROR_MISSINGIDE;
		strcpy(errorMessage, "IDE Library: ");
		strcat(errorMessage, libName);
		strcat(errorMessage, "\r\n");
		return FALSE;
	}

//	ILGetIdeID = (GETIDEID)GetProcAddress(hIdeDll, "GetIdeID"); // Already got!
	ILGetIdeName = (GETIDENAME)GetProcAddress(hIdeDll, "GetIdeName");
	ILGetIdeLogo = (GETIDELOGO)GetProcAddress(hIdeDll, "GetIdeLogo");
	ILGetSupportedFamilys = (GETSUPPORTEDFAMILYS)GetProcAddress(hIdeDll,
		"GetSupportedFamilys");
	ILGetFamilySupportID = (GETFAMILYSUPPORTID)GetProcAddress(hIdeDll,
		"GetFamilySupportID");
	ILGetProjectFileExt = (GETPROJECTFILEEXT)GetProcAddress(hIdeDll,
		"GetProjectFileExt");
	ILGetProjectFileFilter = (GETPROJECTFILEFILTER)GetProcAddress(hIdeDll,
		"GetProjectFileFilter");
	ILIsProjectAssembler = (ISPROJECTASSEMBLER)GetProcAddress(hIdeDll,
		"IsProjectAssembler");
	ILGetOutputDir = (GETOUTPUTDIR)GetProcAddress(hIdeDll, "GetOutputDir");
	ILGetISD = (GETISD)GetProcAddress(hIdeDll, "GetISD");
	ILGetDebugInfo = (GETDEBUGINFO)GetProcAddress(hIdeDll, "GetDebugInfo");
	ILDeleteDebugInfo = (DELETEDEBUGINFO)GetProcAddress(hIdeDll, "DeleteDebugInfo");
	ILGetCodeInfo = (GETCODEINFO)GetProcAddress(hIdeDll, "GetCodeInfo");
	ILFindHardBreakpoints = (FINDHARDBREAKPOINTS)GetProcAddress(hIdeDll, "FindHardBreakpoints");
	ILGetAsmComment = (GETASMCOMMENT)GetProcAddress(hIdeDll, "GetAsmComment");
	ILGetAsmStringChar = (GETASMSTRINGCHAR)GetProcAddress(hIdeDll, "GetAsmStringChar");
	ILGetAsmCharChar = (GETASMCHARCHAR)GetProcAddress(hIdeDll, "GetAsmCharChar");
	ILGetAsmEscapeChar = (GETASMESCAPECHAR)GetProcAddress(hIdeDll, "GetAsmEscapeChar");
	ILGetAsmDirectiveChar = (GETASMDIRECTIVECHAR)GetProcAddress(hIdeDll, "GetAsmDirectiveChar");
	ILGetAsmDirectives = (GETASMDIRECTIVES)GetProcAddress(hIdeDll, "GetAsmDirectives");
	ILGetAsmKeywordChars = (GETASMKEYWORDCHARS)GetProcAddress(hIdeDll, "GetAsmKeywordChars");
	ILGetAsmKeywords = (GETASMKEYWORDS)GetProcAddress(hIdeDll, "GetAsmKeywords");
	ILGetCComment = (GETCCOMMENT)GetProcAddress(hIdeDll, "GetCComment");
	ILGetCCommentStart = (GETCCOMMENTSTART)GetProcAddress(hIdeDll, "GetCCommentStart");
	ILGetCCommentEnd = (GETCCOMMENTEND)GetProcAddress(hIdeDll, "GetCCommentEnd");
	ILGetCStringChar = (GETCSTRINGCHAR)GetProcAddress(hIdeDll, "GetCStringChar");
	ILGetCCharChar = (GETCCHARCHAR)GetProcAddress(hIdeDll, "GetCCharChar");
	ILGetCEscapeChar = (GETCESCAPECHAR)GetProcAddress(hIdeDll, "GetCEscapeChar");
	ILGetCDirectiveChar = (GETCDIRECTIVECHAR)GetProcAddress(hIdeDll, "GetCDirectiveChar");
	ILGetCDirectives = (GETCDIRECTIVES)GetProcAddress(hIdeDll, "GetCDirectives");
	ILGetCKeywordChars = (GETCKEYWORDCHARS)GetProcAddress(hIdeDll, "GetCKeywordChars");
	ILGetCKeywords = (GETCKEYWORDS)GetProcAddress(hIdeDll, "GetCKeywords");

	ILGetLastIsdError = (GETLASTISDERROR)GetProcAddress(hIdeDll, "GetLastIsdError");

	return TRUE;
}


int CStudioApp::OpenFamilyLibrary(char * libName)
{
	int length;
	CString libPath;
	
	if (hFamilyDll)
		::FreeLibrary(hFamilyDll);
	hFamilyDll = NULL;
	numberChips = 0;
	if (chipIDs)
		delete chipIDs;
	chipIDs = NULL;
	if (chipNames)
		delete chipNames;
	chipNames = NULL;
	numberPackages = 0;
	if (packageNames)
		delete packageNames;
	packageNames = NULL;

	libPath = instDir;
	libPath += "Plug-ins\\";
	if (strcmp(libName, "AVR") == 0) // **** Hard coded !!
	{
		libPath += "AVR.dll";
		hFamilyDll = ::LoadLibrary(libPath);
	}

	if (!hFamilyDll)
	{
		errorCode = SPERROR_MISSINGFAMILY;
		strcpy(errorMessage, "Family Library: ");
		strcat(errorMessage, libName);
		strcat(errorMessage, "\r\nIDE Library File: ");
		strcat(errorMessage, "AVR.dll"); // **** Hard coded!!
		strcat(errorMessage, "\r\n");
		return FALSE;
	}

	CLGetFamilyID = (GETFAMILYID)GetProcAddress(hFamilyDll, "GetFamilyID");
	CLGetFamilyName = (GETFAMILYNAME)GetProcAddress(hFamilyDll, "GetFamilyName");
	CLGetFamilyLogo = (GETFAMILYLOGO)GetProcAddress(hFamilyDll, "GetFamilyLogo");
	CLGetNumberChips = (GETNUMBERCHIPS)GetProcAddress(hFamilyDll, "GetNumberChips");
	CLGetChipIDs = (GETCHIPIDS)GetProcAddress(hFamilyDll, "GetChipIDs");
	CLGetChipIdIndex = (GETCHIPIDINDEX)GetProcAddress(hFamilyDll, "GetChipIdIndex");
	CLGetChipNameLength = (GETCHIPNAMELENGTH)GetProcAddress(hFamilyDll, "GetChipNameLength");
	CLGetChipNames = (GETCHIPNAMES)GetProcAddress(hFamilyDll, "GetChipNames");
	CLGetChipPCSize = (GETCHIPPCSIZE)GetProcAddress(hFamilyDll, "GetChipPCSize");
	CLGetChipRegistersSize = (GETCHIPREGISTERSSIZE)GetProcAddress(hFamilyDll, "GetChipRegistersSize");
	CLGetChipSramStart = (GETCHIPSRAMSTART)GetProcAddress(hFamilyDll, "GetChipSramStart");
	CLGetChipSramSize = (GETCHIPSRAMSIZE)GetProcAddress(hFamilyDll, "GetChipSramSize");
	CLGetChipEepromStart = (GETCHIPEEPROMSTART)GetProcAddress(hFamilyDll, "GetChipEepromStart");
	CLGetChipEepromSize = (GETCHIPEEPROMSIZE)GetProcAddress(hFamilyDll, "GetChipEepromSize");
	CLGetNumberPins = (GETNUMBERPINS)GetProcAddress(hFamilyDll, "GetNumberPins");
	CLSetPinsToDefault = (SETPINSTODEFAULT)GetProcAddress(hFamilyDll, "SetPinsToDefault");
	CLSetChipPortPins = (SETCHIPPORTPINS)GetProcAddress(hFamilyDll, "SetChipPortPins");
	CLGetDefaultPackage = (GETDEFAULTPACKAGE)GetProcAddress(hFamilyDll, "GetDefaultPackage");
	CLGetPackageIndex = (GETPACKAGEINDEX)GetProcAddress(hFamilyDll, "GetPackageIndex");
	CLGetChipImage = (GETCHIPIMAGE)GetProcAddress(hFamilyDll, "GetChipImage");
	CLGetChipImageSize = (GETCHIPIMAGESIZE)GetProcAddress(hFamilyDll, "GetChipImageSize");
	CLGetIdFromName = (GETIDFROMNAME)GetProcAddress(hFamilyDll, "GetIdFromName");
	CLGetNumberPackages = (GETNUMBERPACKAGES)GetProcAddress(hFamilyDll, "GetNumberPackages");
	CLGetPackageNameLength = (GETPACKAGENAMELENGTH)GetProcAddress(hFamilyDll, "GetPackageNameLength");
	CLGetPackageNames = (GETPACKAGENAMES)GetProcAddress(hFamilyDll, "GetPackageNames");
	CLGetAvailablePackages = (GETAVAILABLEPACKAGES)GetProcAddress(hFamilyDll, "GetAvailablePackages");
	CLGetPackageIdFromName = (GETPACKAGEIDFROMNAME)GetProcAddress(hFamilyDll, "GetPackageIdFromName");
	CLGetNumberSFRs = (GETNUMBERSFRS)GetProcAddress(hFamilyDll, "GetNumberSFRs");
	CLGetSfrMarkers = (GETSFRMARKERS)GetProcAddress(hFamilyDll, "GetSfrMarkers");
	CLGetSfrNameLength = (GETSFRNAMELENGTH)GetProcAddress(hFamilyDll, "GetSfrNameLength");
	CLGetSfrNames = (GETSFRNAMES)GetProcAddress(hFamilyDll, "GetSfrNames");
	CLGetSfrFlagMarkers = (GETSFRFLAGMARKERS)GetProcAddress(hFamilyDll, "GetSfrFlagMarkers");
	CLGetSfrFlagNameLength = (GETSFRFLAGNAMELENGTH)GetProcAddress(hFamilyDll, "GetSfrFlagNameLength");
	CLGetSfrFlagNames = (GETSFRFLAGNAMES)GetProcAddress(hFamilyDll, "GetSfrFlagNames");

	numberChips = CLGetNumberChips();
	chipIDs = new int[numberChips];
	CLGetChipIDs(chipIDs, &numberChips);
	chipNameLength = CLGetChipNameLength();
	length = (chipIDs[numberChips-1]+1)*chipNameLength;
	chipNames = new char[length];
	CLGetChipNames(chipNames, &length);
	numberPackages = CLGetNumberPackages();
	packageNameLength = CLGetPackageNameLength();
	length = numberPackages*packageNameLength;
	packageNames = new char[length];
	CLGetPackageNames(packageNames, &length);

	return TRUE;
}


void CStudioApp::SaveWindowPreferences()
{
	HKEY localmachine;
	HKEY software;
	HKEY company;
	HKEY product;
	HKEY version;
	unsigned long length;
	long regError;
	int flag;

	// Save the user's Preferences
	if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, NULL, 0, KEY_READ,
		&localmachine) == ERROR_SUCCESS)
	{
		if (regError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software", 0,
			KEY_READ, &software) == ERROR_SUCCESS)
		{
			if (regError = RegOpenKeyEx(software, COMPANYKEY, 0, KEY_READ,
				&company) == ERROR_SUCCESS)
			{
				if (regError = RegOpenKeyEx(company, PRODUCTKEY, 0,
					KEY_READ, &product) == ERROR_SUCCESS)
				{
					if (regError = RegOpenKeyEx(product, VERSIONKEY, 0,
						KEY_READ|KEY_WRITE, &version) == ERROR_SUCCESS)
					{
						length = 4;
						flag = 0;
						if (globals.syntaxHiLighting)
							flag = 1;
						RegSetValueEx(version, "Syntax HiLighting", NULL,
							REG_DWORD, (CONST BYTE *)&flag, length);
						flag = 0;
						if (globals.pcTracking)
							flag = 1;
						RegSetValueEx(version, "PC Tracking", NULL,
							REG_DWORD, (CONST BYTE *)&flag, length);
						flag = 0;
						if (globals.assembler)
							flag = 1;
						RegSetValueEx(version, "Assembler", NULL,
							REG_DWORD, (CONST BYTE *)&flag, length);
						flag = 0;
						if (globals.addresses)
							flag = 1;
						RegSetValueEx(version, "Addresses", NULL,
							REG_DWORD, (CONST BYTE *)&flag, length);
						flag = 0;
						if (globals.machineCode)
							flag = 1;
						RegSetValueEx(version, "MachineCode", NULL,
							REG_DWORD, (CONST BYTE *)&flag, length);
						flag = globals.commsTimeout;
						RegSetValueEx(version, "CommsTimeout", NULL,
							REG_DWORD, (CONST BYTE *)&flag, length);
						RegCloseKey(version);
					}
					RegCloseKey(product);
				}
				RegCloseKey(company);
			}
			RegCloseKey(software);
		}
		RegCloseKey(localmachine);
	}
}


void CStudioApp::OnFileNew()
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);

	// create a new MDI child window
//	pFrame->CreateNewChild(
//		RUNTIME_CLASS(CChildFrame), IDR_STUDIOTYPE, m_hMDIMenu, m_hMDIAccel);
}


int CStudioApp::OpenSourceFile(int index)
{
	if (!project || index < 0 || index >= project->GetNumFiles())
		return FALSE;
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	CSourceWindow * window = (CSourceWindow *)pFrame->CreateNewChild(
		RUNTIME_CLASS(CSourceWindow), IDR_SOURCE);
	window->SetSourceFile(project->GetFile(index));
	window->GetSourceFile()->open = TRUE;
	window->GetSourceFile()->SetWindowHandle(window->m_hWnd);
	if (!window->GetSourceFile()->IsWindowMinimized() &&
		!window->GetSourceFile()->IsWindowMaximized())
		window->ShowWindow(SW_SHOWNORMAL);
	if (window->GetSourceFile()->IsWindowMinimized())
		window->ShowWindow(SW_MINIMIZE);
	if (window->GetSourceFile()->IsWindowMaximized())
		window->ShowWindow(SW_MAXIMIZE);
	window->UpdateWindow();
	sourceWindows[index] = window;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CStudioApp message handlers

BOOL CStudioApp::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// If pHandlerInfo is NULL, then handle the message
	if (pHandlerInfo == NULL)
	{
		if ((int)nID >= ID_FILE_RECENTPROJECT &&
			(int)nID < ID_FILE_RECENTPROJECT+4)
		{
			if (nCode == CN_COMMAND)
			{
				if (nID == ID_FILE_RECENTPROJECT && project != NULL)
					return TRUE;		// Already open!!
				char buffer[1000];
				CMenu * menu = m_pMainWnd->GetMenu();
				menu = menu->GetSubMenu(0); // File Menu
				menu->GetMenuString((nID-ID_FILE_RECENTPROJECT)+6, buffer,
					1000, MF_BYPOSITION);
				if (project)
				{
					if (!project->Close())
						return TRUE;	// User didn't want to close
					delete project;
				}
				project = new FDProject();
				if (!project->Open(&buffer[3]))
				{
					ErrorMessage();
					delete project;
					project = NULL;
					return TRUE;
				}
				SetTitle();
				return TRUE;
			}
			else if (nCode == CN_UPDATE_COMMAND_UI)
			{
				// Update UI element state
				CCmdUI * cmdUI = (CCmdUI *)pExtra;
				cmdUI->Enable(true); // Enable all
				return TRUE;
			}
		}

		if (project != NULL &&
			(int)nID >= ID_VIEW_SOURCE &&
			(int)nID < ID_VIEW_SOURCE+project->GetNumFiles())
		{
			int id = (int)nID-ID_VIEW_SOURCE;
			if (nCode == CN_COMMAND)
			{
				return OpenSourceFile(id);
			}
			else if (nCode == CN_UPDATE_COMMAND_UI)
			{
				CCmdUI * cmdUI = (CCmdUI *)pExtra;
				cmdUI->Enable(!project->GetFile(id)->IsOpen());
										// Enable if not open
				return TRUE;
			}
		}

		if ((int)nID >= IDM_HELP_IDE &&
			(int)nID < IDM_HELP_IDE+numberIDEs)
		{
			int id = (int)nID-IDM_HELP_IDE;
			if (nCode == CN_COMMAND)
			{
				CString helpFile = instDir;
				helpFile += "Plug-ins\\";
				helpFile += ideNames[id*2];
				helpFile += ".chm";
				HWND helpWnd = HtmlHelp(m_pMainWnd->m_hWnd, helpFile,
					HH_DISPLAY_TOPIC, 0);
				helpFile = ideNames[id*2];
				helpFile += " Help";
				::SetWindowText(helpWnd, helpFile);
				return TRUE;
			}
			else if (nCode == CN_UPDATE_COMMAND_UI)
			{
				CCmdUI * cmdUI = (CCmdUI *)pExtra;
				cmdUI->Enable(true);	// Always enable
				return TRUE;
			}
		}
	}

	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CStudioApp::OnViewMaintoolbar() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	if (pFrame->m_wndProjectBar.IsWindowVisible())
		pFrame->ShowControlBar(&(pFrame->m_wndProjectBar), FALSE, FALSE);
	else
		pFrame->ShowControlBar(&(pFrame->m_wndProjectBar), TRUE, FALSE);
}


void CStudioApp::OnUpdateViewMaintoolbar(CCmdUI* pCmdUI) 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	if (pFrame->m_wndProjectBar.IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}


void CStudioApp::OnViewTargetcontrolbar() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	if (pFrame->m_wndTControlBar.IsWindowVisible())
		pFrame->ShowControlBar(&(pFrame->m_wndTControlBar), FALSE, FALSE);
	else
		pFrame->ShowControlBar(&(pFrame->m_wndTControlBar), TRUE, FALSE);
}


void CStudioApp::OnUpdateViewTargetcontrolbar(CCmdUI* pCmdUI) 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	if (pFrame->m_wndTControlBar.IsWindowVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}


void CStudioApp::OnFileNewproject() 
{
	CProjectWizard * wizard = new CProjectWizard();
	int result = wizard->DoModal();
	delete wizard;
	if (result == IDOK)
	{
		// Get the project file from the user
		char filter[1000];
		int max = 1000;
		ILGetProjectFileFilter(filter, &max);
		CFileDialog * dialog;
		dialog = new CFileDialog(TRUE, NULL, NULL,
			OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, filter);
		if (dialog->DoModal() != IDOK)
		{
			delete dialog;
			return;
		}

		// Got the Project file;
		CString name = dialog->GetFileName();
		name = name.Left(name.ReverseFind('.'));
		project = new FDProject();
		project->SetSPPName((char *)LPCTSTR(name)); // Set project's file name
		project->SetName((char *)LPCTSTR(name)); // Set project's name
		project->SetIDE(ILGetIdeID());		// IDE code
		project->selectedComPort = 1;		// Default comms
		project->selectedBaud = 115200;
		CString path = dialog->GetPathName();
		path = path.Left(path.ReverseFind('\\')+1);
		project->SetSPPPath((char *)LPCTSTR(path)); // Set project's file path
		project->SetPath((char *)LPCTSTR(path)); // Path
		CString projectFile = path+name+".spp";
		delete dialog;

		project->SetFamily(CLGetFamilyID());

		// **** This should be done with a wizard
		project->SetChip(18); // ATmega103
		project->SetPackage(CLGetDefaultPackage(18)); // TQFP

		// Save the new project file
		project->SaveToFile((char *)LPCTSTR(projectFile)); // Save Project file
		if (project->Open((char *)LPCTSTR(projectFile)))
		{
			SetTitle();
			project->Changed();			// Needs saving
		}
		else
		{
			ErrorMessage();
			delete project;
			project = NULL;
		}
	}
}


void CStudioApp::OnUpdateFileNewproject(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!project);
	//&& ideFlags&IDE_AVRSTUDIO &&
//		globals.options >= SS_BASIC && globals.ides&IDE_AVRSTUDIO);
}


void CStudioApp::OnFileOpenproject()
{
	CFileDialog * dialog;
	CString filter = "Project Files (*.spp)|*.spp||";
	dialog = new CFileDialog(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, filter);
	if (dialog->DoModal() != IDOK)
	{
		delete dialog;
		return;
	}
	if (project)
		delete project;
	project = new FDProject();
	if (!project->Open((char *)LPCTSTR(dialog->GetPathName())))
	{
		ErrorMessage();
		delete project;
		project = NULL;
		return;
	}
	SetTitle();
}


void CStudioApp::OnUpdateFileOpenproject(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!project);
}


void CStudioApp::OnFileSave() 
{
	project->Save();	
}


void CStudioApp::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(project != NULL);
}


void CStudioApp::OnFileSaveas() 
{
	if (project->SaveAs())
		SetTitle();
}


void CStudioApp::OnUpdateFileSaveas(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(project != 0);
}


void CStudioApp::OnFileCloseproject()
{
	if (!project->Close())
		return;
	delete project;
	project = NULL;
	pcFile = NULL;
	oldPCFile = NULL;
	pcLine = -1;
	SetTitle();
}


void CStudioApp::OnUpdateFileCloseproject(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(project != NULL);
}


void CStudioApp::OnViewProject()
{
	if (!projectWindowOpen)
	{
		CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
		projectWindow = (CProjectWindow *)pFrame->CreateNewChild(
			RUNTIME_CLASS(CProjectWindow), IDR_PROJECT);
		if (!project->projectWindowInfo.minimized &&
			!project->projectWindowInfo.maximized)
			projectWindow->ShowWindow(SW_SHOWNORMAL);
		if (project->projectWindowInfo.minimized)
			projectWindow->ShowWindow(SW_MINIMIZE);
		if (project->projectWindowInfo.maximized)
			projectWindow->ShowWindow(SW_MAXIMIZE);
		projectWindowOpen = true;
	}
}


void CStudioApp::OnUpdateViewProject(CCmdUI* pCmdUI)
{
	if (project != NULL)
		pCmdUI->Enable(!projectWindowOpen);
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnViewRegisters()
{
	OpenRegistersWindow();
	if (endCondition)
		updateRegisters = true;
}


void CStudioApp::OnUpdateViewRegisters(CCmdUI* pCmdUI)
{
	if (project != NULL)
		pCmdUI->Enable(registersWindowHandle == NULL);
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnViewMemorySfrs() 
{
	if (!sfrsWindowOpen)
	{
		CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
		sfrsWindow = 
			(CSfrsWindow *)pFrame->CreateNewChild(RUNTIME_CLASS(CSfrsWindow),
			IDR_SFRS);
		if (!project->sfrsWindowInfo.minimized &&
			!project->sfrsWindowInfo.maximized)
			sfrsWindow->ShowWindow(SW_SHOWNORMAL);
		if (project->sfrsWindowInfo.minimized)
			sfrsWindow->ShowWindow(SW_MINIMIZE);
		if (project->sfrsWindowInfo.maximized)
			sfrsWindow->ShowWindow(SW_MAXIMIZE);
		sfrsWindowOpen = true;
	}
	if (endCondition)
		updateSfrs = true;
}


void CStudioApp::OnUpdateViewMemorySfrs(CCmdUI* pCmdUI) 
{
	if (project != NULL)
		pCmdUI->Enable(!sfrsWindowOpen);
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnViewMemorySram()
{
	OpenSramWindow();
	if (endCondition)
		updateSram = true;
}


void CStudioApp::OnUpdateViewMemorySram(CCmdUI* pCmdUI)
{
	if (project != NULL && sramSize)
		pCmdUI->Enable(sramWindowHandle == NULL);
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnViewMemoryEeprom()
{
	OpenEepromWindow();
	if (endCondition)
		updateEeprom = true;
}


void CStudioApp::OnUpdateViewMemoryEeprom(CCmdUI* pCmdUI)
{
	if (project != NULL && eepromSize)
		pCmdUI->Enable(eepromWindowHandle == NULL);
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnViewWatch()
{
	if (!watchWindowOpen)
	{
		CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
		watchWindow =
			(CWatchWindow *)pFrame->CreateNewChild(RUNTIME_CLASS(CWatchWindow),
			IDR_WATCH);
		if (!project->watchWindowInfo.minimized &&
			!project->watchWindowInfo.maximized)
			watchWindow->ShowWindow(SW_SHOWNORMAL);
		if (project->watchWindowInfo.minimized)
			watchWindow->ShowWindow(SW_MINIMIZE);
		if (project->watchWindowInfo.maximized)
			watchWindow->ShowWindow(SW_MAXIMIZE);
		watchWindowOpen = true;
	}
}


void CStudioApp::OnUpdateViewWatch(CCmdUI* pCmdUI)
{
	if (project != NULL)
		pCmdUI->Enable(!watchWindowOpen);
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnViewStack() 
{
	OpenStackWindow();
	EnsureStackVisible();
	if (endCondition)
		updateStack = true;
}


void CStudioApp::OnUpdateViewStack(CCmdUI* pCmdUI) 
{
	if (project && sramSize)
		pCmdUI->Enable(stackWindowHandle == NULL);
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnViewChip() 
{
//	OpenChipWindow();
//	if (endCondition)
//		updateSfrs = true;
	if (!chipWindowOpen)
	{
		CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
		chipWindow =
			(CChipWindow *)pFrame->CreateNewChild(RUNTIME_CLASS(CChipWindow),
			IDR_CHIP);
		if (!project->chipWindowInfo.minimized &&
			!project->chipWindowInfo.maximized)
			chipWindow->ShowWindow(SW_SHOWNORMAL);
		if (project->chipWindowInfo.minimized)
			chipWindow->ShowWindow(SW_MINIMIZE);
		if (project->chipWindowInfo.maximized)
			chipWindow->ShowWindow(SW_MAXIMIZE);
		chipWindowOpen = true;
	}
}


void CStudioApp::OnUpdateViewChip(CCmdUI* pCmdUI) 
{
//	if (project && project->IsOpen())
//		pCmdUI->Enable(chipWindowHandle == NULL);
//	else
//		pCmdUI->Enable(FALSE);
	if (project != NULL)
		pCmdUI->Enable(!chipWindowOpen);
	else
		pCmdUI->Enable(FALSE);
}






void CStudioApp::OnProjectSettings()
{
	CSettingsDialog dialog;
	dialog.DoModal();
}


void CStudioApp::OnUpdateProjectSettings(CCmdUI* pCmdUI)
{
	if (project != NULL && project->IsOpen())
		pCmdUI->Enable(!debugRunning && !gettingData);
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnProjectRefresh()
{
	bool commsOn = ComPortOpen();
	CString path = project->GetSPPPath();
	path += project->GetSPPName();
	path += ".spp";
	project->Close();
	delete project;
	project = NULL;
	pcFile = NULL;
	pcLine = -1;
	commsTimer = 0;

	project = new FDProject();
	if (!project->Open((char *)LPCTSTR(path)))
	{
		ErrorMessage();
		delete project;
		project = NULL;
		SetTitle();
		return;
	}
	if (commsOn)
	{
		project->firstContact = FALSE;

		debugRunning = true;
		needInfoBlock = true;
		gettingData = false;
		waitingForD = true;
		endCondition = false;
		updateRegisters = false;
		updateSfrs = false;
		updateSram = false;
		updateEeprom = false;
		updateStack = false;
		firstWatchSymbol = 0;
		lastWatchSymbol = 0;
		debugMode = DM_STOP;
		askedFor = SENDPC;

		OpenComPort();
	}
}


void CStudioApp::OnUpdateProjectRefresh(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(project != NULL && project->IsOpen());
}


void CStudioApp::OnPrefsCommstimeout() 
{
	CCommsTimeoutDialog dialog;
	dialog.DoModal();
}


void CStudioApp::OnUpdatePrefsCommstimeout(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}


void CStudioApp::OnPrefsSourceSyntaxhi()
{
	globals.syntaxHiLighting = !globals.syntaxHiLighting;
	SaveWindowPreferences();
	if (project && project->IsOpen())
	{
		for (int i=0; i<project->GetNumFiles(); i++)
		{
			if (project->GetFile(i)->IsOpen())
				sourceWindows[i]->RedrawWindow();
		}
	}
}


void CStudioApp::OnUpdatePrefsSourceSyntaxhi(CCmdUI* pCmdUI)
{
	if (globals.syntaxHiLighting)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}


void CStudioApp::OnPrefsSourceTrackpc()
{
	globals.pcTracking = !globals.pcTracking;
	SaveWindowPreferences();
	if (globals.pcTracking && pcFile)
	{
		int index = pcFile->GetSourceFileIndex();
		if (!pcFile->IsOpen())
			OpenSourceFile(index);
		else
			sourceWindows[index]->BringWindowToTop();
		sourceWindows[index]->Update();
	}
}


void CStudioApp::OnUpdatePrefsSourceTrackpc(CCmdUI* pCmdUI)
{
	if (globals.pcTracking)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}


void CStudioApp::OnPrefsSourceAsm() 
{
	globals.assembler = !globals.assembler;
	SaveWindowPreferences();
}


void CStudioApp::OnUpdatePrefsSourceAsm(CCmdUI* pCmdUI) 
{
	if (globals.assembler)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}


void CStudioApp::OnPrefsSourceAddress() 
{
	globals.addresses = !globals.addresses;
	SaveWindowPreferences();
}


void CStudioApp::OnUpdatePrefsSourceAddress(CCmdUI* pCmdUI) 
{
	if (globals.addresses)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}


void CStudioApp::OnPrefsSourceCode()
{
	globals.machineCode = !globals.machineCode;
	if (!globals.assembler)
		globals.assembler = TRUE;		// MachineCode needs Assembler
	SaveWindowPreferences();
}


void CStudioApp::OnUpdatePrefsSourceCode(CCmdUI* pCmdUI)
{
	if (globals.machineCode)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}


void CStudioApp::OnWindowCascade()
{
	int x = 20;
	int y = 50;
	RECT rect;

	if (projectWindowOpen)
	{
		projectWindow->GetWindowRect(&rect);
		projectWindow->MoveWindow(x, y, rect.right-rect.left,
			rect.bottom-rect.top);
		x += 10;
		y += 20;
	}
	if (registersWindowHandle)
	{
		::GetWindowRect(registersWindowHandle, &rect);
		::MoveWindow(registersWindowHandle, x, y, rect.right-rect.left,
			rect.bottom-rect.top, TRUE);
		x += 10;
		y += 20;
	}
	if (sfrsWindowOpen)
	{
		sfrsWindow->GetWindowRect(&rect);
		sfrsWindow->MoveWindow(x, y, rect.right-rect.left,
			rect.bottom-rect.top);
		x += 10;
		y += 20;
	}
	if (sramWindowHandle)
	{
		::GetWindowRect(sramWindowHandle, &rect);
		::MoveWindow(sramWindowHandle, x, y, rect.right-rect.left,
			rect.bottom-rect.top, TRUE);
		x += 10;
		y += 20;
	}
	if (eepromWindowHandle)
	{
		::GetWindowRect(eepromWindowHandle, &rect);
		::MoveWindow(eepromWindowHandle, x, y, rect.right-rect.left,
			rect.bottom-rect.top, TRUE);
		x += 10;
		y += 20;
	}
	if (watchWindowOpen)
	{
		watchWindow->GetWindowRect(&rect);
		watchWindow->MoveWindow(x, y, rect.right-rect.left,
			rect.bottom-rect.top);
		x += 10;
		y += 20;
	}
	if (stackWindowHandle)
	{
		::GetWindowRect(stackWindowHandle, &rect);
		::MoveWindow(stackWindowHandle, x, y, rect.right-rect.left,
			rect.bottom-rect.top, TRUE);
		x += 10;
		y += 20;
	}
	if (chipWindowOpen)
	{
		chipWindow->GetWindowRect(&rect);
		chipWindow->MoveWindow(x, y, rect.right-rect.left,
			rect.bottom-rect.top);
		x += 10;
		y += 20;
	}
	for (int i=0; i<project->GetNumFiles(); i++)
	{
		if (project->GetFile(i)->IsOpen())
		{
			sourceWindows[i]->GetWindowRect(&rect);
			sourceWindows[i]->MoveWindow(x, y,
				rect.right-rect.left, rect.bottom-rect.top, TRUE);
			x += 10;
			y += 20;
			if (x > ::GetSystemMetrics(SM_CXFULLSCREEN)-100)
				x = 20;
			if (y > ::GetSystemMetrics(SM_CYFULLSCREEN)-100)
				y = 50;
		}
	}
}


void CStudioApp::OnUpdateWindowCascade(CCmdUI* pCmdUI)
{
	if (project && project->IsOpen())
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}


void CStudioApp::OnHelpHelptopics() 
{
	CString helpFile = instDir;
	helpFile += "Studio+ Help.chm"; //::/Help.html";
	HWND helpWnd = HtmlHelp(m_pMainWnd->m_hWnd, helpFile, HH_DISPLAY_TOPIC, 0);
	::SetWindowText(helpWnd, "Studio+ Help");
}


void CStudioApp::OnUpdateHelpHelptopics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
}


void CStudioApp::OnHelpAbout() 
{
	CAboutDialog dialog;
	dialog.DoModal();
}


void CStudioApp::OnUpdateHelpAbout(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}


void CStudioApp::OnHelpPlugins() 
{
	CPluginsDialog dialog;
	dialog.DoModal();
}


void CStudioApp::OnUpdateHelpPlugins(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}


void CStudioApp::OnHelpRegister() 
{
	CLicenseDialog dialog;
	if (dialog.DoModal() == IDOK)
		PostMessage(NULL, WM_QUIT, 0, 0); // Quit NOW
}


void CStudioApp::OnUpdateHelpRegister(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}


void CStudioApp::OnHelpExport() 
{
	char buffer[1000];
	char title[100];
	int type;

	safeSerial->SafeTrans();
	int error = safeSerial->ResetError();
	if (error == 0)						// Success
	{
		strcpy(title, "License Exported");
		strcpy(buffer,
"Your license has been transfered to disk.\r\n\
Studio+ will now close.");
		type = MB_OK|MB_ICONINFORMATION;
	}
	else if (error == 904)				// No access
	{
		strcpy(title, "Bad Disk");
		strcpy(buffer,
"Studio+ cannot transfer it's license to this disk.\r\n\
please make sure the disk is in the drive and\r\n\
that it is NOT write protected.");
		type = MB_OK|MB_ICONSTOP;
	}
	if (error == 0 || error == 904)
		::MessageBox(NULL, buffer, title, type);
	if (error == 0)
		PostMessage(NULL, WM_QUIT, 0, 0); // Quit NOW
}


void CStudioApp::OnUpdateHelpExport(CCmdUI* pCmdUI) 
{
	if (globals.registered)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// Global Functions


void ShowComIndicators()
{
//	CMainFrame * pFrame = STATIC_DOWNCAST(CMainFrame, AfxGetApp()->m_pMainWnd);
//	pFrame->UpdateStatusIndicators();
}


int ahtoi(char * ascii)
{
	if (ascii[0] == '$' || ascii[1] == 'x') // If a hex string
		return htoi(ascii);
	return atoi(ascii);
}


int htoi(char * hex)
{
	CString hexChars = "0123456789ABCDEF";
	CString chars = hex;
	chars.MakeUpper();
	if (chars.Find("0x") == 0)
		chars = chars.Mid(2);
	if (chars.Find("$") == 0)
		chars = chars.Mid(1);
	int integer = 0;
	int index;
	int i = 0;
	do
	{
		integer <<= 4;
		index = hexChars.Find(chars[i++]);
		if (index != -1)
			integer |= index;
	}
	while (i < chars.GetLength());
	return integer;
}


char * itoh(int integer, char * hex, bool trim, int chars)
{
	CString hexChars = "0123456789ABCDEF";
	CString str = "";
	int i;
	int rem = integer;

	do
	{
		i = rem/16;
		str += hexChars[rem-(i*16)];
		rem = i;
	}
	while (rem);
	if (!trim && chars > str.GetLength())
	{
		rem = str.GetLength();
		for (i=0; i<chars-rem; i++)
			str += '0';
	}
	str.MakeReverse();
	strcat(hex, str);
	return hex;
}


void ErrorMessage()
{
	char buffer[1000];
	char title[100];
	switch (errorCode)
	{
		default:
		case SPERROR_SUCCESS:
			strcpy(title, "Error");
			strcpy(buffer, STRING_NAME);
			strcat(buffer, 
" could not open your project.\r\n\
Please make sure it's source files exist and they are in their\r\n\
correct directories.  Try compiling it to make sure. Alternativly\r\n\
use \"File->Open Project\" if it has been moved or use\r\n\
\"File->New Project\" to recreate it.");
			break;

		case SPERROR_NOTINSTALLED:
			strcpy(title, "Missing Settings");
			strcpy(buffer, STRING_NAME);
			strcat(buffer,
" could not find it's settings in the Windows Registry.\r\n\
Please reinstall it to restore them.");
			break;

		case SPERROR_MISSINGIDE:
			strcpy(title, "Missing IDE");
			strcpy(buffer,
"The IDE library file for this project's IDE is missing.\r\n\
Either reinstall it with your installation CD or download\r\n\
it from ");
			strcat(buffer, STRING_FLASHURL);
			strcat(buffer, ".");
			break;

		case SPERROR_IDENOTLICENSED:
			strcpy(title, "Unlicensed IDE");
			strcpy(buffer,
"This project uses an IDE that has not been registered for\r\n\
use with the project's CPU family on this copy of ");
			strcat(buffer, STRING_NAME);
			strcat(buffer, ".\r\n\
To register it, select the CPU family in the \"CPU Familys\"\r\n\
list in the \"Register ");
			strcat(buffer, STRING_NAME);
			strcat(buffer, "\" window. Then, make\r\n\
sure the IDE is selected in the IDEs list. Finally, press\r\n\
the \"Email Flash\" button to send your request to Flash.\r\n\
Alternatively, contact flash by phone for a faster response.");
			break;

		case SPERROR_MISSINGFAMILY:
			strcpy(title, "Missing CPU Family");
			strcpy(buffer,
"The CPU Family library file for this project's CPU Family\r\n\
is missing. Either reinstall it with your installation CD\r\n\
or download it from ");
			strcat(buffer, STRING_FLASHURL);
			strcat(buffer, ".");
			break;

		case SPERROR_FAMILYNOTLICENSED:
			strcpy(title, "Unlicensed CPU Family");
			strcpy(buffer,
"This project uses a CPU Family that has not been registered\r\n\
on this copy of ");
			strcat(buffer, STRING_NAME);
			strcat(buffer, ". To register it, select the CPU Family\r\n\
in the \"CPU Familys\" list in the \"Register ");
			strcat(buffer, STRING_NAME);
			strcat(buffer, "\" window.\r\n\
Then, select the IDE/s you use in the IDEs list. Finally, press\r\n\
the \"Email Flash\" button to send your request to Flash.\r\n\
Alternatively, contact flash by phone for a faster response.");
			break;

		case SPERROR_BADDEBUG:
			strcpy(title, "Corrupted Debug File");
			strcpy(buffer, "Bad Debug File");
			break;

		case SPERROR_BADLISTING:
			strcpy(title, "Corrupted Listing File");
			strcpy(buffer, "Bad Listing File");
			break;

		case SPERROR_BADSOURCELIST:
			strcpy(title, "Corrupted Source List File");
			strcpy(buffer, "Bad Source List");
			break;

		case SPERROR_NOISDVECTOR:
			strcpy(title, "No ISD Interrupt Vector");
			strcpy(buffer,
"There is no ISD Vector in this project's code.\r\n\
Make sure you have either an ISD library linked with\r\n\
your project, or you have included an ISD source file\r\n\
in with your project's other sources.");
			break;

		case SPERROR_NOISD:
			strcpy(title, "No ISD");
			strcpy(buffer,
"The ISD interrupt vector in your project does not point\r\n\
to a Flash ISD routine.\r\n\
Make sure you have either an ISD library linked with\r\n\
your project, or you have included an ISD source file\r\n\
in with your project's other sources.");
			break;

		case SPERROR_NOCODE:
			strcpy(title, "No Project Output");
			strcpy(buffer,
"Make sure you have selected an output format compatible\r\n\
with ");
			strcat(buffer, STRING_NAME);
			strcat(buffer,
" (usually \"Intel Hex\"), in your project's IDE's\r\n\
project settings/options.");
			break;

		case SPERROR_BADPROJECT:
			strcpy(title, "Bad Project File");
			strcpy(buffer, STRING_NAME);
			strcat(buffer, 
" could not open your project.\r\n\
The project file has been damaged. Use \"File->New Project\" to\r\n\
recreate it.  Alternativly, edit it manually with a text\r\n\
editor (use a working project file as a reference).");
			break;

		case SPERROR_MISSINGSOURCE:
			strcpy(title, "Missing Source File/s");
			strcpy(buffer, STRING_NAME);
			strcat(buffer, 
" could not open your project.\r\n\
Some or all of the source files are missing (or not where the IDE's\r\n\
project and/or listing file indicates they are!). Recompile the\r\n\
original project to fix the file paths.");
			break;

		case SPERROR_PROJECTNOTASSEMBLER:
			strcpy(title, "Not an Assembler Project");
			strcpy(buffer, STRING_NAME);
			strcat(buffer, 
" could not open your project.\r\n\
'C' Source debugging has not been licensed. Select\r\n\
\"Professional\" (or better) and get another license\r\n\
from Flash to enable 'C' projects to be loaded.");
			break;

		case SPERROR_2KLIMIT:
			strcpy(title, "2K Code Limit Reached");
			strcpy(buffer, STRING_NAME);
			strcat(buffer, 
" could not open your project.\r\n\
Unregistered copies of ");
			strcat(buffer, STRING_NAME);
			strcat(buffer, " will only load projects up\r\n\
to 2K in size (code).  Your project does not fit into the 2K code limit.\r\n\
To register, please go to ");
			strcat(buffer, STRING_FLASHURL);
			strcat(buffer, " and follow the links for ");
			strcat(buffer, STRING_NAME);
			strcat(buffer, ".");
			break;
	}

	if (errorMessage[0] != 0)			// If a specific Error Message
	{
		strcat(buffer, "\r\n\r\n");		// Add it
		strcat(buffer, errorMessage);
	}
	::MessageBox(NULL, buffer, title, MB_OK|MB_ICONEXCLAMATION);
	errorCode = SPERROR_SUCCESS;
	errorMessage[0] = 0;
}
