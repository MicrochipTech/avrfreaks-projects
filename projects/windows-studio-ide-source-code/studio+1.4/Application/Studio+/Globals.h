// Global Definitions

#ifndef GLOBALS_H
#define GLOBALS_H

//#define _RELEASE						// Comment out for TEST

#include "Global.h"						// Product wide defines
#include "IDEs.h"						// Product wide IDEs defines
#include "Familys.h"					// Product wide Family defines

#include "Studio.h"
#include "Project.h"					// FDProject class
#include "ProjectWindow.h"
#include "SourceWindow.h"
#include "SfrsWindow.h"
#include "WatchWindow.h"
#include "ChipWindow.h"
#include "SourceFile.h"
#include "SplashDialog.h"
#include "SafeSerial.h"					// CSafeSerial class

										// Debugging switches
//#define DEBUG							// Comment out for no debug
#ifdef DEBUG
#include "fstream.h"
#ifdef STUDIO
ofstream debug;
#else
extern ofstream debug;
#endif
//#define COMDEBUG						// Comms.cpp
//#define MAINDEBUG						// Studio.cpp
//#define MEMDEBUG						// Memory.cpp
//#define MFMDEBUG						// MainFrm.cpp
//#define PRJDEBUG						// Project.cpp
//#define PRJDDEBUG						// ProjectDialog.cpp
//#define REGDEBUG						// Registers.cpp
//#define SETDDEBUG						// SettingsDialog.cpp
//#define SFRDEBUG						// SfrsWindow.cpp
//#define SRCDEBUG						// Source.cpp
//#define SRCFDEBUG						// SourceFile.cpp
//#define STKDEBUG						// Stack.cpp
//#define SYMDEBUG						// Symbol.cpp
//#define WCHDDEBUG						// WatchDialog.cpp
//#define AVARDEBUG						// AsmVariableDialog.cpp
#endif

// Maximum number of IDEs & Familys
#define MAX_IDES 256
#define MAX_FAMILYS 256

// SafeSerial product codes
#ifdef _RELEASE
#define SS_CODE1 132
#define SS_CODE2 113
#define SS_CODE3 107
#define SS_CODE4 184
#else
#define SS_CODE1 66
#define SS_CODE2 77
#define SS_CODE3 88
#define SS_CODE4 99
#endif

// SafeSerial Option codes
#define SS_UNIVERSALMASK 0x2000
#define SS_OPTIONMASK 0x1E00
#define SS_BASIC 0x0200
#define SS_PROFESSIONAL 0x0400
#define SS_ENTERPRISE 0x0600
#define SS_ICESUPPORT 0x0800
#define SS_CPUMASK 0x01FF

// IDE library function definitions
typedef int (*GETIDEID)(void);
typedef int (*GETIDENAME)(char *, int *);
typedef int (*GETIDEVERSION)(char *, int *);
typedef HBITMAP (*GETIDELOGO)(void);
typedef int (*GETSUPPORTEDFAMILYS)(int *, int *);
typedef int (*GETFAMILYSUPPORTID)(int);
typedef int (*GETPROJECTFILEEXT)(char *, int *);
typedef int (*GETPROJECTFILEFILTER)(char *, int *);
typedef int (*ISPROJECTASSEMBLER)(const char *);
typedef int (*GETOUTPUTDIR)(char *, const char *, int *);
typedef int (*GETISD)(const char *, const char *, int *);
typedef int (*GETDEBUGINFO)(const char *, const char *, FMSourceFile **,
	int *);
typedef int (*DELETEDEBUGINFO)(FMSourceFile **, int);
typedef int (*GETCODEINFO)(const char *, const char *, FMSourceFile **, int,
	int *, int);
typedef int (*FINDHARDBREAKPOINTS)(const char *, const char *, FMSourceFile **,
	int, int);
