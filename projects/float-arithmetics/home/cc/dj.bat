REM
REM Create subdirectory for exe files
MD \EXDJ
set OE=
set OD=
REM
REM Set exe search path
IF not "%OPATH%"=="" goto :OPATHOK
set OPATH=%PATH%
:OPATHOK
md C:\exdj
SET PATH=C:\EXDJ;c:\DJGPP\BIN;\HOME\CC\DJBAT;%OPATH%
REM
REM Object Directory for *.o files
SET OD=DJ
REM
REM define the compilers object extensions
SET OE=O
set DJGPP=c:/djgpp/djgpp.env
set GO32=driver c:/djgpp/drivers/vga.grd
SET LIB=C:\DJGPP\LIB
