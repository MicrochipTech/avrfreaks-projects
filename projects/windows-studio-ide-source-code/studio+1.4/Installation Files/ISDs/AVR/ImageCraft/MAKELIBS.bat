C:\ICC\BIN\iccavr -c -IC:\icc\include\ -e -l -g -Wa-W -Wf-str_in_flash -Wf-r20_23 ISD16_AT90S8515.s
C:\ICC\BIN\iccavr -c -IC:\icc\include\ -e -l -g -Wa-W -Wf-str_in_flash -Wf-r20_23 ISD16_ATmega8.s
C:\ICC\BIN\iccavr -c -IC:\icc\include\ -e -DATMEGA -l -g -Mavr_mega -Wf-str_in_flash -Wf-r20_23 ISD16_ATmega103.s
C:\ICC\BIN\iccavr -c -IC:\icc\include\ -e -l -g -Wa-W -Wf-str_in_flash -Wf-r20_23 INT0_ISR.s
C:\ICC\BIN\ilibw -a libISD16AT90S8515.a ISD16_AT90S8515.o INT0_ISR.o
C:\ICC\BIN\ilibw -a libISD16ATMEGA8.a ISD16_ATmega8.o INT0_ISR.o
C:\ICC\BIN\iccavr -c -IC:\icc\include\ -e -DATMEGA -l -g -Mavr_mega -Wf-str_in_flash -Wf-r20_23 INT0_ISR.s
C:\ICC\BIN\ilibw -a libISD16ATMEGA103.a ISD16_ATmega103.o INT0_ISR.o
COPY libISD16AT90S8515.a C:\ICC\LIB
COPY libISD16ATMEGA8.a C:\ICC\LIB
COPY libISD16ATMEGA103.a C:\ICC\LIB
DEL *.lis
DEL *.o