typedef int (*GETASMCOMMENT)(char *, int *);
typedef char (*GETASMSTRINGCHAR)(void);
typedef char (*GETASMCHARCHAR)(void);
typedef char (*GETASMESCAPECHAR)(void);
typedef char (*GETASMDIRECTIVECHAR)(void);
typedef int (*GETASMDIRECTIVES)(char *, int *);
typedef int (*GETASMKEYWORDCHARS)(char *, int *);
typedef int (*GETASMKEYWORDS)(char *, int *);
typedef int (*GETCCOMMENT)(char *, int *);
typedef int (*GETCCOMMENTSTART)(char *, int *);
typedef int (*GETCCOMMENTEND)(char *, int *);
typedef char (*GETCSTRINGCHAR)(void);
typedef char (*GETCCHARCHAR)(void);
typedef char (*GETCESCAPECHAR)(void);
typedef char (*GETCDIRECTIVECHAR)(void);
typedef int (*GETCDIRECTIVES)(char *, int *);
typedef int (*GETCKEYWORDCHARS)(char *, int *);
typedef int (*GETCKEYWORDS)(char *, int *);

typedef int (*GETLASTISDERROR)(int *, char *, int *);

// Chip library function definitions
typedef int (*GETFAMILYID)(void);
typedef int (*GETFAMILYNAME)(char *, int *);
typedef int (*GETFAMILYVERSION)(char *, int *);
typedef HBITMAP (*GETFAMILYLOGO)(void);
typedef int (*GETNUMBERCHIPS)(void);
typedef int (*GETCHIPIDS)(int *, int *);
typedef int (*GETCHIPIDINDEX)(int);
typedef int (*GETCHIPNAMELENGTH)(void);
typedef int (*GETCHIPNAMES)(char *, int *);
typedef int (*GETCHIPPCSIZE)(int);
typedef int (*GETCHIPREGISTERSSIZE)(int);
typedef int (*GETCHIPSRAMSTART)(int);
typedef int (*GETCHIPSRAMSIZE)(int);
typedef int (*GETCHIPEEPROMSTART)(int);
typedef int (*GETCHIPEEPROMSIZE)(int);
typedef int (*GETNUMBERPINS)(int, int);
typedef int (*SETPINSTODEFAULT)(int *, int, int);
typedef int (*SETCHIPPORTPINS)(int *, unsigned char *, int, int);
typedef int (*GETDEFAULTPACKAGE)(int);
typedef int (*GETPACKAGEINDEX)(int, int);
typedef HBITMAP (*GETCHIPIMAGE)(int, int);
typedef void (*GETCHIPIMAGESIZE)(int, int, int *, int *);
typedef int (*GETIDFROMNAME)(char *);
typedef int (*GETNUMBERPACKAGES)(void);
typedef int (*GETPACKAGENAMELENGTH)(void);
typedef int (*GETPACKAGENAMES)(char *, int *);
typedef int (*GETAVAILABLEPACKAGES)(int *, int *, int);
typedef int (*GETPACKAGEIDFROMNAME)(char *);
typedef int (*GETNUMBERSFRS)(int);
typedef int (*GETSFRMARKERS)(int *, int, int *);
typedef int (*GETSFRNAMELENGTH)(void);
typedef int (*GETSFRNAMES)(char *, int, int *);
typedef int (*GETSFRFLAGMARKERS)(int *, int, int *);
typedef int (*GETSFRFLAGNAMELENGTH)(void);
typedef int (*GETSFRFLAGNAMES)(char *, int, int *);

// Registry Keys
#define PROJECTSKEY "Projects"

// Debugger codes
#define SENDPC 0
#define SENDREGISTERS 1
#define SENDSFRS 2
#define SENDSRAM 3
#define SENDEEPROM 4

// Studio+ Need-Data codes
#define NEEDREGISTERS 0x0001
#define NEEDSFRS 0x0002
#define NEEDSRAM 0x0004
#define NEEDEEPROM 0x0008
#define NEEDSYMBOLS 0x0010
#define NEEDSTACK 0x0020

// Debugger Modes
#define DM_STOP 0
#define DM_SINGLESTEP 1
#define DM_CSTEP 2
#define DM_STEPINTO 3
#define DM_STEPOVER 4
#define DM_STEPOUT 5
#define DM_STEPTOCURSOR 6
#define DM_ANIMATE 7
#define DM_TOSOFTBP 8
#define DM_COMPLEXBP 9
#define DM_TOHARDBP 10
#define DM_SNAPSHOT 11

// Background gradient colours
#define GRADIENTTOP 0x00cf977f
#define GRADIENTBOTTOM 0x00ffffff

