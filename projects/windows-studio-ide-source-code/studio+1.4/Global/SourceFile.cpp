// SourceFile.cpp

#define SOURCEFILE

#include "stdafx.h"						// MFC
#include "fstream.h"
#include "Shlwapi.h"					// Path functions

#include "SourceFile.h"
#include "Function.h"
#include "ScopeBlock.h"
#include "Symbol.h"
#include "Global.h"


/////////////////////////////////////////////////////////////////////////////
// FMSourceFile functions


FMSourceFile::FMSourceFile(const char * projectPath, const char * file)
{
	char buffer[1000];
	ifstream in;
	CString tabbedLine;
	int tabChars = 4;
	int i, j;
	int extraSpaces;

	CString str = file;
	str = str.Mid(0, str.ReverseFind('\\')+1);
	absolutePath = new char[str.GetLength()+1];
	strcpy(absolutePath, str);

	str = file;
	str = str.Mid(str.ReverseFind('\\')+1);
	fileName = new char[str.GetLength()+1];
	strcpy(fileName, str);

	PathRelativePathTo(buffer, projectPath, FILE_ATTRIBUTE_DIRECTORY,
		absolutePath, FILE_ATTRIBUTE_DIRECTORY);
	if (buffer[strlen(buffer)-1] != '\\')
		strcat(buffer, "\\");
	relativePath = new char[strlen(buffer)+1];
	strcpy(relativePath, buffer);

	open = FALSE;
	wasOpen = FALSE;

	hWnd = NULL;						// Initialise source window attributes
	lines = NULL;
	longestLineLength = 0;
	maximized = FALSE;
	minimized = FALSE;
	numFunctions = 0;
	numHardBPs = 0;
	numLines = 0;
	numSoftBPs = 0;
	numSymbols = 0;
	functions = NULL;
	hardBPs = NULL;
	softBPs = NULL;
	sourceFileIndex = -1;
	symbols = NULL;
	treeItem = 0;
	windowRect.left = 200;				// Default window size
	windowRect.right = 700;
	windowRect.top = 150;
	windowRect.bottom = 550;

//	minWidth = 20;
//	maxWidth = 2000;

	in.open(file, ios::in|ios::nocreate);
	in.seekg(0, ios::beg);
	in.close();

	in.open(file, ios::in|ios::nocreate);
	numLines = 0;
	do
	{
		in.getline(buffer, 1000);
		if (in.gcount() != 0)
			numLines++;
	}
	while (in.gcount() != 0);
	in.close();
	lines = new SourceLine *[numLines];
	in.open(file, ios::in|ios::nocreate);
	longestLineLength = 0;

	for (i=0; i<numLines; i++)
	{
		// Read line from file
		in.getline(buffer, 10000);
		if (buffer[in.gcount()-1] == 13)
			buffer[in.gcount()-1] = 0;
		else
			buffer[in.gcount()] = 0;

		// Get new SourceLine structure
		lines[i] = new SourceLine;
		lines[i]->length = 0;			// Initialise it as empty
		lines[i]->source = NULL;
		lines[i]->line = NULL;
		lines[i]->startAddress = 0;
		lines[i]->endAddress = 0;
		lines[i]->code = FALSE;
		lines[i]->numSegments = 0;		// Not formatted
		lines[i]->segments = NULL;
		lines[i]->numTabs = 0;
		lines[i]->tabs = NULL;
		lines[i]->numCodeLines = 0;
		lines[i]->codeLines = NULL;

		if (!strlen(buffer))			// Ignore empty lines
			continue;

		lines[i]->source = new char[strlen(buffer)+1];
		strcpy(lines[i]->source, buffer); // Save the original source

		// Find all the tab chars
		for (j=0; j<(int)strlen(buffer); j++)
		{
			if (buffer[j] == '\t')
				lines[i]->numTabs = lines[i]->numTabs+1;
		}
		if (lines[i]->numTabs)
		{
			lines[i]->tabs = new int[lines[i]->numTabs];
			int t = 0;
			for (j=0; j<(int)strlen(buffer); j++)
			{
				if (buffer[j] == '\t')
				{
					lines[i]->tabs[t] = j-t; // Tab position
					t++;
				}
			}
		}

		// Insert the tabs
		tabbedLine = "";
		int bpos = 0;
		while (bpos < (int)strlen(buffer))
		{
			if (buffer[bpos] == '\t')	// If a tab
			{
				extraSpaces = (tabbedLine.GetLength()/tabChars)+1;
				extraSpaces *= tabChars;
				extraSpaces -= tabbedLine.GetLength();
				for (j=0; j<extraSpaces; j++)
					tabbedLine += ' ';
				bpos++;
			}
			else
				tabbedLine += buffer[bpos++];
		}
		// Set line length
		lines[i]->length = tabbedLine.GetLength();
		if (longestLineLength < lines[i]->length)
			longestLineLength = lines[i]->length;
		lines[i]->line = new char[tabbedLine.GetLength()+1];

		// Copy line to structure
		strcpy(lines[i]->line, tabbedLine);
	}
	in.close();
}


