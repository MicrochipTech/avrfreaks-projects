// DSymbol.cpp

#include "stdafx.h"						// MFC

#define SYMBOL
#include "Symbol.h"
#include "SourceFile.h"
#include "ScopeBlock.h"
#include "Global.h"

extern char * itoh(int i, char * hex, bool trim, int chars);

extern int pc;
extern unsigned char * registers;
extern unsigned char * sram;
extern int framePointer;
extern int sramStart;


/////////////////////////////////////////////////////////////////////////////
// FMSymbol functions


FMSymbol::FMSymbol()
{
//	contextMenuID = -1;
	sourceFile = NULL;
	sourceLine = -1;
	scope = NULL;
	name = NULL;
	address = 0;
	addressString = NULL;
	dataType = NULL;
	dataTypeChar = 0;
	dataTypeString = NULL;
	description = NULL;
	type = VARIABLE_STATIC;

	valueSize = 0;
	value = NULL;

	pointer = FALSE;
	array = FALSE;
	arraySize = 0;
	numDimensions = 0;
	dimensions = NULL;
	elementSize = 0;

	treeItem = 0;
}


FMSymbol::FMSymbol(FMSymbol * symbol)
{
	sourceFile = NULL;
	sourceLine = -1;
	scope = NULL;
	name = NULL;
	address = 0;
	addressString = NULL;
	dataType = NULL;
	dataTypeChar = 0;
	dataTypeString = NULL;
	description = NULL;
	type = VARIABLE_STATIC;
	valueSize = 0;
	value = NULL;
	pointer = FALSE;
	array = FALSE;
	arraySize = 0;
	numDimensions = 0;
	dimensions = NULL;
	elementSize = 0;
	treeItem = 0;

	SetName(symbol->GetName());
	SetType(symbol->GetType());
	SetAddress(symbol->GetAddress());
	SetFile(symbol->GetFile());
	SetSourceLine(symbol->GetSourceLine());
	SetScope(symbol->GetScope());
	SetDataType(symbol->GetDataType());
	SetTreeItem(symbol->GetTreeItem());
}


FMSymbol::~FMSymbol()
{
	if (name)
		delete name;
	name = NULL;
	if (addressString)
		delete addressString;
	addressString = NULL;
	if (dataType)
		delete dataType;
	dataType = NULL;
	if (dataTypeString)
		delete dataTypeString;
	dataTypeString = NULL;
	if (description)
		delete description;
	description = NULL;

	if (valueSize)
		delete value;
	valueSize = 0;
	value = NULL;

	if (numDimensions)
		delete dimensions;
	dimensions = NULL;
	numDimensions = 0;

	if (type&VARIABLE_ASSEMBLERMASK)	// If an assembler variable
	{
		if (scope)
			delete scope;
		scope = NULL;
	}
}


int FMSymbol::GetAddress()
{
	return address;
}


void FMSymbol::SetAddress(int addr)
{
	address = addr;

	if (addressString)
		delete addressString;
	addressString = NULL;
	char buffer[20];
	switch ((type&VARIABLE_TYPEMASK))
	{
		case VARIABLE_REGISTER:
			itoa(address, buffer, 10);
			addressString = new char[strlen(buffer)+2];
			strcpy(addressString, "R");
			strcat(addressString, buffer);
			break;
		case VARIABLE_STATIC:
			addressString = new char[7];
			strcpy(addressString, "0x");
			itoh(address, addressString, false, 4);
			break;
		case VARIABLE_LOCAL:
			addressString = new char[9];
			strcpy(addressString, "Y+0x");
			itoh(address, &addressString[2], false, 4);
			break;
		case VARIABLE_SYMBOL:
			addressString = new char[3];
			strcpy(addressString, "NA");
			break;
		default:
			addressString = new char[2];
			strcpy(addressString, "?");
			break;
	}
}


