// Project.h

#ifndef PROJECT_H
#define PROJECT_H

class FMSourceFile;
class FMFunction;
class FMSymbol;

///////////////////////////////////////////////////////////////////////////////
// Window Info structure definition

struct WindowInfo
{
	int x;
	int y;
	int width;
	int height;
	int open;
	int minimized;
	int maximized;
};


///////////////////////////////////////////////////////////////////////////////
// FDProject class definition

class FDProject
{
public:
	FDProject();
	~FDProject();
	void Changed();
	bool Close();
//	bool FindISD();
//	void FindHardBreakpoints();
	int GetChip();
	bool GetChipName(int chip, char * buffer, int count);
	int GetComPort();
	int GetFamily();
	int GetFamilyName(char * name, int * max);
	FMSourceFile * GetFile(int index);
	FMSourceFile * GetFile(const char * file);
	FMSourceFile * GetFile(HWND window);
	int GetIDE();
	int GetIDEName(char * name, int * max);
	char * GetName();
	int GetPackage();
	bool GetPackageName(int package, char * buffer, int count);
	char * GetPath();
	int GetPins();
	char * GetSPPName();
	char * GetSPPPath();
	DWORD GetTimeStamp();
	int IsChanged();
	bool LoadFromFile(char * path);
	bool New();
	bool Open(char * path);
//	bool ReadDebugFile();
//	bool ReadImageCraftDebugFile();
//	bool ReadListingFile();
//	bool ReadImageCraftListingFile();
	bool Save();
	bool SaveAs();
	bool SaveToFile(char * path);
	void SetName(char * name);
	void SetPath(char * path);
	void SetIDE(int ide);
	void SetFamily(int family);
	void SetChip(int chip);
	void SetPackage(int package);
	void SetComPort(int port);
	void SetBaud(int baudrate);
	int GetBaud();
	bool IsOpen();
	int GetNumFiles();
	bool AddFile(FMSourceFile * file);
	bool SaveToRegistry();
	int GetTreeItem();
	void SetTreeItem(int ti);
	void SetPC();
	int NumSourcesOpen();
	void UpdateBreakpoints();
	int GetNumSoftBPs();
	int GetNumHardBPs();
	bool GetContextMenu(int ti, int * menu);
	FMSourceFile * GetSourceFileFromTreeItem(int ti);
	FMFunction * GetFunctionFromTreeItem(int ti);
	FMSymbol * GetSymbolFromTreeItem(int ti);
	int GetNumWatchSymbols();
	FMSymbol * GetWatchSymbol(int index);
	bool AddSymbolToWatch(FMSymbol * symbol, bool open);
	bool RemoveSymbolFromWatch(FMSymbol * symbol);
	void UpdateWatchSymbols();
	void ShowSfrFlags(int show);
	int SfrFlagsVisible();
	int GetNumSfrs();
	int * GetSfrFlags();
	int GetSfrFlag(int index);
	void SetSfrFlags(int * flags, int count);
	void SetSfrFlag(int index, int sfr);
	int GetSfrMarker(int sfr);
	char * GetSfrName(int sfr);
	int * GetSfrFlagMarkers(int sfr);
	char * GetSfrFlagName(int sfr, int flag);
	int IsUsingExternalMem();
	int GetExternalMemStart();
	int GetExternalMemSize();
	void SetExternalMem(int start, int size);
	int SetSPPName(char * name);
	int SetSPPPath(char * path);

	int sramStartAddress;
	int sramEndAddress;
	int eepromStartAddress;
	int eepromEndAddress;
	int stackStartAddress;
	int stackEndAddress;
	int debugStackBytes;

	int externalMem;
	int externalMemStart;
	int externalMemSize;

//	int bytesPerVector;
//	int int0Vector;

	WindowInfo projectWindowInfo;
	WindowInfo registersWindowInfo;
	WindowInfo sfrsWindowInfo;
	WindowInfo sramWindowInfo;
	WindowInfo eepromWindowInfo;
	WindowInfo watchWindowInfo;
	WindowInfo stackWindowInfo;
	WindowInfo chipWindowInfo;

	int selectedComPort;				// Comms
	int selectedBaud;
	int firstContact;

private:
	void ReadWindowInfoLine(WindowInfo * info, CString * line);

	int changed;
	char * sppName;
	char * sppPath;
	char * name;
	char * path;
	bool open;
	char * outputDir;
	int ide;
	int family;
	int chip;
	int pins;
	int package;
	int numFiles;
	FMSourceFile ** files;
	int treeItem;
	int setbreakhereAddress;
	int numSoftBPs;
	int numHardBPs;
	int numWatchSymbols;
	FMSymbol ** watchSymbols;
	int numSFRs;
	int * sfrFlags;
	int sfrFlagsVisible;
	int sfrNameLength;
	char * sfrNames;
	int * sfrMarkers;
	int * sfrFlagMarkers;
	int sfrFlagNameLength;
	char * sfrFlagNames;
};

#endif
