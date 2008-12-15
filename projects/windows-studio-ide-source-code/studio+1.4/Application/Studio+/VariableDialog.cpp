// VariableDialog.cpp : implementation file
//

#include "stdafx.h"
#include "studio.h"
#include "VariableDialog.h"
#include "Symbol.h"
#include "Function.h"
#include "ScopeBlock.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVariableDialog dialog


CVariableDialog::CVariableDialog(CWnd* pParent /*=NULL*/, FMSymbol * s)
	: CDialog(CVariableDialog::IDD, pParent)
{
	symbol = s;

	//{{AFX_DATA_INIT(CVariableDialog)
	//}}AFX_DATA_INIT
}


void CVariableDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVariableDialog)
	DDX_Control(pDX, IDC_VARIABLE_LINE, m_line);
	DDX_Control(pDX, IDC_VARIABLE_FUNCTIONTEXT, m_functionText);
	DDX_Control(pDX, IDC_VARIABLE_FUNCTION, m_function);
	DDX_Control(pDX, IDC_VARIABLE_DIMENSIONSTEXT, m_dimensionsText);
	DDX_Control(pDX, IDC_VARIABLE_DIMENSIONS, m_dimensions);
	DDX_Control(pDX, IDC_VARIABLE_DECLARATION, m_declaration);
	DDX_Control(pDX, IDC_VARIABLE_DATATYPE, m_dataType);
	DDX_Control(pDX, IDC_VARIABLE_ARRAY, m_array);
	DDX_Control(pDX, IDC_VARIABLE_POINTER, m_pointer);
	DDX_Control(pDX, IDC_VARIABLE_SCOPEFROM, m_scopeFrom);
	DDX_Control(pDX, IDC_VARIABLE_SCOPETEXT, m_scopeText);
	DDX_Control(pDX, IDC_VARIABLE_SCOPETO, m_scopeTo);
	DDX_Control(pDX, IDC_VARIABLE_SIZE, m_size);
	DDX_Control(pDX, IDC_VARIABLE_VALUE, m_value);
	DDX_Control(pDX, IDC_VARIABLE_SOURCE, m_source);
	DDX_Control(pDX, IDC_VARIABLE_GLOBAL, m_global);
	DDX_Control(pDX, IDC_VARIABLE_ADDRESS, m_address);
	DDX_Control(pDX, IDC_VARIABLE_NAME, m_name);
	DDX_Control(pDX, IDC_VARIABLE_REGISTER, m_register);
	DDX_Control(pDX, IDC_VARIABLE_MEMORY, m_memory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVariableDialog, CDialog)
	//{{AFX_MSG_MAP(CVariableDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVariableDialog message handlers


BOOL CVariableDialog::OnInitDialog() 
{
	int i;
	char buffer[1000];
	int length;

	CDialog::OnInitDialog();

	if (symbol == NULL)
		return TRUE;
	m_name.SetWindowText(symbol->GetName());
	m_address.SetWindowText(symbol->GetAddressString());
	if (symbol->GetType()&VARIABLE_REGISTER)
	{
		m_register.SetCheck(1);
		m_memory.SetCheck(0);
	}
	else
	{
		m_memory.SetCheck(1);
		m_register.SetCheck(0);
	}
	m_source.SetWindowText(symbol->GetFile()->GetFileName());
	strcpy(buffer, "Declared on line ");
	i = symbol->GetSourceLine()+1;
	itoa(i, &buffer[strlen(buffer)], 10);
	m_line.SetWindowText(buffer);
	FMScopeBlock * scope = symbol->GetScope();
	if (!scope)
	{
		m_global.SetCheck(1);
		m_functionText.ShowWindow(SW_HIDE);
		m_function.ShowWindow(SW_HIDE);
		m_scopeFrom.ShowWindow(SW_HIDE);
		m_scopeText.ShowWindow(SW_HIDE);
		m_scopeTo.ShowWindow(SW_HIDE);
	}
	else
	{
		m_global.SetCheck(0);
		m_functionText.ShowWindow(SW_SHOW);
		m_function.ShowWindow(SW_SHOW);
		m_scopeFrom.ShowWindow(SW_SHOW);
		m_scopeText.ShowWindow(SW_SHOW);
		m_scopeTo.ShowWindow(SW_SHOW);
		m_function.SetWindowText(scope->GetFunction()->GetName());
		strcpy(buffer, "0x");
		itoh(scope->GetFirstAddress(), buffer, false, 4);
		m_scopeFrom.SetWindowText(buffer);
		buffer[2] = 0;
		itoh(scope->GetLastAddress(), buffer, false, 4);
		m_scopeTo.SetWindowText(buffer);
	}
	m_dataType.SetWindowText(symbol->DataTypeString());
	if (symbol->IsPointer())
		m_pointer.SetCheck(1);
	else
		m_pointer.SetCheck(0);
	if (symbol->IsArray())
	{
		m_array.SetCheck(1);
		m_dimensionsText.ShowWindow(SW_SHOW);
		m_dimensions.ShowWindow(SW_SHOW);
		buffer[0] = 0;
		for (i=0; i<symbol->GetNumDimensions(); i++)
		{
			strcat(buffer, "[");
			itoa(symbol->GetDimensionSize(i), &buffer[strlen(buffer)], 10);
			strcat(buffer, "]");
		}
		m_dimensions.SetWindowText(buffer);
	}
	else
	{
		m_array.SetCheck(0);
		m_dimensionsText.ShowWindow(SW_HIDE);
		m_dimensions.ShowWindow(SW_HIDE);
	}
	length = symbol->GetValueSize();
	itoa(length, buffer, 10);
	if (length == 1)
		strcat(buffer, " byte");
	else
		strcat(buffer, " bytes");
	m_size.SetWindowText(buffer);
	symbol->UpdateValue();
	symbol->GetValueString(buffer, 1000);
	m_value.SetWindowText(buffer);
	m_declaration.SetWindowText(symbol->GetDescription());

	return TRUE;
}
