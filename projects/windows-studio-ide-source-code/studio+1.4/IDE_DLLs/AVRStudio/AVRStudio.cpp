// AVRStudio.cpp
// Studio+ IDE library for "Atmel AVR Studio" Project support.

#include "stdafx.h"
#include "stdlib.h"
#include "fstream.h"					// File I/O
#include "Shlwapi.h"					// Path functions

#include "AVRStudio.h"					// Definitions & info
#include "SourceFile.h"					// FMSourceFile class
#include "resource.h"					// Bitmap ID

///////////////////////////////////////////////////////////////////////////////
// Dummy globals

int pc;
unsigned char * registers;
unsigned char * sram;
int sramStart;
int framePointer;

///////////////////////////////////////////////////////////////////////////////
// Exported function definitions

extern "C" __declspec(dllexport) int GetIdeID(void);
extern "C" __declspec(dllexport) int GetIdeName(char * name, int * max);
extern "C" __declspec(dllexport) int GetIdeVersion(char * version, int * max);
extern "C" __declspec(dllexport) HBITMAP GetIdeLogo(void);
extern "C" __declspec(dllexport) int GetSupportedFamilys(int * familys,
	int * max);
extern "C" __declspec(dllexport) int GetFamilySupportID(int family);
extern "C" __declspec(dllexport) int GetProjectFileExt(char * ext, int * max);
extern "C" __declspec(dllexport) int GetProjectFileFilter(char * filter,
	int * max);
extern "C" __declspec(dllexport) int IsProjectAssembler(const char * prj);
extern "C" __declspec(dllexport) int GetOutputDir(char * dir, const char * prj,
	int * max);
extern "C" __declspec(dllexport) int GetISD(const char * project,
	const char * outputDir, int * setbreakhereAddress);
extern "C" __declspec(dllexport) int GetDebugInfo(const char * project,
	const char * outputDir, FMSourceFile ** files, int * numFiles);
extern "C" __declspec(dllexport) int DeleteDebugInfo(FMSourceFile ** files,
	int numFiles);
extern "C" __declspec(dllexport) int GetCodeInfo(const char * project,
	const char * outputDir, FMSourceFile ** files, int numFiles,
	int * stackAddress, int registered);
extern "C" __declspec(dllexport) int FindHardBreakpoints(const char * project,
	const char * outputDir, FMSourceFile ** files, int numFiles,
	int setbreakhereAddress);
extern "C" __declspec(dllexport) int GetAsmComment(char * comment, int * max);
extern "C" __declspec(dllexport) char GetAsmStringChar();
extern "C" __declspec(dllexport) char GetAsmCharChar();
extern "C" __declspec(dllexport) char GetAsmEscapeChar();
extern "C" __declspec(dllexport) char GetAsmDirectiveChar();
extern "C" __declspec(dllexport) int GetAsmDirectives(char * directives,
	int * max);
extern "C" __declspec(dllexport) int GetAsmKeywordChars(char * chars,
	int * max);
extern "C" __declspec(dllexport) int GetAsmKeywords(char * keywords,
	int * max);
extern "C" __declspec(dllexport) int GetCComment(char * comment, int * max);
extern "C" __declspec(dllexport) int GetCCommentStart(char * string,
	int * max);
extern "C" __declspec(dllexport) int GetCCommentEnd(char * string, int * max);
extern "C" __declspec(dllexport) char GetCStringChar();
extern "C" __declspec(dllexport) char GetCCharChar();
extern "C" __declspec(dllexport) char GetCEscapeChar();
extern "C" __declspec(dllexport) char GetCDirectiveChar();
extern "C" __declspec(dllexport) int GetCDirectives(char * directives,
	int * max);
extern "C" __declspec(dllexport) int GetCKeywordChars(char * chars, int * max);
extern "C" __declspec(dllexport) int GetCKeywords(char * keywords, int * max);
extern "C" __declspec(dllexport) int GetLastIsdError(int * code,
	char * message, int * max);

///////////////////////////////////////////////////////////////////////////////
// Internal function declarations

void ParseListing(char ** list, int numListLines, int * listLineNum,
	FMSourceFile * file, FMSourceFile ** files,	int numFiles, BOOL registered);
int ahtoi(const char * ascii);
int htoi(const char * hex);
char * itoh(int i, char * hex, bool trim, int chars);


