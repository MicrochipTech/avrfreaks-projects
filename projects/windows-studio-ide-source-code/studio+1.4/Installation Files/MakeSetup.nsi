; NSIS installer script for Studio+ v1.4 by Falsh Microsystems
; Chris Brown, 21/03/2002

Name "Studio+ v1.4"
OutFile "InstallStudio+.exe"
Icon "Installer.ico"
BGGradient 8097CF FFFFFF FFFFFF
SetCompress auto ; (can be off or force)
SetDatablockOptimize on ; (can be off)
CRCCheck on ; (can be off)
AutoCloseWindow false ; (can be true for the window go away automatically at end)
ShowInstDetails nevershow ;show ; (can be show to have them shown, or nevershow to disable)
SetDateSave on ; (can be on to have files restored to their orginal date)


###############################################################################
# License Agreement
LicenseText "You must agree to this license before installing."
LicenseData "license.txt"


###############################################################################
# Optional Component Text
ComponentText "Choose which components you want to install:"
EnabledBitmap Enabled.bmp
DisabledBitmap Disabled.bmp
InstType 'Typical'
InstType 'With "AVR Studio" Support'
InstType 'With "ImageCraft" Support'
InstType 'Full'


###############################################################################
# Installation Directory
InstallDir "$PROGRAMFILES\Flash Microsystems\Studio+ v1.4" # Default
InstallDirRegKey HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\1.4" \
	"Install Directory"
DirShow show # (make this hide to not let the user change it)
DirText "Select the directory you want Studio+ to be installed in:"


###############################################################################
# Splash Function
Function .onInit
  SetOutPath $TEMP
  File /oname=spltmp.dat "Splash.bmp"
  File /oname=spltmp.exe "C:\Program Files\nsis\splash.exe"
  ExecWait '"$TEMP\spltmp.exe" 3000 $HWNDPARENT spltmp.dat'
  Delete $TEMP\spltmp.exe
  Delete $TEMP\spltmp.dat
FunctionEnd


###############################################################################
# Application Install Section
Section "-Studio+ v1.4 Application (required)"
	CreateDirectory $INSTDIR\bin		# Create the bin directory
	CreateDirectory $INSTDIR\license	# Create the license directory
	CreateDirectory $INSTDIR\Plug-ins	# Create the Plug-ins directory
	SetOutPath $INSTDIR\bin				# Application files
	File "Studio+.exe"
	File "Regsvr32.exe"
	SetOutPath $INSTDIR					# Help files
	File "Studio+ Help.chm"
	SetOutPath $SYSDIR					# SafeSerial files
	File "C:\Program Files\SafeSerial2\Sase\sase.lic"
	IfFileExists "$SYSDIR\sase.ocx" SkipSaseOcx
	File "C:\Program Files\SafeSerial2\Sase\sase.ocx"
	SkipSaseOcx:
	SetOutPath $INSTDIR\bin
	ExecWait "Regsvr32.exe /s $SYSDIR\sase.ocx"
	SetOutPath "$INSTDIR\Plug-ins"		# Plug-ins
	File "AVR.dll"

										# Registry entrys
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+" \
		"Current Version" "1.4"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\1.4" \
		"Install Directory" "$INSTDIR"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\1.4" \
		"IDEs" ""
	WriteRegStr HKEY_LOCAL_MACHINE \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\Studio+ v1.4" \
		"DisplayName" "Studio+ v1.4"
	WriteRegStr HKEY_LOCAL_MACHINE \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\Studio+ v1.4" \
		"UninstallString" '"$INSTDIR\Uninst.exe"'

										# File association registry entrys
	WriteRegStr HKEY_CLASSES_ROOT ".spp" "" "Studio+.Project"
	WriteRegStr HKEY_CLASSES_ROOT "Studio+.Project" "" "Studio+ Project"
	WriteRegStr HKEY_CLASSES_ROOT "Studio+.Project\DefaultIcon" "" \
		"$INSTDIR\bin\Studio+.exe,1"
	WriteRegStr HKEY_CLASSES_ROOT "Studio+.Project\shell" "" "open"
	WriteRegStr HKEY_CLASSES_ROOT "Studio+.Project\shell\open\command" "" \
		'"$INSTDIR\bin\Studio+.exe" %1'
SectionEnd


