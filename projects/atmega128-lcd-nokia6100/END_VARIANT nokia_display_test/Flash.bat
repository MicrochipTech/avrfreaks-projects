@set file=nokia_6100_test.hex
@set cpu=M8
@set dudepath=X:\Elektronik\avr\AVRDude\bin\

%dudepath%avrdudeUSB.exe -p %cpu% -c usbasp -e -U flash:w:%file% -F
REM %dudepath%avrdude.exe -p %cpu% -c bascom -e -U flash:w:%file%

@pause