///////////////////////////////////////////////////////////////////////////////
// DLL entry point

BOOL WINAPI DllEntryPoint(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// Exported Functions


__declspec(dllexport) int GetIdeID()
{
	return IDE_ID;
}


__declspec(dllexport) int GetIdeName(char * name, int * max)
{
	if (name && *max >= (int)strlen(IDE_NAME)+1)
	{
		strcpy(name, IDE_NAME);
		return TRUE;
	}
	*max = strlen(IDE_NAME)+1;
	return FALSE;
}


__declspec(dllexport) int GetIdeVersion(char * version, int * max)
{
	if (version && *max >= (int)strlen(IDE_VERSION)+1)
	{
		strcpy(version, IDE_VERSION);
		return TRUE;
	}
	*max = strlen(IDE_VERSION)+1;
	return FALSE;
}


__declspec(dllexport) HBITMAP GetIdeLogo()
{
	HINSTANCE hInstance = ::GetModuleHandle("AVRStudio.dll");
	HBITMAP hBitmap = ::LoadBitmap(hInstance,
		MAKEINTRESOURCE(IDB_AVRSTUDIOLOGO));
	return hBitmap;
}


__declspec(dllexport) int GetSupportedFamilys(int * familys, int * max)
{
	if (familys && *max >= NUMBERFAMILYS)
	{
		for (int i=0; i<NUMBERFAMILYS; i++)
			familys[i] = supportedFamilys[i];
		return TRUE;
	}
	*max = NUMBERFAMILYS;
	return FALSE;
}


__declspec(dllexport) int GetFamilySupportID(int family)
{
	for (int i=0; i<NUMBERFAMILYS; i++)
	{
		if (family == familyIDs[i*2])	// If family is supported
			return familyIDs[i*2+1];	// Return ID for this family
	}
	return -1;							// Not supported
}


__declspec(dllexport) int GetProjectFileExt(char * ext, int * max)
{
	if (ext && *max >= (int)strlen(PROJECTEXTENSION)+1)
	{
		strcpy(ext, PROJECTEXTENSION);
		return TRUE;
	}
	*max = strlen(PROJECTEXTENSION)+1;
	return FALSE;
}


__declspec(dllexport) int GetProjectFileFilter(char * filter, int * max)
{
	if (filter && *max >= (int)strlen(PROJECTFILEFILTER)+1)
	{
		strcpy(filter, PROJECTFILEFILTER);
		return TRUE;
	}
	*max = strlen(PROJECTFILEFILTER)+1;
	return FALSE;
}


__declspec(dllexport) int IsProjectAssembler(const char * prj)
{
	ifstream in;
	unsigned char * fileData;
	int found;

	found = FALSE;

	// Find the file size
	in.open(prj, ios::in|ios::binary|ios::nocreate);
	in.seekg(0, ios::end);
	long size = in.tellg();

	// Allocate a buffer to hold the file data
	fileData = new unsigned char[size+1];

	// Read in the file data
	in.seekg(0, ios::beg);
	long read = 0;
	do
	{
		in.read(&fileData[read], size-read);
		read += in.gcount();
	}
	while (read != size);
	in.close();

	// Try to find the string "SProjectGenericCBuilder"
	int stringLen = strlen("SProjectGenericCBuilder");
	int from = (size-1)-stringLen;
	do
	{
		fileData[from+stringLen] = 0;
		if (strcmp((char *)&fileData[from--], "SProjectGenericCBuilder") == 0)
			found = TRUE;
	}
	while (!found && from);
	delete fileData;

	return !found;
}


__declspec(dllexport) int GetOutputDir(char * dir, const char * prj, int * max)
{
	char * buffer = new char[strlen(prj)+1];
	strcpy(buffer, prj);
	int i = strlen(buffer)-1;
	while(buffer[i] != '\\')
		buffer[i--] = 0;
	if (dir && *max >= (int)strlen(buffer)+1)
	{
		strcpy(dir, buffer);
		delete buffer;
		return TRUE;
	}
	if (!dir)
	{
		*max = strlen(buffer)+1;
		delete buffer;
		return TRUE;
	}
	*max = strlen(buffer)+1;
	delete buffer;
	return FALSE;
}


__declspec(dllexport) int GetISD(const char * project, const char * outputDir,
	int * setbreakhereAddress)
{
	ifstream in;
	char buffer[1000];
	CString line;
	CString file;
	CString hexExt;
	CString hexName;
	CString lstExt;
	CString lstName;
	int bytesPerVector;
	int isdVector;
	char vector[17];
	unsigned int opCode;
	int bytes;
	int address;
	int type;
	int index;
	int i;
	int found;

	*setbreakhereAddress = -1;			// Not found

	// First find out if there's an Intel Hex file
	in.open(project, ios::in|ios::binary|ios::nocreate);
	in.seekg(-12, ios::end);
	unsigned char format;
	in.read(&format, 1);
	if (format != 2)					// If output isn't Intel Hex
	{
		error = SPERROR_NOCODE;
		in.close();
		return FALSE;
	}

	// Next find it's extension
	in.seekg(-13, ios::end);
	index = 0;
	do
	{
		i = in.peek();
		if (i >= ' ')
		{
			in.seekg(-1, ios::cur);		// Back one char
			index++;
		}
	}
	while (i >= ' ');
	in.seekg(1, ios::cur);				// Forward one char
	in.read((unsigned char *)buffer, index);
	buffer[index] = 0;					// Hex file Extension
	hexExt = ".";
	hexExt += buffer;

	in.seekg(-13, ios::end);
	do
	{
		i = in.peek();
		if (i != '\\')
			in.seekg(-1, ios::cur);
	}
	while (i != '\\');
	in.seekg(1, ios::cur);				// Forward one char
	i = 0;
	do									// Get entry file name
	{
		in.read((unsigned char *)&buffer[i], 1);
		if (buffer[i] < ' ')
			buffer[i] = 0;
	}
	while (buffer[i++] != 0);
	in.close();
	hexName = buffer;
	if (hexName.Find('.') != -1)
		hexName = hexName.Mid(0, hexName.Find('.'));
	hexName += hexExt;

	// Next find the ISD interrupt vector in the HEX output file	
	strcpy(vector, "0000000000000000");	// Initialise hex buffer
	file = outputDir;
	file += hexName;					// Intel HEX file
	in.open(file, ios::in|ios::nocreate);
	do
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
			continue;
		buffer[in.gcount()-1] = 0;
		line = buffer;

		if (line.GetAt(0) != ':')
			continue;					// Not a hex line
										// Line: ":NNAAAATT
										//    NN = Number of bytes
										//  AAAA = First Address
										//    TT = Type (00 = Data)
		line = line.Mid(1);
		bytes = htoi(line.Mid(0, 2));
		address = htoi(line.Mid(2, 4));
		type = htoi(line.Mid(6, 2));

		if (type != 0)					// If not data
			continue;					// Next line

		if (address < 8)				// If Vector data
		{
			line = line.Mid(8);
			index = address;
			do
			{							// Get vector data
				vector[index*2] = line.GetAt((index-address)*2);
				vector[index*2+1] =	line.GetAt((index-address)*2+1);
				index++;
			}
			while (index < address+bytes && index < 8);
		}
	}
	while (in.gcount() != 0);
	in.close();

	strcpy(buffer, vector);
	buffer[8] = 0;
	opCode = (unsigned int)htoi(&buffer[4]);
	buffer[4] = 0;
	opCode += ((unsigned int)htoi(buffer))<<16;
	if ((opCode&OPCODE_AVR_JMPMASK) == OPCODE_AVR_JMP) // JMP?
		bytesPerVector = 4;
	else
		bytesPerVector = 2;

	if (bytesPerVector == 4)			// Vectors are JMP instructions
	{
		strcpy(buffer, &vector[8]);		// Second vector (INT0)
		opCode = (unsigned int)htoi(&buffer[4]);
		buffer[4] = 0;
		opCode += ((unsigned int)htoi(buffer))<<16;
		unsigned int oc = (opCode&OPCODE_AVR_JMPMASK);
		if (oc != OPCODE_AVR_JMP) // JMP?
		{
			error = SPERROR_NOISDVECTOR;
			return false;
		}
		isdVector = (opCode&0xF101FFFF);
		isdVector = (isdVector>>8)+((isdVector&0x00FF00FF)<<8);
		isdVector = (isdVector&0x000FFFFF)+((isdVector&0xFFF00000)>>3);
	}
	else								// Vectors are RJMP instructions
	{
		strcpy(buffer, &vector[4]);		// Second vector (INT0)
		buffer[4] = 0;
		opCode = (unsigned int)htoi(buffer);
		unsigned int oc = (opCode&OPCODE_AVR_RJMPMASK);
		if (oc != OPCODE_AVR_RJMP)		// RJMP?
		{
			error = SPERROR_NOISDVECTOR;
			return false;
		}
		opCode = (opCode>>8)+((opCode&0xff)<<8);
		opCode &= 0xfff;
		if (opCode > 2047)				// If it's negative
		{
			opCode = ((~opCode)&0xfff)+1;
			isdVector = 0-(int)opCode;
		}
		else
			isdVector = opCode;
		isdVector += 2;					// PC(1)+offset+1
	}

	isdVector *= 2;						// AVR vectors are WORD addresses

	// Next find the ISD signature from the HEX output file	
	found = FALSE;						// Find ISD signature
	char signature[9];
	strcpy(signature, "00000000");
	in.open(file, ios::in|ios::nocreate);
	do
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
		{
			in.close();
			error = SPERROR_NOISD;
			return FALSE;
		}
		buffer[in.gcount()-1] = 0;
		line = buffer;

		if (line.GetAt(0) != ':')
			continue;					// Not a hex line
		line = line.Mid(1);
		bytes = htoi(line.Mid(0, 2));
		address = htoi(line.Mid(2, 4));
		type = htoi(line.Mid(6, 2));
		if (type != 0)					// If not data
			continue;					// Next line

		if (address < isdVector+ISD_SIGNATURE_END &&
			address+bytes >= isdVector+ISD_SIGNATURE_START)
		{								// If ISD Signature data
			line = line.Mid(8);
			index = address;
			do
			{
				if (index >= isdVector+ISD_SIGNATURE_START &&
					index < isdVector+ISD_SIGNATURE_END)
				{
					signature[(index-(isdVector+ISD_SIGNATURE_START))*2] =
						line.GetAt((index-address)*2);
					signature[(index-(isdVector+ISD_SIGNATURE_START))*2+1] =
						line.GetAt((index-address)*2+1);
				}
				index++;
			}
			while (index < address+bytes &&
				index < isdVector+ISD_SIGNATURE_END);
			if (index >= isdVector+ISD_SIGNATURE_END)
				found = TRUE;			// Got the signature data
		}
	}
	while (!found);
	in.close();

	buffer[23] = 0; 
	for (i=0; i<3; i++)
	{
		strcpy(buffer, signature);
		buffer[2+(i*2)] = 0;
		buffer[20+i] = htoi(&buffer[i*2]);
	}
	if (strcmp(&buffer[20], "ISD") != 0) // No signature
	{
		error = SPERROR_NOISD;
		return FALSE;
	}

	// Finally, the address of the "SetBreakHere" routine is
	// the address of the ISD + ISD_SIGNATURE_END bytes
	*setbreakhereAddress = isdVector+ISD_SIGNATURE_END;

	return TRUE;
}


