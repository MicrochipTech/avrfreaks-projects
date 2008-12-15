// AsmVariableDialog.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "AsmVariableDialog.h"

#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAsmVariableDialog dialog


CAsmVariableDialog::CAsmVariableDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAsmVariableDialog::IDD, pParent)
{
	symbol = NULL;
	scope = NULL;
	backupSymbol = NULL;
	backupScope = NULL;

	//{{AFX_DATA_INIT(CAsmVariableDialog)
	//}}AFX_DATA_INIT
}


CAsmVariableDialog::CAsmVariableDialog(FMSymbol * s, CWnd* pParent /*=NULL*/)
	: CDialog(CAsmVariableDialog::IDD, pParent)
{
	backupSymbol = s;
	if (backupSymbol)
	{
		backupScope = backupSymbol->GetScope();
		symbol = new FMSymbol(backupSymbol);
		if (symbol->GetScope())
		{
			scope = new FMScopeBlock();
			scope->SetFirstAddress(backupScope->GetFirstAddress());
			scope->SetLastAddress(backupScope->GetLastAddress());
			symbol->scope = NULL;
			symbol->SetScope(scope);
		}
		else
			scope = NULL;
	}
	else
		backupScope = NULL;
}


CAsmVariableDialog::~CAsmVariableDialog()
{
	if (symbol)
		delete symbol;
}


void CAsmVariableDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAsmVariableDialog)
	DDX_Control(pDX, IDC_ASMVARIABLE_REGISTER, m_register);
	DDX_Control(pDX, IDC_ASMVARIABLE_MEMORY, m_memory);
	DDX_Control(pDX, IDC_ASMVARIABLE_VALUE, m_value);
	DDX_Control(pDX, IDC_ASMVARIABLE_SCOPETO, m_scopeTo);
	DDX_Control(pDX, IDC_ASMVARIABLE_SCOPETEXT, m_scopeText);
	DDX_Control(pDX, IDC_ASMVARIABLE_SCOPEFROM, m_scopeFrom);
	DDX_Control(pDX, IDC_ASMVARIABLE_POINTER, m_pointer);
	DDX_Control(pDX, IDC_ASMVARIABLE_GLOBAL, m_global);
	DDX_Control(pDX, IDC_ASMVARIABLE_DIMENSIONSTEXT, m_dimensionsText);
	DDX_Control(pDX, IDC_ASMVARIABLE_DIMENSIONS, m_dimensions);
	DDX_Control(pDX, IDC_ASMVARIABLE_DECLARATION, m_declaration);
	DDX_Control(pDX, IDC_ASMVARIABLE_DATATYPE, m_dataType);
	DDX_Control(pDX, IDC_ASMVARIABLE_ARRAY, m_array);
	DDX_Control(pDX, IDC_ASMVARIABLE_ADDRESS, m_address);
	DDX_Control(pDX, IDC_ASMVARIABLE_NAME, m_name);
	DDX_Control(pDX, IDC_ASMVARIABLE_SIZE, m_size);
	//}}AFX_DATA_MAP
}


FMSymbol * CAsmVariableDialog::GetSymbol()
{
	FMSymbol * s = symbol;
	symbol = NULL;
	scope = NULL;
	return s;
}


