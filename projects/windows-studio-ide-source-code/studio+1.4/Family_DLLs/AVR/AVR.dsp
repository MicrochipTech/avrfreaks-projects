# Microsoft Developer Studio Project File - Name="AVR" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AVR - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVR.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVR.mak" CFG="AVR - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVR - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVR - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AVR - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release"
# PROP Intermediate_Dir "Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVR_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "D:\Projects\Freelance\Flash_Designs\Studio+\1.4\Global" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVR_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "AVR - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Intermediate"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVR_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "D:\Products\Freelance\Flash_Designs\Studio+\1.3\include" /I "D:\Projects\Freelance\Flash_Designs\Studio+\1.4\Global" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVR_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AVR - Win32 Release"
# Name "AVR - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AVR.cpp
# End Source File
# Begin Source File

SOURCE=.\avr.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AVR.h
# End Source File
# Begin Source File

SOURCE=.\AVR_Ports.h
# End Source File
# Begin Source File

SOURCE=.\AVR_SFRs.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\res\Chips\AT90C8534-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S1200-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S1200-SOIC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S1200-SSOP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S2313-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S2313-SOIC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S2323-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S2323-SOIC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S2333-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S2333-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S2343-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S2343-SOIC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S4414-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S4414-PLCC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S4414-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S4433-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S4433-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S4434-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S4434-PLCC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S4434-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S8515-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S8515-PLCC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S8515-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S8535-MLF.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S8535-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S8535-PLCC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\AT90S8535-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega103-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega128-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega16-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega16-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega161-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega161-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega163-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega163-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega323-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega323-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega64-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega8-MLF.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega8-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATmega8-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny11-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny11-SOIC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny12-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny12-SOIC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny15-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny15-SOIC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny22-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny22-SOIC.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny28-MLF.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny28-PDIP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chips\ATtiny28-TQFP.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\avr.rc2
# End Source File
# Begin Source File

SOURCE=.\res\AVRLogo.bmp
# End Source File
# End Group
# End Target
# End Project