__declspec(dllexport) int GetDebugInfo(const char * project,
	const char * outputDir, FMSourceFile ** files, int * numFiles)
{
	int fileCount = 0;
	int filesInProject = 0;
	char ** filePaths = NULL;
	ifstream in;
	unsigned char buffer[_MAX_PATH];
	int i, j;
	int length;
	int found;
	FMSourceFile * sourceFile;

	// Extract a file list from the project file

	in.open(project, ios::in|ios::binary|ios::nocreate);
	in.read(buffer, 2);					// Get number of files
	filesInProject = (buffer[1]<<8)+(buffer[0]);

	if (filesInProject)
	{
		in.read(buffer, 4);
		in.read(buffer, 2);				// Length "Project File" section name
		length = (buffer[1]<<8)+(buffer[0]);
		in.read(buffer, length);		// Section name
	}

	for (i=0; i<filesInProject; i++)
	{
		in.read(buffer, 1);				// File name length
		length = buffer[0];
		in.read(buffer, length);
		buffer[length] = 0;

		if (!PathFileExists((char *)buffer))
		{
			in.close();
			error = SPERROR_MISSINGSOURCE;
			for (i=0; i<fileCount; i++)
				delete filePaths[i];
			if (fileCount)
				delete filePaths;
			return FALSE;
		}

		found = FALSE;
		for (j=0; j<fileCount; j++)
		{
			if (stricmp(filePaths[j], (char *)buffer) == 0)
				found = TRUE;
		}
		if (!found)
		{
			char ** temp = new char *[fileCount+1];
			for (j=0; j<fileCount; j++)
				temp[j] = filePaths[j];
			if (fileCount)
				delete filePaths;
			filePaths = temp;
			filePaths[fileCount] = new char[strlen((char *)buffer)+1];
			strcpy(filePaths[fileCount], (char *)buffer);
			fileCount++;
		}

		in.read(buffer, 14);
		if (i < filesInProject-1)
			in.read(buffer, 2);
	}

	if (files == NULL)
	{
		in.close();
		*numFiles = fileCount;
		for (i=0; i<fileCount; i++)
			delete filePaths[i];
		if (fileCount)
			delete filePaths;
		return TRUE;
	}

	if (*numFiles < fileCount)
	{
		in.close();
		*numFiles = fileCount;
		for (i=0; i<fileCount; i++)
			delete filePaths[i];
		if (fileCount)
			delete filePaths;
		return FALSE;					// Need more FMSourceFile pointers
	}

	CString projectDir = project;
	projectDir = projectDir.Mid(0, projectDir.ReverseFind('\\')+1);
	for (i=0; i<fileCount; i++)
	{
		sourceFile = new FMSourceFile(projectDir, filePaths[i]);
		sourceFile->SetSourceFileIndex(i);
		files[i] = sourceFile;
	}

	return TRUE;
}