FMSourceFile::~FMSourceFile()
{
	int i, j;

	if (absolutePath)
		delete [] absolutePath;
	absolutePath = NULL;
	if (fileName)
		delete [] fileName;
	fileName = NULL;
	if (relativePath)
		delete [] relativePath;
	relativePath = NULL;

	if (numSymbols && symbols != NULL)
	{
		for (i=0; i<numSymbols; i++)
		{
			if (symbols[i])
				delete symbols[i];
		}
		delete [] symbols;
	}
	if (numFunctions && functions != NULL)
	{
		for (i=0; i<numFunctions; i++)
		{
			if (functions[i])
				delete functions[i];
		}
		delete [] functions;
	}
	if (numLines && lines != NULL)
	{
		for (i=0; i<numLines; i++)
		{
			if (lines[i])
			{
				if (lines[i]->source)
					delete [] lines[i]->source;
				if (lines[i]->line)
					delete [] lines[i]->line;
				if (lines[i]->numTabs)
					delete [] lines[i]->tabs;
//				if (lines[i]->numSegments) // Allocated in app
//					delete [] lines[i]->segments;
				if (lines[i]->numCodeLines && lines[i]->codeLines != NULL)
				{
					for (j=0; j<lines[i]->numCodeLines; j++)
					{
						if (lines[i]->codeLines[j])
						{
							if (lines[i]->codeLines[j]->source)
								delete [] lines[i]->codeLines[j]->source;
						}
						delete lines[i]->codeLines[j];
					}
					delete [] lines[i]->codeLines;
				}
				delete lines[i];
			}
		}
		delete [] lines;
	}
//	if (numSoftBPs && softBPs != NULL)	// Allocated in app
//		delete [] softBPs;
	if (numHardBPs && hardBPs != NULL)
		delete [] hardBPs;
}


int FMSourceFile::AddFunction(FMFunction * function)
{
	int i;

	if (numFunctions == 0)
	{
		functions = new FMFunction *[1];
		functions[numFunctions++] = function;
		return TRUE;
	}
	for (i=0; i<numFunctions; i++)
	{
		if (stricmp(functions[i]->GetName(), function->GetName()) == 0)
			return FALSE;
	}
	FMFunction ** temp = new FMFunction *[numFunctions+1];
	for (i=0; i<numFunctions; i++)
		temp[i] = functions[i];
	delete functions;
	functions = temp;
	functions[numFunctions++] = function;
	return TRUE;
}


int FMSourceFile::AddSymbol(FMSymbol * symbol)
{
	int i = 0;

	if (numSymbols == 0)
	{
		symbols = new FMSymbol *[1];
		symbols[numSymbols++] = symbol;
		return TRUE;
	}
	for (i=0; i<numSymbols; i++)
	{
		if (stricmp(symbols[i]->GetName(), symbol->GetName()) == 0 &&
			stricmp(symbols[i]->GetDataType(), symbol->GetDataType()) == 0 &&
			symbols[i]->GetAddress() == symbol->GetAddress() &&
			symbols[i]->GetType() == symbol->GetType())
			return FALSE;
	}
	FMSymbol ** temp = new FMSymbol *[numSymbols+1];
	for (i=0; i<numSymbols; i++)
		temp[i] = symbols[i];
	if (numSymbols)
		delete symbols;
	symbols = temp;
	symbols[numSymbols++] = symbol;
	return TRUE;
}


void FMSourceFile::ClearAllBPs()
{
	if (numSoftBPs)
		delete softBPs;
	softBPs = NULL;
	numSoftBPs = 0;
//	if (open)
//		UpdateWindow();
}


