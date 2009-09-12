REM two calls as a bug work-round in avrdude (bug #25971)
avrdude -q -p atmega8 -c usbasp -U lfuse:r:-:h
avrdude -q -p atmega8 -c usbasp -U hfuse:r:-:h
pause