###############################################################################
# "AVR Studio" Support Install Section
Section '"AVR Studio" Support'
	SectionIn 124
	ClearErrors
	ReadRegStr $1 HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\1.4"\
		"IDEs"
	IfErrors 0 SectionAVRStudioPluginIDEs
	StrCpy $2 "AVRStudio"
	SectionAVRStudioPluginIDEs:
	StrCpy $2 "$1,AVR Studio"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\1.4"\
		"IDEs" "$2"
	SetOutPath "$INSTDIR\Plug-ins"
	WriteRegStr HKEY_LOCAL_MACHINE \
		"Software\Flash Microsystems\Studio+\1.4\IDEs\AVR Studio" "Name" \
		"AVRStudio.dll"
	File "AVRStudio.dll"
	SetOutPath "C:\Studio+\Demos"		# Demos
	File /r "Demos\AVR_Studio"
	SetOutPath "$INSTDIR\ISDs\AVR"		# ISDs
	File /r "ISDs\AVR\AVR Studio"
SectionEnd


###############################################################################
# "ImageCraft" Support Install Section
Section '"ImageCraft" Support'
	SectionIn 134
	ClearErrors
	ReadRegStr $1 HKEY_LOCAL_MACHINE \
		"Software\Flash Microsystems\Studio+\1.4" "IDEs"
	IfErrors 0 SectionImageCraftPluginIDEs
	StrCpy $2 "ImageCraft"
	SectionImageCraftPluginIDEs:
	StrCpy $2 "$1,ImageCraft"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\1.4"\
		"IDEs" "$2"
	SetOutPath "$INSTDIR\Plug-ins"
	WriteRegStr HKEY_LOCAL_MACHINE \
		"Software\Flash Microsystems\Studio+\1.4\IDEs\ImageCraft" "Name" \
		"ImageCraft.dll"
	File "ImageCraft.dll"
	SetOutPath "C:\Studio+\Demos"		# Demos
	File /r "Demos\ImageCraft"
	SetOutPath "$INSTDIR\ISDs\AVR"		# ISDs
	File /r "ISDs\AVR\ImageCraft"
	IfFileExists "C:\ICC\Lib" CopyLibs
	Goto CopyLibsDone
	CopyLibs:
	SetOutPath "C:\ICC\Lib"				# Copy ImageCraft ISD Libraries
	File /r "ISDs\AVR\ImageCraft\*.a"
	CopyLibsDone:
SectionEnd


###############################################################################
# Start Menu Shortcuts
Section "Start-Menu Shortcuts"
	SectionIn 1234
	CreateDirectory "$SMPROGRAMS\Flash Microsystems\Studio+ v1.4"
	CreateShortCut \
		"$SMPROGRAMS\Flash Microsystems\Studio+ v1.4\Studio+ v1.4.lnk" \
		"$INSTDIR\bin\Studio+.exe"
	CreateShortCut \
		"$SMPROGRAMS\Flash Microsystems\Studio+ v1.4\Uninstall Studio+ v1.4.lnk"\
		"$INSTDIR\Uninst.exe"
SectionEnd


###############################################################################
# Desktop Icon
Section "Desktop Icon"
	SectionIn 1234
	CreateShortCut "$DESKTOP\Studio+ v1.4.lnk" "$INSTDIR\bin\Studio+.exe"
SectionEnd


###############################################################################
# Uninstall Settings & Section
UninstallText "This will uninstall Studio+ v1.4 from your system"
UninstallExeName "Uninst.exe"
UninstallIcon "Uninstaller.ico"

Section Uninstall
ExecWait "bin\Regsvr32.exe /u /s $SYSDIR\sase.ocx"
Delete "$INSTDIR\license\*.*"			# License files
RMDir "$INSTDIR\license"
Delete "$INSTDIR\Plug-ins\*.*"			# Plug-ins
RMDir "$INSTDIR\Plug-ins"
RMDir /r "$INSTDIR\ISDs"				# ISDs
Delete "$INSTDIR\bin\*.*"				# Application files
RMDir "$INSTDIR\bin"
Delete "$INSTDIR\*.*"					# Help files etc
Delete "$SMPROGRAMS\Flash Microsystems\Studio+ v1.4\Studio+ v1.4.lnk"
Delete "$SMPROGRAMS\Flash Microsystems\Studio+ v1.4\Uninstall Studio+ v1.4.lnk"
Delete "$DESKTOP\Studio+ v1.4.lnk"
DeleteRegKey HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\1.4"
DeleteRegKey HKEY_LOCAL_MACHINE \
	"Software\Microsoft\Windows\CurrentVersion\Uninstall\Studio+ v1.4"
RMDir "$SMPROGRAMS\Flash Microsystems\Studio+ v1.4"
RMDir "$SMPROGRAMS\Flash Microsystems"
RMDir "$INSTDIR"
RMDir /r "C:\Studio+\Demos"				# Demo files
RMDir "C:\Studio+"
SectionEnd ; end of uninstall section

# eof