int FMSymbol::GetContextMenuId(int ti, int * menuId)
{
	if (treeItem == ti)
	{
		if (!(type&VARIABLE_ASSEMBLERMASK) &&
			(type == VARIABLE_STATIC ||
			type == VARIABLE_REGISTER ||
			type == VARIABLE_LOCAL))
		{
//			*menuId = contextMenuID;
			*menuId = CONTEXTMENUID_VARIABLE;
		}
		else
			*menuId = -1;
		return TRUE;
	}
	return FALSE;
}


char FMSymbol::GetDataTypeChar()
{
	return dataTypeChar;
}


char * FMSymbol::GetDescription()
{
	return description;
}


int FMSymbol::GetDimensionSize(int dimension)
{
	if (dimension >= 0 && dimension < numDimensions)
		return dimensions[dimension];
	return -1;
}


FMSourceFile * FMSymbol::GetFile()
{
	return sourceFile;
}


char * FMSymbol::GetName()
{
	return name;
}


int FMSymbol::GetNumDimensions()
{
	return numDimensions;
}


FMScopeBlock * FMSymbol::GetScope()
{
	return scope;
}


int FMSymbol::GetSourceLine()
{
	return sourceLine;
}


void FMSymbol::SetFile(FMSourceFile * sf)
{
	sourceFile = sf;
}


void FMSymbol::SetName(char * n)
{
	if (name)
		delete name;
	name = new char[strlen(n)+1];
	strcpy(name, n);

	if (description)
		delete description;
	CString dtString = "";
	CString desc;
	if (dataTypeString)
		dtString = dataTypeString;
	if (!array)
		desc = dtString;
	else
		desc = dtString.Mid(0, dtString.Find('['));
	desc += " ";
	desc += name;
	if (array)
		desc += dtString.Mid(dtString.Find('['));
	description = new char[desc.GetLength()+1];
	strcpy(description, LPCTSTR(desc));
}


void FMSymbol::SetScope(FMScopeBlock * b)
{
//	if (scope && (type&VARIABLE_ASSEMBLERMASK))
//		delete scope;
	scope = b;
}


void FMSymbol::SetSourceLine(int line)
{
	sourceLine = line;
}


void FMSymbol::SetAddress(char * addr)
{
	CString ascii = "0123456789ABCDEF";
	int a = 0;
	int i = 0;
	do
	{
		a <<= 4;
		a |= ascii.Find(addr[i++]);
	}
	while (i < (int)strlen(addr));
	SetAddress(a);
}


char * FMSymbol::GetAddressString()
{
	return addressString;
}


char * FMSymbol::GetDataType()
{
	return dataType;
}


