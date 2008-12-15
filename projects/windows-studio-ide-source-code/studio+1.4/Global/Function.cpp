// Function.cpp

#include "stdafx.h"						// MFC

#define FUNCTION
#include "Function.h"
#include "SourceFile.h"
#include "ScopeBlock.h"
#include "Symbol.h"
#include "Global.h"


/////////////////////////////////////////////////////////////////////////////
// FMSymbol functions

FMFunction::FMFunction(const char * n)
{
	name = new char[strlen(n)+1];
	strcpy(name, n);

	address = 0;
//	contextMenuID = -1;
	numScopeBlocks = 0;
	scopeBlocks = NULL;
	sourceFile = NULL;
	sourceLine = -1;
	treeItem = 0;
	type = (char)0;
}


FMFunction::~FMFunction()
{
	if (name)
		delete name;
	if (numScopeBlocks)
	{
		for (unsigned int i=0; i<numScopeBlocks; i++)
			delete scopeBlocks[i];
		delete scopeBlocks;
	}
}


int FMFunction::AddScopeBlock(FMScopeBlock * scopeBlock)
{
	unsigned int i = 0;

	scopeBlock->SetFunction(this);
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
//			return FALSE;
//	}
	FMScopeBlock ** temp = new FMScopeBlock *[numScopeBlocks+1];
	for (i=0; i<numScopeBlocks; i++)
		temp[i] = scopeBlocks[i];
	if (numScopeBlocks)
		delete scopeBlocks;
	scopeBlocks = temp;
	scopeBlocks[numScopeBlocks++] = scopeBlock;
	return TRUE;
}


int FMFunction::GetAddress()
{
	return address;
}


int FMFunction::GetContextMenuId(int ti, int * menuId)
{
	if (treeItem == ti)
	{
//		*menuId = contextMenuID;
		*menuId = CONTEXTMENUID_FUNCTION;
		return TRUE;
	}
	for (unsigned int i=0; i<numScopeBlocks; i++)
	{
		if (scopeBlocks[i]->GetContextMenuId(ti, menuId))
			return TRUE;
	}
	return FALSE;
}


FMSourceFile * FMFunction::GetFile()
{
	return sourceFile;
}


char * FMFunction::GetName()
{
	return name;
}


unsigned int FMFunction::GetNumScopeBlocks()
{
	return numScopeBlocks;
}


FMScopeBlock * FMFunction::GetScopeBlock(unsigned int index)
{
	if (index < numScopeBlocks)
		return scopeBlocks[index];
	return NULL;
}


FMScopeBlock * FMFunction::GetScopeBlock(int address, int different)
{
	FMScopeBlock * scopeBlock;
	FMScopeBlock * innerBlock;
	for (unsigned int i=0; i<numScopeBlocks; i++)
	{
		scopeBlock = GetScopeBlock(i);
		if (address >= scopeBlock->GetFirstAddress() &&
			address <= scopeBlock->GetLastAddress())
		{
			if (innerBlock = scopeBlock->GetScopeBlock(address, TRUE))
				return innerBlock;
			return scopeBlock;
		}
	}
	return NULL;
}


int FMFunction::GetSourceLine()
{
	return sourceLine;
}


FMSymbol * FMFunction::GetSymbol(const char * symbolName)
{
	FMSymbol * symbol = NULL;
	for (unsigned int i=0; i<numScopeBlocks; i++)
	{
		symbol = scopeBlocks[i]->GetSymbol(symbolName);
		if (symbol != NULL)
			return symbol;
	}
	return symbol;
}


FMSymbol * FMFunction::GetSymbolFromTreeItem(int ti)
{
	FMSymbol * symbol;
	if (treeItem == ti)
		return NULL;
	for (unsigned int i=0; i<numScopeBlocks; i++)
	{
		symbol = scopeBlocks[i]->GetSymbolFromTreeItem(ti);
		if (symbol != NULL)
			return symbol;
	}
	return NULL;
}


int FMFunction::GetTreeItem()
{
	return treeItem;
}


char FMFunction::GetType()
{
	return type;
}


void FMFunction::SetAddress(int addr)
{
	address = addr;
}


void FMFunction::SetAddress(const char * addr)
{
	CString ascii = "0123456789ABCDEF";
	address = 0;
	int i = 0;
	do
	{
		address <<= 4;
		address |= ascii.Find(addr[i++]);
	}
	while (i < (int)strlen(addr));
}


void FMFunction::SetFile(FMSourceFile * sf)
{
	sourceFile = sf;
}


void FMFunction::SetName(const char * n)
{
	if (name)
		delete name;
	name = new char[strlen(n)+1];
	strcpy(name, n);
}


void FMFunction::SetSourceLine(int line)
{
	sourceLine = line;
}


void FMFunction::SetTreeItem(int ti)
{
	treeItem = ti;
}


void FMFunction::SetType(const char t)
{
	type = t;
}