__declspec(dllexport) int DeleteDebugInfo(FMSourceFile ** files, int numFiles)
{
	for (int i=0; i<numFiles; i++)
	{
		if (files[i])
			delete files[i];
		files[i] = NULL;
	}
	return TRUE;
}


__declspec(dllexport) int GetCodeInfo(const char * project,
	const char * outputDir, FMSourceFile ** files, int numFiles,
	int * stackAddress, int registered)
{
	CString projectDir;
	CString projectName;
	CString fileName; 
	CString lstExt;
	CString lstName;
	CString line;
	ifstream in;
	char ** list;
	int numListLines;
	int listLineNum;
	FMSourceFile * entryFile;
	char buffer[1000];
	int i, j;

	projectDir = project;
	projectDir = projectDir.Mid(0, projectDir.ReverseFind('\\')+1);

	projectName = project;
	projectName = projectName.Mid(projectName.ReverseFind('\\')+1);
	projectName = projectName.Mid(0, projectName.ReverseFind('.'));

	in.open(project, ios::in|ios::binary|ios::nocreate);
	// Find the list file's extension
	in.seekg(-13, ios::end);
	j = 0;
	do
	{
		i = in.peek();
		if (i >= ' ')
			in.seekg(-1, ios::cur);		// Back one char
		else
		{
			j++;
			if (j < 2)
				in.seekg(-1, ios::cur);	// Back one char
		}
	}
	while (j < 2);
	in.seekg(1, ios::cur);				// Forward one char
	in.read((unsigned char *)buffer, i);
	buffer[i] = 0;						// List file Extension
	lstExt = ".";
	lstExt += buffer;
	
	in.seekg(-13, ios::end);
	do
	{
		i = in.peek();
		if (i != '\\')
			in.seekg(-1, ios::cur);
	}
	while (i != '\\');
	in.seekg(1, ios::cur);				// Forward one char
	i = 0;
	do									// Get entry file name
	{
		in.read((unsigned char *)&buffer[i], 1);
		if (buffer[i] < ' ')
			buffer[i] = 0;
	}
	while (buffer[i++] != 0);
	in.close();
	lstName = buffer;
	if (lstName.Find('.') != -1)
		lstName = lstName.Mid(0, lstName.Find('.'));
	lstName += lstExt;
	in.close();

	fileName = outputDir;
	fileName += lstName;				// List file
	in.open(fileName, ios::in|ios::nocreate);
	in.seekg(0, ios::beg);
	in.close();

	in.open(fileName, ios::in|ios::nocreate);
	numListLines = 0;
	do									// Get number of list lines
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
			continue;
		numListLines++;
	}
	while (in.gcount() != 0);
	in.close();
	list = new char *[numListLines];
	in.open(fileName, ios::in|ios::nocreate);
	listLineNum = 0;
	do									// Read the list file in
	{
		in.getline(buffer, 1000);
		if (in.gcount() == 0)
			continue;
		buffer[in.gcount()-1] = 0;
		list[listLineNum] = new char[strlen(buffer)+1];
		strcpy(list[listLineNum++], buffer);
	}
	while (in.gcount() != 0);
	in.close();

	entryFile = NULL;					// Find the entry file
	listLineNum = 0;
	do
	{
		line = list[listLineNum];		// Next line

		if (line.Find("AVRASM", 0) != 0) // Not the header
		{
			listLineNum++;
			continue;
		}

										// Found the Header
		line = line.Mid(line.Find(' ')+1); // Removed "AVRASM"
		line = line.Mid(line.Find(' ')+1); // Removed "ver."
		line = line.Mid(line.Find(' ')+1); // Removed version number
		line = line.Mid(0, line.ReverseFind(' ')); // Removed year
		line = line.Mid(0, line.ReverseFind(' ')); // Removed time
		line = line.Mid(0, line.ReverseFind(' ')); // Removed date
		line = line.Mid(0, line.ReverseFind(' ')); // Removed month
		line = line.Mid(0, line.ReverseFind(' ')); // Removed day
		line.TrimLeft();
		line.TrimRight();
		for (i=0; i<numFiles; i++)
		{
			fileName = files[i]->GetAbsolutePath();
			fileName += files[i]->GetFileName();
			if (fileName.CompareNoCase(line) == 0)
			{
				entryFile = files[i];	// The entry file
				listLineNum++;			// Read past two blank lines
				break;
			}
		}
		listLineNum++;
	}
	while (!entryFile && listLineNum < numListLines);
	if (!entryFile)
	{
		error = SPERROR_BADLISTING;
		for (i=0; i<numListLines; i++)
			delete list[i];
		delete list;
		return FALSE;
	}

	error = SPERROR_SUCCESS;			// Got the entry file
	ParseListing(list, numListLines, &listLineNum, entryFile, files,
		numFiles, registered);
	for (i=0; i<numListLines; i++)
		delete list[i];
	delete list;

	if (error != SPERROR_SUCCESS)
		return FALSE;
	return TRUE;
}


