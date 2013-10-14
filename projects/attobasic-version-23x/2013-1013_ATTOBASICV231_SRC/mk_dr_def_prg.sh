# This program processes the AttoBASIC "default capture"
# program (for the data recorder) to a format that can
# be used with the AVR assembler
#
#! /bin/bash

SRCFILE="../../../DataRecorder/Code/Applications/2CH-RTC.txt"
INCLFILE="Include/Data_DR_Prog.inc"

# Create the header
echo ";************************************************" > ${INCLFILE}
echo "; This is the default program that is loaded upon" >> ${INCLFILE}
echo ";  execution of the "LDD" command.  It is copied " >> ${INCLFILE}
echo ";  directly to program RAM.  User must then save " >> ${INCLFILE}
echo ";  to EEPROM                                     " >> ${INCLFILE}
echo ";************************************************" >> ${INCLFILE}
echo "Default_Prog:                                    " >> ${INCLFILE}

# Process each program line.  We must insure that we replace
# each quote with a ,'"', sequence before writing.  We must
# also strip all comments.

TMPFILE=${RANDOM}.txt
dos2unix <${SRCFILE} >${TMPFILE} # convert to LF only

cat ${TMPFILE} | while read LINE ; do
   IDX=`expr index "${LINE}" "#"`
   if [ ${IDX} != 0 ] ; then LINE=${LINE:0:$(( ${IDX} -1 ))} ; fi
   LINE="`echo ${LINE} | sed 's/"/",'\''"'\'',"/g'`"
   if [ `expr ${#LINE} % 2` == 0 ] ; then
      LINE=".db \"`echo ${LINE}` \",0x0d"
   else
      LINE=".db \"`echo ${LINE}`\",0x0d"
   fi
   echo -e "\t${LINE}" >>  ${INCLFILE}
done
echo -e "\t.dw 0\t\t\t;Terminate string" >>  ${INCLFILE}

rm -f ${TMPFILE}
