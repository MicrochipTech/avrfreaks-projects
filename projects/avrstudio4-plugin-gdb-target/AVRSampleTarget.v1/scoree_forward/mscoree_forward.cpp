// mscoree_forward.cpp : Definiert die exportierten Funktionen für die DLL-Anwendung.
//

#include "stdafx.h"

//
// load setup file
//
#include "mscoree_forward.h"


//////////////// nothing to be done down here:

#pragma managed

#using <System.dll>
#using <System.Windows.Forms.dll>

void managedDllRegisterServer()
{
	System::Reflection::Assembly ^ myass = System::Reflection::Assembly::GetExecutingAssembly();
	System::IO::FileInfo ^ myfi = gcnew System::IO::FileInfo(myass->Location);

	try
	{
		System::Reflection::Assembly ^ ass = System::Reflection::Assembly::LoadFile( myfi->Directory->FullName + "\\" + myCOMdll );
		System::Type ^ t = ass->GetType( myCOMclass );
		System::Object ^ o = ass->CreateInstance( "AVRStudioSDK.AvrStudioPlugin" );
		o->GetType()->InvokeMember("ComRegisterFunction_mscoree_forward", System::Reflection::BindingFlags::InvokeMethod, nullptr,o, gcnew array<System::Object^> {t, true} );
	}
	catch( System::Exception ^ e)
	{
		System::Windows::Forms::MessageBox::Show( e->ToString(), "managedDllRegisterServer failed");
	}
	//myCompany::myPlugin::ComRegisterFunction(myCompany::myPlugin::typeid);
}

void managedDllUnregisterServer()
{
	System::Reflection::Assembly ^ myass = System::Reflection::Assembly::GetExecutingAssembly();
	System::IO::FileInfo ^ myfi = gcnew System::IO::FileInfo(myass->Location);

	try
	{
		System::Reflection::Assembly ^ ass = System::Reflection::Assembly::LoadFile( myfi->Directory->FullName + "\\" + myCOMdll );
		System::Type ^ t = ass->GetType( myCOMclass );
		System::Object ^ o = ass->CreateInstance( "AVRStudioSDK.AvrStudioPlugin" );
		o->GetType()->InvokeMember("ComUnregisterFunction", System::Reflection::BindingFlags::InvokeMethod, nullptr, o, gcnew array<System::Object^> {t} );
	}
	catch( System::Exception ^ e)
	{
		System::Windows::Forms::MessageBox::Show( e->ToString(), "managedDllUnregisterServer failed");
	}
	//myCompany::myPlugin::ComUnregisterFunction(myCompany::myPlugin::typeid);
};

void managedGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	System::Reflection::Assembly ^ myass = System::Reflection::Assembly::GetExecutingAssembly();
	System::IO::FileInfo ^ myfi = gcnew System::IO::FileInfo(myass->Location);

	try
	{
		System::Reflection::Assembly ^ ass = System::Reflection::Assembly::LoadFile( myfi->Directory->FullName + "\\" + myCOMdll );
		System::Type ^ t = ass->GetType( myCOMclass );
		System::Object ^ o = ass->CreateInstance( "AVRStudioSDK.COMfactory" );
		o->GetType()->InvokeMember("Typ", System::Reflection::BindingFlags::SetProperty, nullptr, o, gcnew array<System::Object^> {t} );
		*ppv = (LPVOID)System::Runtime::InteropServices::Marshal::GetComInterfaceForObject(o, ass->GetType( "AVRStudioSDK.IClassFactory")).ToPointer();
	}
	catch( System::Exception ^ e)
	{
		System::Windows::Forms::MessageBox::Show( e->ToString(), "managedGetClassObject failed");
	}
	// get IID_IClassFactory (not the actual class...)
	//AVRStudioSDK::COMfactory ^ pF = gcnew AVRStudioSDK::COMfactory;
	//pF->Typ = myCompany::myPlugin::typeid;
	//*ppv = (LPVOID)System::Runtime::InteropServices::Marshal::GetComInterfaceForObject(pF, AVRStudioSDK::IClassFactory::typeid).ToPointer();
}

#pragma unmanaged

#include "mscoree_forward.h"
#include <stdio.h>
#include "olectl.h"

extern "C" {

typedef HRESULT(__stdcall *mscoreeGetClassObject_ftype)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
__declspec(dllexport) HRESULT __stdcall myDllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	HRESULT hr = S_OK;
	//MessageBox(NULL, L"DllGetClassObject", L"DllGetClassObject start", MB_OK);

	// call mscoree.dll
	HMODULE	Hmscoree = LoadLibrary(L"mscoree.dll");
	mscoreeGetClassObject_ftype mscoreeGetClassObject = 0;
	mscoreeGetClassObject = (mscoreeGetClassObject_ftype)GetProcAddress( Hmscoree, "DllGetClassObject");
	hr = mscoreeGetClassObject(rclsid, riid, ppv);

	//managedGetClassObject(rclsid, riid, ppv);

	//WCHAR msg[2000];
	//swprintf(msg, 2000, L"hr=%x ppv = 0x%x *ppv=0x%0x", hr, (long)ppv, (long)*ppv);
	//MessageBox(NULL, msg, L"DllGetClassObject done", MB_OK);
    return hr;
}

__declspec(dllexport) HRESULT __stdcall myDllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
	//MessageBox(NULL, L"DllRegisterServer", L"DllRegisterServer start", MB_OK);
	managedDllRegisterServer();
	//MessageBox(NULL, L"DllRegisterServer", L"DllRegisterServer done", MB_OK);
    return S_OK;
}

__declspec(dllexport) HRESULT __stdcall myDllUnregisterServer(void)
{
	//MessageBox(NULL, L"DllUnregisterServer", L"DllUnregisterServer start", MB_OK);
	managedDllUnregisterServer();
	//MessageBox(NULL, L"DllUnregisterServer", L"DllUnregisterServer done", MB_OK);
	return S_OK;
}

__declspec(dllexport) HRESULT __stdcall myDllCanUnloadNow(void)
{
	//MessageBox(NULL, L"DllCanUnloadNow", L"DllCanUnloadNow", MB_OK);
	return S_OK;
}

} //extern "C"