__declspec(dllexport) int FindHardBreakpoints(const char * project,
	const char * outputDir, FMSourceFile ** files, int numFiles,
	int setbreakhereAddress)
{
	FMSourceFile * file;
	SourceLine * line;
	unsigned int address;
	unsigned int opCode;
	int i, j;

	for (i=0 ;i<numFiles; i++)
	{
		file = files[i];
		for (j=0; j<file->GetNumLines(); j++)
		{
			line = file->GetLine(j);
			if (line->code && line->numCodeLines)
			{
				opCode = line->codeLines[0]->opCode;
				if ((opCode&OPCODE_AVR_RCALLMASK) == OPCODE_AVR_RCALL) // RCALL ?
				{
					int offset = 0;
					opCode &= (~OPCODE_AVR_RCALLMASK);
					opCode >>= 16;
					if (opCode > 2047)	// If it's negative
					{
						opCode = ((~opCode)&0x0fff)+1;
						offset = 0-(int)opCode;
					}
					else
						offset = opCode;
					offset *= 2;		// AVR addresses are WORDs
					address = line->codeLines[0]->address;
					if (((int)address+offset)+2 == setbreakhereAddress)
						file->SetHardBP(j);
				}
				else if ((opCode&OPCODE_AVR_CALLMASK) == OPCODE_AVR_CALL)
				{						// A CALL instruction

					unsigned int addr = 0;
					addr = (opCode&(~OPCODE_AVR_CALLMASK));
					addr = ((addr&0x01F00000)>>3)|(addr&0x0001FFFF);
					addr *= 2;			// AVR addresses are WORDs
					if (addr == (unsigned int)setbreakhereAddress)
						file->SetHardBP(j);
				}
			}
		}
	}
	return TRUE;
}


