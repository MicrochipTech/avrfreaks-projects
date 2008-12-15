; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CStudioApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "studio.h"
LastPage=0

ClassCount=24
Class1=CAboutDialog
Class2=CAsmVariableDialog
Class3=CCBListCtrl
Class4=CCBTreeCtrl
Class5=CChildFrame
Class6=CChildView
Class7=CChipWindow
Class8=CFunctionDialog
Class9=CLicenseDialog
Class10=CMainFrame
Class11=CProjectWindow
Class12=CProjectWizard
Class13=CSettingsDialog
Class14=CSfrsWindow
Class15=CSourceFileDialog
Class16=CSourceWindow
Class17=CSplashDialog
Class18=CStudioApp
Class19=CVariableDialog
Class20=CWatchWindow

ResourceCount=21
Resource1=IDD_COMMSTIMEOUT
Resource2=IDT_TARGETCONTROLBAR (English (U.S.))
Resource3=IDM_CONTEXT_WATCHASM (English (U.S.))
Resource4=IDM_CONTEXT_WATCHNEW (English (U.S.))
Resource5=IDM_CONTEXT_PROJECT (English (U.S.))
Resource6=IDM_CONTEXT_FILE (English (U.S.))
Resource7=IDM_CONTEXT_FUNCTION (English (U.S.))
Resource8=IDM_CONTEXT_VARIABLE (English (U.S.))
Resource9=IDM_CONTEXT_WATCHSRC (English (U.S.))
Resource10=IDD_LICENSE (English (U.S.))
Resource11=IDD_ABOUT (English (U.S.))
Resource12=IDT_PROJECTBAR (English (U.S.))
Resource13=IDD_SETTINGS
Resource14=IDD_SPLASH
Resource15=IDD_PROJECTWIZARD
Resource16=IDD_SOURCEFILE
Resource17=IDD_VARIABLE
Resource18=IDD_PLUGINS
Class21=CHelpWindow
Class22=CCBToolBarCtrl
Resource19=IDD_ASMVARIABLE
Class23=CPluginsDialog
Resource20=IDD_FUNCTION
Class24=CCommsTimeoutDialog
Resource21=IDR_MAINFRAME (English (U.S.))

[CLS:CAboutDialog]
Type=0
BaseClass=CDialog
HeaderFile=AboutDialog.h
ImplementationFile=AboutDialog.cpp
LastObject=CAboutDialog

[CLS:CAsmVariableDialog]
Type=0
BaseClass=CDialog
HeaderFile=AsmVariableDialog.h
ImplementationFile=AsmVariableDialog.cpp

[CLS:CCBListCtrl]
Type=0
BaseClass=CListCtrl
HeaderFile=CBListCtrl.h
ImplementationFile=CBListCtrl.cpp

[CLS:CCBTreeCtrl]
Type=0
BaseClass=CTreeCtrl
HeaderFile=CBTreeCtrl.h
ImplementationFile=CBTreeCtrl.cpp

[CLS:CChildFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp

[CLS:CChildView]
Type=0
BaseClass=CWnd
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp

[CLS:CChipWindow]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChipWindow.h
ImplementationFile=ChipWindow.cpp

[CLS:CFunctionDialog]
Type=0
BaseClass=CDialog
HeaderFile=FunctionDialog.h
ImplementationFile=FunctionDialog.cpp

[CLS:CLicenseDialog]
Type=0
BaseClass=CDialog
HeaderFile=LicenseDialog.h
ImplementationFile=LicenseDialog.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CProjectWindow]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ProjectWindow.h
ImplementationFile=ProjectWindow.cpp
LastObject=CProjectWindow

[CLS:CProjectWizard]
Type=0
BaseClass=CDialog
HeaderFile=ProjectWizard.h
ImplementationFile=ProjectWizard.cpp

[CLS:CSettingsDialog]
Type=0
BaseClass=CDialog
HeaderFile=SettingsDialog.h
ImplementationFile=SettingsDialog.cpp

[CLS:CSfrsWindow]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=SfrsWindow.h
ImplementationFile=SfrsWindow.cpp

[CLS:CSourceFileDialog]
Type=0
BaseClass=CDialog
HeaderFile=SourceFileDialog.h
ImplementationFile=SourceFileDialog.cpp

[CLS:CSourceWindow]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=SourceWindow.h
ImplementationFile=SourceWindow.cpp

