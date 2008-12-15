// ScopeBlock.h

#ifndef SCOPEBLOCK_H
#define SCOPEBLOCK_H

class FMSymbol;
class FMFunction;

///////////////////////////////////////////////////////////////////////////////
// FMScopeBlock class definition

class FMScopeBlock
{
public:
	FMScopeBlock();
	~FMScopeBlock();
	int AddScopeBlock(FMScopeBlock * scopeBlock);
	int AddSymbol(FMSymbol * symbol);
	int GetContextMenuId(int ti, int * menuId);
	int GetFirstAddress();
	int GetFirstLine();
	FMFunction * GetFunction();
	int GetLastAddress();
	int GetLastLine();
	int GetNumScopeBlocks();
	int GetNumSymbols();
	FMScopeBlock * GetScopeBlock(int index);
	FMScopeBlock * GetScopeBlock(int address, int different);
	FMSymbol * GetSymbol(int index);
	FMSymbol * GetSymbol(const char * symbolName);
	FMSymbol * GetSymbolFromTreeItem(int ti);
	void SetFirstAddress(int address);
	void SetFirstAddress(const char * address);
	void SetFirstLine(int line);
	void SetFirstLine(const char * line);
	void SetFunction(FMFunction * func);
	void SetLastAddress(int address);
	void SetLastAddress(const char * address);
	void SetLastLine(int line);
	void SetLastLine(const char * line);

protected:
	int firstAddress;
	int firstLine;
	FMFunction * function;
	int lastAddress;
	int lastLine;
	int numScopeBlocks;
	int numSymbols;
	FMScopeBlock ** scopeBlocks;
	FMSymbol ** symbols;
};

#endif // SCOPEBLOCK_H
