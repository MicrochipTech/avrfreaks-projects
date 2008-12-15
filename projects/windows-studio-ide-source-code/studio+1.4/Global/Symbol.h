// Symbol.h

#ifndef SYMBOL_H
#define SYMBOL_H

#include "Symbol.h"

// Forward declarations
class FMSourceFile;
class FMScopeBlock;

///////////////////////////////////////////////////////////////////////////////
// FDSymbol class definition

class FMSymbol
{
public:
	FMSymbol();
	FMSymbol(FMSymbol * symbol);
	~FMSymbol();
	char * DataTypeString();
	int GetAddress();
	char * GetAddressString();
	int GetContextMenuId(int ti, int * menuId);
	char * GetDataType();
	char GetDataTypeChar();
	char * GetDescription();
	int GetDimensionSize(int dimension);
	FMSourceFile * GetFile();
	char * GetName();
	int GetNumDimensions();
	FMScopeBlock * GetScope();
	int GetSourceLine();
	int GetTreeItem();
	int GetType();
	unsigned char * GetValue();
	int GetValueSize();
	int GetValueString(char * buffer, int max);
	int IsArray();
	int IsPointer();
	void SetAddress(char * addr);
	void SetAddress(int addr);
//	void SetContextMenuId(int menuId);
	void SetDataType(char * dataType);
	void SetFile(FMSourceFile * sf);
	void SetName(char * n);
	void SetScope(FMScopeBlock * b);
	void SetSourceLine(int line);
	void SetType(int t);
	void SetTreeItem(int ti);
	void UpdateValue();

	FMScopeBlock * scope;

private:
//	int contextMenuID;
	FMSourceFile * sourceFile;
	int sourceLine;
	char * name;
	int address;
	char * addressString;
	char * dataType;
	char dataTypeChar;
	char * dataTypeString;
	char * description;
	int type;

	int valueSize;
	unsigned char * value;				// Value of symbol

	int pointer;
	int array;
	int elementSize;
	int arraySize;						// Array info
	int numDimensions;
	int * dimensions;

	int treeItem;
};

#endif // SYMBOL_H
