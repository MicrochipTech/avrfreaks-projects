// AVR.cpp
// Studio+ family library for the Atmel AVR range of CPUs.

#include "stdafx.h"
#include "stdlib.h"
#include "resource.h"

#include "AVR.h"						// Chip definitions & info
#include "AVR_Ports.h"					// Chip Ports
#include "AVR_SFRs.h"					// Chip SFRs

// Exported function definitions

extern "C" __declspec(dllexport) int GetFamilyID(void);
extern "C" __declspec(dllexport) int GetFamilyName(char * name, int * max);
extern "C" __declspec(dllexport) int GetFamilyVersion(char * version, int * max);
extern "C" __declspec(dllexport) HBITMAP GetFamilyLogo(void);
extern "C" __declspec(dllexport) int GetNumberChips(void);
extern "C" __declspec(dllexport) int GetChipIDs(int * array, int * max);
extern "C" __declspec(dllexport) int GetChipIdIndex(int id);
extern "C" __declspec(dllexport) int GetChipNameLength(void);
extern "C" __declspec(dllexport) int GetChipNames(char * names, int * max);
extern "C" __declspec(dllexport) int GetChipPCSize(int id);
extern "C" __declspec(dllexport) int GetChipRegistersSize(int id);
extern "C" __declspec(dllexport) int GetChipSramStart(int id);
extern "C" __declspec(dllexport) int GetChipSramSize(int id);
extern "C" __declspec(dllexport) int GetChipEepromStart(int id);
extern "C" __declspec(dllexport) int GetChipEepromSize(int id);
extern "C" __declspec(dllexport) int GetNumberPins(int id, int package);
extern "C" __declspec(dllexport) int SetPinsToDefault(int * pins, int id,
	int package);
extern "C" __declspec(dllexport) int SetChipPortPins(int * pins,
	unsigned char * sfrs, int id, int package);
extern "C" __declspec(dllexport) int GetDefaultPackage(int id);
extern "C" __declspec(dllexport) int GetPackageIndex(int id, int package);
extern "C" __declspec(dllexport) HBITMAP GetChipImage(int id, int package);
extern "C" __declspec(dllexport) void GetChipImageSize(int id, int package,
	int * width, int * height);
extern "C" __declspec(dllexport) int GetIdFromName(char *);

extern "C" __declspec(dllexport) int GetNumberPackages(void);
extern "C" __declspec(dllexport) int GetPackageNameLength(void);
extern "C" __declspec(dllexport) int GetPackageNames(char * names, int * max);
extern "C" __declspec(dllexport) int GetAvailablePackages(int * packages,
	int * max, int id);
extern "C" __declspec(dllexport) int GetPackageIdFromName(char *);

extern "C" __declspec(dllexport) int GetNumberSFRs(int id);
extern "C" __declspec(dllexport) int GetSfrMarkers(int * markers, int id,
	int * max);
extern "C" __declspec(dllexport) int GetSfrNameLength();
extern "C" __declspec(dllexport) int GetSfrNames(char * names, int id,
	int * max);
extern "C" __declspec(dllexport) int GetSfrFlagMarkers(int * markers, int id,
	int * max);
extern "C" __declspec(dllexport) int GetSfrFlagNameLength();
extern "C" __declspec(dllexport) int GetSfrFlagNames(char * names, int id,
	int * max);

// Internal function definitions
void SetPortPins(int * pinFlags, unsigned char ddr, unsigned char port,
	unsigned char pins, int index, int first, int count, int ascending);

// DLL entry point
BOOL WINAPI DllEntryPoint(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}


// Exported Functions


__declspec(dllexport) int GetFamilyID()
{
	return FAMILY_ID;
}


__declspec(dllexport) int GetFamilyName(char * name, int * max)
{
	if (name && *max >= (int)strlen(FAMILY_NAME)+1)
	{
		strcpy(name, FAMILY_NAME);
		return TRUE;
	}
	*max = strlen(FAMILY_NAME)+1;
	return FALSE;
}


