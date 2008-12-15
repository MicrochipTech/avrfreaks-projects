// Function.cpp

#ifndef FUNCTION_H
#define FUNCTION_H

class FMScopeBlock;
class FMSourceFile;
class FMSymbol;

///////////////////////////////////////////////////////////////////////////////
// FMFunction class definition

class FMFunction
{
public:
	FMFunction(const char * n);
	~FMFunction();
	int AddScopeBlock(FMScopeBlock * scopeBlock);
	int GetAddress();
	int GetContextMenuId(int ti, int * menuId);
	FMSourceFile * GetFile();
	char * GetName();
	unsigned int GetNumScopeBlocks();
	FMScopeBlock * GetScopeBlock(unsigned int index);
	FMScopeBlock * GetScopeBlock(int address, int different);
	int GetSourceLine();
	FMSymbol * GetSymbol(const char * symbolName);
	FMSymbol * GetSymbolFromTreeItem(int ti);
	int GetTreeItem();
	char GetType();
	void SetAddress(int addr);
	void SetAddress(const char * addr);
//	void SetContextMenuID(int menuId);
	void SetFile(FMSourceFile * sf);
	void SetName(const char * n);
	void SetSourceLine(int line);
	void SetType(const char type);
	void SetTreeItem(int ti);

protected:
	int address;
//	int contextMenuID;
	char * name;
	unsigned int numScopeBlocks;
	FMScopeBlock ** scopeBlocks;
	FMSourceFile * sourceFile;
	int sourceLine;
	int treeItem;
	char type;


};

#endif // FUNCTION_H