void CAsmVariableDialog::SetSymbol()
{
	char buffer[1000];
	int i;

	if (symbol == NULL)
		return;							// Don't be a twat!!
	if (!(symbol->GetType()&VARIABLE_ASSEMBLERMASK))
		return;							// Wrong type

	m_name.SetWindowText(symbol->GetName());

	if (symbol->GetType()&VARIABLE_REGISTER)
	{
		m_memory.SetCheck(0);
		m_register.SetCheck(1);
		OnAsmvariableRegister();
	}
	else
	{
		m_register.SetCheck(0);
		m_memory.SetCheck(1);
		OnAsmvariableMemory();
	}

	if (!scope)
	{
		m_global.SetCheck(1);
		m_scopeFrom.EnableWindow(FALSE);
		m_scopeText.EnableWindow(FALSE);
		m_scopeTo.EnableWindow(FALSE);
	}
	else
	{
		m_global.SetCheck(0);
		strcpy(buffer, "0x");
		itoh(scope->GetFirstAddress(), buffer, false, 4);
		m_scopeFrom.SetWindowText(buffer);
		buffer[2] = 0;
		itoh(scope->GetLastAddress(), buffer, false, 4);
		m_scopeTo.SetWindowText(buffer);
		m_scopeFrom.EnableWindow(TRUE);
		m_scopeText.EnableWindow(TRUE);
		m_scopeTo.EnableWindow(TRUE);
	}

	if (symbol->IsPointer())
		m_pointer.SetCheck(1);
	else
		m_pointer.SetCheck(0);

	switch ((int)(symbol->GetDataTypeChar()))
	{
		default:
		case 'c':
			m_dataType.SetCurSel(0);
			break;
		case 'C':
			m_dataType.SetCurSel(1);
			break;
		case 's':
			m_dataType.SetCurSel(2);
			break;
		case 'S':
			m_dataType.SetCurSel(3);
			break;
		case 'i':
			m_dataType.SetCurSel(4);
			break;
		case 'I':
			m_dataType.SetCurSel(5);
			break;
		case 'l':
			m_dataType.SetCurSel(6);
			break;
		case 'L':
			m_dataType.SetCurSel(7);
			break;
		case 'D':
			m_dataType.SetCurSel(8);
			break;
	}

	if (symbol->IsArray())
	{
		m_array.SetCheck(1);
		buffer[0] = 0;
		for (i=0; i<symbol->GetNumDimensions(); i++)
		{
			strcat(buffer, "[");
			itoa(symbol->GetDimensionSize(i), &buffer[strlen(buffer)], 10);
			strcat(buffer, "]");
		}
		m_dimensions.SetWindowText(buffer);
		m_dimensions.EnableWindow(TRUE);
	}
	else
	{
		m_dimensions.SetWindowText("");
		m_dimensions.EnableWindow(FALSE);
		m_array.SetCheck(0);
	}
}


bool CAsmVariableDialog::BuildDataTypeString(char * buffer, int * max)
{
	int i;
	char buf[1000];
	CString str = "";
	char type[10];
	int elementSize = 0;

	switch (m_dataType.GetCurSel())
	{
		default:
		case 0:							// unsigned char
			strcpy(type, "c");
			elementSize = 1;
			break;
		case 1:							// char
			strcpy(type, "C");
			elementSize = 1;
			break;
		case 2:							// unsigned short
			strcpy(type, "s");
			elementSize = 2;
			break;
		case 3:							// short
			strcpy(type, "S");
			elementSize = 2;
			break;
		case 4:							// unsigned int
			strcpy(type, "i");
			elementSize = 2;
			break;
		case 5:							// int
			strcpy(type, "I");
			elementSize = 2;
			break;
		case 6:							// unsigned long
			strcpy(type, "l");
			elementSize = 4;
			break;
		case 7:							// long
			strcpy(type, "L");
			elementSize = 4;
			break;
		case 8:							// float
			strcpy(type, "D");
			elementSize = 4;
			break;
	}
	
	if (m_pointer.GetCheck())
	{
		str += "p";
		elementSize = 2;
	}

	if (m_array.GetCheck())
	{
		CString dimStr;
		int dims[1000];
		int numDims = 0;
		int arraySize = 0;
		m_dimensions.GetWindowText(dimStr);
		i = 1;
		do
		{
			if (dimStr.Find(']', i) == -1)
			{
				i = 0;
				continue;
			}
			if (dimStr.Find(']', i) == i)
			{
				i = dimStr.Find('[', i)+1;
				continue;
			}
			strcpy(buf, dimStr.Mid(i, dimStr.Find(']', i)-i));
			if (buf[0] == '0' && buf[1] == 'x')
				dims[numDims] = htoi(buf);
			else
				dims[numDims] = atoi(buf);
			if (dims[numDims] == 0)
			{
				dims[numDims] = 1;
				AfxMessageBox(
"Array dimensions must contain 1 or more elements.",
					MB_OK|MB_ICONEXCLAMATION, 0);
			}
			numDims++;
			i = dimStr.Find('[', i)+1;
		}
		while (i != 0);

		if (numDims)
		{
			arraySize = 1;
			for (i=0; i<numDims; i++)
				arraySize *= dims[i];
			arraySize *= elementSize;
			str += "A[";
			itoa(arraySize, buf, 10);
			str += buf;
			for (i=0; i<numDims; i++)
			{
				itoa(dims[i], buf, 10);
				str += ":";
				str += buf;
			}
			str += "]";
		}
	}

	str += type;

	if (str.GetLength()+1 <= *max)
	{
		strcpy(buffer, str);
		return true;
	}
	*max = str.GetLength()+1;
	return false;
}


