#! /bin/bash

if [ `tput colors` -ge 8 ] ; then	#check if terminal supports colors
   . colordefs.sh			# set terminal color definitions
fi

NOBUILD=0				#"1" to bypass builds

MKLST=N					#"Y" to copy LST files
#
DEFTMPL=Include/Defs_Constants.tmpl	#definition template file
DEFS="Include/Defs_Constants.inc"	#target definition file
FLAVDIR=AVR_Specific_Builds		#flavors build directory
BLDRDIR="_Bootloaders"			#location of USART bootloaders
USBSIODIR="_USB_Serial"			#location of USB Serial I/O library
MINDEFSSIZE=1500			#Defs_Constants.inc must be at least this size
ERRFN=`tempfile`			#place to store erroneous builds

logerror() {	# function to display error message and log it to a file
   echo "${RED}Error producing HEX file for ${WHT}${TARGET}${RED} @ ${WHT}${MCLK}MHZ${RED} with ${YEL}${BUILD}${RED} option ...${NORMAL}" | \
   tee -a ${ERRFN} ;
}

howmanyfiles() {			#returns the number of HEX files built within the directory passed to it
   find $1 -type f -name "*.hex" | wc -l
}

# Start a timer
START=$(date +%s.%N)

#create the file name prefix and revision level
pushd ../../ > /dev/null ;			# up two levels
FNBASE=$(basename $PWD | sed 's/-//g' |  tr '[:lower:]' '[:upper:]')
popd > /dev/null
REV=$(basename $PWD |  tr '[:lower:]' '[:upper:]') ;
FNPFX=$(echo ${FNBASE}${REV} | tr '[:lower:]' '[:upper:]') ;

#ask to build libraries or not
echo
read -t 30 -N 1 -p "${YEL}Build support libraries? (y/n) :${NORMAL} " BUILDLIBS
if [ $? -ge 128 ] ; then BUILDLIBS=n ; fi
echo
if [ ${BUILDLIBS} == y ] ; then
   echo "${BLU}Processing ${WHT}Bootloaders ${BLU}file(s)${NORMAL}"
   pushd ${BLDRDIR} > /dev/null 2>&1
   source mk_attobasic_bldr.sh
   popd > /dev/null 2>&1 			#return to project directory
   
   echo "${BLU}Processing ${WHT}USB Serial I/O ${BLU}file(s)${NORMAL}"
   pushd ${USBSIODIR} > /dev/null 2>&1
   source /makeall.sh > /dev/null 2>&1
   popd > /dev/null 2>&1 			#return to project directory
   
   echo "${BLU}Processing ${WHT}Data Recorder ${BLU}file(s)${NORMAL}"
   ./mk_dr_def_prg.sh > /dev/null 2>&1
fi

if [ -e ${DEFS} ] ; then
   DEFSSIZE=`stat -c %s ${DEFS}`		# get current DEFS file size
   if [ ${DEFSSIZE} -ge ${MINDEFSSIZE} ] ; then
      cp ${DEFS} ${DEFS}.bak				#make a copy of current Defs_Constants.inc
   else
      echo ;
      echo "${RED}The file ${DEFS} is not complete!${NORMAL}" ;
      echo "  Restore from a backup and restart." ;
      echo ;
      exit ;
   fi
else
   echo "Error: ${DEFS} does not exist!"
   exit
fi

#PSCLR=$(cat ${DEFS}.bak | egrep  "#define	FCLK_PS") #save the prescaler value
LINE=$(cat ${DEFS}.bak | egrep -n "+++ Break Here +++")
LINE=${LINE:0:2} ;					# extract the line number
#cat ${DEFS}.bak | sed '1,'${LINE}'d' > ${DEFS}

#get the actual file name
FN=$(ls *.asm | tr '[:lower:]' '[:upper:]') ;
if [ ${FN} == $(basename ${FNPFX}.ASM) ] ; then
   FN=$(ls *.asm) ;