__declspec(dllexport) int GetFamilyVersion(char * version, int * max)
{
	if (version && *max >= (int)strlen(FAMILY_VERSION)+1)
	{
		strcpy(version, FAMILY_VERSION);
		return TRUE;
	}
	*max = strlen(FAMILY_VERSION)+1;
	return FALSE;
}


__declspec(dllexport) HBITMAP GetFamilyLogo()
{
	HINSTANCE hInstance = ::GetModuleHandle("AVR.dll");
	HBITMAP hBitmap = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_AVRLOGO));
	return hBitmap;
}


__declspec(dllexport) int GetNumberChips()
{
	return NUMBERCHIPS;
}


__declspec(dllexport) int GetChipIDs(int * array, int * max)
{
	if (array && *max >= NUMBERCHIPS)	// If enough room in array
	{
		for (int i=0; i<NUMBERCHIPS; i++)
			array[i] = ids[i];
		return TRUE;
	}
	*max = NUMBERCHIPS;					// Return needed size
	return FALSE;
}


__declspec(dllexport) int GetChipIdIndex(int id)
{
	int i = 0; 
	do
	{
		if (id == ids[i])
			return i;
		i++;
	}
	while (i < NUMBERCHIPS);
	return -1;
}


__declspec(dllexport) int GetChipNameLength(void)
{
	return CHIPNAMELENGTH;
}


__declspec(dllexport) int GetChipNames(char * names, int * max)
{
	if (names && *max >= (LASTCHIPID+1)*CHIPNAMELENGTH)
	{									// If enough room in array
		for (int i=0; i<LASTCHIPID+1; i++)
			strcpy(&names[i*CHIPNAMELENGTH], chipNames[i]);
		return TRUE;
	}
	*max = (LASTCHIPID+1)*CHIPNAMELENGTH; // Return needed size
	return FALSE;
}


__declspec(dllexport) int GetChipPCSize(int id)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return 0;
	return chipPC[id];
}


__declspec(dllexport) int GetChipRegistersSize(int id)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return 0;
	return chipRegisters[id];
}


__declspec(dllexport) int GetChipSramStart(int id)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return 0;
	return chipSramStart[id];
}


__declspec(dllexport) int GetChipSramSize(int id)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return 0;
	return chipSRAM[id];
}


__declspec(dllexport) int GetChipEepromStart(int id)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return 0;
	return chipEepromStart[id];
}


__declspec(dllexport) int GetChipEepromSize(int id)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return 0;
	return chipEEPROM[id];
}


__declspec(dllexport) int GetNumberPins(int id, int package)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return 0;
	return chipPins[id*6+package];
}


__declspec(dllexport) int SetPinsToDefault(int * pinFlags, int id, int package)
{
	// I'm assuming <pins> has been allocated with enough room!!
	int i = 0;

	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return FALSE;
	if (chipPins[(id*NUMBERPACKAGES)+package] == 0)
		return FALSE;
	
	int offset = 0;
	for (i=0; i<package; i++)
		offset += chipPins[(id*NUMBERPACKAGES)+i];
	for (i=0; i<chipPins[(id*NUMBERPACKAGES)+package]; i++)	
		pinFlags[i] = ((int *)(pinFlagPointers[id]+offset))[i];
	return TRUE;
}