BEGIN_MESSAGE_MAP(CAsmVariableDialog, CDialog)
	//{{AFX_MSG_MAP(CAsmVariableDialog)
	ON_BN_CLICKED(IDC_ASMVARIABLE_GLOBAL, OnAsmvariableGlobal)
	ON_BN_CLICKED(IDC_ASMVARIABLE_ARRAY, OnAsmvariableArray)
	ON_BN_CLICKED(IDC_ASMVARIABLE_REGISTER, OnAsmvariableRegister)
	ON_BN_CLICKED(IDC_ASMVARIABLE_MEMORY, OnAsmvariableMemory)
	ON_CBN_SELCHANGE(IDC_ASMVARIABLE_DATATYPE, OnSelchangeAsmvariableDatatype)
	ON_BN_CLICKED(IDC_ASMVARIABLE_POINTER, OnAsmvariablePointer)
	ON_EN_CHANGE(IDC_ASMVARIABLE_ADDRESS, OnChangeAsmvariableAddress)
	ON_EN_CHANGE(IDC_ASMVARIABLE_DIMENSIONS, OnChangeAsmvariableDimensions)
	ON_EN_CHANGE(IDC_ASMVARIABLE_NAME, OnChangeAsmvariableName)
	ON_EN_KILLFOCUS(IDC_ASMVARIABLE_SCOPEFROM, OnKillfocusAsmvariableScopefrom)
	ON_EN_KILLFOCUS(IDC_ASMVARIABLE_SCOPETO, OnKillfocusAsmvariableScopeto)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsmVariableDialog message handlers


BOOL CAsmVariableDialog::OnInitDialog() 
{
	int length = 1000;
	char buffer[1000];

	CDialog::OnInitDialog();

	m_dataType.AddString("unsigned char");
	m_dataType.AddString("char");
	m_dataType.AddString("unsigned short");
	m_dataType.AddString("short");
	m_dataType.AddString("unsigned int");
	m_dataType.AddString("int");
	m_dataType.AddString("unsigned long");
	m_dataType.AddString("long");
	m_dataType.AddString("float");

	if (!symbol)
	{
		symbol = new FMSymbol();

		symbol->SetName("var");
//		m_name.SetWindowText("var");

		symbol->SetType(VARIABLE_REGISTER|VARIABLE_ASSEMBLER);
//		m_register.SetCheck(1);
//		m_memory.SetCheck(0);

		symbol->SetAddress(0);
//		m_address.SetWindowText("R0");

		symbol->SetScope(scope);
//		m_global.SetCheck(1);
//		m_scopeFrom.EnableWindow(FALSE);
//		m_scopeText.EnableWindow(FALSE);
//		m_scopeTo.EnableWindow(FALSE);

//		m_pointer.SetCheck(0);
//		m_array.SetCheck(0);
//		m_dimensionsText.EnableWindow(FALSE);
//		m_dimensions.EnableWindow(FALSE);
//		m_dataType.SetCurSel(0);

		BuildDataTypeString(buffer, &length);
		symbol->SetDataType(buffer);		// ImageCraft debug type code
	}
//	else

	SetSymbol();

	length = symbol->GetValueSize();
	itoa(length, buffer, 10);
	if (length == 1)
		strcat(buffer, " byte");
	else
		strcat(buffer, " bytes");
	m_size.SetWindowText(buffer);
	m_declaration.SetWindowText(symbol->GetDescription());
	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);

	return TRUE;
}


