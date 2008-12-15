// Project.cpp

#include "stdafx.h"
#include "fstream.h"
#include "Shlwapi.h"					// Path functions

#define PROJECT

#include "Studio.h"
#include "Project.h"
#include "SourceWindow.h"
#include "Registers.h"
#include "Memory.h"
#include "Stack.h"
#include "Comms.h"						// Comms functions
#include "SourceFile.h"
#include "Function.h"
#include "ScopeBlock.h"
#include "Symbol.h"

#include "Globals.h"

char * ascii = {"0123456789ABCDEF"};

/////////////////////////////////////////////////////////////////////////////
// FDProject functions


FDProject::FDProject()
{
#ifdef PRJDEBUG
debug<<"FDProject::FDProject():"<<endl;
#endif
	
	changed = FALSE;
	sppName = NULL;
	sppPath = NULL;
	name = NULL;
	ide = -1; //IDE_IMAGECRAFT;
	externalMem = FALSE;
	externalMemStart = 0;
	externalMemSize = 0;
	numSFRs = 0; //GetNumberSFRs(chip);
	sfrFlags = NULL; //new int[numSFRs];
//	for (int i=0; i<numSFRs; i++)
//		sfrFlags[i] = FALSE;
	sfrFlagsVisible = FALSE;
	sfrNameLength = 0;
	sfrNames = NULL;
	sfrMarkers = NULL;
	sfrFlagMarkers = NULL;
	sfrFlagNameLength = 0;
	sfrFlagNames = NULL;
	family = -1; //SetFamily(CLGetFamilyID());
	chip = -1; //SetChip(CLGetIdFromName("ATmega103"));
	package = -1; //SetPackage(CLGetPackageIdFromName("TQFP"));
	path = NULL;
	open = false;
	outputDir = NULL;
	numFiles = 0;
	files = NULL;
	treeItem = 0;
	numSoftBPs = 0;
	numHardBPs = 0;
	numWatchSymbols = 0;
	watchSymbols = NULL;
	selectedComPort = 1;
	selectedBaud = 115200;
	setbreakhereAddress = -1;
//	bytesPerVector = 0;
//	int0Vector = 0;

	// Default window dimensions
	projectWindowInfo.x = 0;
	projectWindowInfo.y = 0;
	projectWindowInfo.width = 200;
	projectWindowInfo.height = 300;
	projectWindowInfo.open = TRUE;
	projectWindowInfo.minimized = FALSE;
	projectWindowInfo.maximized = FALSE;
	registersWindowInfo.x = 300;
	registersWindowInfo.y = 200;
	registersWindowInfo.width = 200;
	registersWindowInfo.height = 200;
	registersWindowInfo.open = FALSE;
	registersWindowInfo.minimized = FALSE;
	registersWindowInfo.maximized = FALSE;
	sfrsWindowInfo.x = 250;
	sfrsWindowInfo.y = 225;
	sfrsWindowInfo.width = 300;
	sfrsWindowInfo.height = 150;
	sfrsWindowInfo.open = FALSE;
	sfrsWindowInfo.minimized = FALSE;
	sfrsWindowInfo.maximized = FALSE;
	sramWindowInfo.x = 121;
	sramWindowInfo.y = 181;
	sramWindowInfo.width = 558;
	sramWindowInfo.height = 237;
	sramWindowInfo.open = FALSE;
	sramWindowInfo.minimized = FALSE;
	sramWindowInfo.maximized = FALSE;
	eepromWindowInfo.x = 121;
	eepromWindowInfo.y = 181;
	eepromWindowInfo.width = 558;
	eepromWindowInfo.height = 237;
	eepromWindowInfo.open = FALSE;
	eepromWindowInfo.minimized = FALSE;
	eepromWindowInfo.maximized = FALSE;
	watchWindowInfo.x = 200;
	watchWindowInfo.y = 225;
	watchWindowInfo.width = 400;
	watchWindowInfo.height = 150;
	watchWindowInfo.open = FALSE;
	watchWindowInfo.minimized = FALSE;
	watchWindowInfo.maximized = FALSE;
	stackWindowInfo.x = 330;
	stackWindowInfo.y = 150;
	stackWindowInfo.width = 137;
	stackWindowInfo.height = 300;
	stackWindowInfo.open = FALSE;
	stackWindowInfo.minimized = FALSE;
	stackWindowInfo.maximized = FALSE;
	chipWindowInfo.x = 260;
	chipWindowInfo.y = 150;
	chipWindowInfo.width = 280;
	chipWindowInfo.height = 300;
	chipWindowInfo.open = FALSE;
	chipWindowInfo.minimized = FALSE;
	chipWindowInfo.maximized = FALSE;
}


FDProject::~FDProject()
{
#ifdef PRJDEBUG
debug<<"FDProject::~FDProject():"<<endl;
#endif

	if (sppName)
		delete [] sppName;
	if (sppPath)
		delete [] sppPath;
	if (name)
		delete [] name;
	if (path)
		delete [] path;
	if (outputDir)
		delete [] outputDir;
	if (numWatchSymbols)
	{
		for (int i=0; i<numWatchSymbols; i++)
		{
			if (watchSymbols[i]->GetType()&VARIABLE_ASSEMBLERMASK)
				delete watchSymbols[i];
		}
		delete [] watchSymbols;
	}
	if (numFiles)
	{
		ILDeleteDebugInfo(files, numFiles);
		delete [] files;
	}

	if (sfrFlags)
		delete [] sfrFlags;
	if (sfrMarkers)
		delete [] sfrMarkers;
	if (sfrNames)
		delete [] sfrNames;
	if (sfrFlagMarkers)
		delete [] sfrFlagMarkers;
	if (sfrFlagNames)
		delete [] sfrFlagNames;
}


void FDProject::Changed()
{
	changed = TRUE;
}


char * FDProject::GetName()
{
	return name;
}


char * FDProject::GetPath()
{
	return path;
}


char * FDProject::GetSPPName()
{
	return sppName;
}


char * FDProject::GetSPPPath()
{
	return sppPath;
}


DWORD FDProject::GetTimeStamp()
{
	CFileFind finder;
	FILETIME time;
	DWORD timeStamp;
	CString file = outputDir;

	if (ide == IDE_IMAGECRAFT)
	{
		file += name;
		file += ".lst";					// Get timestamp of listing file
	}

	if (!finder.FindFile(file))
		return 0;
	finder.FindNextFile();
	if (!finder.GetLastWriteTime(&time))
		return 0;

	timeStamp = time.dwHighDateTime<<16;
	timeStamp |= time.dwLowDateTime>>16;
	return timeStamp;
}


int FDProject::IsChanged()
{
	return changed;
}


bool FDProject::New()
{
	if (open)
		Close();
	open = false;
	return true;
}


bool FDProject::Open(char * file)
{
	int i;

#ifdef PRJDEBUG
debug<<"FDProject::Open(): ->"<<endl;
debug<<"   file = "<<file<<endl;
#endif

	if (open)
		Close();

	if (!LoadFromFile(file))
		return false;

#ifdef PRJDEBUG
debug<<"FDProject::Open(): opening windows"<<endl;
debug<<"FDProject::Open():    opening Project Window"<<endl;
#endif

	if (projectWindowInfo.open == TRUE)	// If Project window was open
		theApp.OnViewProject();			// Open it!!
	if (registersWindowInfo.open == TRUE)
		OpenRegistersWindow();
	if (sfrsWindowInfo.open == TRUE)
		theApp.OnViewMemorySfrs();
	if (sramWindowInfo.open == TRUE)
		OpenSramWindow();
	if (eepromWindowInfo.open == TRUE)
		OpenEepromWindow();
	if (watchWindowInfo.open == TRUE)
		theApp.OnViewWatch();
	if (stackWindowInfo.open == TRUE)
		OpenStackWindow();
	if (chipWindowInfo.open == TRUE)
		theApp.OnViewChip();

	if (sourceWindows)
		delete sourceWindows;
	sourceWindows = NULL;
	if (numFiles)
		sourceWindows = new CSourceWindow *[numFiles];
	for (i=0; i<numFiles; i++)
	{
		if (GetFile(i)->wasOpen)
			theApp.OpenSourceFile(i);
	}

	needInfoBlock = true;
	debugRunning = false; //true;
	gettingData = false;
	waitingForD = true;
	endCondition = false;
	debugMode = DM_STOP;
	askedFor = SENDPC;
	sramStartAddress = 0x60;
	sramEndAddress = 0x61;
	eepromStartAddress = 0;
	eepromEndAddress = 1;
	stackStartAddress = 0x60;
	stackEndAddress = 0x61;
	updateRegisters = false;
	updateSfrs = false;
	updateSram = false;
	updateEeprom = false;
	updateStack = false;
	firstWatchSymbol = 0;
	lastWatchSymbol = 0;

	targetFamily = -1; //family;
	targetCPU = -1; //chip;

	oldPCFile = NULL;

	SaveToRegistry();

	firstContact = TRUE;

	cursorFile = NULL;
	cursorLine = -1;
	open = true;

	changed = FALSE;

	theApp.SetProjectMenu();
	theApp.SetSourceMenu();

	theApp.GetMainWnd()->SetFocus();

#ifdef PRJDEBUG
debug<<"FDProject::Open(): <-"<<endl;
#endif

	return true;
}