void FMSourceFile::ClearSoftBP(int line)
{
	int i;
	bool found;
	int ln;

	if (!numSoftBPs)
		return;
	for (i=0; i<numSoftBPs; i++)
	{
		if (softBPs[i].line == line)
		{
			ln = i;
			found = true;
		}
	}
	if (!found)
		return;

	Breakpoint * temp = new Breakpoint[numSoftBPs-1];
	for (i=0; i<ln; i++)
	{
		temp[i].line = softBPs[i].line;
		temp[i].address = softBPs[i].address;
		temp[i].type = softBPs[i].type;
	}
	for (i=ln+1; i<numSoftBPs; i++)
	{
		temp[i-1].line = softBPs[i].line;
		temp[i-1].address = softBPs[i].address;
		temp[i-1].type = softBPs[i].type;
	}
	if (numSoftBPs)
		delete softBPs;
	softBPs = temp;
	numSoftBPs--;
}


int FMSourceFile::CodeAtAddress(int address)
{
	SourceLine * line;
	for (int i=0; i<numLines; i++)
	{
		line = lines[i];
		if (line->code && line->startAddress == address)
			return TRUE;
	}
	return FALSE;
}


int FMSourceFile::FindFunction(FMFunction * function, int blockLine)
{
	int i;
	CString line;

	if (blockLine >= numLines)			// Shouldn't happen unless file is duff
		return -1;

	i = blockLine;
	do
	{
		line = lines[i]->line;
		if (line.Find(function->GetName()) != -1)
			return i;
		i--;
	}
	while (i != 0);
	return -1;
}


int FMSourceFile::FindPCLine(int pc)
{
	int line = -1;
	for (int i=0; i<numLines; i++)		// For each line
	{
		if (lines[i]->code &&			// If a code line
			pc >= lines[i]->startAddress &&
			pc <= lines[i]->endAddress)	// If PC is on this line
		{
			line = i;					// Found the line
		}
	}
	return line;
}


int FMSourceFile::FindSymbol(FMSymbol * symbol)
{
	int startLine = 0;

#ifdef SRCFDEBUG
debug<<"FMSourceFile::FindSymbol():"<<endl;
#endif

	CString line;
	CString symbolDec = symbol->DataTypeString();
	if (symbol->IsArray())
		symbolDec = symbolDec.Mid(0, symbolDec.Find('['));
	if (symbolDec.Find("unsigned") == 0) // If an unsigned type
		symbolDec = symbolDec.Mid(symbolDec.Find("unsigned")+9);
										// Just search on the type
	symbolDec += " ";
//	symbolDec += symbol->GetName();

#ifdef SRCFDEBUG
debug<<"   Search string: "<<symbolDec<<endl;
#endif

	if (symbol->GetScope() != NULL)
		startLine = symbol->GetScope()->GetFirstLine();
	for (int i=startLine; i<numLines; i++)
	{
		line = lines[i]->line;
		if (line.Find(symbolDec) != -1)
		{

#ifdef SRCFDEBUG
debug<<"      Found type in line "<<i<<endl;
#endif

			if (line.Find(symbolDec) < line.Find(symbol->GetName()))
			{							// Type is before symbol name
				if (line.Find('=') != -1)
				{
					if (line.Find('=') < line.Find(symbol->GetName()))
						continue;		// Try again
				}
				return i;
			}
		}
	}
	return -1;
}