void CAsmVariableDialog::OnChangeAsmvariableName() 
{
	int i, j;
	char buffer[1000];
	CString str;
	char ch;
	bool badName = false;

	j = 0;
	m_name.GetWindowText(buffer, 1000);
	str = buffer;
	buffer[0] = 0;
	for (i=0; i<m_name.GetWindowTextLength(); i++)
	{
		ch = str.GetAt(i);
		if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') ||
			(ch >= 'a' && ch <= 'z') || ch == '_')	
		{								// If a valid name character
			buffer[j++] = ch;
		}
		else
			badName = true;
	}
	buffer[j] = 0;
	if (badName)
	{
		m_name.SetWindowText(buffer);
		AfxMessageBox(
"Valid characters for variable names are:\r\n\
a->z, A->Z, 0->9 & _", MB_OK|MB_ICONEXCLAMATION, 0);
	}

	symbol->SetName(buffer);
	m_declaration.SetWindowText(symbol->GetDescription());
}


void CAsmVariableDialog::OnChangeAsmvariableAddress() 
{
#ifdef AVARDEBUG
debug<<"CAsmVariableDialog::OnChangeAsmvariableAddress(): <-"<<endl;
#endif
	int i, j;
	char buffer[1000];
	CString str;
	char ch;
	bool badAddress = false;
	bool hex = false;
	bool reg = false;
	int address = 0;

	j = 0;
	m_address.GetWindowText(buffer, 1000);
	str = buffer;
	buffer[0] = 0;
	if (m_register.GetCheck())			// If a register variable
	{
		reg = true;
		if (str.GetAt(0) == 'R' || str.GetAt(0) == 'r')
			strcpy(buffer, str);
		else
			strcpy(buffer, "R0");
		j = 1;
		for (i=j; i<m_address.GetWindowTextLength(); i++)
		{
			ch = str.GetAt(i);
			if (ch >= '0' && ch <= '9')
				buffer[j++] = ch;
			else
				badAddress = true;
		}
	}
	else								// If a memory variable
	{
		if (str.GetAt(0) == '0' && str.GetAt(1) == 'x') // Hex value
		{
			hex = true;
			j = 2;
			strcpy(buffer, "0x0");
		}
		else
			strcpy(buffer, "0");
		for (i=j; i<m_address.GetWindowTextLength(); i++)
		{
			ch = str.GetAt(i);
			if ((ch >= '0' && ch <= '9') ||
				(hex && i<6 &&
				((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))))
			{								// If a valid character
				buffer[j++] = ch;
			}
			else
				badAddress = true;
		}
	}
	buffer[j] = 0;
	if (badAddress)
	{
		m_address.SetWindowText(buffer);
		AfxMessageBox(
"Addresses must be in the form \"0xHHHH\" where 'H' is\r\n\
a valid hex character (0->9, A->F & a->f) for hex values\r\n\
and \"99999\" where '9' is a valid decimal character\r\n\
(0->9) for decimal values", MB_OK|MB_ICONEXCLAMATION, 0);
	}

	if (hex)
		address = htoi(buffer);
	else
	{
		if (reg)
			address = atoi(&buffer[1]);
		else
			address = atoi(buffer);
	}
	if (address > 65536-symbol->GetValueSize())
	{
		address = 65536-symbol->GetValueSize();
		AfxMessageBox(
"The address you entered is too high to fit your variable\r\n\
into the available address space. It has been lowered to\r\n\
fit your variable in.", MB_OK|MB_ICONEXCLAMATION, 0);
	}
	symbol->SetAddress(address);

	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
#ifdef AVARDEBUG
debug<<"CAsmVariableDialog::OnChangeAsmvariableAddress(): ->"<<endl;
#endif
}