void FMSymbol::SetDataType(char * dt)
{
	int i;
	int s, e;
	int pos;
	CString string;
	int dim;
	int size;
	char buffer[10];

	if (dataType)
		delete dataType;
	if (numDimensions)
	{
		delete dimensions;
		dimensions = NULL;
		numDimensions = 0;
	}
	pointer = false;
	array = false;

	dataType = new char[strlen(dt)+1];
	strcpy(dataType, dt);

	if (dataTypeString)
		delete dataTypeString;
	dataTypeString = NULL;

	if (dataType)
	{
		CString string;
		pos = 0;
		pointer = false;
		if (dataType[pos] == 'p')			// It's a pointer
		{
			pointer = true;
			pos++;
		}
		array = false;
		if (dataType[pos] == 'A')			// It's an array
		{
			array = true;
			pos++;
		}
		if (array)
		{
			pos++;							// First bracket
			s = pos;
			e = s;
			arraySize = 0;
			while (dataType[e] != ':' && dataType[e] != ']')
			{
				arraySize *= 10;
				arraySize += (dataType[e]-'0');
				e++;
			}
			e++;							// Past the ':'
			s = e;
			do
			{
				dim = 0;
				while (dataType[e] != ':' && dataType[e] != ']')
				{
					dim *= 10;
					dim += (dataType[e]-'0');
					e++;
				}
				e++;
				int * temp = new int[numDimensions+1];
				for (i=0; i<numDimensions; i++)
					temp[i] = dimensions[i];
				if (numDimensions)
					delete dimensions;
				dimensions = temp;
				dimensions[numDimensions++] = dim;
			}
			while (dataType[e-1] != ']');
			pos = e;
		}
		dataTypeChar = dataType[pos];
		switch (dataType[pos])				// Type
		{
			case 'c':						// unsigned char
				string = "unsigned char";
				elementSize = 1;
				break;
			case 's':
				string = "unsigned short";
				elementSize = 2;
				break;
			case 'i':
				string = "unsigned int";
				elementSize = 2;
				break;
			case 'l':
				string = "unsigned long";
				elementSize = 4;
				break;
			case 'C':
				string = "char";
				elementSize = 1;
				break;
			case 'S':
				string = "short";
				elementSize = 2;
				break;
			case 'I':
				string = "int";
				elementSize = 2;
				break;
			case 'L':
				string = "long";
				elementSize = 4;
				break;
			case 'D':
				string = "float";
				elementSize = 4;
				break;
			case 'V':
				string = "void";
				elementSize = 0;
				break;
			case 'X':
			case 'x':
			default:
				string = "Not Yet Supported";
				elementSize = 0;
				break;
		}
		if (pointer)
		{
			elementSize = 2; // What about different memory pointers?
			string += " *";
		}
		size = elementSize;
		if (array)
		{
			int as = 1;
			for (i=0; i<numDimensions; i++)
			{
				as *= dimensions[i];
				string += "[";
				itoa(dimensions[i], buffer, 10);
				string += buffer;
				string += "]";
			}
			size *= as;					// Total array size
		}
		dataTypeString = new char[string.GetLength()+1];
		strcpy(dataTypeString, LPCTSTR(string));

		if (valueSize)
			delete value;
		value = NULL;
		if (size)
		{
			valueSize = size;
			value = new unsigned char[valueSize];
		}
	}

	if (description)
		delete description;
	CString dtString = "";
	CString desc;
	if (dataTypeString)
		dtString = dataTypeString;
	if (!array)
		desc = dtString;
	else
		desc = dtString.Mid(0, dtString.Find('['));
	desc += " ";
	desc += name;
	if (array)
		desc += dtString.Mid(dtString.Find('['));
	description = new char[desc.GetLength()+1];
	strcpy(description, LPCTSTR(desc));
}


char * FMSymbol::DataTypeString()
{
	return dataTypeString;
}


int FMSymbol::GetType()
{
	return type;
}


void FMSymbol::SetType(int t)
{
	type = t;
	SetAddress(address);				// Set address string
}


int FMSymbol::GetValueSize()
{
	return valueSize;
}


unsigned char * FMSymbol::GetValue()
{
	return value;
}