bool FDProject::Close()
{
#ifdef PRJDEBUG
debug<<"FDProject::Close():"<<endl;
#endif
	int i, j;

	// Save the project file
	if (sppName && sppPath)
	{
		if (changed)
		{
			int result = ::MessageBox(NULL,
"Some of your project's settings have changed.\r\n\
Would you like to save them?", "Settings Changed",
				MB_YESNOCANCEL|MB_ICONQUESTION);
			if (result == IDYES)
				Save();
			if (result == IDCANCEL)
				return FALSE;
		}
		else
			Save();						// Save window positions
	}

	// Close all the project's windows
	if (projectWindowOpen)
	{
		projectWindow->DestroyWindow();
		projectWindow = NULL;
		projectWindowOpen = false;
	}
	if (registersWindowHandle)
		::DestroyWindow(registersWindowHandle);
	if (sfrsWindowOpen)
	{
		sfrsWindow->DestroyWindow();
		sfrsWindow = NULL;
		sfrsWindowOpen = false;
	}
	if (sramWindowHandle)
		::DestroyWindow(sramWindowHandle);
	if (eepromWindowHandle)
		::DestroyWindow(eepromWindowHandle);
	for (i=0; i<numFiles; i++)
	{
		if (files[i]->IsOpen())
		{
			if (sourceWindows[i])
				sourceWindows[i]->DestroyWindow();
			files[i]->open = FALSE;
		}
	}
	if (watchWindowOpen)
	{
		watchWindow->DestroyWindow();
		watchWindow = NULL;
		watchWindowOpen = false;
	}
	if (stackWindowHandle)
		::DestroyWindow(stackWindowHandle);
	if (chipWindowOpen)
	{
		chipWindow->DestroyWindow();
		chipWindow = NULL;
		chipWindowOpen = false;
	}

	if (numWatchSymbols)
	{
		for (int i=0; i<numWatchSymbols; i++)
		{
			if (watchSymbols[i]->GetType()&VARIABLE_ASSEMBLERMASK)
				delete watchSymbols[i];
		}
		delete watchSymbols;
		watchSymbols = NULL;
		numWatchSymbols = 0;
	}

	// Give back source file memory allocated from app
	// The rest will be deallocated from the IDE dll by the
	// source file's destructor
	SourceLine * line;
	for (i=0; i<numFiles; i++)
	{
		for (j=0; j<files[i]->GetNumLines(); j++)
		{
			line = files[i]->GetLine(j);
			if (line->numSegments)
				delete [] line->segments;
			line->segments = NULL;
			line->numSegments = 0;
		}
		files[i]->ClearAllBPs();		// Clear all soft breakpoints
										// Hard breakpoints are allocated
										// in the IDE dll
	}


	// Give back SFR flag memory
	if (sfrFlags)
		delete sfrFlags;
	sfrFlags = NULL;
	if (sfrMarkers)
		delete sfrMarkers;
	sfrMarkers = NULL;
	if (sfrNames)
		delete sfrNames;
	sfrNames = NULL;
	if (sfrFlagMarkers)
		delete sfrFlagMarkers;
	sfrFlagMarkers = NULL;
	if (sfrFlagNames)
		delete sfrFlagNames;
	sfrFlagNames = NULL;
	numSFRs = 0;

	// Close the com port
	CloseComPort();

	// It's closed!!
	open = false;

	// Set the App's menus
	theApp.SetProjectMenu();
	theApp.SetSourceMenu();
	return true;
}


