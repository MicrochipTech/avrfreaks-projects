#!/bin/bash
#
#
# Added by Scott Vitale for AttoBASIC support
echo "  ${YEL}Processing ${WHT}AT90usb1286 ${YEL}file(s)${NORMAL}"
make clean > /dev/null 2>&1 ; make USB1286-8M > /dev/null 2>&1 ; if [ $? == 1 ] ; then echo "Make failed building USB1286-8M" ; fi
make clean > /dev/null 2>&1 ; make USB1286-16M > /dev/null 2>&1 ; if [ $? == 1 ] ; then echo "Make failed building USB1286-16M" ; fi

echo "  ${YEL}Processing ${WHT}ATmega32u4 ${YEL}file(s)${NORMAL}"
make clean > /dev/null 2>&1 ; make M32U4-8M > /dev/null 2>&1 ; if [ $? == 1 ] ; then echo "Make failed building M32U4-8M" ; fi
make clean > /dev/null 2>&1 ; make M32U4-16M > /dev/null 2>&1 ; if [ $? == 1 ] ; then echo "Make failed building M32U4-16M" ; fi
#make clean ; make TEENSY20P
make clean > /dev/null 2>&1
