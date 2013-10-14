# Line renumbering util for AttoBASIC 
#! /bin/bash

if [ $# != 1 ] ; then
   echo
   echo "Usage $0: [FileToRenumber]";
   echo
else
   BASEDIR=`dirname $0`
   RENUMBER=${BASEDIR}/renumber 
   TMPFILE=`tempfile`
   INFILE=$1
   OUTFILE=_${INFILE} ;
   if [ ! -e ${INFILE} ] ; then
      echo
      echo "Error: File name ${INFILE} does not exist!" ;
      echo
   else
  cat ${INFILE} | sed 's/\n\n/\n/g' | sed 's/\r\r/\r/g' | sed 's/\r/\r\n/g' > ${OUTFILE}	# insure CR/LF combo
      if [ $? == 0 ] ; then
         mv ${OUTFILE} ${INFILE}
      fi
      ${RENUMBER} ${INFILE} ${OUTFILE} 5 5	# renumber the file
      if [ $? == 0 ] ; then
         mv ${OUTFILE} ${INFILE}
      fi
      rm -f ${TMPFILE}
   fi
fi
