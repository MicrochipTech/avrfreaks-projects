REM *** MAKE SURE YOUR SERIAL CONVERTER IS CONNECTED @ 19200 baud ***
avrdude -p m32 -c arduino -PCOM15 -b 19200  %1 %2

