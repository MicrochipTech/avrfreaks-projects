REM three calls as a bug work-round in avrdude (bug #25971)
avrdude -q -p atmega328p -c usbasp -U lfuse:r:-:h
avrdude -q -p atmega328p -c usbasp -U hfuse:r:-:h
avrdude -q -p atmega328p -c usbasp -U efuse:r:-:h
pause
