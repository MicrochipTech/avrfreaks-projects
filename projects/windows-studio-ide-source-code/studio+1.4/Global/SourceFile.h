// SourceFile.h

#ifndef SOURCEFILE_H
#define SOURCEFILE_H

// Breakpoint structure definition
struct Breakpoint
{
	int type;
	int line;
	int address;
};

// LineSegment structure definition
struct LineSegment
{
	int start;
	int length;
	int colour;
};

// CodeLine structure definition
struct CodeLine
{
	unsigned int address;
	unsigned int opCode;
	unsigned int sourceLength;
	char * source;
};

// SourceLine structure definition
struct SourceLine
{
	char * source;
	BOOL code;
	int startAddress;
	int endAddress;						// Address range (not inc. end address)
	int length;
	char * line;						// Source line
	int numTabs;
	int * tabs;
	int numSegments;					// Number of coloured segments
	LineSegment * segments;
	int numCodeLines;
	CodeLine ** codeLines;
};

// Forward declarations
class FMFunction;
class FMSymbol;

///////////////////////////////////////////////////////////////////////////////
// CSourceFile class definition

class FMSourceFile
{
public:
	FMSourceFile(const char * projectPath,const char * file);
	~FMSourceFile();

	int AddFunction(FMFunction * function);
	int AddSymbol(FMSymbol * symbol);
	void ClearAllBPs();
	void ClearSoftBP(int line);
	int CodeAtAddress(int address);
	int FindFunction(FMFunction * function, int blockLine);
	int FindPCLine(int pc);
	int FindSymbol(FMSymbol * symbol);
	void FormatAsmSourceText(CString comment, char stringChar,
		char charChar, char escapeChar, char directiveChar,
		CString directives, CString keywordChars, CString keywords);
	void FormatCSourceText(CString comment, CString commentStart,
		CString commentEnd, char stringChar, char charChar, char escapeChar,
		char directiveChar, CString directives, CString keywordChars,
		CString keywords);
	char * GetAbsolutePath();
	int GetContextMenuId(int ti, int * menuId);
	char * GetFileName();
	FMFunction * GetFunction(int index);
	FMFunction * GetFunction(const char * function);
	FMSymbol * GetFunctionSymbol(const char * functionName,
		const char * symbolName);
	FMSymbol * GetGlobalSymbol(const char * symbolName);
	Breakpoint * GetHardBP(int bp);
	SourceLine * GetLine(int index);
	int GetLineBreakpoint(int line);
	int GetNumHardBPs();
	int GetNumFunctions();
	int GetNumLines();
	int GetNumSymbols();
	char * GetRelativePath();
	int GetSourceFileIndex();
	FMSymbol * GetSymbol(int index);
	FMSymbol * GetSymbolFromTreeItem(int ti);
	int GetTreeItem();
	HWND GetWindowHandle();
	void GetWindowRect(RECT * rect);
	int IsC();
	int IsOpen();
	int IsWindowMaximized();
	int IsWindowMinimized();
	int SetHardBP(int line);
	void SetLineEndAddress(int line, int address);
	void SetLineEndAddress(int line, const char * address);
	void SetLineStartAddress(int line, int address);
	void SetLineStartAddress(int line, const char * address);
	void SetMaximized(int max);
	void SetMinimized(int min);
	int SetSoftBP(int line);
	void SetSourceFileIndex(int index);
	void SetTreeItem(int ti);
	void SetWindowHandle(HWND wh);
	void SetWindowRect(RECT * rect);
	void TabLine(int lineNum);

	int longestLineLength;
	int open;
	int wasOpen;
	
	
//	void Paint(HDC);
//	void DrawSource();
//	int UpdateWindow();
//	void RedrawWindow();
//	int Open();
//	int Close();
//	void ShowLine(int line);

//	int maxWidth;
//	int minWidth;

private:
	LineSegment * NewSegment(int lineNum, int start);
	void SegmentAssemblerLine(int lineNum, bool * inComment, bool * inString,
		bool * inChar);
	void SegmentCLine(int lineNum, bool * inComment, bool * inString,
		bool * inChar);

	char * absolutePath;
	char * fileName;
	FMFunction ** functions;
	Breakpoint * hardBPs;
	HWND hWnd;
	SourceLine ** lines;
	int maximized;
	int minimized;
	int numFunctions;
	int numHardBPs;
	int numLines;
	int numSoftBPs;
	int numSymbols;
	char * relativePath;
	Breakpoint * softBPs;
	FMSymbol ** symbols;
	int treeItem;
	RECT windowRect;
	int sourceFileIndex;
};

#endif
