# Microsoft Developer Studio Project File - Name="Studio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Studio - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Studio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Studio.mak" CFG="Studio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Studio - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Studio - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Studio - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "C:\Program Files\HTML Help Workshop\include" /I "D:\Projects\Freelance\Flash_Designs\Studio+\1.4\Global" /D _WIN32_WINNT=0x0500 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/Studio+.exe"

!ELSEIF  "$(CFG)" == "Studio - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "C:\Program Files\HTML Help Workshop\include" /I "D:\Projects\Freelance\Flash_Designs\Studio+\1.4\Global" /D "_DEBUG" /D _WIN32_WINNT=0x0500 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/Studio+.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Studio - Win32 Release"
# Name "Studio - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AsmVariableDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CBListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CBToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CBTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChipWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Comms.cpp
# End Source File
# Begin Source File

SOURCE=.\CommsTimeoutDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Global\Function.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LicenseDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Memory.cpp
# End Source File
# Begin Source File

SOURCE=.\PluginsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Project.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\Registers.cpp
# End Source File
# Begin Source File

SOURCE=.\safeserial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Global\ScopeBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SfrsWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Global\SourceFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SourceFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SourceWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Stack.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Studio.cpp
# End Source File
# Begin Source File

SOURCE=.\Studio.rc
# End Source File
# Begin Source File

SOURCE=..\..\Global\Symbol.cpp
# End Source File
# Begin Source File

SOURCE=.\VariableDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\WatchWindow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDialog.h
# End Source File
# Begin Source File

SOURCE=.\AsmVariableDialog.h
# End Source File
# Begin Source File

SOURCE=.\CBListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CBToolBar.h
# End Source File
# Begin Source File

SOURCE=.\CBTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\ChipWindow.h
# End Source File
# Begin Source File

SOURCE=.\Comms.h
# End Source File
# Begin Source File

SOURCE=.\CommsTimeoutDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\Global\Function.h
# End Source File
# Begin Source File

SOURCE=.\FunctionDialog.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\LicenseDialog.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Memory.h
# End Source File
# Begin Source File

SOURCE=.\PluginsDialog.h
# End Source File
# Begin Source File

SOURCE=.\Project.h
# End Source File
# Begin Source File

SOURCE=.\ProjectWindow.h
# End Source File
# Begin Source File

SOURCE=.\ProjectWizard.h
# End Source File
# Begin Source File

SOURCE=.\Registers.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\safeserial.h
# End Source File
# Begin Source File

SOURCE=..\..\Global\ScopeBlock.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDialog.h
# End Source File
# Begin Source File

SOURCE=.\SfrsWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\Global\SourceFile.h
# End Source File
# Begin Source File

SOURCE=.\SourceFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\SourceWindow.h
# End Source File
# Begin Source File

SOURCE=.\SplashDialog.h
# End Source File
# Begin Source File

SOURCE=.\Stack.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Studio.h
# End Source File
# Begin Source File

SOURCE=..\..\Global\Symbol.h
# End Source File
# Begin Source File

SOURCE=.\VariableDialog.h
# End Source File
# Begin Source File

SOURCE=.\WatchWindow.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\Program Files\Microsoft Visual Studio\Vc98\Include\Wincrypt.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Anims\AboutAnim1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim13.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim14.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim15.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim17.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim18.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim19.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim20.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Anims\AboutAnim9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\AboutImage.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\AT90S8515_DIP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\AT90S8515_P6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\AT90S8515_PLCC.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\AT90S8515_TQFP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\ATmega103.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\ATmega103_TQFP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\ATmega8_DIP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\ATmega8_MLF.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\ATmega8_TQFP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Baud.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Baud.ico
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Book.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\BookOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Books.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\BooksOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\chip.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Chip_sml.ico
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Circle.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\CircleDot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Close.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CloseHi.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Com.bmp
# End Source File
# Begin Source File

SOURCE=.\res\com.ico
# End Source File
# Begin Source File

SOURCE=.\res\Comms.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Comms.ico
# End Source File
# Begin Source File

SOURCE=.\res\Cursor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CursorMask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Desk.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Eeprom_sml.ico
# End Source File
# Begin Source File

SOURCE=.\res\FDLogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\File.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Folder.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\FolderOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Function.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Functions.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Glasses.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HardBP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HardBPMask.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Help.ico
# End Source File
# Begin Source File

SOURCE=.\Res\TreeImages\HelpBook.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\TreeImages\HelpBookOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\TreeImages\HelpPage.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\id_targe.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\idt_proj.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\TreeImages\idt_view.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\LedOff16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chips\LedOn16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Lightning.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PC.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pc.ico
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Project.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Project.ico
# End Source File
# Begin Source File

SOURCE=.\res\Project.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Project_sml.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Project_sml.ico
# End Source File
# Begin Source File

SOURCE=.\res\ProjectBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbars\ProjectBar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Registers_sml.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\SFRs_sml.ico
# End Source File
# Begin Source File

SOURCE=.\res\SoftBP.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SoftBPMask.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Source.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Source.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Source_sml.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Source_sml.ico
# End Source File
# Begin Source File

SOURCE=.\res\Splash.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Sram_sml.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Stack.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Stack.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Stack_sml.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Stack_sml.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Studio.ico
# End Source File
# Begin Source File

SOURCE=.\res\Studio.ico
# End Source File
# Begin Source File

SOURCE=.\res\Studio.ico
# End Source File
# Begin Source File

SOURCE=.\res\Studio.rc2
# End Source File
# Begin Source File

SOURCE=.\res\TargetControl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbars\TargetControl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbars\TargetControlBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TControl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Variable.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Variables.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Toolbars\ViewBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TreeImages\Watch.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Watch.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Icons\Watch_sml.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Watch_sml.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Anims\About.avi
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\Vc98\Lib\Advapi32.lib"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\Vc98\Lib\Msimg32.lib"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\Vc98\Lib\Shlwapi.lib"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\Vc98\Lib\Mapi32.lib"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\HTML Help Workshop\lib\htmlhelp.lib"
# End Source File
# End Target
# End Project
# Section Studio : {E242A781-E945-11D2-BDA2-0000C0EB2A6A}
# 	2:5:Class:CSafeSerial
# 	2:10:HeaderFile:safeserial.h
# 	2:8:ImplFile:safeserial.cpp
# End Section
# Section Studio : {E242A783-E945-11D2-BDA2-0000C0EB2A6A}
# 	2:21:DefaultSinkHeaderFile:safeserial.h
# 	2:16:DefaultSinkClass:CSafeSerial
# End Section
