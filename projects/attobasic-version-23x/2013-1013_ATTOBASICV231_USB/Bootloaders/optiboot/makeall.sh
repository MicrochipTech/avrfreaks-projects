#!/bin/bash

. colordefs.sh				# terminal color definitions

make clean > /dev/null 2>&1
#
# The "big three" standard bootloaders.
#make atmega8
#make atmega168
#make atmega328
#
# Added by Scott Vitale for AttoBASIC support
#make m88-20
#make m88-16
#make m88-8
#make m88-4

echo "  ${YEL}Processing Mega168 flavors ...${NORMAL}"
make m168-20 > /dev/null 2>&1
make m168-16 > /dev/null 2>&1
make m168-8 > /dev/null 2>&1
make m168-4 > /dev/null 2>&1

echo "  ${YEL}Processing Mega328 flavors ...${NORMAL}"
make m328-20 > /dev/null 2>&1
make m328-16 > /dev/null 2>&1
make m328-8 > /dev/null 2>&1
make m328-4 > /dev/null 2>&1

#echo "  ${YEL}Processing Mega32U4 flavors ...${NORMAL}"
#make m32u4-20
#make m32u4-16
#make m32u4-8
#make m32u4-4

#make m328-20-drec
#make m328-16-drec
#make m328-8-drec
#make m328-4-drec

# additional buildable platforms of
#  somewhat questionable support level
#make lilypad
#make lilypad_resonator
#make pro8
#make pro16
#make pro20
#make atmega328_pro8
#make sanguino
#make mega
#make atmega88
#make luminet