int FMSymbol::GetValueString(char * buffer, int max)
{
	CString string;
	char buf[20];
	int val;
	unsigned int bigval;
	int numDims;
	int dim;
	int elements;
	int i, j;
	int offset;
	int * dimSizes = NULL;

	if (!dataType)
		return NULL;

	if (scope != NULL && (
		pc < scope->GetFirstAddress() ||
		pc > scope->GetLastAddress()))	// If variable is out-of-scope
	{
		string = "Out of Scope";
		if (string.GetLength() > max-1)
			string = string.Mid(0, max-1);
		strcpy(buffer, LPCTSTR(string));
		return string.GetLength();
	}

	offset = 0;
	string = "";
	numDims = 1;
	dim = 0;
	elements = 1;
	if (array)
	{
		dimSizes = new int[numDimensions-1];
		elements = dimensions[numDimensions-1];
		for (i=0; i<numDimensions-1; i++)
		{
			numDims *= dimensions[(numDimensions-2)-i];
			dimSizes[(numDimensions-2)-i] = numDims;
		}
	}
	do
	{
		if (array)
		{
			for (i=0; i<numDimensions-1; i++)
			{
				if ((dim/dimSizes[i])*dimSizes[i] == dim)
					string += '{';
			}
			if (dataTypeChar == 'C')	// If a string
				string += '\"';
			else
				string += '{';
		}

		for (i=0; i<elements; i++)
		{
			if (pointer)
			{
				val = value[offset+1]<<8;
				val += value[offset];
				offset += 2;
				strcpy(buf, "0x");
				itoh(val, buf, false, 4);
				string += buf;
				if (i != elements-1)
					string += ',';
				continue;
			}
			switch (dataTypeChar)
			{
				case 'c':				// unsigned char
					val = value[offset++];
					itoa(val, buf, 10);
					string += buf;
					break;
				case 's':
				case 'i':
					val = value[offset+1]<<8;
					val += value[offset];
					offset += 2;
					itoa(val, buf, 10);
					string += buf;
					break;
				case 'l':
					bigval = 0;
					for (j=0; j<4; j++)
					{
						bigval <<= 8;
						bigval += value[offset+(3-j)];
					}
					offset += 4;
					ltoa(bigval, buf, 10);
					string += buf;
					break;
				case 'C':
					val = value[offset++];
					bigval = val;		// Temp store
					if (!array && val > 0xef)
						val = 0-(int)(((~val)&0xff)+1);
					if (!array)
					{
						itoa(val, buf, 10);
						string += buf;
						string += " (";
					}
					if (bigval < 32)
						string += '.';
					else
						string += (char)bigval;
					if (!array)
						string += ")";
					break;
				case 'S':
				case 'I':
					val = value[offset+1]<<8;
					val += value[offset];
					if (val > 0xefff)
						val = 0-(int)(((~val)&0xffff)+1);
					offset += 2;
					itoa(val, buf, 10);
					string += buf;
					break;
				case 'L':
					bigval = 0;
					for (j=0; j<4; j++)
					{
						bigval <<= 8;
						bigval += value[offset+(3-j)];
					}
					if (bigval > 0xefffffff)
						val = 0-(int)(((~bigval)&0xffffffff)+1);
					else
						val = bigval;
					offset += 4;
					itoa(val, buf, 10);
					string += buf;
					break;
				case 'D':
					val = 0;
					for (j=0; j<4; j++)
					{
						val <<= 8;
						val += value[offset+(3-j)];
					}
					offset += 4;
					itoa(val, buf, 10);
					string += buf;
					break;
				case 'V':
					string += "NA";
					break;
				case 'X':
				case 'x':
				default:
					string += "?";
					break;
			}
			if (i != elements-1 && dataTypeChar != 'C')
				string += ',';
		}

		dim++;

		if (array)
		{
			if (dataTypeChar == 'C')	// If a string
				string += '\"';
			else
				string += '}';
			for (i=0; i<numDimensions-1; i++)
			{
				if ((dim/dimSizes[i])*dimSizes[i] == dim)
					string += '}';
			}
			if (dim != numDims)
				string += ',';
		}
	}
	while (dim < numDims);

	if (string.GetLength() > max-1)
	{
		string = string.Mid(0, max-4);
		string += "...";
	}
	strcpy(buffer, LPCTSTR(string));
	return string.GetLength();
}


int FMSymbol::GetTreeItem()
{
	return treeItem;
}


void FMSymbol::SetTreeItem(int ti)
{
	treeItem = ti;
}


int FMSymbol::IsArray()
{
	return array;
}


int FMSymbol::IsPointer()
{
	return pointer;
}


void FMSymbol::UpdateValue()
{
	int i;
	int bytes;

	if (elementSize == 0 || elementSize == -1)
		return;

	bytes = elementSize;
	if (array)
		bytes = arraySize;

	if ((type&VARIABLE_TYPEMASK) == VARIABLE_REGISTER)
	{
		for (i=0; i<bytes; i++)
			value[i] = registers[address+i];
	}
	else if ((type&VARIABLE_TYPEMASK) == VARIABLE_LOCAL)
	{
		for (i=0; i<bytes; i++)
			value[i] = sram[(framePointer+address+i)-sramStart];
	}
	else if ((type&VARIABLE_TYPEMASK) == VARIABLE_STATIC)
	{
		for (i=0; i<bytes; i++)
			value[i] = sram[(address+i)-sramStart];
	}
}