void FMSourceFile::FormatAsmSourceText(CString comment,
	char stringChar, char charChar, char escapeChar, char directiveChar,
	CString directives, CString keywordChars, CString keywords)
{
	BOOL inString = FALSE;
	BOOL inChar = FALSE;
	LineSegment * segment;
	SourceLine * sourceLine;
	CString line;
	CString string;
	int s, e;
	int i, j;

	for (i=0; i<numLines; i++)
	{
		sourceLine = GetLine(i);

		if (sourceLine->numSegments)
			delete sourceLine->segments;
		sourceLine->numSegments = 0;

		if (!sourceLine->length)		// If line has no characters
			continue;					// No segments, done!!

		line = sourceLine->line;		// Get line

		e = 0;							// Start at the beginning
		segment = NewSegment(i, 0);		// Create a new Segment
		do
		{
			s = e;						// Start of this segment

/*			if (inComment)				// First check for comment
			{
				segment->colour = COMMENTCOLOUR;
				if (line.Find(commentEnd, s) != -1) // Comment ends
				{
					e = line.Find(commentEnd)+commentEnd.GetLength();
					segment->length = e-segment->start;
					if (e < lines[lineNum]->length)
						segment = NewSegment(lineNum, e);
					inComment = FALSE;
				}
				else
				{
					e = lines[lineNum]->length;
					segment->length = e-segment->start;
				}
				continue;
			}*/

			if (inString)				// Next check for strings
			{
				segment->colour = STRINGCOLOUR;
				if (line.Find(stringChar, s) != -1 &&
					(line.Find(stringChar, s) == 0 ||
					line[line.Find(stringChar, s)-1] != escapeChar))
										// Found string end
				{
					e = line.Find(stringChar, s)+1;
					segment->length = e-segment->start;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					inString = FALSE;
				}
				else
				{
					e = lines[i]->length;
					segment->length = e-segment->start;
				}
				continue;
			}

			if (inChar)					// Next check for chars
			{
				segment->colour = CHARCOLOUR;
				if (line.Find(charChar, s) != -1 &&
					(line.Find(charChar, s) == 0 ||
					line[line.Find(charChar, s)-1] != escapeChar))
										// Found char end
				{
					e = line.Find(charChar, s)+1;
					segment->length = e-segment->start;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					inChar = FALSE;
				}
				else
				{
					e = lines[i]->length;
					segment->length = e-segment->start;
				}
				continue;
			}

										// Next check for other hilighting
			if (line.Find(comment, s) == s) // Single comment
			{
				if (segment->start != s) // If a text gap
				{
					segment->length = s-segment->start;
					segment = NewSegment(i, s);
				}
				e = lines[i]->length;
				segment->length = e-s;
				segment->colour = COMMENTCOLOUR;
				continue;
			}

/*			if (line.Find(commentStart, s) == s) // Comment Start
			{
				if (segment->start != s) // If a text gap
				{
					segment->length = s-segment->start;
					segment = NewSegment(lineNum, s);
				}
				e = s+commentStart.GetLength();
				inComment = TRUE;
				continue;
			}*/

			if (line.Find(stringChar, s) == s) // String Start
			{
				if (segment->start != s) // If a text gap
				{
					segment->length = s-segment->start;
					segment = NewSegment(i, s);
				}
				e = s+1;
				inString = TRUE;
				continue;
			}

			if (line.Find(charChar, s) == s) // Char Start
			{
				if (segment->start != s) // If a text gap
				{
					segment->length = s-segment->start;
					segment = NewSegment(i, s);
				}
				e = s+1;
				inChar = TRUE;
				continue;
			}

			if (line.Find(directiveChar, s) == s)
			{							// Start of Directive
				j = s+1;
				while (j < line.GetLength() &&
					keywordChars.Find(line[j]) != -1)
				{
					j++;				// Find the end of the directive
				}
				string = line.Mid(s, j-s); // Word to test
				string.MakeLower();
				if (directives.Find(string) != -1)
				{						// If it's a directive
					if (segment->start != s) // If a text gap
					{
						segment->length = s-segment->start;
						segment = NewSegment(i, s);
					}
					e = s+string.GetLength();
					segment->length = e-s;
					segment->colour = DIRECTIVECOLOUR;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					continue;
				}
				e = s+string.GetLength();
				segment->length = e-segment->start;
			}

			if (keywordChars.Find(line[s]) != -1) // Might be a Keyword
			{
				j = s;
				while (j < line.GetLength() &&
					keywordChars.Find(line[j]) != -1)
				{
					j++;
				}
				string = ",";
				string += line.Mid(s, j-s); // Word to test
				string += ",";
				string.MakeLower();
				if (keywords.Find(string) != -1) // Maybe a keyword
				{
					if (segment->start != s) // If a text gap
					{
						segment->length = s-segment->start;
						segment = NewSegment(i, s);
					}
					e = s+(string.GetLength()-2);
					segment->length = e-s;
					segment->colour = KEYWORDCOLOUR;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					continue;
				}
				e = s+(string.GetLength()-2);
				segment->length = e-segment->start;
				continue;
			}

			e++;
			if (e >= lines[i]->length)
				segment->length = lines[i]->length-segment->start;
		}
		while (e < lines[i]->length);
	}
}