void CAsmVariableDialog::OnAsmvariableRegister() 
{
	char buffer[1000];

	symbol->SetType(VARIABLE_ASSEMBLER|VARIABLE_REGISTER);
	m_memory.SetCheck(0);
	m_register.SetCheck(1);
	if (symbol->GetAddress() > 31)
		symbol->SetAddress(31);
	m_address.SetWindowText(symbol->GetAddressString());

	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnAsmvariableMemory() 
{
	char buffer[1000];

	symbol->SetType(VARIABLE_ASSEMBLER|VARIABLE_STATIC);
	m_register.SetCheck(0);
	m_memory.SetCheck(1);
	m_address.SetWindowText(symbol->GetAddressString());

	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnAsmvariableGlobal() 
{
	char buffer[1000];

	if (m_global.GetCheck())
	{
		if (scope)
		{
			delete scope;
			scope = NULL;
		}
		symbol->SetScope(scope);
		m_scopeFrom.EnableWindow(FALSE);
		m_scopeText.EnableWindow(FALSE);
		m_scopeTo.EnableWindow(FALSE);
	}
	else
	{
		if (scope == NULL)
		{
			scope = new FMScopeBlock();
			scope->SetLastAddress(65535);
		}
		symbol->SetScope(scope);
		strcpy(buffer, "0x");
		itoh(scope->GetFirstAddress(), buffer, false, 4);
		m_scopeFrom.SetWindowText(buffer);
		buffer[2] = 0;
		itoh(scope->GetLastAddress(), buffer, false, 4);
		m_scopeTo.SetWindowText(buffer);
		m_scopeFrom.EnableWindow(TRUE);
		m_scopeText.EnableWindow(TRUE);
		m_scopeTo.EnableWindow(TRUE);
	}
	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnKillfocusAsmvariableScopefrom() 
{
	int i, j;
	char buffer[1000];
	CString str;
	char ch;
	bool badAddress = false;
	bool hex = false;
	int address = 0;

	j = 0;
	m_scopeFrom.GetWindowText(buffer, 1000);
	str = buffer;
	buffer[0] = 0;

	if (str.GetAt(0) == '0' && str.GetAt(1) == 'x') // Hex value
	{
		hex = true;
		j = 2;
		strcpy(buffer, "0x0");
	}
	else
		strcpy(buffer, "0");
	for (i=j; i<m_scopeFrom.GetWindowTextLength(); i++)
	{
		ch = str.GetAt(i);
		if ((ch >= '0' && ch <= '9') ||
			(hex && i<6 &&
			((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))))
		{								// If a valid character
			buffer[j++] = ch;
		}
		else
			badAddress = true;
	}

	buffer[j] = 0;
	if (badAddress)
	{
		AfxMessageBox(
"Addresses must be in the form \"0xHHHH\" where 'H' is\r\n\
a valid hex character (0->9, A->F & a->f) for hex values\r\n\
and \"99999\" where '9' is a valid decimal character\r\n\
(0->9) for decimal values", MB_OK|MB_ICONEXCLAMATION, 0);
	}

	if (hex)
		address = htoi(buffer);
	else
		address = atoi(buffer);
	scope->SetFirstAddress(address);
	m_scopeFrom.SetWindowText(buffer);

	address = scope->GetLastAddress();
	if (hex)
	{
		strcpy(buffer, "0x");
		itoh(address, buffer, false, 4);
	}
	else
		itoa(address, buffer, 10);
	m_scopeTo.SetWindowText(buffer);

	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnKillfocusAsmvariableScopeto() 
{
	int i, j;
	char buffer[1000];
	CString str;
	char ch;
	bool badAddress = false;
	bool hex = false;
	int address = 0;

	j = 0;
	m_scopeTo.GetWindowText(buffer, 1000);
	str = buffer;
	buffer[0] = 0;

	if (str.GetAt(0) == '0' && str.GetAt(1) == 'x') // Hex value
	{
		hex = true;
		j = 2;
		strcpy(buffer, "0x0");
	}
	else
		strcpy(buffer, "0");
	for (i=j; i<m_scopeTo.GetWindowTextLength(); i++)
	{
		ch = str.GetAt(i);
		if ((ch >= '0' && ch <= '9') ||
			(hex && i<6 &&
			((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f'))))
		{								// If a valid character
			buffer[j++] = ch;
		}
		else
			badAddress = true;
	}

	buffer[j] = 0;
	if (badAddress)
	{
		AfxMessageBox(
"Addresses must be in the form \"0xHHHH\" where 'H' is\r\n\
a valid hex character (0->9, A->F & a->f) for hex values\r\n\
and \"99999\" where '9' is a valid decimal character\r\n\
(0->9) for decimal values", MB_OK|MB_ICONEXCLAMATION, 0);
	}

	if (hex)
		address = htoi(buffer);
	else
		address = atoi(buffer);
	scope->SetLastAddress(address);
	m_scopeTo.SetWindowText(buffer);

	address = scope->GetFirstAddress();
	if (hex)
	{
		strcpy(buffer, "0x");
		itoh(address, buffer, false, 4);
	}
	else
		itoa(address, buffer, 10);
	m_scopeFrom.SetWindowText(buffer);

	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnAsmvariablePointer() 
{
	int length = 1000;
	char buffer[1000];

	BuildDataTypeString(buffer, &length);
	symbol->SetDataType(buffer);		// ImageCraft debug type code
	length = symbol->GetValueSize();
	itoa(length, buffer, 10);
	if (length == 1)
		strcat(buffer, " byte");
	else
		strcat(buffer, " bytes");
	m_size.SetWindowText(buffer);
	m_declaration.SetWindowText(symbol->GetDescription());
	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnAsmvariableArray() 
{
	int length = 1000;
	char buffer[1000];

	if (m_array.GetCheck())
	{
		if (m_dimensions.GetWindowTextLength() == 0)
			m_dimensions.SetWindowText("[1]");
		m_dimensionsText.EnableWindow(TRUE);
		m_dimensions.EnableWindow(TRUE);
	}
	else
	{
		m_dimensionsText.EnableWindow(FALSE);
		m_dimensions.EnableWindow(FALSE);
	}

	BuildDataTypeString(buffer, &length);
	symbol->SetDataType(buffer);		// ImageCraft debug type code
	length = symbol->GetValueSize();
	itoa(length, buffer, 10);
	if (length == 1)
		strcat(buffer, " byte");
	else
		strcat(buffer, " bytes");
	m_size.SetWindowText(buffer);
	m_declaration.SetWindowText(symbol->GetDescription());
	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnChangeAsmvariableDimensions() 
{
	int length = 1000;
	char buffer[1000];

	BuildDataTypeString(buffer, &length);
	symbol->SetDataType(buffer);		// ImageCraft debug type code
	length = symbol->GetValueSize();
	itoa(length, buffer, 10);
	if (length == 1)
		strcat(buffer, " byte");
	else
		strcat(buffer, " bytes");
	m_size.SetWindowText(buffer);
	m_declaration.SetWindowText(symbol->GetDescription());
	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnSelchangeAsmvariableDatatype() 
{
	int length = 1000;
	char buffer[1000];

	BuildDataTypeString(buffer, &length);
	symbol->SetDataType(buffer);		// ImageCraft debug type code
	length = symbol->GetValueSize();
	itoa(length, buffer, 10);
	if (length == 1)
		strcat(buffer, " byte");
	else
		strcat(buffer, " bytes");
	m_size.SetWindowText(buffer);
	m_declaration.SetWindowText(symbol->GetDescription());
	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
}


void CAsmVariableDialog::OnOK() 
{
	if (backupSymbol)
	{
		backupSymbol->SetName(symbol->GetName());
		backupSymbol->SetAddress(symbol->GetAddress());
		backupSymbol->SetType(symbol->GetType());
		backupSymbol->SetDataType(symbol->GetDataType());
		backupSymbol->SetScope(symbol->GetScope());
		symbol->scope = NULL;
		scope = NULL;
		delete symbol;
		symbol = backupSymbol;
	}
	CDialog::OnOK();
}


void CAsmVariableDialog::OnCancel() 
{
	delete symbol;
	symbol = NULL;
	scope = NULL;
	CDialog::OnCancel();
}