bool FDProject::LoadFromFile(char * p)
{
#ifdef PRJDEBUG
debug<<"FDProject::LoadFromFile():"<<endl;
debug<<"   file = "<<p<<endl;
#endif

	ifstream in;
	char buffer[1000];
	CString line;
	CString fileName;
	FMSourceFile * file;
	CString symbolName;
	CString symbolStr;
	CString symbolDataType;
	CString functionName;
	FMSymbol * symbol;
	FMScopeBlock * scope;
	WindowInfo wndInfo;
	RECT rect;
	bool main = false;
	bool windows = false;
	bool watch = false;
	int i;
	int max;

	line = p;
	line = line.Mid(0, line.ReverseFind('\\')+1);
	sppPath = new char[line.GetLength()+1];
	strcpy(sppPath, LPCTSTR(line));
#ifdef PRJDEBUG
debug<<"   sppPath = "<<sppPath<<endl;
#endif
//	strcpy(globals.usersDirectory, path);
//	globals.usersDrive = PathGetDriveNumber(globals.usersDirectory);
	line = p;
	line = line.Mid(line.ReverseFind('\\')+1);
	line = line.Mid(0, line.ReverseFind('.'));
	sppName = new char[line.GetLength()+1];
	strcpy(sppName, line);

	in.open(p, ios::in|ios::nocreate);
	bool done = false;
	bool first = true;

	do
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
		{
			in.close();
			if (!main)
			{
#ifdef PRJDEBUG
debug<<"      File is BAD"<<endl;
#endif
				errorCode = SPERROR_BADPROJECT;
				return false;			// Bad file
			}
			return true;				// File OK, probably old!!
		}
		buffer[in.gcount()-1] = 0;
		if (first)
		{
			first = false;
			line = buffer;
			if (line.Find("# Studio+ Project File:", 0) != 0)
			{
				in.close();
				errorCode = SPERROR_BADPROJECT;
				return false;			// Bad file
			}
		}
		if (strcmp(buffer, "[PROJECT]") != 0)
			continue;
		main = true;
		do
		{
			in.getline(buffer, 1000);
			buffer[in.gcount()-1] = 0;
			line = buffer;
			if (buffer[0] == '[')		// If start of new section
			{
				done = true;
				continue;
			}
			if (line.Find("NAME=") == 0)
			{
				line = line.Mid(5);
				if (name)
					delete name;
				name = new char[line.GetLength()+1];
				strcpy(name, LPCTSTR(line));
			}
			else if (line.Find("PATH=", 0) == 0)
			{
				line = line.Mid(5);
				if (path)
					delete path;
				path = new char[line.GetLength()+1];
				strcpy(path, line);
			}
			else if (line.Find("IDE=") == 0)
			{
				line = line.Mid(4);

				if (!theApp.OpenIdeLibrary((char *)LPCTSTR(line)))
					return FALSE;

				ide = ILGetIdeID();
			}
			else if (line.Find("TARGET_FAMILY=") == 0)
			{
				line = line.Mid(14);

				if (!theApp.OpenFamilyLibrary((char *)LPCTSTR(line)))
					return FALSE;

				SetFamily(CLGetFamilyID());
			}
			else if (line.Find("TARGET=") == 0)
			{
				line = line.Mid(7);
				SetChip(CLGetIdFromName((char *)LPCTSTR(line)));
			}
			else if (line.Find("TARGET_PACKAGE=") == 0)
			{
				line = line.Mid(15);
				SetPackage(CLGetPackageIdFromName((char *)LPCTSTR(line)));
			}
			else if (line.Find("COMPORT=") == 0)
			{
				line = line.Mid(8);
				selectedComPort = atoi(LPCTSTR(line));
			}
			else if (line.Find("BAUD=") == 0)
			{
				line = line.Mid(5);
				selectedBaud = atoi(LPCTSTR(line));
			}
			else if (line.Find("SFRFLAGS=") == 0)
			{
				line = line.Mid(9);
				sfrFlagsVisible = atoi(LPCTSTR(line));
			}
			else if (line.Find("MEMORY=", 0) == 0)
			{
				line = line.Mid(7);
				externalMem = atoi(line.Mid(0, line.Find(',')));
				line = line.Mid(line.Find(',')+1);
				externalMemStart = atoi(line.Mid(0, line.Find(',')));
				line = line.Mid(line.Find(',')+1);
				externalMemSize = atoi(line);
				SetChip(chip);
			}
			else if (line.Find("STACKADDRESS=", 0) == 0)
			{
				line = line.Mid(13);
				stackAddress = atoi(line);
			}
		}
		while (!done);

		// Now decide how to load the IDE's project.

		// First find out if it's a C project and if we're allowed
		// to open it.
		fileName = path;
		fileName += name;
		max = 1000;
		ILGetProjectFileExt(buffer, &max);
		fileName += buffer;				// Project file
		if (!ILIsProjectAssembler(fileName) &&
			globals.options < SS_PROFESSIONAL)
		{
			errorCode = SPERROR_PROJECTNOTASSEMBLER;
			return FALSE;
		}

		// Next get the project's output directory
		fileName = path;
		fileName += name;
		max = 1000;
		ILGetProjectFileExt(buffer, &max);
		fileName += buffer;				// Project file
		if (!ILGetOutputDir(NULL, fileName, &max)) // Get dir name length
		{
			if (max == -1)				// If no output setting
			{
				max = 1000;
				ILGetLastIsdError(&errorCode, errorMessage, &max);
				return FALSE;
			}
		}
		if (outputDir)
			delete outputDir;
		outputDir = new char[max];
		ILGetOutputDir(outputDir, fileName, &max); // Get dir

		// Then find the ISD and get the Hard Breakpoint rountine's address.
		if (!ILGetISD(fileName, outputDir, &setbreakhereAddress))
		{
			max = 1000;
			ILGetLastIsdError(&errorCode, errorMessage, &max);
			return FALSE;
		}

		// Next get all the debug info. Assembler IDEs will only
		// get a list of source files. C IDEs will get a full
		// heirarchy of source files, functions, scopes & variables.
		ILGetDebugInfo(fileName, outputDir, NULL, &numFiles);
		files = new FMSourceFile *[numFiles];
		if (!ILGetDebugInfo(fileName, outputDir, files, &numFiles))
		{
			max = 1000;
			ILGetLastIsdError(&errorCode, errorMessage, &max);
			return FALSE;
		}

		// Next get the generated assembly code that was generated from
		// the source files. This is extracted from the listing file.
		// This function also gets the Stack address and checks the 2K
		// code limit if the user hasn't registered.
		if (!ILGetCodeInfo(fileName, outputDir, files, numFiles, &stackAddress,
			globals.registered))
		{
			max = 1000;
			ILGetLastIsdError(&errorCode, errorMessage, &max);
			return FALSE;
		}
		stackPointer = stackAddress;

		// Finally check the listing file for hard breakpoints and mark
		// them in the source files
		ILFindHardBreakpoints(fileName, outputDir, files, numFiles,
			setbreakhereAddress);
		UpdateBreakpoints();

		if (!GetNumHardBPs())
		{
			::MessageBox(NULL,
"Studio+ could not find any hard (coded) breakpoints in your project. \r\n\
There needs to be at least one for Studio+ to communicate with the \r\n\
debugger in your target.", "No Breakpoints",
				MB_OK|MB_ICONEXCLAMATION);
		}

		done = false;
		if (strcmp(line, "[WINDOWS]") != 0)
			continue;
		windows = true;
		do
		{
			in.getline(buffer, 1000);
			if (in.gcount() == 0)
			{
				in.close();
				return true;			// File OK, probably old!!
			}
			buffer[in.gcount()-1] = 0;
			if (buffer[0] == '[')		// If start of new section
			{
				done = true;
				continue;
			}
			line = buffer;
			if (line.Find("PROJECT") != -1)
			{
				line = line.Mid(8);
				ReadWindowInfoLine(&projectWindowInfo, &line);
			}
			else if (line.Find("REGISTERS") != -1)
			{
				line = line.Mid(10);
				ReadWindowInfoLine(&registersWindowInfo, &line);
			}
			else if (line.Find("SFRS") != -1)
			{
				line = line.Mid(5);
				ReadWindowInfoLine(&sfrsWindowInfo, &line);
			}
			else if (line.Find("SRAM") != -1)
			{
				line = line.Mid(5);
				ReadWindowInfoLine(&sramWindowInfo, &line);
			}
			else if (line.Find("EEPROM") != -1)
			{
				line = line.Mid(7);
				ReadWindowInfoLine(&eepromWindowInfo, &line);
			}
			else if (line.Find("WATCH") != -1)
			{
				line = line.Mid(6);
				ReadWindowInfoLine(&watchWindowInfo, &line);
			}
			else if (line.Find("STACK") != -1)
			{
				line = line.Mid(6);
				ReadWindowInfoLine(&stackWindowInfo, &line);
			}
			else if (line.Find("CHIP") != -1)
			{
				line = line.Mid(5);
				ReadWindowInfoLine(&chipWindowInfo, &line);
			}
			else if (line.Find("SOURCE") != -1)
			{
				line = line.Mid(7);
				fileName = line.Mid(0, line.Find(','));
				line = line.Mid(line.Find(',')+1);
				fileName = line.Mid(0, line.Find(','))+fileName;
				line = line.Mid(line.Find(',')+1);
				file = GetFile(fileName);
				if (file != NULL)		// If the file is still in the project
				{
					ReadWindowInfoLine(&wndInfo, &line);
					rect.left = wndInfo.x;
					rect.top = wndInfo.y;
					rect.right = rect.left+wndInfo.width;
					rect.bottom = rect.top+wndInfo.height;
					file->SetWindowRect(&rect);
					file->SetMinimized(wndInfo.minimized);
					file->SetMaximized(wndInfo.maximized);
					if (wndInfo.open)
						file->wasOpen = TRUE;
				}
			}
		}
		while (!done);

		done = false;
		if (strcmp(buffer, "[WATCH]") != 0)
			continue;
		watch = true;
		do
		{
			in.getline(buffer, 1000);
			if (in.gcount() == 0)
			{
				in.close();
				return true;			// File OK, probably old!!
			}
			buffer[in.gcount()-1] = 0;
			if (buffer[0] == '[')		// If start of new section
			{
				done = true;
				continue;
			}
			line = buffer;
			if (line.Find("SYMBOL", 0) != -1)
			{
				line = line.Mid(7);
				symbolName = line.Mid(0, line.Find(','));
				line = line.Mid(line.Find(',')+1);
				fileName = line.Mid(0, line.Find(','));
				line = line.Mid(line.Find(',')+1);
				fileName = line.Mid(0, line.Find(','))+fileName;
				line = line.Mid(line.Find(',')+1);
				file = GetFile((char *)LPCTSTR(fileName));
				if (!file)				// If file isn't in the project anymore
					continue;
				if (line.Find(',') != -1) // A Function name
				{
					functionName = line.Mid(line.Find(',')+1);
					symbol = file->GetFunctionSymbol(
						(char *)LPCTSTR(functionName),
						(char *)LPCTSTR(symbolName));
				}
				else					// Symbol is Global
					symbol = file->GetGlobalSymbol(
						(char *)LPCTSTR(symbolName));
				if (symbol)
				{
					AddSymbolToWatch(symbol, false);
					firstWatchSymbol = 0; // Disable auto update
					lastWatchSymbol = 0;
				}
			}
			if (line.Find("ASMVAR", 0) != -1)
			{
				line = line.Mid(7);
				symbol = new FMSymbol();
				symbolName = line.Mid(0, line.Find(','));
				symbol->SetName((char *)LPCTSTR(symbolName));
				line = line.Mid(line.Find(',')+1);
				symbolStr = line.Mid(0, line.Find(','));
				symbol->SetAddress(atoi(symbolStr));
				line = line.Mid(line.Find(',')+1);
				symbolStr = line.Mid(0, line.Find(','));
				symbol->SetType(atoi(symbolStr));
				line = line.Mid(line.Find(',')+1);
				symbolDataType = line.Mid(0, line.Find(','));
				line = line.Mid(line.Find(',')+1);
				if (line.GetAt(0) == '1') // If a scope
				{
					line = line.Mid(2);
					scope = new FMScopeBlock();
					symbolStr = line.Mid(0, line.Find(','));
					scope->SetFirstAddress(atoi(symbolStr));
					line = line.Mid(line.Find(',')+1);
					symbolStr = line.Mid(0, line.Find(','));
					scope->SetLastAddress(atoi(symbolStr));
					line = line.Mid(line.Find(',')+1);
					symbol->SetScope(scope);
				}
				else
				{
					line = line.Mid(6);
					symbol->SetScope(NULL);
				}
				CString symbolTypeStr = "";
				bool pointer = false;
				if (line.GetAt(0) == '1') // If a pointer
				{
					symbolTypeStr += "p";
					pointer = true;
				}
				line = line.Mid(2);
				if (line.GetAt(0) == '1') // If an array
				{
					line = line.Mid(2);
					symbolTypeStr += "A[";
					symbolStr = line.Mid(0, line.Find(','));
					int numDims = atoi(symbolStr);
					if (numDims < 1)
						numDims = 1;
					int * dims = new int[numDims]; 
					int arraySize = 1;
					for (i=0; i<numDims; i++)
					{
						line = line.Mid(line.Find(',')+1);
						if (i < numDims-1)
							symbolStr = line.Mid(0, line.Find(','));
						else
							symbolStr = line;
						dims[i] = atoi(symbolStr);
						if (dims[i] < 1)
							dims[i] = 1;
						arraySize *= dims[i];
					}
					if (pointer)
						arraySize *= 2;
					else
					{
						char dataType = symbolDataType.GetAt(0);
						switch ((int)dataType)
						{
							case 's':
							case 'S':
							case 'i':
							case 'I':
								arraySize *= 2;
								break;
							case 'l':
							case 'L':
							case 'D':
								arraySize *= 4;
								break;
							default:
								arraySize *= 1;
								break;
						}
					}
					itoa(arraySize, buffer, 10);
					symbolTypeStr += buffer;
					for (i=0; i<numDims; i++)
					{
						symbolTypeStr += ":";
						itoa(dims[i], buffer, 10);
						symbolTypeStr += buffer;
					}
					delete dims;
					symbolTypeStr += "]";
				}
				symbolTypeStr += symbolDataType;
				symbol->SetDataType((char *)LPCTSTR(symbolTypeStr));

				AddSymbolToWatch(symbol, false);
				firstWatchSymbol = 0; // Disable auto update
				lastWatchSymbol = 0;
			}
		}
		while (!done);

		done = false;
		if (strcmp(buffer, "[SFRS]") != 0)
			continue;
		do
		{
			in.getline(buffer, 1000);
			if (in.gcount() == 0)
			{
				in.close();
				if (sfrsWindowOpen)
					sfrsWindow->Refresh();
				return true;			// End of File!!
			}
			buffer[in.gcount()-1] = 0;
//			if (buffer[0] == '[')		// If start of new section
//			{
//				done = true;
//				continue;
//			}
			if (strlen(buffer) == 0)
				continue;
			line = buffer;
			line += " ";				// SFR name might be part of another
			for (i=0; i<numSFRs; i++)
			{
				if (strlen(GetSfrName(i)) &&
					line.Find(GetSfrName(i)) == 0)
				{
					SetSfrFlag(i, TRUE);
					break;
				}
			}
		}
		while (!done);
	}
	while (!done);
	in.close();

	return true;
}


void FDProject::ReadWindowInfoLine(WindowInfo * info, CString * line)
{
#ifdef PRJDEBUG
//debug<<"FDProject::ReadWindowInfoLine():"<<endl;
//debug<<"   line = "<<line<<endl;
#endif

	CString value;

	value = line->Mid(0, line->Find(','));
	*line = line->Mid(line->Find(',')+1);
	info->x = atoi(LPCTSTR(value));
	value = line->Mid(0, line->Find(','));
	*line = line->Mid(line->Find(',')+1);
	info->y = atoi(LPCTSTR(value));
	value = line->Mid(0, line->Find(','));
	*line = line->Mid(line->Find(',')+1);
	info->width = atoi(LPCTSTR(value));
	value = line->Mid(0, line->Find(','));
	*line = line->Mid(line->Find(',')+1);
	info->height = atoi(LPCTSTR(value));
	value = line->Mid(0, line->Find(','));
	*line = line->Mid(line->Find(',')+1);
	info->open = atoi(LPCTSTR(value));
	value = line->Mid(0, line->Find(','));
	*line = line->Mid(line->Find(',')+1);
	info->minimized = atoi(LPCTSTR(value));
	info->maximized = atoi(LPCTSTR(*line));
}


bool FDProject::Save()
{
	CString fileName = sppPath;
	fileName += sppName;
	fileName += ".spp";
	SaveToFile((char *)LPCTSTR(fileName));
	changed = FALSE;
	return TRUE;
}