__declspec(dllexport) int SetChipPortPins(int * pinFlags, unsigned char * sfrs,
	int id, int package)
{
	int i = 0;
	int ddr, port, pins;
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return FALSE;
	if (chipPins[(id*NUMBERPACKAGES)+package] == 0)
		return FALSE;
	int packageIndex = GetPackageIndex(id, package);
	int * portInfo = portInfoPointers[id];
	int rows = portInfo[packageIndex];
	int rowsIndex = 6;
	for (i=0; i<packageIndex; i++)
		rowsIndex += portInfo[i]*PORTINFOLENGTH;
	for (i=0; i<rows; i++)
	{
		ddr = portInfo[rowsIndex+(i*PORTINFOLENGTH)+0];
		if (ddr == -1)					// If not bi-directional
		{
			port = portInfo[rowsIndex+(i*PORTINFOLENGTH)+1];
			pins = portInfo[rowsIndex+(i*PORTINFOLENGTH)+2];
			if (port == -1)				// If input only
			{
				ddr = 0;
				pins = sfrs[pins];
			}
			if (pins == -1)				// If output only
			{
				ddr = 0xff;
				port = sfrs[port];
			}
		}
		else
		{
			ddr = sfrs[ddr];
			port = sfrs[portInfo[rowsIndex+(i*PORTINFOLENGTH)+1]];
			pins = sfrs[portInfo[rowsIndex+(i*PORTINFOLENGTH)+2]];
		}
		SetPortPins(pinFlags, ddr, port, pins,
			portInfo[rowsIndex+(i*PORTINFOLENGTH)+3],
			portInfo[rowsIndex+(i*PORTINFOLENGTH)+4],
			portInfo[rowsIndex+(i*PORTINFOLENGTH)+5],
			portInfo[rowsIndex+(i*PORTINFOLENGTH)+6]);
	}
	return TRUE;
}


__declspec(dllexport) int GetDefaultPackage(int id)
{
	return availablePackages[id*NUMBERPACKAGES]; // First available
}


__declspec(dllexport) int GetPackageIndex(int id, int package)
{
	int i = 0;
	int index = 0;
	for (i=0; i<NUMBERPACKAGES; i++)
	{
		if (availablePackages[(id*NUMBERPACKAGES)+i] == package)
			index = i;
	}
	return index;
}


__declspec(dllexport) HBITMAP GetChipImage(int id, int package)
{
	HINSTANCE hInstance = NULL;
	HBITMAP hBitmap = NULL;
	int resId = 0;

	resId = IDB_CHIP_BASE+(id*100)+package;
	hInstance = ::GetModuleHandle("AVR.dll");

//	if (!hInstance)
//		::MessageBox(NULL, "AVR.dll: GetChipImage(): No hInstance!!", "Error", MB_OK);

	hBitmap = ::LoadBitmap(hInstance, MAKEINTRESOURCE(resId));

//	if (!hBitmap)
//	{
//		char buffer[100];
//		strcpy(buffer, "AVR.dll: GetChipImage(): No Bitmap!!\r\n\r\n");
//		strcat(buffer, "resId = ");
//		itoa(resId, &buffer[strlen(buffer)], 10);
//		::MessageBox(NULL, buffer, "Error", MB_OK);
//	}
	
//	hBitmap = (HBITMAP)::LoadImage(hInstance, MAKEINTRESOURCE(resId),
//		IMAGE_BITMAP, 0, 0, 0);

	return hBitmap;
}


__declspec(dllexport) void GetChipImageSize(int id, int package, int * width,
	int * height)
{
	*width = imageSizes[(id*NUMBERPACKAGES*2)+(package*2)];
	*height = imageSizes[(id*NUMBERPACKAGES*2)+(package*2)+1];
}


__declspec(dllexport) int GetIdFromName(char * name)
{
	for (int i=FIRSTCHIPID; i<LASTCHIPID+1; i++)
	{
		if (strcmp(name, chipNames[i]) == 0)
			return i;
	}
	return 0;
}


__declspec(dllexport) int GetNumberPackages()
{
	return NUMBERPACKAGES;
}


__declspec(dllexport) int GetPackageNameLength(void)
{
	return PACKAGENAMELENGTH;
}


__declspec(dllexport) int GetPackageNames(char * names, int * max)
{
	if (names && *max >= NUMBERPACKAGES*PACKAGENAMELENGTH)
	{									// If enough room in array
		for (int i=0; i<NUMBERPACKAGES; i++)
			strcpy(&names[i*PACKAGENAMELENGTH], packageNames[i]);
		return TRUE;
	}
	*max = NUMBERPACKAGES*PACKAGENAMELENGTH; // Return needed size
	return FALSE;
}


__declspec(dllexport) int GetAvailablePackages(int * packages, int * max,
	int id)
{
	int i;

	if (packages && *max >= NUMBERPACKAGES)
	{
		for (i=0; i<NUMBERPACKAGES; i++)
			packages[i] = availablePackages[(id*NUMBERPACKAGES)+i];
	}
	*max = NUMBERPACKAGES;
	return false;
}