__declspec(dllexport) int GetAsmComment(char * comment, int * max)
{
	if (comment && *max >= (int)strlen(ASMCOMMENT)+1)
	{
		strcpy(comment, ASMCOMMENT);
		return TRUE;
	}
	*max = strlen(ASMCOMMENT)+1;
	if (!comment)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) char GetAsmStringChar()
{
	return ASMSTRINGCHAR;
}


__declspec(dllexport) char GetAsmCharChar()
{
	return ASMCHARCHAR;
}


__declspec(dllexport) char GetAsmEscapeChar()
{
	return ASMESCAPECHAR;
}


__declspec(dllexport) char GetAsmDirectiveChar()
{
	return ASMDIRECTIVECHAR;
}


__declspec(dllexport) int GetAsmDirectives(char * directives, int * max)
{
	if (directives && *max >= (int)strlen(ASMDIRECTIVES)+1)
	{
		strcpy(directives, ASMDIRECTIVES);
		return TRUE;
	}
	*max = strlen(ASMDIRECTIVES)+1;
	if (!directives)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) int GetAsmKeywordChars(char * chars, int * max)
{
	if (chars && *max >= (int)strlen(ASMKEYWORDCHARS)+1)
	{
		strcpy(chars, ASMKEYWORDCHARS);
		return TRUE;
	}
	*max = strlen(ASMKEYWORDCHARS)+1;
	if (!chars)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) int GetAsmKeywords(char * keywords, int * max)
{
	if (keywords && *max >= (int)strlen(ASMKEYWORDS)+1)
	{
		strcpy(keywords, ASMKEYWORDS);
		return TRUE;
	}
	*max = strlen(ASMKEYWORDS)+1;
	if (!keywords)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) int GetCComment(char * comment, int * max)
{
	if (comment && *max >= (int)strlen(CCOMMENT)+1)
	{
		strcpy(comment, CCOMMENT);
		return TRUE;
	}
	*max = strlen(CCOMMENT)+1;
	if (!comment)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) int GetCCommentStart(char * start, int * max)
{
	if (start && *max >= (int)strlen(CCOMMENTSTART)+1)
	{
		strcpy(start, CCOMMENTSTART);
		return TRUE;
	}
	*max = strlen(CCOMMENTSTART)+1;
	if (!start)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) int GetCCommentEnd(char * end, int * max)
{
	if (end && *max >= (int)strlen(CCOMMENTEND)+1)
	{
		strcpy(end, CCOMMENTEND);
		return TRUE;
	}
	*max = strlen(CCOMMENTEND)+1;
	if (!end)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) char GetCStringChar()
{
	return CSTRINGCHAR;
}