bool FDProject::SaveAs()
{
	CFileDialog * dialog;
	CString filter = "Studio+ Project (*.spp)|*.spp||";
	CString fileName = sppName;
	fileName += ".spp";
	dialog = new CFileDialog(FALSE, ".spp", fileName,
		OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, filter);
	if (dialog->DoModal() != IDOK)
	{
		delete dialog;
		return FALSE;
	}
	// Got the Project file;
	if (sppName)
		delete sppName;
	fileName = dialog->GetFileName();
	fileName = fileName.Left(fileName.ReverseFind('.'));
	sppName = new char[fileName.GetLength()+1];
	strcpy(sppName, fileName);

	if (sppPath)
		delete sppPath;
	fileName = dialog->GetPathName();
	fileName = fileName.Mid(0, fileName.ReverseFind('\\')+1);
	sppPath = new char[fileName.GetLength()+1];
	strcpy(sppPath, fileName);
	
	fileName += sppName;
	fileName += ".spp";
	SaveToFile((char *)LPCTSTR(fileName));
	changed = FALSE;

	SaveToRegistry();					// Add to recent projects
	theApp.SetProjectMenu();

	return TRUE;
}


bool FDProject::SaveToFile(char * file)
{
#ifdef PRJDEBUG
debug<<"FDProject::SaveToFile():"<<endl;
debug<<"   file = "<<file<<endl;
#endif
	int i, j;
	CString str;
	RECT rect;
	ofstream out;
	char buffer[100];
	int max = 100;

	out.open(file, ios::out);
	out<<"# Studio+ Project File: "<<sppName<<endl;
	out<<"[PROJECT]"<<endl;
	out<<"NAME="<<name<<endl;
	out<<"PATH="<<path<<endl;
	ILGetIdeName(buffer, &max);
	out<<"IDE="<<buffer<<endl;
	CLGetFamilyName(buffer, &max);
	out<<"TARGET_FAMILY="<<buffer<<endl;
	GetChipName(chip, buffer, 100);
	out<<"TARGET="<<buffer<<endl;
	GetPackageName(package, buffer, 100);
	out<<"TARGET_PACKAGE="<<buffer<<endl;
	out<<"COMPORT="<<selectedComPort<<endl;
	out<<"BAUD="<<selectedBaud<<endl;
	out<<"SFRFLAGS="<<sfrFlagsVisible<<endl;
	out<<"MEMORY="<<externalMem<<","<<externalMemStart<<","<<externalMemSize<<endl;
	out<<"STACKADDRESS="<<stackAddress<<endl;
	out<<endl;

	out<<"[WINDOWS]"<<endl;
	for (i=0; i<numFiles; i++)
	{
		files[i]->GetWindowRect(&rect);
		out<<"SOURCE "<<files[i]->GetFileName()<<","<<
			files[i]->GetRelativePath()<<","<<rect.left<<","<<rect.top<<","<<
			rect.right-rect.left<<","<<rect.bottom-rect.top<<","<<
			files[i]->IsOpen()<<","<<
			files[i]->IsWindowMinimized()<<","<<
			files[i]->IsWindowMaximized()<<endl;
	}
	out<<"REGISTERS "<<registersWindowInfo.x<<","<<registersWindowInfo.y<<","<<
		registersWindowInfo.width<<","<<registersWindowInfo.height<<","<<
		registersWindowInfo.open<<","<<registersWindowInfo.minimized<<","<<
		registersWindowInfo.maximized<<endl;
	out<<"SFRS "<<sfrsWindowInfo.x<<","<<sfrsWindowInfo.y<<","<<
		sfrsWindowInfo.width<<","<<sfrsWindowInfo.height<<","<<
		sfrsWindowInfo.open<<","<<sfrsWindowInfo.minimized<<","<<
		sfrsWindowInfo.maximized<<endl;
	out<<"SRAM "<<sramWindowInfo.x<<","<<sramWindowInfo.y<<","<<
		sramWindowInfo.width<<","<<sramWindowInfo.height<<","<<
		sramWindowInfo.open<<","<<sramWindowInfo.minimized<<","<<
		sramWindowInfo.maximized<<endl;
	out<<"EEPROM "<<eepromWindowInfo.x<<","<<eepromWindowInfo.y<<","<<
		eepromWindowInfo.width<<","<<eepromWindowInfo.height<<","<<
		eepromWindowInfo.open<<","<<eepromWindowInfo.minimized<<","<<
		eepromWindowInfo.maximized<<endl;
	out<<"STACK "<<stackWindowInfo.x<<","<<stackWindowInfo.y<<","<<
		stackWindowInfo.width<<","<<stackWindowInfo.height<<","<<
		stackWindowInfo.open<<","<<stackWindowInfo.minimized<<","<<
		stackWindowInfo.maximized<<endl;
	out<<"CHIP "<<chipWindowInfo.x<<","<<chipWindowInfo.y<<","<<
		chipWindowInfo.width<<","<<chipWindowInfo.height<<","<<
		chipWindowInfo.open<<","<<chipWindowInfo.minimized<<","<<
		chipWindowInfo.maximized<<endl;
	out<<"PROJECT "<<projectWindowInfo.x<<","<<projectWindowInfo.y<<","<<
		projectWindowInfo.width<<","<<projectWindowInfo.height<<","<<
		projectWindowInfo.open<<","<<projectWindowInfo.minimized<<","<<
		projectWindowInfo.maximized<<endl;
	out<<"WATCH "<<watchWindowInfo.x<<","<<watchWindowInfo.y<<","<<
		watchWindowInfo.width<<","<<watchWindowInfo.height<<","<<
		watchWindowInfo.open<<","<<watchWindowInfo.minimized<<","<<
		watchWindowInfo.maximized<<endl;
	out<<endl;

	out<<"[WATCH]"<<endl;

	FMSymbol * symbol;
	FMScopeBlock * scope;
	for (i=0; i<numWatchSymbols; i++)
	{
		symbol = watchSymbols[i];
		if (!(watchSymbols[i]->GetType()&VARIABLE_ASSEMBLERMASK))
		{								// Source variables
			out<<"SYMBOL "<<symbol->GetName()<<","<<
				symbol->GetFile()->GetFileName()<<","<<
				symbol->GetFile()->GetRelativePath()<<",";
			if (symbol->GetScope() != NULL)
				out<<"1,"<<symbol->GetScope()->GetFunction()->GetName();
			else
				out<<"0";
		}
		else							// Assembler variables
		{
			out<<"ASMVAR "<<symbol->GetName()<<","<<
				symbol->GetAddress()<<","<<
				symbol->GetType()<<","<<
				symbol->GetDataTypeChar()<<",";
			scope = symbol->GetScope();
			if (scope)
				out<<"1,"<<scope->GetFirstAddress()<<","<<
					scope->GetLastAddress()<<",";
			else
				out<<"0,0,0,";
			if (symbol->IsPointer())
				out<<"1,";
			else
				out<<"0,";
			if (symbol->IsArray())
			{
				out<<"1,"<<symbol->GetNumDimensions();
				for (j=0; j<symbol->GetNumDimensions(); j++)
					out<<","<<symbol->GetDimensionSize(j);
			}
			else
				out<<"0";
		}
		// ** Remember to store function signatures when they are available.
		out<<endl;
	}
	out<<endl;

	out<<"[SFRS]"<<endl;
	for (i=0; i<numSFRs; i++)
	{
		if (GetSfrFlag(i))
		{
			str = GetSfrName(i);
			str.TrimRight();
			out<<str<<endl;
		}
	}
	out<<endl;

	out.close();
	return true;
}


/*
bool FDProject::ReadListingFile()
{
#ifdef PRJDEBUG
debug<<"FDProject::ReadListingFile():"<<endl;
#endif

	switch (ide)
	{
		case IDE_AVRSTUDIO:
		{


			break;
		}

		case IDE_IMAGECRAFT:
			return ReadImageCraftListingFile();
	}

	return false;
}
*/