[CLS:CSplashDialog]
Type=0
BaseClass=CDialog
HeaderFile=SplashDialog.h
ImplementationFile=SplashDialog.cpp

[CLS:CStudioApp]
Type=0
BaseClass=CWinApp
HeaderFile=Studio.h
ImplementationFile=Studio.cpp
Filter=N
VirtualFilter=AC
LastObject=IDM_PREFS_COMMSTIMEOUT

[CLS:CVariableDialog]
Type=0
BaseClass=CDialog
HeaderFile=VariableDialog.h
ImplementationFile=VariableDialog.cpp

[CLS:CWatchWindow]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=WatchWindow.h
ImplementationFile=WatchWindow.cpp

[DLG:IDD_ABOUT]
Type=1
Class=CAboutDialog

[DLG:IDD_ASMVARIABLE]
Type=1
Class=CAsmVariableDialog
ControlCount=27
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_ASMVARIABLE_REGISTER,button,1342177284
Control5=IDC_ASMVARIABLE_MEMORY,button,1342177284
Control6=IDC_ASMVARIABLE_NAME,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_ASMVARIABLE_ADDRESS,edit,1350631552
Control10=IDC_STATIC,button,1342177287
Control11=IDC_ASMVARIABLE_GLOBAL,button,1342242819
Control12=IDC_ASMVARIABLE_SCOPETEXT,static,1342308352
Control13=IDC_ASMVARIABLE_SCOPEFROM,edit,1350631552
Control14=IDC_ASMVARIABLE_SCOPETO,edit,1350631552
Control15=IDC_STATIC,button,1342177287
Control16=IDC_ASMVARIABLE_DATATYPE,combobox,1344340035
Control17=IDC_STATIC,static,1342308352
Control18=IDC_ASMVARIABLE_POINTER,button,1342242819
Control19=IDC_ASMVARIABLE_ARRAY,button,1342242819
Control20=IDC_ASMVARIABLE_DIMENSIONS,edit,1350631552
Control21=IDC_ASMVARIABLE_DIMENSIONSTEXT,static,1342308352
Control22=IDC_STATIC,static,1342308354
Control23=IDC_ASMVARIABLE_SIZE,static,1342308864
Control24=IDC_STATIC,static,1342308354
Control25=IDC_ASMVARIABLE_VALUE,static,1342308864
Control26=IDC_STATIC,static,1342308354
Control27=IDC_ASMVARIABLE_DECLARATION,static,1342308864

[DLG:IDD_FUNCTION]
Type=1
Class=CFunctionDialog
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308864
Control3=IDC_FUNCTION_NAME,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_FUNCTION_FILE,static,1342308864
Control6=IDC_FUNCTION_LINE,static,1342308352
Control7=IDC_STATIC,button,1342177287
Control8=IDC_FUNCTION_FROM,static,1342308864
Control9=IDC_STATIC,static,1342308864
Control10=IDC_FUNCTION_TO,static,1342308864

[DLG:IDD_LICENSE]
Type=1
Class=CLicenseDialog

[DLG:IDD_PROJECTWIZARD]
Type=1
Class=CProjectWizard
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_PROJECTWIZARD_IDE,combobox,1344339971
Control4=IDC_PROJECTWIZARD_FAMILY,combobox,1344339971
Control5=IDC_PROJECTWIZARD_IDELOGO,static,1342177294
Control6=IDC_PROJECTWIZARD_FAMILYLOGO,static,1342177294

