@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by Studio+.hpj. >"hlp\Studio+.hm"
echo. >>"hlp\Studio+.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\Studio+.hm"
"C:\Program Files\Microsoft Visual Studio\Common\Tools\makehm" ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\Studio+.hm"
echo. >>"hlp\Studio+.hm"
echo // Prompts (IDP_*) >>"hlp\Studio.hm"
"C:\Program Files\Microsoft Visual Studio\Common\Tools\makehm" IDP_,HIDP_,0x30000 resource.h >>"hlp\Studio+.hm"
echo. >>"hlp\Studio+.hm"
echo // Resources (IDR_*) >>"hlp\Studio.hm"
"C:\Program Files\Microsoft Visual Studio\Common\Tools\makehm" IDR_,HIDR_,0x20000 resource.h >>"hlp\Studio+.hm"
echo. >>"hlp\Studio+.hm"
echo // Dialogs (IDD_*) >>"hlp\Studio.hm"
"C:\Program Files\Microsoft Visual Studio\Common\Tools\makehm" IDD_,HIDD_,0x20000 resource.h >>"hlp\Studio+.hm"
echo. >>"hlp\Studio+.hm"
echo // Frame Controls (IDW_*) >>"hlp\Studio.hm"
"C:\Program Files\Microsoft Visual Studio\Common\Tools\makehm" IDW_,HIDW_,0x50000 resource.h >>"hlp\Studio+.hm"
REM -- Make help for Project STUDIO


echo Building Win32 Help files
start /wait "C:\Program Files\Microsoft Visual Studio\Common\Tools\Hcw" /C /E /M "hlp\Studio+.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\Studio+.hlp" goto :Error
if not exist "hlp\Studio+.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\Studio+.hlp" Debug
if exist Debug\nul copy "hlp\Studio+.cnt" Debug
if exist Release\nul copy "hlp\Studio+.hlp" Release
if exist Release\nul copy "hlp\Studio+.cnt" Release
echo.
goto :done

:Error
echo hlp\Studio+.hpj(1) : error: Problem encountered creating help file

:done
echo.