// Stack colours
#define STACKADDRESSCOLOUR 0x00ffd0c0	// Pale purple
#define STACKPOINTERCOLOUR 0x008080ff	// Red
#define NOTSTACKCOLOUR 0x00e0ffff		// Very pale yellow
#define DEBUGSTACKCOLOUR 0x00e0e0e0		// Very pale grey

// Default Type sizes
#define BYTESIZE 1
#define CHARSIZE 1
#define SHORTSIZE 2
#define INTSIZE 2

// Default Comms Defines
// Data sizes
//#define BLOCKSIZE 256
#define OLDINFOBLOCKSIZE 10
#define MAXPCSIZE 4
#define MAXREGISTERSSIZE 37
#define MAXSFRSSIZE 224
#define MAXSRAMSIZE 65536
#define MAXEEPROMSIZE 65536
#define RXBUFFERSIZE 37+65536+65536
#define RXTIMEOUT 500					// Default Comms Timeout period (500ms)

// Pin state flags
#define PS_NOPIN 1
#define PS_UNKNOWN 2
#define PS_LOW 4
#define PS_HIGH 8
#define PS_XTAL 16
#define PS_ANALOGUE 32

// Global Settings Structure Definition
struct GlobalSettings
{
	int workingDrive;
	char * workingDirectory;
//	int usersDrive;
//	char * usersDirectory;

	int registered;
	int universal;
	int options;
	int cpus;
	int ides;

	int syntaxHiLighting;
	int pcTracking;
	int assembler;
	int addresses;
	int machineCode;

	int commsTimeout;
};

#ifdef STUDIO

// Global functions
void ShowComIndicators();
int ahtoi(char * ascii);
int htoi(char * hex);
char * itoh(int i, char * hex, bool trim, int chars);
void ErrorMessage();

// Global variables
HINSTANCE hInstance;					// Application instance
HINSTANCE hFamilyDll;					// Family library instance
HINSTANCE hIdeDll;						// IDE library handle

char * instDir;							// Installation Directory

int numberIDEs;							// Number of Supported IDEs
int numberFamilys;						// Number of Supported Familys
//int * ideFamilyTable;					// Lookup table for compatibility
			// FAMILY / IDE
			//          AVR Studio   ImageCraft  Generic 8051 Asm  Kiel C
			//         _____________________________________________________
			//   AVR  |   0x01     |     0x02   |       -1        |   -1
			//  8051  |    -1      |      -1    |     0x01        |   0x02


CSafeSerial * safeSerial;				// Protection mechanism

HWND mainWindowHandle;					// Main window handle
HWND clientAreaHandle;					// Main window client area

int errorCode;							// Global error code
char * errorMessage;					// Global error message

GlobalSettings globals;					// Global settings

int ideFlags;							// IDE available bit flags
int * ideIDs;							// IDE IDs
char ** ideNames;						// IDE Names
char ** ideVersions;					// IDE Versions

GETIDEID ILGetIdeID;					// IDE Library functions
GETIDENAME ILGetIdeName;
GETIDEVERSION ILGetIdeVersion;
GETIDELOGO ILGetIdeLogo;
GETSUPPORTEDFAMILYS ILGetSupportedFamilys;
GETFAMILYSUPPORTID ILGetFamilySupportID;
GETPROJECTFILEEXT ILGetProjectFileExt;
GETPROJECTFILEFILTER ILGetProjectFileFilter;
ISPROJECTASSEMBLER ILIsProjectAssembler;
GETOUTPUTDIR ILGetOutputDir;
GETISD ILGetISD;
GETDEBUGINFO ILGetDebugInfo;
DELETEDEBUGINFO ILDeleteDebugInfo;
GETCODEINFO ILGetCodeInfo;
FINDHARDBREAKPOINTS ILFindHardBreakpoints;
GETASMCOMMENT ILGetAsmComment;
GETASMSTRINGCHAR ILGetAsmStringChar;
GETASMCHARCHAR ILGetAsmCharChar;
GETASMESCAPECHAR ILGetAsmEscapeChar;
GETASMDIRECTIVECHAR ILGetAsmDirectiveChar;
GETASMDIRECTIVES ILGetAsmDirectives;
GETASMKEYWORDCHARS ILGetAsmKeywordChars;
GETASMKEYWORDS ILGetAsmKeywords;
GETCCOMMENT ILGetCComment;
GETCCOMMENTSTART ILGetCCommentStart;
GETCCOMMENTEND ILGetCCommentEnd;
GETCSTRINGCHAR ILGetCStringChar;
GETCCHARCHAR ILGetCCharChar;
GETCESCAPECHAR ILGetCEscapeChar;
GETCDIRECTIVECHAR ILGetCDirectiveChar;
GETCDIRECTIVES ILGetCDirectives;
GETCKEYWORDCHARS ILGetCKeywordChars;
GETCKEYWORDS ILGetCKeywords;