[DLG:IDD_SETTINGS]
Type=1
Class=CSettingsDialog
ControlCount=26
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SETTINGS_TAB,SysTabControl32,1342193728
Control4=IDC_SETTINGS_SFRSLIST,SysListView32,1350631425
Control5=IDC_SETTINGS_COMMSPORTSTATIC,static,1342308352
Control6=IDC_SETTINGS_COMMSPORT,combobox,1344339971
Control7=IDC_SETTINGS_COMMSBAUD,combobox,1344339971
Control8=IDC_SETTINGS_COMMSSTANDARDSTATIC,static,1342308352
Control9=IDC_SETTINGS_COMMSPORTGROUP,button,1342177287
Control10=IDC_SETTINGS_COMMSBAUDGROUP,button,1342177287
Control11=IDC_SETTINGS_COMMSCHECK,button,1342242819
Control12=IDC_SETTINGS_COMMSCUSTOM,edit,1350639744
Control13=IDC_SETTINGS_CPUCPUGROUP,button,1342177287
Control14=IDC_SETTINGS_CPUCPU,combobox,1344339971
Control15=IDC_SETTINGS_CPUPACKAGEGROUP,button,1342177287
Control16=IDC_SETTINGS_CPUPACKAGE,combobox,1344339971
Control17=IDC_SETTINGS_SFRSSELECT,button,1342242816
Control18=IDC_SETTINGS_SFRSCLEAR,button,1342242816
Control19=IDC_SETTINGS_SFRSFLAGS,button,1342242819
Control20=IDC_SETTINGS_EXTERNALMEM,button,1342242819
Control21=IDC_SETTINGS_EXMEMSTARTTEXT,static,1342308352
Control22=IDC_SETTINGS_EXMEMSTART,edit,1350631552
Control23=IDC_SETTINGS_EXMEMSIZETEXT,static,1342308352
Control24=IDC_SETTINGS_EXMEMSIZE,edit,1350631552
Control25=IDC_SETTINGS_STACKTEXT,static,1073872896
Control26=IDC_SETTINGS_STACK,edit,1082196096

[DLG:IDD_SOURCEFILE]
Type=1
Class=CSourceFileDialog
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308864
Control3=IDC_SOURCEFILE_NAME,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_SOURCEFILE_PATH,static,1342308864
Control6=IDC_STATIC,static,1342308864
Control7=IDC_SOURCEFILE_LINES,static,1342308864

[DLG:IDD_SPLASH]
Type=1
Class=CSplashDialog
ControlCount=1
Control1=IDC_STATIC,static,1342179342

[DLG:IDD_VARIABLE]
Type=1
Class=CVariableDialog
ControlCount=31
Control1=IDC_STATIC,static,1342308352
Control2=IDC_VARIABLE_NAME,static,1342308864
Control3=IDC_STATIC,static,1342308352
Control4=IDC_VARIABLE_ADDRESS,static,1342308864
Control5=IDC_STATIC,button,1342177287
Control6=IDC_VARIABLE_REGISTER,button,1342177284
Control7=IDC_VARIABLE_MEMORY,button,1342177284
Control8=IDC_STATIC,button,1342177287
Control9=IDC_VARIABLE_GLOBAL,button,1342242818
Control10=IDC_VARIABLE_SCOPETEXT,static,1342308352
Control11=IDC_STATIC,button,1342177287
Control12=IDC_STATIC,static,1342308352
Control13=IDC_VARIABLE_POINTER,button,1342242818
Control14=IDC_VARIABLE_ARRAY,button,1342242818
Control15=IDC_VARIABLE_DIMENSIONSTEXT,static,1342308352
Control16=IDC_STATIC,static,1342308354
Control17=IDC_VARIABLE_SIZE,static,1342308864
Control18=IDC_STATIC,static,1342308354
Control19=IDC_VARIABLE_VALUE,static,1342308864
Control20=IDC_VARIABLE_DECLARATIONTEXT,static,1342308354
Control21=IDC_VARIABLE_DECLARATION,static,1342308864
Control22=IDOK,button,1342242817
Control23=IDC_STATIC,static,1342308352
Control24=IDC_VARIABLE_SOURCE,static,1342308864
Control25=IDC_VARIABLE_FUNCTIONTEXT,static,1342308352
Control26=IDC_VARIABLE_FUNCTION,static,1342308864
Control27=IDC_VARIABLE_DIMENSIONS,static,1342308864
Control28=IDC_VARIABLE_DATATYPE,static,1342308864
Control29=IDC_VARIABLE_SCOPEFROM,static,1342308864
Control30=IDC_VARIABLE_SCOPETO,static,1342308864
Control31=IDC_VARIABLE_LINE,static,1342308352