void FMSourceFile::FormatCSourceText(CString comment, CString commentStart,
	CString commentEnd, char stringChar, char charChar, char escapeChar,
	char directiveChar, CString directives, CString keywordChars,
	CString keywords)
{
	BOOL inComment = FALSE;
	BOOL inString = FALSE;
	BOOL inChar = FALSE;
	LineSegment * segment;
	SourceLine * sourceLine;
	CString line;
	CString string;
	int s, e;
	int i, j;

	for (i=0; i<numLines; i++)
	{
		sourceLine = GetLine(i);

		if (sourceLine->numSegments)
			delete sourceLine->segments;
		sourceLine->numSegments = 0;

		if (!sourceLine->length)		// If line has no characters
			continue;					// No segments, done!!

		line = sourceLine->line;		// Get line

		e = 0;							// Start at the beginning
		segment = NewSegment(i, 0); // Create a new Segment
		do
		{
			s = e;						// Start of this segment

			if (inComment)				// First check for comment
			{
				segment->colour = COMMENTCOLOUR;
				if (line.Find(commentEnd, s) != -1) // Comment ends
				{
					e = line.Find(commentEnd)+commentEnd.GetLength();
					segment->length = e-segment->start;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					inComment = FALSE;
				}
				else
				{
					e = lines[i]->length;
					segment->length = e-segment->start;
				}
				continue;
			}

			if (inString)				// Next check for strings
			{
				segment->colour = STRINGCOLOUR;
				if (line.Find(stringChar, s) != -1 &&
					(line.Find(stringChar, s) == 0 ||
					line[line.Find(stringChar, s)-1] != escapeChar))
										// Found string end
				{
					e = line.Find(stringChar, s)+1;
					segment->length = e-segment->start;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					inString = FALSE;
				}
				else
				{
					e = lines[i]->length;
					segment->length = e-segment->start;
				}
				continue;
			}

			if (inChar)					// Next check for chars
			{
				segment->colour = CHARCOLOUR;
				if (line.Find(charChar, s) != -1 &&
					(line.Find(charChar, s) == 0 ||
					line[line.Find(charChar, s)-1] != escapeChar))
										// Found char end
				{
					e = line.Find(charChar, s)+1;
					segment->length = e-segment->start;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					inChar = FALSE;
				}
				else
				{
					e = lines[i]->length;
					segment->length = e-segment->start;
				}
				continue;
			}

										// Next check for other hilighting
			if (line.Find(comment, s) == s) // Single comment
			{
				if (segment->start != s) // If a text gap
				{
					segment->length = s-segment->start;
					segment = NewSegment(i, s);
				}
				e = lines[i]->length;
				segment->length = e-s;
				segment->colour = COMMENTCOLOUR;
				continue;
			}

			if (line.Find(commentStart, s) == s) // Comment Start
			{
				if (segment->start != s) // If a text gap
				{
					segment->length = s-segment->start;
					segment = NewSegment(i, s);
				}
				e = s+commentStart.GetLength();
				inComment = TRUE;
				continue;
			}

			if (line.Find(stringChar, s) == s) // String Start
			{
				if (segment->start != s) // If a text gap
				{
					segment->length = s-segment->start;
					segment = NewSegment(i, s);
				}
				e = s+1;
				inString = TRUE;
				continue;
			}

			if (line.Find(charChar, s) == s) // Char Start
			{
				if (segment->start != s) // If a text gap
				{
					segment->length = s-segment->start;
					segment = NewSegment(i, s);
				}
				e = s+1;
				inChar = TRUE;
				continue;
			}

			if (line.Find(directiveChar, s) == s)
			{							// Start of Directive
				j = s+1;
				while (j < line.GetLength() &&
					keywordChars.Find(line[j]) != -1)
				{
					j++;				// Find the end of the directive
				}
				string = line.Mid(s, j-s); // Word to test
				string.MakeLower();
				if (directives.Find(string) != -1)
				{						// If it's a directive
					if (segment->start != s) // If a text gap
					{
						segment->length = s-segment->start;
						segment = NewSegment(i, s);
					}
					e = s+string.GetLength();
					segment->length = e-s;
					segment->colour = DIRECTIVECOLOUR;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					continue;
				}
				e = s+string.GetLength();
				segment->length = e-segment->start;
			}

			if (keywordChars.Find(line[s]) != -1) // Might be a Keyword
			{
				j = s;
				while (j < line.GetLength() &&
					keywordChars.Find(line[j]) != -1)
				{
					j++;
				}
				string = ",";
				string += line.Mid(s, j-s); // Word to test
				string += ",";
				string.MakeLower();
				if (keywords.Find(string) != -1) // Maybe a keyword
				{
					if (segment->start != s) // If a text gap
					{
						segment->length = s-segment->start;
						segment = NewSegment(i, s);
					}
					e = s+(string.GetLength()-2);
					segment->length = e-s;
					segment->colour = KEYWORDCOLOUR;
					if (e < lines[i]->length)
						segment = NewSegment(i, e);
					continue;
				}
				e = s+(string.GetLength()-2);
				segment->length = e-segment->start;
				continue;
			}

			e++;
			if (e >= lines[i]->length)
				segment->length = lines[i]->length-segment->start;
		}
		while (e < lines[i]->length);
	}
}