GETLASTISDERROR ILGetLastIsdError;

int * familyIDs;						// Family IDs
char ** familyNames;					// Family Names
char ** familyVersions;					// Family Versions

int numberChips;						// Number of chips
int * chipIDs;							// Chip IDs
int chipNameLength;						// Length of a chip name
char * chipNames;						// Chip Names
int numberPackages;						// Number of packages
int packageNameLength;					// Length of a package name
char * packageNames;					// Package Names

GETFAMILYID CLGetFamilyID;				// Chip library function pointers
GETFAMILYNAME CLGetFamilyName;
GETFAMILYVERSION CLGetFamilyVersion;
GETFAMILYLOGO CLGetFamilyLogo;
GETNUMBERCHIPS CLGetNumberChips;
GETCHIPIDS CLGetChipIDs;
GETCHIPIDINDEX CLGetChipIdIndex;
GETCHIPNAMELENGTH CLGetChipNameLength;
GETCHIPNAMES CLGetChipNames;
GETCHIPPCSIZE CLGetChipPCSize;
GETCHIPREGISTERSSIZE CLGetChipRegistersSize;
GETCHIPSRAMSTART CLGetChipSramStart;
GETCHIPSRAMSIZE CLGetChipSramSize;
GETCHIPEEPROMSTART CLGetChipEepromStart;
GETCHIPEEPROMSIZE CLGetChipEepromSize;
GETNUMBERPINS CLGetNumberPins;
SETPINSTODEFAULT CLSetPinsToDefault;
SETCHIPPORTPINS CLSetChipPortPins;
GETDEFAULTPACKAGE CLGetDefaultPackage;
GETPACKAGEINDEX CLGetPackageIndex;
GETCHIPIMAGE CLGetChipImage;
GETCHIPIMAGESIZE CLGetChipImageSize;
GETIDFROMNAME CLGetIdFromName;
GETNUMBERPACKAGES CLGetNumberPackages;
GETPACKAGENAMELENGTH CLGetPackageNameLength;
GETPACKAGENAMES CLGetPackageNames;
GETAVAILABLEPACKAGES CLGetAvailablePackages;
GETPACKAGEIDFROMNAME CLGetPackageIdFromName;
GETNUMBERSFRS CLGetNumberSFRs;
GETSFRMARKERS CLGetSfrMarkers;
GETSFRNAMELENGTH CLGetSfrNameLength;
GETSFRNAMES CLGetSfrNames;
GETSFRFLAGMARKERS CLGetSfrFlagMarkers;
GETSFRFLAGNAMELENGTH CLGetSfrFlagNameLength;
GETSFRFLAGNAMES CLGetSfrFlagNames;

FDProject * project;					// Current project

FMSourceFile * cursorFile;				// Source cursor
int cursorLine;

int pc;									// PC
FMSourceFile * pcFile;
FMSourceFile * oldPCFile;
int pcLine;
int stackAddress;						// Stack start address
int stackPointer;						// Stack pointer
int framePointer;						// Frame pointer

FMSourceFile * cStepFile;				// C-Step
int cStepLine;

CProjectWindow * projectWindow;			// Project window
bool projectWindowOpen;

CSourceWindow ** sourceWindows;			// SourceWindows

CSfrsWindow * sfrsWindow;				// SFRs window
bool sfrsWindowOpen;

CWatchWindow * watchWindow;				// Watch window
bool watchWindowOpen;

CChipWindow * chipWindow;				// Chip window
bool chipWindowOpen;

