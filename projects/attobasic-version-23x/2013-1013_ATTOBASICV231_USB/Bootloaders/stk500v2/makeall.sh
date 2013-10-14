#!/bin/bash

. colordefs.sh				# terminal color definitions

#
echo "  ${YEL}Processing Mega2560 flavors ...${NORMAL}"
make clean > /dev/null 2>&1 ; make m2560-20 > /dev/null 2>&1 
make clean > /dev/null 2>&1 ; make m2560-16 > /dev/null 2>&1 
make clean > /dev/null 2>&1 ; make m2560-8 > /dev/null 2>&1 
make clean > /dev/null 2>&1 ; make m2560-4 > /dev/null 2>&1 
make clean > /dev/null 2>&1