/*
bool FDProject::ReadImageCraftListingFile()
{
#ifdef PRJDEBUG
debug<<"FDProject::ReadImageCraftListingFile():"<<endl;
#endif
	int i;
	ifstream in;
	CString line;
	CString srcLine;
	int lineNum;
	FMSourceFile * file = NULL;
	char buffer[1000];
	char filePath[_MAX_PATH];
	int index;
	bool firstLine = false;
	bool zeroBased = true;
	int address;
	int numSources = 0;
	char ** sources = NULL;
	bool done;

#ifdef PRJDEBUG
debug<<"FDProject::ReadImageCraftListingFile():"<<endl;
#endif

	line = path;						// Get a list of the source files
	line += name;
	line += ".src";
	in.open(line, ios::in|ios::nocreate);
	done = false;
	do
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
		{
			in.close();
			errorCode = SPERROR_BADSOURCELIST;
			return false;
		}
		buffer[in.gcount()-1] = 0;
		line = buffer;
		if (line.Find("[Files]") == 0)
			done = true;
	}
	while (in.gcount() != 0 && !done);
	done = false;
	do
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
		{
			in.close();
			errorCode = SPERROR_BADSOURCELIST;
			return false;
		}
		buffer[in.gcount()-1] = 0;
		line = buffer;
		if (line.Find("[Headers]") == 0)
			done = true;
		else
		{
			if (line.ReverseFind('\\') != -1)
				line = line.Mid(line.ReverseFind('\\')+1);
			if (numSources == 0)
				sources = new char *[1];
			else
			{
				char ** temp = new char *[numSources+1];
				for (i=0; i<numSources; i++)
					temp[i] = sources[i];
				delete sources;
				sources = temp;
			}
			sources[numSources] = new char [line.GetLength()+1];
			strcpy(sources[numSources], (char *)LPCTSTR(line));
			numSources++;
		}
	}
	while (in.gcount() != 0 && !done);
	in.close();

	line = outputDir;					// Read the listing file
	line += name;
	line += ".lst";
	
	in.open(line, ios::in|ios::nocreate);

	stackAddress = -1;
	do
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
		{
			in.close();
			for (i=0; i<numSources; i++)
				delete sources[i];
			if (numSources)
				delete sources;
			errorCode = SPERROR_BADLISTING;
			strcpy(errorMessage,
"The listing file either dosn't exist or has been corrupted.\r\n\
Recompile your project to fix it.");
			return false;
		}
		buffer[in.gcount()-1] = 0;
		line = buffer;
		if (line.Find("__start:") == 0)
		{
			in.getline(buffer, 1000);
			if (buffer[0] != ' ')
				in.getline(buffer, 1000);
			buffer[in.gcount()-1] = 0;
			line = buffer;
			buffer[902] = line.GetAt(10);
			buffer[903] = line.GetAt(12);
			in.getline(buffer, 1000);
			buffer[in.gcount()-1] = 0;
			line = buffer;
			buffer[900] = line.GetAt(10);
			buffer[901] = line.GetAt(12);
			buffer[904] = 0;

#ifdef PRJDEBUG
debug<<"   stackAddress (hex) = "<<(char *)&buffer[900]<<endl;
#endif
			
			stackAddress = htoi(&buffer[900]);
		}
	}
	while (stackAddress == -1);
	stackPointer = stackAddress;
	
#ifdef PRJDEBUG
debug<<"   stackAddress = "<<stackAddress<<endl;
#endif

	do
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
			continue;
		buffer[in.gcount()-1] = 0;
		line = buffer;

		if (buffer[0] == 'F')			// If a file
		{
			line = line.Mid(6);			// Is the file a source file?
			if (line.ReverseFind('\\') != -1)
				line = line.Mid(line.ReverseFind('\\')+1);
			done = false;
			i = 0;
			do
			{
				if (line.CompareNoCase(sources[i]) == 0)
					done = true;
				i++;
			}
			while (!done && i<numSources);
			if (!done)					// If the file isn't a source file
				continue;				// Skip it
			strcpy(filePath, &buffer[6]); // Copy the absolute path
			PathRelativePathTo(filePath, path,
				FILE_ATTRIBUTE_DIRECTORY, &buffer[6],
				FILE_ATTRIBUTE_NORMAL);
			file = GetFile(filePath);
			if (!file)					// If the file dosn't exist
			{
				in.close();				// Close listing file
				for (i=0; i<numSources; i++)
					delete sources[i];
				if (numSources)
					delete sources;
				errorCode = SPERROR_MISSINGSOURCE;
				return false;			// Failed.
			}
			firstLine = true;
#ifdef PRJDEBUG
if (file)
	debug<<"   File: "<<file->GetName()<<endl;
#endif

			continue;
		}
		if (!file)						// If not in a file,
			continue;					// Ignore it
		if (buffer[0] == '(')			// If a source line
		{
			if (firstLine)				// If the first line in a file
			{
				firstLine = false;
				lineNum = 0;
				index = 1;
				while (line.GetAt(index) != ')')
				{
					lineNum *= 10;
					lineNum += line.GetAt(index++)-'0';
				}
				if (lineNum == 0)
					zeroBased = true;
				else
					zeroBased = false;
			}
			srcLine = line;				// Store it

#ifdef PRJDEBUG
debug<<"      Source Line: "<<srcLine<<endl;
#endif
			continue;
		}

										// A code line
		if (srcLine.GetLength() == 0)	// No source line
				continue;
		if (line.Find("    ", 0) != 0)	// Not code
		{
				// Labels & variables are here
			continue;
		}
		lineNum = 0;
		index = 1;
		while (srcLine.GetAt(index) != ')')
		{
			lineNum *= 10;
			lineNum += srcLine.GetAt(index++)-'0';
		}
		if (!zeroBased)
			lineNum--;
		file->GetLine(lineNum)->code = true; // A code line

		address = htoi((char *)LPCTSTR(line.Mid(4,4)))*2;

		if (!globals.registered && address >= 2048)
		{
			in.close();					// Close listing file
			for (i=0; i<numSources; i++)
				delete sources[i];
			if (numSources)
				delete sources;
			errorCode = SPERROR_2KLIMIT;
			return false;				// Failed.
		}

		file->SetLineStartAddress(lineNum, address);
		do
		{
			in.getline(buffer, 1000);
			if (in.gcount() == 0)
				break;
			buffer[in.gcount()-1] = 0;
			if (buffer[0] == '(')		// If a source line
			{
				srcLine = buffer;		// Store it

#ifdef PRJDEBUG
debug<<"      Source Line: "<<srcLine<<endl;
#endif
				
			}
			else if (buffer[0] == 'F')	// A file line
			{
				line = buffer;
				line = line.Mid(6);
				if (line.ReverseFind('\\') != -1)
					line = line.Mid(line.ReverseFind('\\')+1);
				done = false;
				i = 0;
				do
				{
					if (line.CompareNoCase(sources[i]) == 0)
						done = true;
					i++;
				}
				while (!done && i<numSources);
				if (!done)				// If the file isn't a source file
					continue;			// Skip it
				strcpy(filePath, &buffer[6]); // Copy the absolute path
				PathRelativePathTo(filePath, path,
					FILE_ATTRIBUTE_DIRECTORY, &buffer[6],
					FILE_ATTRIBUTE_NORMAL);
				file = GetFile(filePath);
				if (!file)				// If the file dosn't exist
				{
					in.close();			// Close listing file
					for (i=0; i<numSources; i++)
						delete sources[i];
					if (numSources)
						delete sources;
					errorCode = SPERROR_MISSINGSOURCE;
					return false;		// Failed.
				}
				firstLine = true;
			}
			else if (buffer[0] == ' ')	// A code line
			{
				line = buffer;
				address = htoi((char *)LPCTSTR(line.Mid(4,4)))*2;

				if (!globals.registered && address >= 2048)
				{
					in.close();			// Close listing file
					for (i=0; i<numSources; i++)
						delete sources[i];
					if (numSources)
						delete sources;
					errorCode = SPERROR_2KLIMIT;
					return false;		// Failed.
				}

				file->SetLineEndAddress(lineNum, address);
			}
			else
				srcLine = "";			// Finished with this source line
		}
		while (buffer[0] == ' ');	// While code lines
	}
	while (in.gcount() != 0);
	in.close();

	for (i=0; i<numSources; i++)
		delete sources[i];
	if (numSources)
		delete sources;

#ifdef PRJDEBUG
debug<<"   Done"<<endl;
#endif

	return true;
}
*/

void FDProject::SetName(char * n)
{
	if (name)
		delete name;
	name = new char[strlen(n)+1];
	strcpy(name, n);
}


void FDProject::SetPath(char * p)
{
	if (path)
		delete path;
	path = new char[strlen(p)+1];
	strcpy(path, p);
}


//char * FDProject::GetListingFile()
//{
//	return listingFile;
//}


//void FDProject::SetListingFile(char * file)
//{
//	if (listingFile)
//		delete listingFile;
//	listingFile = new char[strlen(file)+1];
//	strcpy(listingFile, file);
//}


//char * FDProject::GetDebugFile()
//{
//	return debugFile;
//}


//void FDProject::SetDebugFile(char * file)
//{
//	if (debugFile)
//		delete debugFile;
//	debugFile = new char[strlen(file)+1];
//	strcpy(debugFile, file);
//}


int FDProject::GetIDE()
{
	return ide;
}


void FDProject::SetIDE(int i)
{
	ide = i;
}


int FDProject::GetIDEName(char * buffer, int * max)
{
	return ILGetIdeName(buffer, max);
}


int FDProject::GetFamily()
{
	return family;
}


void FDProject::SetFamily(int f)
{
	family = f;
}


int FDProject::GetFamilyName(char * buffer, int * max)
{
	return CLGetFamilyName(buffer, max);
}


int FDProject::GetChip()
{
	return chip;
}