[TB:IDT_PROJECTBAR (English (U.S.))]
Type=1
Class=?
Command1=IDC_PROJECTBAR_OPEN
Command2=IDC_PROJECTBAR_SAVE
Command3=IDC_PROJECTBAR_SETTINGS
Command4=IDC_PROJECTBAR_REFRESH
Command5=IDC_PROJECTBAR_COMMS
Command6=IDC_PROJECTBAR_VIEWPROJECT
Command7=IDC_PROJECTBAR_VIEWREGISTERS
Command8=IDC_PROJECTBAR_VIEWSFRS
Command9=IDC_PROJECTBAR_VIEWSRAM
Command10=IDC_PROJECTBAR_VIEWEEPROM
Command11=IDC_PROJECTBAR_VIEWWATCH
Command12=IDC_PROJECTBAR_VIEWSTACK
Command13=IDC_PROJECTBAR_VIEWCHIP
Command14=IDC_PROJECTBAR_SYNTAXHI
Command15=IDC_PROJECTBAR_TRACKPC
Command16=IDC_PROJECTBAR_ASSEMBLER
Command17=IDC_PROJECTBAR_ADDRESSES
Command18=IDC_PROJECTBAR_MACHINECODE
CommandCount=18

[TB:IDT_TARGETCONTROLBAR (English (U.S.))]
Type=1
Class=?
Command1=ID_SINGLESTEP
Command2=ID_CSTEP
Command3=ID_CSTEPINTO
Command4=ID_CSTEPOVER
Command5=ID_CSTEPOUT
Command6=ID_STEPTOCURSOR
Command7=ID_ANIMATE
Command8=ID_TOSOFTBP
Command9=ID_COMPLEXBP
Command10=ID_TOHARDBP
Command11=ID_SNAPSHOT
Command12=ID_PAUSETARGET
Command13=ID_RESETTARGET
CommandCount=13

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEWPROJECT
Command2=ID_FILE_OPENPROJECT
Command3=IDM_FILE_SAVE
Command4=IDM_FILE_SAVEAS
Command5=ID_FILE_CLOSEPROJECT
Command6=ID_APP_EXIT
Command7=ID_VIEW_STATUS_BAR
Command8=IDM_VIEW_MAINTOOLBAR
Command9=IDM_VIEW_TARGETCONTROLBAR
Command10=ID_VIEW_PROJECT
Command11=ID_VIEW_REGISTERS
Command12=ID_VIEW_MEMORY_SFRS
Command13=ID_VIEW_MEMORY_SRAM
Command14=ID_VIEW_MEMORY_EEPROM
Command15=ID_VIEW_WATCH
Command16=ID_VIEW_STACK
Command17=IDM_VIEW_CHIP
Command18=ID_PROJECT_SETTINGS
Command19=ID_PROJECT_REFRESH
Command20=IDM_PREFS_COMMSTIMEOUT
Command21=ID_PREFS_SOURCE_SYNTAXHI
Command22=ID_PREFS_SOURCE_TRACKPC
Command23=IDM_PREFS_SOURCE_ASM
Command24=IDM_PREFS_SOURCE_ADDRESS
Command25=ID_PREFS_SOURCE_CODE
Command26=IDM_WINDOW_CASCADE
Command27=IDM_WINDOW_PREV
Command28=IDM_WINDOW_NEXT
Command29=ID_HELP_HELPTOPICS
Command30=ID_HELP_ABOUT
Command31=IDM_HELP_PLUGINS
Command32=ID_HELP_REGISTER
Command33=IDM_HELP_EXPORT
CommandCount=33

[MNU:IDM_CONTEXT_PROJECT (English (U.S.))]
Type=1
Class=?
Command1=IDM_CONTEXT_PROJECT_EXPAND
CommandCount=1

[MNU:IDM_CONTEXT_FILE (English (U.S.))]
Type=1
Class=?
Command1=IDM_CONTEXT_FILE_OPEN
Command2=IDM_CONTEXT_FILE_PROPERTIES
Command3=IDM_CONTEXT_FILE_EXPAND
CommandCount=3

[MNU:IDM_CONTEXT_FUNCTION (English (U.S.))]
Type=1
Class=?
Command1=IDM_CONTEXT_FUNCTION_SHOW
Command2=IDM_CONTEXT_FUNCTION_PROPERTIES
Command3=IDM_CONTEXT_FUNCTION_EXPAND
CommandCount=3

[MNU:IDM_CONTEXT_VARIABLE (English (U.S.))]
Type=1
Class=?
Command1=IDM_CONTEXT_VARIABLE_SHOW
Command2=IDM_CONTEXT_VARIABLE_ADDTOWATCH
Command3=IDM_CONTEXT_VARIABLE_PROPERTIES
CommandCount=3

