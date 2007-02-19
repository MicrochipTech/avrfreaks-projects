set XX=
set OD=TC
set OE=OBJ
IF not "%OPATH%"=="" goto :OPATHOK
set OPATH=%PATH%
:OPATHOK
path=C:\EXTC;C:\TC\BIN;\HOME\CC\TCBAT;%OPATH%
SET INCLUDE=C:\HOME\CC\INCLUDE
md C:\extc
set OD=TC
set OE=OBJ
@echo Use the #ifdef __TURBOC__ to compile special code for Borlands DOS CPP