void FDProject::SetChip(int c)
{
	int i, j;

	chip = c;
	
	SetPackage(package);

	pcSize = CLGetChipPCSize(chip);

	if (registersSize)
	{
		delete registers;
		delete registersMask;
	}
	registers = NULL;
	registersMask = NULL;
	if (sfrsSize)
	{
		delete sfrs;
		delete sfrsMask;
	}
	sfrs = NULL;
	sfrsMask = NULL;
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
	registersSize = CLGetChipRegistersSize(chip);
	sfrsSize = CLGetNumberSFRs(chip);
	sramStart = CLGetChipSramStart(chip);
	sramSize = CLGetChipSramSize(chip);
	eepromStart = CLGetChipEepromStart(chip);
	eepromSize = CLGetChipEepromSize(chip);
	if (registersSize)
	{
		registers = new unsigned char[registersSize];
		registersMask = new unsigned char[registersSize];
		for (i=0; i<registersSize; i++)
			registersMask[i] = 0;
	}
	if (sfrsSize)
	{
		sfrs = new unsigned char[sfrsSize];
		sfrsMask = new unsigned char[sfrsSize];
		for (i=0; i<sfrsSize; i++)
			sfrsMask[i] = 0;
	}
	if (externalMem)
	{
		if (externalMemStart < sramStart)
		{
			sramSize += sramStart-externalMemStart;
			sramStart = externalMemStart;
		}
		if (externalMemStart+externalMemSize > sramStart+sramSize)
			sramSize = (externalMemStart+externalMemSize)-sramStart;
	}
	if (sramSize)
	{
		sram = new unsigned char[sramSize];
		sramMask = new unsigned char[sramSize];
		for (i=0; i<sramSize; i++)
			sramMask[i] = 0;
		if (sramWindowHandle)			// If Sram window open
		{								// Get it to recalculate itself
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
		if (eepromWindowHandle)			// If Eeprom window open
		{								// Get it to recalculate itself
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

	int numOldSFRs = numSFRs;
	int * oldSfrFlags = sfrFlags;
	numSFRs = CLGetNumberSFRs(chip);
	sfrFlags = new int[numSFRs];
	for (i=0; i<numSFRs; i++)
		sfrFlags[i] = FALSE;			// Not selected is default

	if (sfrMarkers)
		delete sfrMarkers;
	i = numSFRs;
	sfrMarkers = new int[i];
	CLGetSfrMarkers(sfrMarkers, chip, &i);

	sfrNameLength = CLGetSfrNameLength();
	char * oldSfrNames = sfrNames;
	i = 0;
	CLGetSfrNames(NULL, chip, &i);		// Get needed size
	sfrNames = new char[i];
	CLGetSfrNames(sfrNames, chip, &i);	// Get names

	if (sfrFlagMarkers)
		delete sfrFlagMarkers;
	i = numSFRs*8;
	sfrFlagMarkers = new int[i];
	CLGetSfrFlagMarkers(sfrFlagMarkers, chip, &i);

	sfrFlagNameLength = CLGetSfrFlagNameLength();
	char * oldSfrFlagNames = sfrFlagNames;
	i = 0;
	CLGetSfrFlagNames(NULL, chip, &i);	// Get needed size
	sfrFlagNames = new char[i];
	CLGetSfrFlagNames(sfrFlagNames, chip, &i); // Get names

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
	if (oldSfrFlagNames)
		delete oldSfrFlagNames;

	if (sfrsWindowOpen)
		sfrsWindow->Refresh();
	if (chipWindowOpen)
		chipWindow->Refresh();
}


bool FDProject::GetChipName(int c, char * buffer, int count)
{
	if (c < 0 || c > chipIDs[numberChips-1])
		return false;
	if (count >= chipNameLength)
	{
		strcpy(buffer, &chipNames[c*chipNameLength]);
		return true;
	}
	return false;
}


int FDProject::GetPins()
{
	return pins;
}


//void FDProject::SetPins(int p)
//{
//	pins = p;
//}


int FDProject::GetPackage()
{
	return package;
}


void FDProject::SetPackage(int p)
{
	int i;
	int * packages = new int[numberPackages];
	i = numberPackages;
	CLGetAvailablePackages(packages, &i, chip);
	for (i=0; i<numberPackages; i++)
	{
		if (packages[i] == p)
		{
			package = p;
			pins = CLGetNumberPins(chip, package);
			return;
		}
	}
	package = CLGetDefaultPackage(chip);
	pins = CLGetNumberPins(chip, package);
}


bool FDProject::GetPackageName(int p, char * buffer, int count)
{
	if (p < 0 || p >= numberPackages)
		return false;
	if (buffer && count >= packageNameLength)
	{
		strcpy(buffer, &packageNames[p*packageNameLength]);
		return true;
	}
	return false;
}


void FDProject::SetComPort(int port)
{
	if (ComPortOpen())
		CloseComPort();
	selectedComPort = port;
	OpenComPort();
}


int FDProject::GetComPort()
{
	return selectedComPort;
}


void FDProject::SetBaud(int baud)
{
	if (ComPortOpen())
		CloseComPort();
	selectedBaud = baud;
	OpenComPort();
}


int FDProject::GetBaud()
{
	return selectedBaud;
}


bool FDProject::IsOpen()
{
	return open;
}


int FDProject::GetNumFiles()
{
	return numFiles;
}


FMSourceFile * FDProject::GetFile(int index)
{
	if (index < numFiles)
		return files[index];
	return NULL;
}


FMSourceFile * FDProject::GetFile(const char * file)
{
	CString fname = file;
	CString fpath = fname.Left(fname.ReverseFind('\\')+1);
	fname = fname.Mid(fpath.GetLength());
	for (int i=0; i<numFiles; i++)
	{
		if (stricmp(files[i]->GetFileName(), LPCTSTR(fname)) == 0 &&
			stricmp(files[i]->GetRelativePath(), LPCTSTR(fpath)) == 0)
			return files[i];
	}
	return NULL;
}


FMSourceFile * FDProject::GetFile(HWND window)
{
	for (int i=0; i<numFiles; i++)
	{
		if (files[i]->GetWindowHandle() == window)
			return files[i];
	}
	return NULL;
}


bool FDProject::AddFile(FMSourceFile * file)
{
	int i;

	if (numFiles == 0)
	{
		files = new FMSourceFile *[1];
		files[numFiles++] = file;
		return true;
	}
	for (i=0; i<numFiles; i++)
	{
		if (stricmp(files[i]->GetFileName(), file->GetFileName()) == 0 &&
			stricmp(files[i]->GetRelativePath(), file->GetRelativePath()) == 0)
			return false;
	}
	FMSourceFile ** temp = new FMSourceFile *[numFiles+1];
	for (i=0; i<numFiles; i++)
		temp[i] = files[i];
	delete files;
	files = temp;
	files[numFiles++] = file;
	return true;
}


bool FDProject::SaveToRegistry()
{
	HKEY localmachine;
	HKEY software;
	HKEY company;
	HKEY product;
	HKEY version;
	HKEY projects;
	unsigned long length;
	unsigned long disposition;
	long regError;
	char buffer[10000];
	char saved[10000];

	saved[0] = 0;
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
						if (regError = RegOpenKeyEx(version, PROJECTSKEY, 0,
							KEY_READ|KEY_WRITE, &projects) != ERROR_SUCCESS)
						{
							RegCreateKeyEx(version, PROJECTSKEY, 0, NULL,
								REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE,
								NULL, &projects, &disposition);
						}

						char recent = '4';
						CString key = "Recent 4";
						bool done = false;
						do
						{
							length = 10000;
							key.Insert(7, recent);
							key = key.Left(8);
							regError = RegQueryValueEx(projects, LPCTSTR(key),
								NULL, NULL, (unsigned char *)buffer, &length);
							if (regError == ERROR_SUCCESS || recent == '0')
							{				// Found last recent project
											// Shift existing entries
								if (recent == '4')
								{
									length = 10000;
									key.Insert(7, recent);
									key = key.Left(8);
									RegQueryValueEx(projects, LPCTSTR(key),
										NULL, NULL, (unsigned char *)saved,
										&length); // Save for later
									recent = '3'; // Ditch 4th project
								}
								while (recent != '0')
								{
									length = 10000;
									key.Insert(7, recent);
									key = key.Left(8);
									RegQueryValueEx(projects, LPCTSTR(key),
										NULL, NULL, (unsigned char *)buffer,
										&length);
									length = strlen(buffer)+1;
									recent = recent+1;
									key.Insert(7, recent);
									key = key.Left(8);
									RegSetValueEx(projects, LPCTSTR(key), NULL,
										REG_SZ, (CONST BYTE *)buffer, length);
									recent = recent-2;
								}
								strcpy(buffer, GetSPPPath());
								strcat(buffer, GetSPPName());
								strcat(buffer, ".spp");
								length = strlen(buffer)+1;
								RegSetValueEx(projects, "Recent 1", NULL, REG_SZ,
									(CONST BYTE *)buffer, length);
								done = true;
							}
							else
								recent = recent-1;
						}
						while (!done);

						recent = '4';
						key = "Recent 4";
						done = false;
						CString projectFile = GetSPPPath();
						projectFile += GetSPPName();
						projectFile += ".spp";
						do
						{
							length = 10000;
							key.Insert(7, recent);
							key = key.Left(8);
							regError = RegQueryValueEx(projects, LPCTSTR(key),
								NULL, NULL, (unsigned char *)buffer, &length);
							if (regError != ERROR_SUCCESS)
							{
								recent = recent-1;
								continue;
							}
							if (strcmp(LPCTSTR(projectFile), buffer) != 0)
							{				// If not found project already in list
								recent = recent-1;
								continue;
							}
											// Project is already in list
											// Shift existing entries forward
							while (!done && recent != '5')
							{
								length = 10000;
								recent = recent+1;
								key.Insert(7, recent);
								key = key.Left(8);
								regError = RegQueryValueEx(projects, LPCTSTR(key),
									NULL, NULL, (unsigned char *)buffer, &length);
								if (regError != ERROR_SUCCESS)
								{
									recent = recent-1;
									key.Insert(7, recent);
									key = key.Left(8);
									if (saved[0]) // If saved last project earlier
									{
										length = strlen(saved)+1;
										RegSetValueEx(projects, LPCTSTR(key), NULL,
											REG_SZ, (CONST BYTE *)saved, length);
									}
									else
										RegDeleteValue(projects, LPCTSTR(key));
									done = true;
									continue;
								}
								length = strlen(buffer)+1;
								recent = recent-1;
								key.Insert(7, recent);
								key = key.Left(8);
								RegSetValueEx(projects, LPCTSTR(key), NULL,
									REG_SZ, (CONST BYTE *)buffer, length);
								recent = recent+1;
							}
						}
						while (!done && recent != '1');

						RegCloseKey(projects);
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
	return true;
}


int FDProject::GetTreeItem()
{
	return treeItem;
}


void FDProject::SetTreeItem(int ti)
{
	treeItem = ti;
}


void FDProject::SetPC()
{
	int i;
	FMSourceFile * file;
	// Set PC info

	pcLine = -1;
	pcFile = NULL;
	i = 0;
	while (i < numFiles)
	{
		file = files[i];
		pcLine = file->FindPCLine(pc);
		if (pcLine != -1)
		{
			pcFile = file;
			break;
		}
		i++;
	}
//	if (pcFile)
//		pcFile->UpdateWindow();
//	if (oldPCFile && oldPCFile != pcFile)
//		oldPCFile->UpdateWindow();
//	oldPCFile = pcFile;
}

/*
bool FDProject::FindISD()
{
	int i;
	bool found;
	int isdAddress = -1;
	char vector[17];
	unsigned int opCode;
	int bytes;
	int address;
	int type;
	int index;

	found = false;
	if (ide == IDE_IMAGECRAFT)
	{
		ifstream in;
		char buffer[1000];
		CString line;

		line = path;
		line += name;
		line += ".prj";					// Project file
		found = false;					// Find output setting
		in.open(line, ios::in|ios::nocreate);
		do
		{
			in.getline(buffer, 1000);
			if (in.gcount() == 0)
			{
				in.close();
				errorCode = SPERROR_NOCODE;
				strcpy(errorMessage,
		"Couldn't find the output option in your ImageCraft project file.");
				return false;
			}
			buffer[in.gcount()-1] = 0;
			line = buffer;
			if (line.Find("ComboBox1=") != -1) // Found it!
			{
				line = line.Mid(10);
				if (line.GetAt(0) != '0' && line.GetAt(0) != '2')
				{
					in.close();
					errorCode = SPERROR_NOCODE;
					strcpy(errorMessage,
"Your ImageCraft project's output option is set to COFF only.\r\n\
Studio+ needs a HEX output file, change the output option in\r\n\
ImageCraft to \"COFF\\HEX\" or \"Intel HEX\" and recompile it.");
					return false;
				}
				found = true;			// There is a HEX file
			}
		}
		while (!found);
		in.close();

		line = outputDir;
		line += name;
		line += ".hex";					// Intel HEX file
		found = false;					// Find Interrupt Vector
		strcpy(vector, "0000000000000000");
		in.open(line, ios::in|ios::nocreate);
		do
		{
			in.getline(buffer, 1000);
			if (in.gcount() == 0)
			{
				in.close();
				errorCode = SPERROR_NOISDVECTOR;
				return false;
			}
			buffer[in.gcount()-1] = 0;
			line = buffer;

			if (line.GetAt(0) != ':')
				continue;				// Not a hex line
			line = line.Mid(1);
			strcpy(buffer, "0000");
			buffer[2] = line.GetAt(0);
			buffer[3] = line.GetAt(1);
			bytes = htoi(buffer);
			strcpy(buffer, "0000");
			for (i=0; i<4; i++)
				buffer[i] = line.GetAt(2+i);
			address = htoi(buffer);
			strcpy(buffer, "0000");
			buffer[2] = line.GetAt(6);
			buffer[3] = line.GetAt(7);
			type = htoi(buffer);

			if (type != 0)				// If not data
				continue;

			if (address < 8)			// If Vector data
			{
				line = line.Mid(8);
				index = address;
				do
				{
					vector[index*2] = line.GetAt((index-address)*2);
					vector[index*2+1] =	line.GetAt((index-address)*2+1);
					index++;
				}
				while (index < address+bytes && index < 8);
				if (index >= 8)
				{
					vector[16] = 0;		// Terminating null
					found = true;		// Got the vector data
				}
			}
			else
				found = true;			// Got all available vectors
										// Either only 1 JMP vector
										// or 1,2 or 3 RJMP vectors.
		}
		while (!found);
		in.close();

#ifdef PRJDEBUG
debug<<"   Vector: "<<vector<<endl;
#endif

		strcpy(buffer, vector);
		buffer[8] = 0;
		opCode = (unsigned int)htoi(&buffer[4]);
		buffer[4] = 0;
		opCode += ((unsigned int)htoi(buffer))<<16;
		if ((opCode&0x0EFE0000) == OPCODE_AVR_JMP) // JMP?
			bytesPerVector = 4;
		else
			bytesPerVector = 2;

		if (bytesPerVector == 4)		// Vectors are JMP instructions
		{
			strcpy(buffer, &vector[8]);	// Second vector (INT0)
			opCode = (unsigned int)htoi(&buffer[4]);
			buffer[4] = 0;
			opCode += ((unsigned int)htoi(buffer))<<16;
			unsigned int oc = (opCode&0x0EFE0000);
			if (oc != OPCODE_AVR_JMP) // JMP?
			{
				errorCode = SPERROR_NOISDVECTOR;
				return false;
			}
			int0Vector = (opCode&0xF101FFFF);
			int0Vector = (int0Vector>>8)+((int0Vector&0x00FF00FF)<<8);
			int0Vector = (int0Vector&0x000FFFFF)+((int0Vector&0xFFF00000)>>3);
		}
		else							// Vectors are RJMP instructions
		{
			strcpy(buffer, &vector[4]);	// Second vector (INT0)
			buffer[4] = 0;
			opCode = (unsigned int)htoi(buffer);
			unsigned int oc = (opCode&0x00F0);
			if (oc != OPCODE_AVR_RJMP)	// RJMP?
			{
				errorCode = SPERROR_NOISDVECTOR;
				return false;
			}
			opCode = (opCode>>8)+((opCode&0xff)<<8);
			opCode &= 0xfff;
			if (opCode > 2047)			// If it's negative
			{
				opCode = ((~opCode)&0xfff)+1;
				int0Vector = 0-(int)opCode;
			}
			else
				int0Vector = opCode;
			int0Vector += 2;			// PC(1)+offset+1
		}

		int0Vector *= 2;				// AVR vectors are WORD addresses

#ifdef PRJDEBUG
debug<<"   INT0 Vector: "<<int0Vector<<endl;
#endif

		line = outputDir;
		line += name;
		line += ".hex";
		found = false;					// Find ISD signature
		char signature[13];
		strcpy(signature, "000000000000");
		in.open(line, ios::in|ios::nocreate);
		do
		{
			in.getline(buffer, 1000);
			if (in.gcount() == 0)
			{
				in.close();
				errorCode = SPERROR_NOISD;
				return false;
			}
			buffer[in.gcount()-1] = 0;
			line = buffer;

			if (line.GetAt(0) != ':')
				continue;				// Not a hex line
			line = line.Mid(1);
			strcpy(buffer, "0000");
			buffer[2] = line.GetAt(0);
			buffer[3] = line.GetAt(1);
			bytes = htoi(buffer);
			strcpy(buffer, "0000");
			for (i=0; i<4; i++)
				buffer[i] = line.GetAt(2+i);
			address = htoi(buffer);
			strcpy(buffer, "0000");
			buffer[2] = line.GetAt(6);
			buffer[3] = line.GetAt(7);
			type = htoi(buffer);

			if (type != 0)				// If not data
				continue;

			if (address < int0Vector+24 &&
				address+bytes >= int0Vector+18) // If Signature data
			{

#ifdef PRJDEBUG
debug<<"      address = "<<address<<endl;
#endif
				
				line = line.Mid(8);
				index = address;
				do
				{
					if (index >= int0Vector+18 &&
						index < int0Vector+24)
					{
						signature[(index-(int0Vector+18))*2] =
							line.GetAt((index-address)*2);
						signature[(index-(int0Vector+18))*2+1] =
							line.GetAt((index-address)*2+1);
					}
					index++;
				}
				while (index < address+bytes && index < int0Vector+24);
				if (index >= int0Vector+24)
				{
					signature[12] = 0;	// Terminating null
					found = true;		// Got the signature data
				}

#ifdef PRJDEBUG
debug<<"      signature: "<<signature<<endl;
#endif

			}
		}
		while (!found);
		in.close();

#ifdef PRJDEBUG
debug<<"   Got signature: "<<signature<<endl;
#endif

		buffer[23] = 0; 
		for (i=0; i<3; i++)
		{
			strcpy(buffer, signature);
			buffer[2+(i*2)] = 0;

#ifdef PRJDEBUG
debug<<"      chars: "<<(char *)(&buffer[i*2])<<endl;
int temp = htoi(&buffer[i*2]);
debug<<"      int: "<<temp<<endl;
#endif

			buffer[20+i] = htoi(&buffer[i*2]);

#ifdef PRJDEBUG
debug<<"      buffer: "<<(char *)(&buffer[20])<<endl;
#endif
		}

#ifdef PRJDEBUG
debug<<"   Signature chars: "<<(char *)(&buffer[20])<<endl;
#endif

		if (strcmp(&buffer[20], "ISD") != 0) // No signature
		{
			errorCode = SPERROR_NOISD;
			return false;
		}

		setbreakhereAddress = htoi(&signature[8]);
		setbreakhereAddress =
			(setbreakhereAddress>>8)+((setbreakhereAddress&0xff)<<8);

#ifdef PRJDEBUG
debug<<"   Got _setbreakhere: "<<setbreakhereAddress<<endl;
#endif
	}

	return found;
}
*/

/*void FDProject::FindHardBreakpoints()
{
	if (ide == IDE_IMAGECRAFT)
	{
		ifstream in;
		CString line;
		CString srcLine;
		int lineNum;
		FMSourceFile * file = NULL;
		char buffer[1000];
		char filePath[_MAX_PATH];
		int index;
		unsigned int opCode;
		int offset;
		bool firstLine = false;
		bool zeroBased = true;

		line = outputDir;
		line += name;
		line += ".lst";
		in.open(line, ios::in|ios::nocreate);
		do
		{
			in.getline(buffer, 1000);
			if (in.gcount() == 0)
				continue;
			buffer[in.gcount()-1] = 0;
			line = buffer;

			if (buffer[0] == 'F')			// If a file
			{
				strcpy(filePath, &buffer[6]); // Copy the absolute path
				line = (char *)&buffer[6];
				line = line.Mid(0, line.ReverseFind('\\')+1);
				PathRelativePathTo(filePath, path,
					FILE_ATTRIBUTE_DIRECTORY, line,
					FILE_ATTRIBUTE_DIRECTORY);
				if (filePath[strlen(filePath)-1] != '\\')
					strcat(filePath, "\\");
				line = (char *)&buffer[6];
				line = line.Mid(line.ReverseFind('\\')+1);
				strcat(filePath, line);
				file = GetFile(filePath);
				firstLine = true;
				continue;
			}
			if (!file)						// If not in a file,
				continue;					// Ignore it
			if (buffer[0] == '(')			// If a source line
			{
				if (firstLine)				// If the first line in a file
				{							// Determine line nos. are 0 based
					firstLine = false;
					lineNum = 0;
					index = 1;
					while (line.GetAt(index) != ')')
					{
						lineNum *= 10;
						lineNum += line.GetAt(index++)-'0';
					}
					if (lineNum == 0)
						zeroBased = true;
					else
						zeroBased = false;
				}
				srcLine = line;				// Store it
				continue;
			}
			if (line.Find("    ", 0) != 0)	// Not code
				continue;					// Labels etc
											// A code line

			opCode = (unsigned int)htoi((char *)LPCTSTR(line.Mid(9,4)));
			if (opCode>>12 == 0xD) //OPCODE_AVR_RCALL) // RCALL ?
			{
				lineNum = 0;
				index = 1;
				while (srcLine.GetAt(index) != ')')
				{
					lineNum *= 10;
					lineNum += srcLine.GetAt(index++)-'0';
				}
				if (!zeroBased)
					lineNum--;
				opCode &= 0xfff;
				if (opCode > 2047)			// If it's negative
				{
					opCode = ((~opCode)&0xfff)+1;
					offset = 0-(int)opCode;
				}
				else
					offset = opCode;
				int address = htoi((char *)LPCTSTR(line.Mid(4,4)));
				if ((address+offset)+1 == setbreakhereAddress/2)
					file->SetHardBP(lineNum);
			}
		}
		while (in.gcount() != 0);
		in.close();
	}
	UpdateBreakpoints();
}
*/

int FDProject::NumSourcesOpen()
{
	int numOpen = 0;
	for (int i=0; i<numFiles; i++)
	{
		if (files[i]->IsOpen())
			numOpen++;
	}
	return numOpen;
}


void FDProject::UpdateBreakpoints()
{
	int bp;
	numSoftBPs = 0;
	numHardBPs = 0;
	for (int i=0; i<numFiles; i++)
	{
		for (int j=0; j<files[i]->GetNumLines(); j++)
		{
			bp = files[i]->GetLineBreakpoint(j);
			if (bp == SOFTBREAKPOINT)
				numSoftBPs++;
			if (bp == HARDBREAKPOINT)
				numHardBPs++;
		}
	}
}


int FDProject::GetNumSoftBPs()
{
	return numSoftBPs;
}


int FDProject::GetNumHardBPs()
{
	return numHardBPs;
}


bool FDProject::GetContextMenu(int ti, int * menu)
{
	int i;

	if (treeItem == ti)
	{
		*menu = IDM_CONTEXT_PROJECT;
		return true;
	}

	for (i=0; i<numFiles; i++)
	{
		if (files[i]->GetContextMenuId(ti, menu))
			return true;
	}
	return false;
}


FMSourceFile * FDProject::GetSourceFileFromTreeItem(int ti)
{
	if (treeItem == ti)
		return NULL;
	for (int i=0; i<numFiles; i++)
	{
		if (files[i]->GetTreeItem() == ti)
			return files[i];
	}
	return NULL;
}


FMFunction * FDProject::GetFunctionFromTreeItem(int ti)
{
	if (treeItem == ti)
		return NULL;
	for (int i=0; i<numFiles; i++)
	{
		if (files[i]->GetTreeItem() == ti)
			return NULL;
		for (int j=0; j<files[i]->GetNumFunctions(); j++)
		{
			if (files[i]->GetFunction(j)->GetTreeItem() == ti)
				return files[i]->GetFunction(j);
		}
	}
	return NULL;
}


FMSymbol * FDProject::GetSymbolFromTreeItem(int ti)
{
	FMSymbol * symbol;
	if (treeItem == ti)
		return NULL;
	for (int i=0; i<numFiles; i++)
	{
		symbol = files[i]->GetSymbolFromTreeItem(ti);
		if (symbol != NULL)
			return symbol;
	}
	return NULL;
}


int FDProject::GetNumWatchSymbols()
{
	return numWatchSymbols;
}


FMSymbol * FDProject::GetWatchSymbol(int index)
{
	if (index < 0 || index > numWatchSymbols-1)
		return NULL;
	return watchSymbols[index];
}


bool FDProject::AddSymbolToWatch(FMSymbol * symbol, bool open)
{
	int item = 0;
	int i;

#ifdef PRJDEBUG
debug<<"FDProject::AddSymbolToWatch():"<<endl;
#endif

	for (i=0; i<numWatchSymbols; i++)
	{
		if (watchSymbols[i] == symbol)
		{
			if (!watchWindowOpen && open)
				theApp.OnViewWatch();	// Open the Watch window
			if (watchWindowOpen)
			{
				watchWindow->SelectSymbol(symbol);
				watchWindow->SetFocus();
			}
			return false;				// Already been added!!
		}
	}

#ifdef PRJDEBUG
debug<<"   Adding: "<<symbol->GetName()<<endl;
#endif

	if (!watchWindowOpen && open)
		theApp.OnViewWatch();

	FMSymbol ** temp = new FMSymbol *[numWatchSymbols+1];
	for (i=0; i<numWatchSymbols; i++)
		temp[i] = watchSymbols[i];
	if (numWatchSymbols)
		delete watchSymbols;
	watchSymbols = temp;
	watchSymbols[numWatchSymbols++] = symbol;

	symbol->UpdateValue();
	if (watchWindowOpen)
		watchWindow->AddSymbol(symbol);

	firstWatchSymbol = numWatchSymbols-1;
	lastWatchSymbol = numWatchSymbols;

	if (projectWindowOpen &&
		!(symbol->GetType()&VARIABLE_ASSEMBLERMASK))
	{
		TVITEM tItem;
		tItem.mask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		tItem.hItem = (HTREEITEM)symbol->GetTreeItem();
		projectWindow->tree.GetItem(&tItem);
		tItem.iImage = 10;
		tItem.iSelectedImage = 10;
		projectWindow->tree.SetItem(&tItem);
	}

	return true;
}


bool FDProject::RemoveSymbolFromWatch(FMSymbol * symbol)
{
	int i;
	int index = -1;

	for (i=0; i<numWatchSymbols; i++)
		if (symbol == watchSymbols[i])
			index = i;
	if (index == -1)
		return false;
	if (numWatchSymbols == 1)
	{
		numWatchSymbols = 0;
		delete watchSymbols;
		watchSymbols = NULL;
	}
	else
	{
		FMSymbol ** temp = new FMSymbol *[numWatchSymbols-1];
		for (i=0; i<index; i++)
			temp[i] = watchSymbols[i];
		for (i=index+1; i<numWatchSymbols; i++)
			temp[i-1] = watchSymbols[i];
		numWatchSymbols--;
		delete watchSymbols;
		watchSymbols = temp;
	}

	if (projectWindowOpen &&
		!(symbol->GetType()&VARIABLE_ASSEMBLERMASK))
	{
		TVITEM tItem;
		tItem.mask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		tItem.hItem = (HTREEITEM)symbol->GetTreeItem();
		projectWindow->tree.GetItem(&tItem);
		tItem.iImage = 9;
		tItem.iSelectedImage = 9;
		projectWindow->tree.SetItem(&tItem);
	}

	return true;
}


void FDProject::UpdateWatchSymbols()
{
	for (int i=0; i<numWatchSymbols; i++)
		watchSymbols[i]->UpdateValue();
}


int * FDProject::GetSfrFlags()
{
	return sfrFlags;
}


int FDProject::GetNumSfrs()
{
	return numSFRs;
}


int FDProject::GetSfrFlag(int index)
{
	if (index > -1 && index < numSFRs)
		return sfrFlags[index];
	return -1;
}


void FDProject::SetSfrFlags(int * flags, int count)
{
	if (numSFRs)
		delete sfrFlags;
	sfrFlags = flags;
	numSFRs = count;
}


void FDProject::SetSfrFlag(int index, int sfr)
{
	if (index > -1 && index < numSFRs)
		sfrFlags[index] = sfr;
}


void FDProject::ShowSfrFlags(int show)
{
	sfrFlagsVisible = show;
}


int FDProject::SfrFlagsVisible()
{
	return sfrFlagsVisible;
}


int FDProject::GetSfrMarker(int sfr)
{
	if (sfr > -1 && sfr < numSFRs)
		return sfrMarkers[sfr];
	return FALSE;
}


char * FDProject::GetSfrName(int sfr)
{
	if (sfr > -1 && sfr < numSFRs)
		return &sfrNames[sfr*sfrNameLength];
	return NULL;
}


int * FDProject::GetSfrFlagMarkers(int sfr)
{
	if (sfr > -1 && sfr < numSFRs)
		return &sfrFlagMarkers[sfr*8];
	return NULL;
}


char * FDProject::GetSfrFlagName(int sfr, int flag)
{
	if (sfr > -1 && sfr < numSFRs && flag > -1 && flag < 8)
		return &sfrFlagNames[((sfr*8)+flag)*sfrFlagNameLength];
	return NULL;
}


int FDProject::IsUsingExternalMem()
{
	return externalMem;
}


int FDProject::GetExternalMemStart()
{
	return externalMemStart;
}


int FDProject::GetExternalMemSize()
{
	return externalMemSize;
}


void FDProject::SetExternalMem(int start, int size)
{
	externalMemStart = start;
	externalMemSize = size;
	if (externalMemStart == 0 && externalMemSize == 0)
		externalMem = FALSE;
	else
		externalMem = TRUE;
	SetChip(chip);						// Reallocate SRAM memory
}


int FDProject::SetSPPName(char * buffer)
{
	if (sppName)
		delete sppName;
	sppName = NULL;
	if (buffer)
	{
		sppName = new char[strlen(buffer)+1];
		strcpy(sppName, buffer);
	}
	return TRUE;
}


int FDProject::SetSPPPath(char * buffer)
{
	if (sppPath)
		delete sppPath;
	sppPath = NULL;
	if (buffer)
	{
		sppPath = new char[strlen(buffer)+1];
		strcpy(sppPath, buffer);
	}
	return TRUE;
}