char * FMSourceFile::GetAbsolutePath()
{
	return absolutePath;
}


int FMSourceFile::GetContextMenuId(int ti, int * menu)
{
	int i;

	if (treeItem == ti)
	{
		*menu = CONTEXTMENUID_SOURCEFILE;
		return TRUE;
	}

	for (i=0; i<numFunctions; i++)
	{
		if (functions[i]->GetContextMenuId(ti, menu))
			return TRUE;
	}
	for (i=0; i<numSymbols; i++)
	{
		if (symbols[i]->GetContextMenuId(ti, menu))
			return TRUE;
	}
	return FALSE;
}


char * FMSourceFile::GetFileName()
{
	return fileName;
}


FMFunction * FMSourceFile::GetFunction(int index)
{
	if (index < numFunctions)
		return functions[index];
	return NULL;
}


FMFunction * FMSourceFile::GetFunction(const char * function)
{
	for (int i=0; i<numFunctions; i++)
	{
		if (stricmp(functions[i]->GetName(), function) == 0)
			return functions[i];
	}
	return NULL;
}


FMSymbol * FMSourceFile::GetFunctionSymbol(const char * functionName,
	const char * symbolName)
{
	FMFunction * function = GetFunction(functionName);
	if (function != NULL)
		return function->GetSymbol(symbolName);
	return NULL;
}


FMSymbol * FMSourceFile::GetGlobalSymbol(const char * symbolName)
{
	for (int i=0; i<numSymbols; i++)
	{
		if (strcmp(symbols[i]->GetName(), symbolName) == 0)
			return symbols[i];
	}
	return NULL;
}


Breakpoint * FMSourceFile::GetHardBP(int bp)
{
	if (bp >= 0 && bp < numHardBPs)
		return &hardBPs[bp];
	return NULL;
}


SourceLine * FMSourceFile::GetLine(int index)
{
	if (index >= 0 && index < numLines)
		return lines[index];
	return NULL;
}


int FMSourceFile::GetLineBreakpoint(int line)
{
	int i;

	for (i=0; i<numSoftBPs; i++)
		if (softBPs[i].line == line)
			return SOFTBREAKPOINT;
	for (i=0; i<numHardBPs; i++)
		if (hardBPs[i].line == line)
			return HARDBREAKPOINT;
	return NOBREAKPOINT;
}


int FMSourceFile::GetNumHardBPs()
{
	return numHardBPs;
}


int FMSourceFile::GetNumFunctions()
{
	return numFunctions;
}


int FMSourceFile::GetNumLines()
{
	return numLines;
}


int FMSourceFile::GetNumSymbols()
{
	return numSymbols;
}


char * FMSourceFile::GetRelativePath()
{
	return relativePath;
}


int FMSourceFile::GetSourceFileIndex()
{
	return sourceFileIndex;
}


FMSymbol * FMSourceFile::GetSymbol(int index)
{
	if (index > -1 && index < numSymbols)
		return symbols[index];
	return NULL;
}


FMSymbol * FMSourceFile::GetSymbolFromTreeItem(int ti)
{
	FMSymbol * symbol;
	int i;

	if (treeItem == ti)
		return NULL;
	for (i=0; i<numFunctions; i++)
	{
		symbol = functions[i]->GetSymbolFromTreeItem(ti);
		if (symbol != NULL)
			return symbol;
	}
	for (i=0; i<numSymbols; i++)
	{
		if (symbols[i]->GetTreeItem() == ti)
			return symbols[i];
	}
	return NULL;
}


int FMSourceFile::GetTreeItem()
{
	return treeItem;
}


HWND FMSourceFile::GetWindowHandle()
{
	return hWnd;
}


void FMSourceFile::GetWindowRect(RECT * rect)
{
	rect->left = windowRect.left;
	rect->top = windowRect.top;
	rect->right = windowRect.right;
	rect->bottom = windowRect.bottom;
}


