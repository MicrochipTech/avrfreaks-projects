@echo off

rem This batch file (re)installs the TestHotKeyService, then starts it.

rem Change this path:
set SERVICEPATH=C:\Troels\Delphi\HotKeyManager\HotKeyService

rem %SERVICEPATH%\HotKeyService.exe /uninstall /silent
%SERVICEPATH%\HotKeyService.exe /install /silent
net start TestHotKeyService

rem Start the Service Manager
rem %SystemRoot%\system32\services.msc /s