__declspec(dllexport) char GetCCharChar()
{
	return CCHARCHAR;
}


__declspec(dllexport) char GetCEscapeChar()
{
	return CESCAPECHAR;
}


__declspec(dllexport) char GetCDirectiveChar()
{
	return CDIRECTIVECHAR;
}


__declspec(dllexport) int GetCDirectives(char * directives, int * max)
{
	if (directives && *max >= (int)strlen(CDIRECTIVES)+1)
	{
		strcpy(directives, CDIRECTIVES);
		return TRUE;
	}
	*max = strlen(CDIRECTIVES)+1;
	if (!directives)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) int GetCKeywordChars(char * chars, int * max)
{
	if (chars && *max >= (int)strlen(CKEYWORDCHARS)+1)
	{
		strcpy(chars, CKEYWORDCHARS);
		return TRUE;
	}
	*max = strlen(CKEYWORDCHARS)+1;
	if (!chars)
		return TRUE;
	return FALSE;
}


__declspec(dllexport) int GetCKeywords(char * keywords, int * max)
{
	if (keywords && *max >= (int)strlen(CKEYWORDS)+1)
	{
		strcpy(keywords, CKEYWORDS);
		return TRUE;
	}
	*max = strlen(CKEYWORDS)+1;
	if (!keywords)
		return TRUE;
	return FALSE;
}