int FMSourceFile::IsC()
{
	CString ext = fileName;
	if (ext.Find('.') != -1)
	{
		ext = ext.Mid(ext.ReverseFind('.')+1);
		ext.MakeLower();
		if (ext.Compare("c") == 0 || ext.Compare("h") == 0)
			return TRUE;
	}
	return FALSE;
}


int FMSourceFile::IsOpen()
{
	return open;
}


int FMSourceFile::IsWindowMaximized()
{
	return maximized;
}


int FMSourceFile::IsWindowMinimized()
{
	return minimized;
}


int FMSourceFile::SetHardBP(int line)
{
	int i;

	for (i=0; i<numHardBPs; i++)
		if (hardBPs[i].line == line)
			return FALSE;
	for (i=0; i<numSoftBPs; i++)
		if (softBPs[i].line == line)
			return FALSE;
	Breakpoint * temp = new Breakpoint[numHardBPs+1];
	for (i=0; i<numHardBPs; i++)
	{
		temp[i].line = hardBPs[i].line;
		temp[i].address = hardBPs[i].address;
		temp[i].type = hardBPs[i].type;
	}
	if (numHardBPs)
		delete hardBPs;
	hardBPs = temp;
	hardBPs[numHardBPs].line = line;
	hardBPs[numHardBPs].address = lines[line]->startAddress;
	hardBPs[numHardBPs].type = HARDBREAKPOINT;
	numHardBPs++;

#ifdef SRCFDEBUG
debug<<"FMSourceFile::SetHardBP():"<<endl;
debug<<"   Hard BP in file "<<GetName()<<endl;
debug<<"   On line "<<line<<endl;
debug<<"   It's address is "<<hardBPs[numHardBPs-1].address<<endl;
#endif

	return TRUE;
}


void FMSourceFile::SetLineEndAddress(int line, int address)
{
	if (line < numLines)
	{
		lines[line]->endAddress = address;
//		if (lines[line]->startAddress >= address)
//			lines[line]->startAddress = address;
		lines[line]->code = true;
	}
}


void FMSourceFile::SetLineEndAddress(int line, const char * address)
{
	CString ascii = "0123456789ABCDEF";
	int addr = 0;
	int i = 0;
	do
	{
		addr <<= 4;
		addr |= ascii.Find(address[i++]);
	}
	while (i < (int)strlen(address));
	SetLineEndAddress(line, addr);
}


void FMSourceFile::SetLineStartAddress(int line, int address)
{
	if (line < numLines)
	{
		lines[line]->startAddress = address;
		if (lines[line]->endAddress <= address)
			lines[line]->endAddress = address;
		lines[line]->code = true;
	}
}


void FMSourceFile::SetLineStartAddress(int line, const char * address)
{
	CString ascii = "0123456789ABCDEF";
	int addr = 0;
	int i = 0;
	do
	{
		addr <<= 4;
		addr |= ascii.Find(address[i++]);
	}
	while (i < (int)strlen(address));
	SetLineStartAddress(line, addr);
}


void FMSourceFile::SetMaximized(int max)
{
	maximized = max;
}


void FMSourceFile::SetMinimized(int min)
{
	minimized = min;
}


int FMSourceFile::SetSoftBP(int l)
{
	int i;

	for (i=0; i<numSoftBPs; i++)
		if (softBPs[i].line == l)
			return FALSE;
	for (i=0; i<numHardBPs; i++)
		if (hardBPs[i].line == l)
			return FALSE;
	Breakpoint * temp = new Breakpoint[numSoftBPs+1];
	for (i=0; i<numSoftBPs; i++)
	{
		temp[i].line = softBPs[i].line;
		temp[i].address = softBPs[i].address;
		temp[i].type = softBPs[i].type;
	}
	if (numSoftBPs)
		delete softBPs;
	softBPs = temp;
	softBPs[numSoftBPs].line = l;
	softBPs[numSoftBPs].address = lines[l]->startAddress;
	softBPs[numSoftBPs].type = SOFTBREAKPOINT;
	numSoftBPs++;
	return TRUE;
}


void FMSourceFile::SetSourceFileIndex(int index)
{
	sourceFileIndex = index;
}


void FMSourceFile::SetTreeItem(int ti)
{
	treeItem = ti;
}


void FMSourceFile::SetWindowHandle(HWND windowHandle)
{
	hWnd = windowHandle;
}