[MNU:IDM_CONTEXT_WATCHSRC (English (U.S.))]
Type=1
Class=?
Command1=IDM_CONTEXT_WATCHSRC_PROPERTIES
Command2=IDM_CONTEXT_WATCHSRC_DELETE
CommandCount=2

[MNU:IDM_CONTEXT_WATCHASM (English (U.S.))]
Type=1
Class=?
Command1=IDM_CONTEXT_WATCHASM_EDIT
Command2=IDM_CONTEXT_WATCHASM_DELETE
CommandCount=2

[MNU:IDM_CONTEXT_WATCHNEW (English (U.S.))]
Type=1
Class=?
Command1=IDM_CONTEXT_WATCHNEW_ADD
CommandCount=1

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW
Command3=ID_EDIT_PASTE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_NEXT_PANE
Command7=ID_PREV_PANE
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_EDIT_CUT
Command11=ID_EDIT_UNDO
Command12=ID_CONTEXT_HELP
Command13=ID_HELP
CommandCount=13

[DLG:IDD_ABOUT (English (U.S.))]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342373889
Control2=IDC_ABOUTANIM,SysAnimate32,1350631428

[DLG:IDD_LICENSE (English (U.S.))]
Type=1
Class=?
ControlCount=31
Control1=IDCLOSE,button,1342242816
Control2=IDC_STATIC,button,1342177287
Control3=IDC_LICENSE_IN,edit,1350631552
Control4=IDC_LICENSE_LICENSE,button,1342242816
Control5=IDC_STATIC,static,1342308364
Control6=IDC_STATIC,static,1342308352
Control7=IDC_LICENSE_CODE,edit,1342244993
Control8=IDC_LICENSE_BASIC,button,1476460578
Control9=IDC_LICENSE_ANIM,SysAnimate32,1350631428
Control10=IDC_LICENSE_FROMDISK,button,1342242816
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_LICENSE_BASICWANT,button,1476460547
Control14=IDC_LICENSE_PROF,button,1476460578
Control15=IDC_LICENSE_PROFWANT,button,1342242819
Control16=IDC_LICENSE_ENTWANT,button,1476460547
Control17=IDC_LICENSE_ENT,button,1476460578
Control18=IDC_LICENSE_ICE,button,1476460578
Control19=IDC_LICENSE_ICEWANT,button,1476460547
Control20=IDC_STATIC,button,1342177287
Control21=IDC_STATIC,button,1342177287
Control22=IDC_STATIC,static,1342308352
Control23=IDC_LICENSE_CPUS,listbox,1352728833
Control24=IDC_STATIC,static,1342308352
Control25=IDC_LICENSE_IDES,listbox,1352728841
Control26=IDC_LICENSE_EMAIL,button,1342242817
Control27=IDC_STATIC,static,1342177280
Control28=IDC_STATIC,static,1476526080
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1476526080
Control31=IDC_STATIC,static,1476526080

[CLS:CHelpWindow]
Type=0
HeaderFile=HelpWindow.h
ImplementationFile=HelpWindow.cpp
BaseClass=CFrameWnd
Filter=T
VirtualFilter=fWC
LastObject=CHelpWindow

[CLS:CCBToolBarCtrl]
Type=0
HeaderFile=CBToolBarCtrl.h
ImplementationFile=CBToolBarCtrl.cpp
BaseClass=CToolBarCtrl
Filter=W
VirtualFilter=YWC
LastObject=CCBToolBarCtrl

[DLG:IDD_PLUGINS]
Type=1
Class=CPluginsDialog
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_PLUGINS_FAMILYS,listbox,1352745216
Control3=IDC_PLUGINS_IDES,listbox,1352745216

[CLS:CPluginsDialog]
Type=0
HeaderFile=PluginsDialog.h
ImplementationFile=PluginsDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CPluginsDialog

[DLG:IDD_COMMSTIMEOUT]
Type=1
Class=CCommsTimeoutDialog
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_COMMSTIMEOUT_ENABLE,button,1342242819
Control4=IDC_COMMSTIMEOUT_TEXT,static,1342308354
Control5=IDC_COMMSTIMEOUT_PERIOD,edit,1350639744
Control6=IDC_COMMSTIMEOUT_MS,static,1342308352

[CLS:CCommsTimeoutDialog]
Type=0
HeaderFile=CommsTimeoutDialog.h
ImplementationFile=CommsTimeoutDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDOK

