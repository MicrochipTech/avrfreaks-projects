; NSIS installer script for Studio+ "AVR Studio 4" Plug-in by Flash Microsystems
; Chris Brown, 31/03/2002

Name 'Studio+ "AVR Studio 4" Plug-in'
OutFile "Install_AVRStudio4_Plugin.exe"
Icon "Installer.ico"
BGGradient 8097CF FFFFFF FFFFFF
SetCompress auto ; (can be off or force)
SetDatablockOptimize on ; (can be off)
CRCCheck on ; (can be off)
AutoCloseWindow true ; (can be true for the window go away automatically at end)
ShowInstDetails nevershow ;show ; (can be show to have them shown, or nevershow to disable)
SetDateSave on ; (can be on to have files restored to their orginal date)


###############################################################################
# License Agreement
LicenseText "You must agree to this license before installing."
LicenseData "license.txt"


###############################################################################
# Optional Component Text
#ComponentText "Choose which components you want to install:"
#EnabledBitmap Enabled.bmp
#DisabledBitmap Disabled.bmp
#InstType 'Typical'
#InstType 'With "AVR Studio" Support'
#InstType 'With "ImageCraft" Support'
#InstType 'Full'


###############################################################################
# Installation Directory
InstallDir "$PROGRAMFILES\Flash Microsystems\Studio+ v1.4" # Default
InstallDirRegKey HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\1.4" \
	"Install Directory"
DirShow hide # (make this hide to not let the user change it)
#DirText "Select the directory you want Studio+ to be installed in:"


###############################################################################
# Splash Function
Function .onInit
  SetOutPath $TEMP
  File /oname=spltmp.dat "AVRStudio4Splash.bmp"
  File /oname=spltmp.exe "C:\Program Files\nsis\splash.exe"
  ExecWait '"$TEMP\spltmp.exe" 3000 $HWNDPARENT spltmp.dat'
  Delete $TEMP\spltmp.exe
  Delete $TEMP\spltmp.dat
FunctionEnd


###############################################################################
# Application Install Section
Section "-AVR Studio 4 Plug-in"
	ClearErrors
	ReadRegStr $1 HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+"\
		"Current Version"
	IfErrors 0 StudioPlusInstalled

	MessageBox MB_OK 'Please install Studio+ before installing$\r$\n\
this Plug-in. Studio+ can be downloaded from:$\r$\n\
   www.flash.co.uk'
	Abort " "

	StudioPlusInstalled:

	ClearErrors
	ReadRegStr $2 HKEY_LOCAL_MACHINE \
		"Software\Flash Microsystems\Studio+\$1\IDEs\AVR Studio 4" "Name"
	IfErrors 0 DLLInstalled

	ReadRegStr $2 HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\$1" \
		"IDEs"							# Get IDEs
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Flash Microsystems\Studio+\$1" \
		"IDEs" "$2,AVR Studio 4"		# Add "AVR Studio 4" to them
	WriteRegStr HKEY_LOCAL_MACHINE \
		"Software\Flash Microsystems\Studio+\$1\IDEs\AVR Studio 4" "Name" \
		"AVRStudio4.dll"				# Add it's DLL name
	Goto PluginFile

	DLLInstalled:
	MessageBox MB_YESNO|MB_ICONQUESTION \
		'The "AVR Studio 4" Plug-in is already in your plug-ins directory.$\r$\n\
Would you like to overwrite it?' IDNO AllDone

	PluginFile:
	SetOverwrite try

	SetOutPath $INSTDIR\Plug-ins
	File "AVRStudio4.dll"				# Copy the DLL file
	SetOutPath C:\Studio+\Demos
	File /r "Demos\AVR_Studio4"			# Copy the Demo projects
#	SetOutPath $INSTDIR\ISDs\AVR
#	File /r "ISDs\AVR\AVR Studio 4"		# ISDs (Use "AVR Studio"'s ISDs)

	AllDone:

SectionEnd

# eof