void FMSourceFile::SetWindowRect(RECT * rect)
{
	windowRect.left = rect->left;
	windowRect.top = rect->top;
	windowRect.right = rect->right;
	windowRect.bottom = rect->bottom;
}


void FMSourceFile::TabLine(int lineNum)
{

}


/////////////////////////////////////////////////////////////////////////////
// FMSourceFile internal functions


LineSegment * FMSourceFile::NewSegment(int lineNum, int start)
{
	int i;
	LineSegment * temp;

	temp = new LineSegment[lines[lineNum]->numSegments+1];
	if (lines[lineNum]->numSegments)
	{
		for (i=0; i<lines[lineNum]->numSegments; i++)
		{
			temp[i].start = lines[lineNum]->segments[i].start;
			temp[i].length = lines[lineNum]->segments[i].length;
			temp[i].colour = lines[lineNum]->segments[i].colour;
		}
		delete lines[lineNum]->segments;
	}
	lines[lineNum]->segments = temp;

	lines[lineNum]->segments[lines[lineNum]->numSegments].start = start;
	lines[lineNum]->segments[lines[lineNum]->numSegments].length = 1;
	lines[lineNum]->segments[lines[lineNum]->numSegments].colour = TEXTCOLOUR;

	return &(lines[lineNum]->segments[lines[lineNum]->numSegments++]);
}



//void FMSourceFile::DrawSource()
//{
//	HDC hDC = GetDC(window);
//	Paint(hDC);
//	ReleaseDC(window, hDC);
//}


/*int FMSourceFile::UpdateWindow()
{
	if (!open)
		return false;					// Not open!!

	if (globals.pcTracking && pcFile == this) // If PC tracking enabled
	{
										// Set the scrollers
		hScrollPos = 0;					// Start of line
		hScrollInfo.fMask = SIF_POS;
		hScrollInfo.nPos = hScrollPos;
		::SetScrollInfo(hScroll, SB_CTL, &hScrollInfo, TRUE);

		vScrollPos = pcLine-1; //(vScrollPage/2);
		if (vScrollPos < 0)
			vScrollPos = 0;
		if (vScrollPos > vScrollMax-(vScrollPage-1))
			vScrollPos = vScrollMax-(vScrollPage-1);
		vScrollInfo.fMask = SIF_POS;
		vScrollInfo.nPos = vScrollPos;
		::SetScrollInfo(vScroll, SB_CTL, &vScrollInfo, TRUE);
	}

	::InvalidateRect(window, NULL, FALSE);
	::UpdateWindow(window);

	return TRUE;
}
*/

/*void FMSourceFile::RedrawWindow()
{
	if (!open)
		return;							// Not open!!
	::InvalidateRect(window, NULL, FALSE);
	::UpdateWindow(window);
}
*/

//int FMSourceFile::IsOpen()
//{
//	return open;
//}


/*int FMSourceFile::Open()
{
	if (window)
		::DestroyWindow(window);
	open = FALSE;
	int min = minimized;
	int max = maximized;
	if (OpenSourceWindow(this))
	{
		if (min)
			::ShowWindow(window, SW_MINIMIZE);
		if (max)
			::ShowWindow(window, SW_MAXIMIZE);
		open = TRUE;
	}
	return open;
}
*/

/*int FMSourceFile::Close()
{
	if (window)
		::DestroyWindow(window);
	window = NULL;
	open = FALSE;
	return open;
}
*/

/*void FMSourceFile::ShowLine(int line)
{
	if (line < 0 || line >= sourceFile->GetNumLines())
		return;							// No can do!!
	if (!open)
		Open();							// Open if closed

	hScrollPos = 0;						// Start of line
	hScrollInfo.fMask = SIF_POS;
	hScrollInfo.nPos = hScrollPos;
	::SetScrollInfo(hScroll, SB_CTL, &hScrollInfo, TRUE);

	vScrollPos = line-1;
	if (vScrollPos < 0)
		vScrollPos = 0;
	if (vScrollPos > vScrollMax-(vScrollPage-1))
		vScrollPos = vScrollMax-(vScrollPage-1);
	vScrollInfo.fMask = SIF_POS;
	vScrollInfo.nPos = vScrollPos;
	::SetScrollInfo(vScroll, SB_CTL, &vScrollInfo, TRUE);

	::InvalidateRect(window, NULL, FALSE);
	::UpdateWindow(window);
}
*/