int pcSize;								// Data buffer sizes
int registersSize;
int sfrsSize;
int sramStart;
int sramSize;
int eepromStart;
int eepromSize;
unsigned char * registers;				// Data buffers
unsigned char * sfrs;
unsigned char * sram;
unsigned char * eeprom;
unsigned char * registersMask;			// Data Masks
unsigned char * sfrsMask;
unsigned char * sramMask;
unsigned char * eepromMask;

int targetFamily;						// Target CPU family
int targetCPU;							// Target CPU

bool debugRunning;						// Debug info
bool gettingData;
bool waitingForD;
bool endCondition;
bool needInfoBlock;
int debugMode;
unsigned char controlChar;
int askedFor;
int expected;
int firstWatchSymbol;
int lastWatchSymbol;
bool updateRegisters;
bool updateSfrs;
bool updateSram;
bool updateEeprom;
bool updateStack;

int numComPorts;
int * availableComPorts;
int * comPortIndexes;
int dataIndex;
unsigned char * rxBuffer;
int rxCount;
int totalRxd;

#else

extern CStudioApp theApp;				// The Application

extern void ShowComIndicators();
extern int ahtoi(char * ascii);
extern int htoi(char * hex);
extern char * itoh(int i, char * hex, bool trim, int chars);
extern void ErrorMessage();

extern HINSTANCE hInstance;
extern HINSTANCE hFamilyDll;
extern HINSTANCE hIdeDll;

extern char * instDir;

extern int numberIDEs;
extern int numberFamilys;
extern int * ideFamilyLookupTable;

extern CSafeSerial * safeSerial;

extern HWND mainWindowHandle;
extern HWND clientAreaHandle;

extern int errorCode;
extern char * errorMessage;

extern GlobalSettings globals;

extern int ideFlags;
extern int * ideIDs;
extern char ** ideNames;
extern char ** ideVersions;

extern GETIDEID ILGetIdeID;
extern GETIDENAME ILGetIdeName;
extern GETIDEVERSION ILGetIdeVersion;
extern GETIDELOGO ILGetIdeLogo;
extern GETSUPPORTEDFAMILYS ILGetSupportedFamilys;
extern GETFAMILYSUPPORTID ILGetFamilySupportID;
extern GETPROJECTFILEEXT ILGetProjectFileExt;
extern GETPROJECTFILEFILTER ILGetProjectFileFilter;
extern ISPROJECTASSEMBLER ILIsProjectAssembler;
extern GETOUTPUTDIR ILGetOutputDir;
extern GETISD ILGetISD;
extern GETDEBUGINFO ILGetDebugInfo;
extern DELETEDEBUGINFO ILDeleteDebugInfo;
extern GETCODEINFO ILGetCodeInfo;
extern FINDHARDBREAKPOINTS ILFindHardBreakpoints;
extern GETASMCOMMENT ILGetAsmComment;
extern GETASMSTRINGCHAR ILGetAsmStringChar;
extern GETASMCHARCHAR ILGetAsmCharChar;
extern GETASMESCAPECHAR ILGetAsmEscapeChar;
extern GETASMDIRECTIVECHAR ILGetAsmDirectiveChar;
extern GETASMDIRECTIVES ILGetAsmDirectives;
extern GETASMKEYWORDCHARS ILGetAsmKeywordChars;
extern GETASMKEYWORDS ILGetAsmKeywords;
extern GETCCOMMENT ILGetCComment;
extern GETCCOMMENTSTART ILGetCCommentStart;
extern GETCCOMMENTEND ILGetCCommentEnd;
extern GETCSTRINGCHAR ILGetCStringChar;
extern GETCCHARCHAR ILGetCCharChar;
extern GETCESCAPECHAR ILGetCEscapeChar;
extern GETCDIRECTIVECHAR ILGetCDirectiveChar;
extern GETCDIRECTIVES ILGetCDirectives;
extern GETCKEYWORDCHARS ILGetCKeywordChars;
extern GETCKEYWORDS ILGetCKeywords;

extern GETLASTISDERROR ILGetLastIsdError;

extern int * familyIDs;
extern char ** familyNames;
extern char ** familyVersions;

extern int numberChips;
extern int * chipIDs;
extern int chipNameLength;
extern char * chipNames;
extern int numberPackages;
extern int packageNameLength;
extern char * packageNames;

