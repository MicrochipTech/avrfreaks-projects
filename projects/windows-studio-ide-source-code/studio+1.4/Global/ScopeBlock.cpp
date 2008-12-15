// ScopeBlock.cpp

#include "stdafx.h"						// MFC

#define SCOPEBLOCK
#include "ScopeBlock.h"
#include "Symbol.h"


/////////////////////////////////////////////////////////////////////////////
// FMScopeBlock functions

FMScopeBlock::FMScopeBlock()
{
	firstAddress = 0;
	firstLine = 0;
	function = NULL;
	lastAddress = 0;
	lastLine = 0;
	numScopeBlocks = 0;
	numSymbols = 0;
	scopeBlocks = NULL;
	symbols = NULL;
}


FMScopeBlock::~FMScopeBlock()
{
	int i;

	if (numSymbols)
	{
		for (i=0; i<numSymbols; i++)
			delete symbols[i];
		delete symbols;
	}
	if (numScopeBlocks)
	{
		for (i=0; i<numScopeBlocks; i++)
			delete scopeBlocks[i];
		delete scopeBlocks;
	}
}


int FMScopeBlock::AddScopeBlock(FMScopeBlock * scopeBlock)
{
	int i = 0;

	if (numScopeBlocks == 0)
	{
		scopeBlocks = new FMScopeBlock *[1];
		scopeBlocks[numScopeBlocks++] = scopeBlock;
		return TRUE;
	}
//	for (i=0; i<numFiles; i++)
//	{
//		if (stricmp(files[i]->GetName(), file->GetName()) == 0 &&
//			stricmp(files[i]->GetPath(), file->GetPath()) == 0)
//			return false;
//	}
	FMScopeBlock ** temp = new FMScopeBlock *[numScopeBlocks+1];
	for (i=0; i<numScopeBlocks; i++)
		temp[i] = scopeBlocks[i];
	delete scopeBlocks;
	scopeBlocks = temp;
	scopeBlocks[numScopeBlocks++] = scopeBlock;
	return TRUE;
}


int FMScopeBlock::AddSymbol(FMSymbol * symbol)
{
	int i = 0;

	if (numSymbols == 0)
	{
		symbols = new FMSymbol *[1];
		symbols[numSymbols++] = symbol;
		return true;
	}
	for (i=0; i<numSymbols; i++)
	{
		if (stricmp(symbols[i]->GetName(), symbol->GetName()) == 0 &&
			stricmp(symbols[i]->GetDataType(), symbol->GetDataType()) == 0 &&
			symbols[i]->GetAddress() == symbol->GetAddress() &&
			symbols[i]->GetType() == symbol->GetType())
			return false;
	}
	FMSymbol ** temp = new FMSymbol *[numSymbols+1];
	for (i=0; i<numSymbols; i++)
		temp[i] = symbols[i];
	if (numSymbols)
		delete symbols;
	symbols = temp;
	symbols[numSymbols++] = symbol;
	return true;
}


int FMScopeBlock::GetContextMenuId(int ti, int * menuId)
{
	int i;

	for (i=0; i<numScopeBlocks; i++)
	{
		if (scopeBlocks[i]->GetContextMenuId(ti, menuId))
			return TRUE;
	}
	for (i=0; i<numSymbols; i++)
	{
		if (symbols[i]->GetContextMenuId(ti, menuId))
			return TRUE;
	}
	return FALSE;
}


int FMScopeBlock::GetFirstAddress()
{
	return firstAddress;
}


int FMScopeBlock::GetFirstLine()
{
	return firstLine;
}


FMFunction * FMScopeBlock::GetFunction()
{
	return function;
}


int FMScopeBlock::GetLastAddress()
{
	return lastAddress;
}


int FMScopeBlock::GetLastLine()
{
	return lastLine;
}


int FMScopeBlock::GetNumScopeBlocks()
{
	return numScopeBlocks;
}


int FMScopeBlock::GetNumSymbols()
{
	return numSymbols;
}


FMScopeBlock * FMScopeBlock::GetScopeBlock(int index)
{
	if (index < numScopeBlocks)
		return scopeBlocks[index];
	return NULL;
}


FMScopeBlock * FMScopeBlock::GetScopeBlock(int address, int different)
{
	FMScopeBlock * innerBlock;
	int i;
	if (address >= firstAddress && address <= lastAddress)
	{									// If address is in scope
		for (i=0; i<numScopeBlocks; i++)
		{
			if (innerBlock = GetScopeBlock(address, TRUE))
				return innerBlock;
		}
		return this;
	}
	return NULL;
}


FMSymbol * FMScopeBlock::GetSymbol(int index)
{
	if (index > -1 && index < numSymbols)
		return symbols[index];
	return NULL;
}


FMSymbol * FMScopeBlock::GetSymbol(const char * symbolName)
{
	int i;
	FMSymbol * symbol = NULL;
	for (i=0; i<numSymbols; i++)
	{
		if (strcmp(symbols[i]->GetName(), symbolName) == 0)
			return symbols[i];
	}
	for (i=0; i<numScopeBlocks; i++)
	{
		symbol = scopeBlocks[i]->GetSymbol(symbolName);
		if (symbol != NULL)
			return symbol;
	}
	return symbol;
}


FMSymbol * FMScopeBlock::GetSymbolFromTreeItem(int ti)
{
	FMSymbol * symbol;
	int i;

	for (i=0; i<numScopeBlocks; i++)
	{
		symbol = scopeBlocks[i]->GetSymbolFromTreeItem(ti);
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


void FMScopeBlock::SetFirstAddress(int address)
{
	firstAddress = address;
	if (lastAddress < firstAddress)
		lastAddress = firstAddress;
}


void FMScopeBlock::SetFirstAddress(const char * addr)
{
	CString ascii = "0123456789ABCDEF";
	int address = 0;
	int i = 0;
	do
	{
		address <<= 4;
		address |= ascii.Find(addr[i++]);
	}
	while (i < (int)strlen(addr));
	SetFirstAddress(address);
}


void FMScopeBlock::SetFirstLine(int line)
{
	firstLine = line;
	if (lastLine < firstLine)
		lastLine = firstLine;
}


void FMScopeBlock::SetFirstLine(const char * line)
{
	SetFirstLine(atoi(line));
}


void FMScopeBlock::SetFunction(FMFunction * func)
{
	function = func;
}


void FMScopeBlock::SetLastAddress(int address)
{
	lastAddress = address;
	if (firstAddress > lastAddress)
		firstAddress = lastAddress;
}


void FMScopeBlock::SetLastAddress(const char * addr)
{
	CString ascii = "0123456789ABCDEF";
	int address = 0;
	int i = 0;
	do
	{
		address <<= 4;
		address |= ascii.Find(addr[i++]);
	}
	while (i < (int)strlen(addr));
	SetLastAddress(address);
}


void FMScopeBlock::SetLastLine(int line)
{
	lastLine = line;
	if (firstLine > lastLine)
		firstLine = lastLine;
}


void FMScopeBlock::SetLastLine(const char * line)
{
	SetLastLine(atoi(line));
}




