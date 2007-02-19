
echo invoked lnk %1 %2 %3 %4
mylib=~/lq/avr
if '%2'=='' then goto :p1
if '%3'=='' then goto :p2
if '%4'=='' then goto :p3
if '%5'=='' then goto :p4

:P1
avr-g++ avr/%1.o -g -L../lq/avr -llq -L/usr/lib -L/lib -lc -lm -o avr/%1.elf
goto :PE
:P2
avr-g++ avr/%1.o avr/%2.o -g -L../lq/avr -llq -L/usr/lib -L/lib -lc -lm -o avr/%1.elf ;
goto :PE
:P2
avr-g++ avr/%1.o avr/%2.o avr/%3.o -g -L../lq/avr -llq -L/usr/lib -L/lib -lc -lm -o avr/%1.elf ;
goto :PE
:P2
avr-g++ avr/%1.o avr/%2.o avr/%3.o avr/%4.o -g -L../lq/avr -llq -L/usr/lib -L/lib -lc -lm -o avr/%1.elf ;
goto :PE



:PE
avr-objcopy -j .text avr/%1.elf avr/%1.hex -O ihex