extern GETFAMILYID CLGetFamilyID;
extern GETFAMILYNAME CLGetFamilyName;
extern GETFAMILYVERSION CLGetFamilyVersion;
extern GETFAMILYLOGO CLGetFamilyLogo;
extern GETNUMBERCHIPS CLGetNumberChips;
extern GETCHIPIDS CLGetChipIDs;
extern GETCHIPIDINDEX CLGetChipIdIndex;
extern GETCHIPNAMELENGTH CLGetChipNameLength;
extern GETCHIPNAMES CLGetChipNames;
extern GETCHIPPCSIZE CLGetChipPCSize;
extern GETCHIPREGISTERSSIZE CLGetChipRegistersSize;
extern GETCHIPSRAMSTART CLGetChipSramStart;
extern GETCHIPSRAMSIZE CLGetChipSramSize;
extern GETCHIPEEPROMSTART CLGetChipEepromStart;
extern GETCHIPEEPROMSIZE CLGetChipEepromSize;
extern GETNUMBERPINS CLGetNumberPins;
extern SETPINSTODEFAULT CLSetPinsToDefault;
extern SETCHIPPORTPINS CLSetChipPortPins;
extern GETDEFAULTPACKAGE CLGetDefaultPackage;
extern GETPACKAGEINDEX CLGetPackageIndex;
extern GETCHIPIMAGE CLGetChipImage;
extern GETCHIPIMAGESIZE CLGetChipImageSize;
extern GETIDFROMNAME CLGetIdFromName;
extern GETNUMBERPACKAGES CLGetNumberPackages;
extern GETPACKAGENAMELENGTH CLGetPackageNameLength;
extern GETPACKAGENAMES CLGetPackageNames;
extern GETAVAILABLEPACKAGES CLGetAvailablePackages;
extern GETPACKAGEIDFROMNAME CLGetPackageIdFromName;
extern GETNUMBERSFRS CLGetNumberSFRs;
extern GETSFRMARKERS CLGetSfrMarkers;
extern GETSFRNAMELENGTH CLGetSfrNameLength;
extern GETSFRNAMES CLGetSfrNames;
extern GETSFRFLAGMARKERS CLGetSfrFlagMarkers;
extern GETSFRFLAGNAMELENGTH CLGetSfrFlagNameLength;
extern GETSFRFLAGNAMES CLGetSfrFlagNames;

extern FDProject * project;

extern FMSourceFile * cursorFile;
extern int cursorLine;

extern int pc;
extern FMSourceFile * pcFile;
extern FMSourceFile * oldPCFile;
extern int pcLine;
extern int stackAddress;
extern int stackPointer;
extern int framePointer;

extern FMSourceFile * cStepFile;
extern int cStepLine;

extern CProjectWindow * projectWindow;
extern bool projectWindowOpen;

extern CSourceWindow ** sourceWindows;

extern CSfrsWindow * sfrsWindow;
extern bool sfrsWindowOpen;

extern CWatchWindow * watchWindow;
extern bool watchWindowOpen;

extern CChipWindow * chipWindow;
extern bool chipWindowOpen;

extern int pcSize;
extern int registersSize;
extern int sfrsSize;
extern int sramStart;
extern int sramSize;
extern int eepromStart;
extern int eepromSize;
extern unsigned char * registers;
extern unsigned char * sfrs;
extern unsigned char * sram;
extern unsigned char * eeprom;
extern unsigned char * registersMask;
extern unsigned char * sfrsMask;
extern unsigned char * sramMask;
extern unsigned char * eepromMask;

extern int targetFamily;
extern int targetCPU;

extern bool debugRunning;
extern bool gettingData;
extern bool waitingForD;
extern bool endCondition;
extern bool needInfoBlock;
extern int debugMode;
extern unsigned char controlChar;
extern int askedFor;
extern int expected;
extern int firstWatchSymbol;
extern int lastWatchSymbol;
extern bool updateRegisters;
extern bool updateSfrs;
extern bool updateSram;
extern bool updateEeprom;
extern bool updateStack;

extern int numComPorts;
extern int * availableComPorts;
extern int * comPortIndexes;
extern int dataIndex;
extern unsigned char * rxBuffer;
extern int rxCount;
extern int totalRxd;

#endif

#endif // GLOBALS_H