else echo "No project file found!" ;
fi

rm -fr ${FLAVDIR}/*						#clear out target HEX file directory

if [ ${NOBUILD} == 0 ] ; then
   for TARGET in m2560 m32u4 usb1286 m328 m168 m88 ; do		#target MCUs
      # Pull the MCU signature bytes from the DEF file.
      SIG000="$(cat Include/${TARGET}def.inc | egrep "SIGNATURE_000" | cut -d ' ' -f2)" ; SIG000=${SIG000:0:4} ;
      SIG001="$(cat Include/${TARGET}def.inc | egrep "SIGNATURE_001" | cut -d ' ' -f2)" ; SIG001=${SIG001:0:4} ;
      SIG002="$(cat Include/${TARGET}def.inc | egrep "SIGNATURE_002" | cut -d ' ' -f2)" ; SIG002=${SIG002:0:4} ;
      mkdir -p ${FLAVDIR}/${TARGET}/					#make the target directory
      for MCLK in 4 8 16 20 ; do						#target MCLK
         for BUILD in usb_btldr usb_nobtldr uart_btldr uart_nobtldr datarecdr_btldr datarecdr_nobtldr teensypp20 ; do		#target build options
            case "${BUILD}" in
               #=====================================================
               "usb_btldr" )
                  if ( [ ${TARGET} == m32u4 ] || [ ${TARGET} == usb1286 ] ) \
                  && ( [ ${MCLK} == 8 ] || [ ${MCLK} == 16 ] ) ; then #USB builds only for m32u4 and usb1286 @8/16MHz
                     echo "${BLU}Processing ${WHT}${TARGET} ${BLU}@ ${WHT}${MCLK}MHZ${BLU} with ${YEL}${BUILD}${BLU} option ..."
                     HEXFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.hex ;
                     MAPFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.map ;
                     LOGFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.log ;
                     LSTFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.lst ;
                     echo ".include \"Include/${TARGET}def.inc\"" > ${DEFS} ;	#1st line
                     echo "#define FCLK_PS 1" >> ${DEFS} ;				# next line
                     echo "#define FCLK ${MCLK}000000" >> ${DEFS} ;		# next line
                     echo "#define BTLDR 1" >> ${DEFS} ;				# next line
                     echo "#define USB 1" >> ${DEFS} ;				# next line
                     echo "#define TEENSY 0" >> ${DEFS} ;				# next line
                     echo "#define DREC 0" >> ${DEFS} ;				# next line
                     echo "#define TEST 0" >> ${DEFS} ;				# next line
                     cat ${DEFS}.bak | sed '1,'${LINE}'d' >> ${DEFS}
                     ./WineBuild.sh >/dev/null
                     if [ -e $(basename $FN .asm).hex ] ; then
                        echo "${BLU}Moving ${YEL}${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}${BLU} files..."
                        echo
                        mv $(basename $FN .asm).hex ${HEXFN} ;
                        mv $(basename $FN .asm).map ${MAPFN} ;
                        mv $(basename $FN .asm).log ${LOGFN} ;
                        if [ ${MKLST} == Y ] ; then mv $(basename $FN .asm).lst ${LSTFN} ; fi
                     else
                        echo
                        logerror ${TARGET} ${MCLK} ${BUILD}
                        #                     echo "${RED}Error producing HEX file for ${TARGET} @ ${MCLK}MHZ with ${BUILD} option ...${NORMAL}"
                        echo
                     fi
                  fi
               ;;
               #=====================================================
               "usb_nobtldr" )
                  if ( [ ${TARGET} == m32u4 ] || [ ${TARGET} == usb1286 ] ) \
                  && ( [ ${MCLK} == 8 ] || [ ${MCLK} == 16 ] ) ; then #USB builds only for m32u4 and usb1286 @8/16MHz
                     echo "${BLU}Processing ${WHT}${TARGET} ${BLU}@ ${WHT}${MCLK}MHZ${BLU} with ${YEL}${BUILD}${BLU} option ..."
                     HEXFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.hex ;
                     MAPFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.map ;
                     LOGFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.log ;
                     LSTFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.lst ;
                     echo ".include \"Include/${TARGET}def.inc\"" > ${DEFS} ;	#1st line
                     echo "#define FCLK_PS 1" >> ${DEFS} ;				# next line
                     echo "#define FCLK ${MCLK}000000" >> ${DEFS} ;		#next line
                     echo "#define BTLDR 0" >> ${DEFS} ;				# next line
                     echo "#define USB 1" >> ${DEFS} ;				# next line
                     echo "#define TEENSY 0" >> ${DEFS} ;				# next line
                     echo "#define DREC 0" >> ${DEFS} ;				# next line
                     echo "#define TEST 0" >> ${DEFS} ;				# next line
                     cat ${DEFS}.bak | sed '1,'${LINE}'d' >> ${DEFS}
                     ./WineBuild.sh >/dev/null
                     if [ -e $(basename $FN .asm).hex ] ; then
                        echo "${BLU}Moving ${YEL}${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}${BLU} files..."
                        echo
                        mv $(basename $FN .asm).hex ${HEXFN} ;
                        mv $(basename $FN .asm).map ${MAPFN} ;
                        mv $(basename $FN .asm).log ${LOGFN} ;
                        if [ ${MKLST} == Y ] ; then mv $(basename $FN .asm).lst ${LSTFN} ; fi
                     else
                        echo
                        logerror ${TARGET} ${MCLK} ${BUILD}
                        #                     echo "${RED}Error producing HEX file for ${TARGET} @ ${MCLK}MHZ with ${BUILD} option ...${NORMAL}"
                        echo
                     fi
                  fi
               ;;
               #=====================================================
               "uart_btldr" )
                  if [ ${TARGET} != m88 ] ; then
                     echo "${BLU}Processing ${WHT}${TARGET} ${BLU}@ ${WHT}${MCLK}MHZ${BLU} with ${YEL}${BUILD}${BLU} option ..."
                     HEXFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.hex ;
                     MAPFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.map ;
                     LOGFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.log ;
                     LSTFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.lst ;
                     echo ".include \"Include/${TARGET}def.inc\"" > ${DEFS} ;	#1st line
                     echo "#define FCLK_PS 1" >> ${DEFS} ;				# next line
                     echo "#define FCLK ${MCLK}000000" >> ${DEFS} ;		#next line
                     echo "#define BTLDR 1" >> ${DEFS} ;				# next line
                     echo "#define USB 0" >> ${DEFS} ;				# next line
                     echo "#define TEENSY 0" >> ${DEFS} ;				# next line
                     echo "#define DREC 0" >> ${DEFS} ;				# next line
                     echo "#define TEST 0" >> ${DEFS} ;				# next line
                     cat ${DEFS}.bak | sed '1,'${LINE}'d' >> ${DEFS}
                     ./WineBuild.sh >/dev/null
                     if [ -e $(basename $FN .asm).hex ] ; then
                        echo "${BLU}Moving ${YEL}${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}${BLU} files..."
                        echo
                        mv $(basename $FN .asm).hex ${HEXFN} ;
                        mv $(basename $FN .asm).map ${MAPFN} ;
                        mv $(basename $FN .asm).log ${LOGFN} ;
                        if [ ${MKLST} == Y ] ; then mv $(basename $FN .asm).lst ${LSTFN} ; fi
                        #                     #mv $(basename $FN .asm).lst ${LSTFN} ;
                     else
                        echo
                        logerror ${TARGET} ${MCLK} ${BUILD}
                        #                     echo "${RED}Error producing HEX file for ${TARGET} @ ${MCLK}MHZ with ${BUILD} option ...${NORMAL}"
                        echo
                     fi
                  fi
               ;;
               #=====================================================
               "uart_nobtldr" )
                  if [ ${TARGET} == m88 ] ; then #USB builds only for m32u4 @8/16MHz
                     BUILD=uart
                  fi
                  echo "${BLU}Processing ${WHT}${TARGET} ${BLU}@ ${WHT}${MCLK}MHZ${BLU} with ${YEL}${BUILD}${BLU} option ..."
                  HEXFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.hex ;
                  MAPFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.map ;
                  LOGFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.log ;
                  LSTFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.lst ;
                  echo ".include \"Include/${TARGET}def.inc\"" > ${DEFS} ;		#1st line
                  echo "#define FCLK_PS 1" >> ${DEFS} ;				# next line
                  echo "#define FCLK ${MCLK}000000" >> ${DEFS} ;			#next line
                  echo "#define BTLDR 0" >> ${DEFS} ;				# next line
                  echo "#define USB 0" >> ${DEFS} ;				# next line
                  echo "#define TEENSY 0" >> ${DEFS} ;				# next line
                  echo "#define DREC 0" >> ${DEFS} ;				# next line
                  echo "#define TEST 0" >> ${DEFS} ;				# next line
                  cat ${DEFS}.bak | sed '1,'${LINE}'d' >> ${DEFS} ;
                  ./WineBuild.sh >/dev/null
                  if [ -e $(basename $FN .asm).hex ] ; then
                     echo "${BLU}Moving ${YEL}${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}${BLU} files..."
                     echo
                     mv $(basename $FN .asm).hex ${HEXFN} ;
                     mv $(basename $FN .asm).map ${MAPFN} ;
                     mv $(basename $FN .asm).log ${LOGFN} ;
                     if [ ${MKLST} == Y ] ; then mv $(basename $FN .asm).lst ${LSTFN} ; fi
                  else
                     echo
                     logerror ${TARGET} ${MCLK} ${BUILD}
                     #                     echo "${RED}Error producing HEX file for ${TARGET} @ ${MCLK}MHZ with ${BUILD} option ...${NORMAL}"
                     echo
                  fi
               ;;
               "teensypp20" )		# Specific build for Teensy++ 2.0
                  if [ ${TARGET} == usb1286 ] && [ ${MCLK} == 16 ] ; then
                     echo "${BLU}Processing ${WHT}${TARGET} ${BLU}@ ${WHT}${MCLK}MHZ${BLU} with ${YEL}${BUILD}${BLU} option ..."
                     HEXFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.hex ;
                     MAPFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.map ;
                     LOGFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.log ;
                     LSTFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.lst ;
                     echo ".include \"Include/${TARGET}def.inc\"" > ${DEFS} ;		#1st line
                     echo "#define FCLK_PS 1" >> ${DEFS} ;					# next line
                     echo "#define FCLK ${MCLK}000000" >> ${DEFS} ;			#next line
                     echo "#define BTLDR 0" >> ${DEFS} ;					# next line
                     echo "#define USB 1" >> ${DEFS} ;					# next line
                     echo "#define TEENSY 1" >> ${DEFS} ;					# next line
                     echo "#define DREC 0" >> ${DEFS} ;						# next line
                     echo "#define TEST 0" >> ${DEFS} ;						# next line
                     cat ${DEFS}.bak | sed '1,'${LINE}'d' >> ${DEFS} ;
                     ./WineBuild.sh >/dev/null
                     if [ -e $(basename $FN .asm).hex ] ; then
                        echo "${BLU}Moving ${YEL}${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}${BLU} files..."
                        echo
                        mv $(basename $FN .asm).hex ${HEXFN} ;
                        mv $(basename $FN .asm).map ${MAPFN} ;
                        mv $(basename $FN .asm).log ${LOGFN} ;
                        if [ ${MKLST} == Y ] ; then mv $(basename $FN .asm).lst ${LSTFN} ; fi
                     else
                        echo
                        logerror ${TARGET} ${MCLK} ${BUILD}
                        #                     echo "${RED}Error producing HEX file for ${TARGET} @ ${MCLK}MHZ with ${BUILD} option ...${NORMAL}"
                        echo
                     fi
                     elif [ ${TARGET} == usb1286 ] && [ ${MCLK} == 8 ] ; then
                     echo "${BLU}Processing ${WHT}${TARGET} ${BLU}@ ${WHT}${MCLK}MHZ${BLU} with ${YEL}${BUILD}${BLU} option ..."
                     HEXFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.hex ;
                     MAPFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.map ;
                     LOGFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.log ;
                     LSTFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.lst ;
                     echo ".include \"Include/${TARGET}def.inc\"" > ${DEFS} ;		#1st line
                     echo "#define FCLK_PS 2" >> ${DEFS} ;					# next line
                     echo "#define FCLK ${MCLK}000000" >> ${DEFS} ;			#next line
                     echo "#define BTLDR 0" >> ${DEFS} ;					# next line
                     echo "#define USB 1" >> ${DEFS} ;					# next line
                     echo "#define TEENSY 1" >> ${DEFS} ;					# next line
                     echo "#define DREC 0" >> ${DEFS} ;						# next line
                     echo "#define TEST 0" >> ${DEFS} ;						# next line
                     cat ${DEFS}.bak | sed '1,'${LINE}'d' >> ${DEFS} ;
                     ./WineBuild.sh >/dev/null
                     if [ -e $(basename $FN .asm).hex ] ; then
                        echo "${BLU}Moving ${YEL}${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}${BLU} files..."
                        echo
                        mv $(basename $FN .asm).hex ${HEXFN} ;
                        mv $(basename $FN .asm).map ${MAPFN} ;
                        mv $(basename $FN .asm).log ${LOGFN} ;
                        if [ ${MKLST} == Y ] ; then mv $(basename $FN .asm).lst ${LSTFN} ; fi
                     else
                        echo
                        logerror ${TARGET} ${MCLK} ${BUILD}
                        #                     echo "${RED}Error producing HEX file for ${TARGET} @ ${MCLK}MHZ with ${BUILD} option ...${NORMAL}"
                        echo
                     fi
                  fi
               ;;
               #=====================================================
               "datarecdr_btldr" )		# Specific build for AVR Data Recorder
                  if [ ${TARGET} == m328 ] ; then #Data Recorder builds only for M328
                     echo "${BLU}Processing ${WHT}${TARGET} ${BLU}@ ${WHT}${MCLK}MHZ${BLU} with ${YEL}${BUILD}${BLU} option ..."
                     HEXFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.hex ;
                     MAPFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.map ;
                     LOGFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.log ;
                     LSTFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.lst ;
                     echo ".include \"Include/${TARGET}def.inc\"" > ${DEFS} ;		#1st line
                     echo "#define FCLK_PS 1" >> ${DEFS} ;					# next line
                     echo "#define FCLK ${MCLK}000000" >> ${DEFS} ;		#next line
                     echo "#define BTLDR 1" >> ${DEFS} ;					# next line
                     echo "#define USB 0" >> ${DEFS} ;					# next line
                     echo "#define TEENSY 0" >> ${DEFS} ;					# next line
                     echo "#define DREC 1" >> ${DEFS} ;						# next line
                     echo "#define TEST 0" >> ${DEFS} ;						# next line
                     cat ${DEFS}.bak | sed '1,'${LINE}'d' >> ${DEFS} ;
                     ./WineBuild.sh >/dev/null
                     if [ -e $(basename $FN .asm).hex ] ; then
                        echo "${BLU}Moving ${YEL}${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}${BLU} files..."
                        echo
                        mv $(basename $FN .asm).hex ${HEXFN} ;
                        mv $(basename $FN .asm).map ${MAPFN} ;
                        mv $(basename $FN .asm).log ${LOGFN} ;
                        if [ ${MKLST} == Y ] ; then mv $(basename $FN .asm).lst ${LSTFN} ; fi
                     else
                        echo
                        logerror ${TARGET} ${MCLK} ${BUILD}
                        #                     echo "${RED}Error producing HEX file for ${TARGET} @ ${MCLK}MHZ with ${BUILD} option ...${NORMAL}"
                        echo
                     fi
                  fi
               ;;
               #=====================================================
               "datarecdr_nobtldr" )		# Specific build for AVR Data Recorder
                  if [ ${TARGET} == m328 ] ; then #Data Recorder builds only for M328
                     echo "${BLU}Processing ${WHT}${TARGET} ${BLU}@ ${WHT}${MCLK}MHZ${BLU} with ${YEL}${BUILD}${BLU} option ..."
                     HEXFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.hex ;
                     MAPFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.map ;
                     LOGFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.log ;
                     LSTFN=${FLAVDIR}/${TARGET}/${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}.lst ;
                     echo ".include \"Include/${TARGET}def.inc\"" > ${DEFS} ;		#1st line
                     echo "#define FCLK_PS 1" >> ${DEFS} ;					# next line
                     echo "#define FCLK ${MCLK}000000" >> ${DEFS} ;		#next line
                     echo "#define BTLDR 0" >> ${DEFS} ;					# next line
                     echo "#define USB 0" >> ${DEFS} ;					# next line
                     echo "#define TEENSY 0" >> ${DEFS} ;					# next line
                     echo "#define DREC 1" >> ${DEFS} ;						# next line
                     echo "#define TEST 0" >> ${DEFS} ;						# next line
                     cat ${DEFS}.bak | sed '1,'${LINE}'d' >> ${DEFS} ;
                     ./WineBuild.sh >/dev/null
                     if [ -e $(basename $FN .asm).hex ] ; then
                        echo "${BLU}Moving ${YEL}${FNPFX}_${TARGET}-${MCLK}MHZ-${BUILD}${BLU} files..."
                        echo
                        mv $(basename $FN .asm).hex ${HEXFN} ;
                        mv $(basename $FN .asm).map ${MAPFN} ;
                        mv $(basename $FN .asm).log ${LOGFN} ;
                        if [ ${MKLST} == Y ] ; then mv $(basename $FN .asm).lst ${LSTFN} ; fi
                     else
                        echo
                        logerror ${TARGET} ${MCLK} ${BUILD}
                        #                     echo "${RED}Error producing HEX file for ${TARGET} @ ${MCLK}MHZ with ${BUILD} option ...${NORMAL}"
                        echo
                     fi
                  fi
               ;;
            esac
         done
      done
   done
fi
#
#update the timer and calculate the difference
END=$(date +%s.%N) ; DIFF=$(echo "scale=2 ; $END - $START" | bc)
MINS=$(echo "scale=0 ; ( ($DIFF) / 60 )" | bc)
SECS=$(echo "scale=2 ; ( 100 * ($DIFF) % 60 )" | bc | xargs printf "%1.0f") ; if [ ${SECS} -le 9 ] ; then SECS=0${SECS} ; fi
#
if [ -s ${ERRFN} ] ; then
   echo "${BLU}============================ ${YEL}Error List${BLU} ====================================${NORMAL}"
   cat ${ERRFN} ;						#print the error log
   echo "${BLU}============================================================================${NORMAL}"
fi
echo
echo "${CYA}`howmanyfiles ${FLAVDIR}` files were built."
echo "${YEL}${MINS}:${SECS%*.*}${CYA} (mm:ss) to build.${NORMAL}"
echo
#
rm -f ${ERRFN} 2>&1 >/dev/null				#remove error log file
cp -f ${DEFS}.bak ${DEFS} 2>&1 >/dev/null		#restore original Defs_Constants.inc file