__declspec(dllexport) int GetLastIsdError(int * code, char * message,
	int * max)
{
	if (message && *max >= (int)strlen(errorMessage)+1)
	{
		*code = error;
		strcpy(message, errorMessage);
		return TRUE;
	}
	*code = error;						// Might only want code
	*max = strlen(errorMessage)+1;
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// Internal functions

void ParseListing(char ** list, int numListLines, int * listLineNum,
	FMSourceFile * file, FMSourceFile ** files, int numFiles, BOOL registered)
{
	char buffer[1000];
	CString sourceLine;
	CString listLine;
	CString fileName;
	int lineNum = 0;
	int address;
	unsigned int opCode;
	BOOL found;
	int i;

	do
	{
		sourceLine = file->GetLine(lineNum)->source;
		sourceLine.TrimLeft();
		sourceLine.TrimRight();

		if (sourceLine.GetLength() == 0) // Don't bother with empty lines
		{
			lineNum++;
			continue;
		}

		found = FALSE;
		for (i=*listLineNum; i<numListLines; i++) // Find line in listing
		{
			listLine = list[i];
			if (listLine.Find(sourceLine) != -1) // Got it!!
			{
				*listLineNum = i;
				found = TRUE;
				break;
			}
		}
		if (!found)						// Couldn't find the line
		{
			lineNum++;					// Skip it!!
			continue;
		}

										// Got the list line & the source line
		if (sourceLine.Find(".include", 0) == 0) // An "include" line
		{
			sourceLine = sourceLine.Mid(8);
			sourceLine.TrimLeft();
			sourceLine = sourceLine.Mid(sourceLine.Find('\"')+1);
			if (sourceLine.Find('\"') != -1)
				sourceLine = sourceLine.Mid(0, sourceLine.Find('\"'));
			if (sourceLine.Find(':') == -1 &&
				sourceLine.Find("\\\\", 0) != 0) // A relative path
			{
				if (sourceLine.Find('\\') == -1) // In same directory
				{
					fileName = file->GetAbsolutePath();
					sourceLine = fileName+sourceLine;
				}
				else
				{
					fileName = file->GetAbsolutePath();
					fileName += sourceLine;
					_fullpath(buffer, fileName, 1000);
					sourceLine = buffer;
				}
			}
			FMSourceFile * nextFile = NULL;
			for (i=0; i<numFiles; i++)
			{
				fileName = files[i]->GetAbsolutePath();
				fileName += files[i]->GetFileName();
				if (sourceLine.CompareNoCase(fileName) == 0)
				{
					nextFile = files[i];
					break;
				}
			}
			*listLineNum += 1;
			if (nextFile != NULL)
			{
				ParseListing(list, numListLines, listLineNum, nextFile,
					files, numFiles, registered);
				if (error != SPERROR_SUCCESS) // If an error
					return;				// Bomb out!!
			}
			lineNum++;
			continue;
		}
		
		if (sourceLine.Find(".exit", 0) == 0) // Exit this file
		{
			*listLineNum += 1;			// Next list line
			return;						// Return to previous file
		}

		if (listLine.GetAt(7) == ' ')	// Not a code line
		{
			lineNum++;					// Not interested
			*listLineNum += 1;
			continue;
		}

										// A code line
		address = htoi(listLine.Mid(0, 6)); // Address
		address *= 2;					// AVR addresses are WORDs

		if (!registered && address >= 2048) // 2K code limit reached
		{
			error = SPERROR_2KLIMIT;
			return;
		}

		if (lineNum == 164)
		{
			int letMeSee = lineNum;
		}

		file->SetLineStartAddress(lineNum, address); // Set line's code address

		file->GetLine(lineNum)->numCodeLines = 1;
		file->GetLine(lineNum)->codeLines = new CodeLine *[1];
		file->GetLine(lineNum)->codeLines[0] = new CodeLine;
		file->GetLine(lineNum)->codeLines[0]->address = address;
		file->GetLine(lineNum)->codeLines[0]->sourceLength =
			sourceLine.GetLength()+1;
		file->GetLine(lineNum)->codeLines[0]->source =
			new char[sourceLine.GetLength()+1];
		strcpy(file->GetLine(lineNum)->codeLines[0]->source, sourceLine);

		opCode = (unsigned int)htoi(listLine.Mid(7, 4)); // OpCode
		opCode <<= 16;
		if (listLine.Find(12) != ' ')	// 2 Word opcode
		{
			i = htoi(listLine.Mid(12, 4)); // Second opCode word
			opCode |= (unsigned int)i;
		}

		file->GetLine(lineNum)->codeLines[0]->opCode = opCode;
		
		lineNum++;						// Next!!
		*listLineNum += 1;
		continue;
	}
	while (lineNum < file->GetNumLines() && *listLineNum < numListLines);
}


int ahtoi(const char * ascii)
{
	if (ascii[0] == '$' || ascii[1] == 'x') // If a hex string
		return htoi(ascii);
	return atoi(ascii);
}


int htoi(const char * hex)
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


