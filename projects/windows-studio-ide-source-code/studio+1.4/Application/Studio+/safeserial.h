#if !defined(AFX_SAFESERIAL_H__99E9DE05_0BE7_11D6_B181_00E02950FE66__INCLUDED_)
#define AFX_SAFESERIAL_H__99E9DE05_0BE7_11D6_B181_00E02950FE66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CSafeSerial wrapper class

class CSafeSerial : public CWnd
{
protected:
	DECLARE_DYNCREATE(CSafeSerial)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xe242a783, 0xe945, 0x11d2, { 0xbd, 0xa2, 0x0, 0x0, 0xc0, 0xeb, 0x2a, 0x6a } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:

// Operations
public:
	void Loaded();
	void WriteLicenseType(long number);
	void WriteCounter(long number);
	long ReadLicenseType();
	long ReadInstallDate();
	long ReadMaxExecs();
	long ReadExecs();
	long readcounter();
	long ReadDaysAllowed();
	BOOL RegisterDlg();
	long Checklicense();
	long IsRegistered();
	BOOL Execution();
	void SafeTrans();
	void SafeImport();
	long GetEncodeNum1();
	void SetEncodeNum1(long nNewValue);
	long GetEncodeNum2();
	void SetEncodeNum2(long nNewValue);
	long GetEncodeNum3();
	void SetEncodeNum3(long nNewValue);
	long GetEncodeNum4();
	void SetEncodeNum4(long nNewValue);
	long GetLanguage();
	void SetLanguage(long nNewValue);
	CString GetProductName();
	void SetProductName(LPCTSTR lpszNewValue);
	CString GetCompanyName();
	void SetCompanyName(LPCTSTR lpszNewValue);
	CString GetAdressLine1();
	void SetAdressLine1(LPCTSTR lpszNewValue);
	CString GetAdressLine2();
	void SetAdressLine2(LPCTSTR lpszNewValue);
	long GetMaxExecs();
	void SetMaxExecs(long nNewValue);
	long GetCounter();
	void SetCounter(long nNewValue);
	long GetDaysAllowed();
	void SetDaysAllowed(long nNewValue);
	long GetLicenseType();
	void SetLicenseType(long nNewValue);
	BOOL GetDefaultMessage();
	void SetDefaultMessage(BOOL bNewValue);
	BOOL GetNetwork();
	void SetNetwork(BOOL bNewValue);
	BOOL GetLimitUsers();
	void SetLimitUsers(BOOL bNewValue);
	BOOL GetAutoTerminate();
	void SetAutoTerminate(BOOL bNewValue);
	long GetVersion();
	void SetVersion(long nNewValue);
	long GetExpiration();
	void SetExpiration(long nNewValue);
	CString CustomerCode();
	CString ValidateCode(LPCTSTR unlockcode);
	BOOL GetLogFile();
	void SetLogFile(BOOL bNewValue);
	long GetLastSSError();
	void SetLastSSError(long nNewValue);
	long ResetError();
	CString GetSSPath();
	void SetSSPath(LPCTSTR lpszNewValue);
	BOOL GetOldFormat();
	void SetOldFormat(BOOL bNewValue);
	BOOL GetVisible();
	void SetVisible(BOOL bNewValue);
	BOOL GetEnabled();
	void SetEnabled(BOOL bNewValue);
	short GetCursor();
	void SetCursor(short nNewValue);
	long GetDaysMultiplier();
	void SetDaysMultiplier(long nNewValue);
	long GetDaysLeft();
	void SetDaysLeft(long nNewValue);
	long GetUsersCount();
	void SetUsersCount(long nNewValue);
	void LogInUser();
	void LogOutUser();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAFESERIAL_H__99E9DE05_0BE7_11D6_B181_00E02950FE66__INCLUDED_)