__declspec(dllexport) int GetPackageIdFromName(char * name)
{
	for (int i=0; i<NUMBERPACKAGES; i++)
	{
		if (strcmp(name, packageNames[i]) == 0)
			return i;
	}
	return 0;
}


__declspec(dllexport) int GetNumberSFRs(int id)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return -1;
	return sfrsSize[id];
}
	

__declspec(dllexport) int GetSfrMarkers(int * markers, int id, int * max)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return FALSE;
	if (markers && *max >= sfrsSize[id])
	{									// If enough room in array
		int * pointer = sfrsPointers[id];
		for (int i=0; i<sfrsSize[id]; i++)
			markers[i] = pointer[i];
		return TRUE;
	}
	*max = sfrsSize[id];				// Return needed size
	return FALSE;
}


__declspec(dllexport) int GetSfrNameLength()
{
	return SFRNAMELENGTH;
}


__declspec(dllexport) int GetSfrNames(char * names, int id, int * max)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return FALSE;
	if (names && *max >= sfrsSize[id]*SFRNAMELENGTH)
	{									// If enough room in array
		char * namesPointer = sfrNamesPointers[id];
		for (int i=0; i<sfrsSize[id]; i++)
			strcpy(&names[i*SFRNAMELENGTH], &namesPointer[i*SFRNAMELENGTH]);
		return TRUE;
	}
	*max = sfrsSize[id]*SFRNAMELENGTH;	// Return needed size
	return FALSE;
}


__declspec(dllexport) int GetSfrFlagMarkers(int * markers, int id, int * max)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return FALSE;
	if (markers && *max >= sfrsSize[id*8])
	{									// If enough room in array
		int * pointer = sfrFlagsPointers[id];
		for (int i=0; i<sfrsSize[id]*8; i++)
			markers[i] = pointer[i];
		return TRUE;
	}
	*max = sfrsSize[id*8];				// Return needed size
	return FALSE;
}


__declspec(dllexport) int GetSfrFlagNameLength()
{
	return SFRFLAGNAMELENGTH;
}


__declspec(dllexport) int GetSfrFlagNames(char * names, int id, int * max)
{
	if (id < FIRSTCHIPID || id > LASTCHIPID)
		return FALSE;
	if (names && *max >= sfrsSize[id]*8*SFRFLAGNAMELENGTH)
	{									// If enough room in array
		char * pointer = sfrFlagNamesPointers[id];
		for (int i=0; i<sfrsSize[id]*8; i++)
			strcpy(&names[i*SFRFLAGNAMELENGTH], &pointer[i*SFRFLAGNAMELENGTH]);
		return TRUE;
	}
	*max = sfrsSize[id]*8*SFRFLAGNAMELENGTH; // Return needed size
	return FALSE;
}



///////////////////////////////////////////////////////////////////////////////
// Internal functions

void SetPortPins(int * pinFlags, unsigned char ddr, unsigned char port,
	unsigned char pins, int index, int first, int count, int ascending)
{
	int i;
	int fIndex;

	i = first;
	do									// For each port pin
	{
		if (ascending)
			fIndex = index+(i-first);
		else
			fIndex = index+(count-(i-first));
		if ((ddr&(1<<i)))				// If pin is an output
		{
			if (port&(1<<i))			// If set high
				pinFlags[fIndex] = PINH;
			else
				pinFlags[fIndex] = PINL;
		}
		else							// If pin is an input
		{
			if (pins&(1<<i))			// If pin is high
				pinFlags[fIndex] = PINH;
			else
				pinFlags[fIndex] = PINL;
		}
		i++;
	}
	while (i < first+count);
}










// This is the constructor of a class that has been exported.
// see AVR.h for the class definition
//CAVR::CAVR()
//{ 
//	return; 
//}

// This is an example of an exported variable
//AVR_API int nAVR = 0;

// This is an example of an exported function.
//AVR_API int fnAVR(void)
//{
//	return 42;
//}

