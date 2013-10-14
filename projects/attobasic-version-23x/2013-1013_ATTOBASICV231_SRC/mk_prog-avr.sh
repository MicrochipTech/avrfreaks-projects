# mk_prog-duino.sh - allows one to upload AttoBASIC to an ARDUINO compatible
#  platform using the bootloader.
#  Support added for DFU bootloaders on USB capable parts.
#  Also allows one to upload any HEX file.
#  File size is checked to insure that the file contents do not overwrite
#  the bootloader.  If so, the file is truncated then uploaded.
#
#! /bin/bash

#set some defaults
MENUNAME=menu.lst
HEXDIR=AVR_Specific_Builds		#base location of HEX files
BASEDIR=${PWD}				#base directory of AttoBASIC project
MCUTYPE=(`ls -1v ${HEXDIR}`)		#create a list of available MCU types
AVRDUDE=`which avrdude`			#find AVRDUDE

# ANSI COLORS
CRE="$(echo -e '\r\033[K')"
NML="$(echo -e '\033[0;39m')"
RED="$(echo -e '\033[1;31m')"
GRN="$(echo -e '\033[1;32m')"
YEL="$(echo -e '\033[1;33m')"
BLU="$(echo -e '\033[1;34m')"
MAG="$(echo -e '\033[1;35m')"
CYN="$(echo -e '\033[1;36m')"
WHT="$(echo -e '\033[1;37m')"
#
udevinfo () {
   local UDEVINFO=`udevadm info -a -p $(udevadm info -q path -n $1) |  \
   egrep "ATTRS{product}==" | \
   head -n 1  | \
   sed 's/ATTRS{product}==//g' | \
   sed 's/"//g' | \
   sed 's/ /_/g' | \
   sed 's/____/-/g'`
   echo ${UDEVINFO} ;
}

SREC=`which srec_cat`			#find srec_cat in case we need it
if [ ${#SREC} == 0 ] ; then
   echo
   echo "${RED}SREC_CAT not found!${NML}"
   echo
   exit
fi
#
if [ ${#AVRDUDE} != 0 ] ; then
   BAUD=(230400 115200 57600 38400)			#AVRDude's available baud rates (as an array)
else
   echo
   echo "${RED}AVRdude not found!${NML}"
   echo
   exit
fi
#
#always select the MCU type
echo
echo "${MAG}Select the MCU:${YEL}"
select PN in ${MCUTYPE[@]} Quit ; do
   if [ "${PN}" == "Quit" ] ; then
      echo ${NML}
      exit ;
   else
      echo ${NML}
      break ;
   fi ;
done
#
if [ $# == 0 ] ; then
   if [ -e ${HEXDIR}/${PN} ] ; then
      pushd ${HEXDIR}/${PN} 2>&1> /dev/null;
   else
      echo
      echo "${RED}This error should not occur!${NML}"
      echo
   fi
   #
   # set the AVR part number for avrdude
   #  and FLASH size for the part.
   #
   case ${PN} in
      m32u4)
         #PN=m32u4
         PN=atmega32u4
         ROM_SIZE=32768;
         BTLD_SIZE=2048				#size of the bootloader in bytes
         PGMR=DFU					#which programming algorithm to use
         SUPPORT=N
      ;;
      usb1286)
         #PN=usb1286
         PN=at90usb1286
         ROM_SIZE=131072;
         BTLD_SIZE=2048				#size of the bootloader in bytes
         PGMR=DFU					#which programming algorithm to use
         SUPPORT=N
      ;;
      m2560)
         PN=m2560
         ROM_SIZE=262144;
         BTLD_SIZE=8192				#size of the bootloader in bytes
         PGMR=stk500v2				#which programming algorithm to use
         SUPPORT=Y					#supported by AVRDUDE
      ;;
      m328)
         PN=m328p
         ROM_SIZE=32768;
         BTLD_SIZE=512				#size of the bootloader in bytes
         PGMR=arduino				#which programming algorithm to use
         SUPPORT=Y					#supported by AVRDUDE
      ;;
      m168)
         PN=m168
         ROM_SIZE=16384;
         BTLD_SIZE=512				#size of the bootloader in bytes
         PGMR=arduino				#which programming algorithm to use
         SUPPORT=Y					#supported by AVRDUDE
      ;;
      m88)
         PN=m88
         ROM_SIZE=8192;
         BTLD_SIZE=0				#size of the bootloader in bytes
         PGMR=arduino				#which programming algorithm to use
         SUPPORT=Y					#supported by AVRDUDE
      ;;
   esac
   #
   if [ ${SUPPORT} == N ] ; then
      echo "${RED}Sorry! ${PN} not supported in Serial Port bootloader mode!"
      echo "   ${PN} uses the ${PGMR} bootloader mode.${NORMAL}"
      echo "   ${YEL}Please download ATMEL's FLIP programmer.${NORMAL}"
      echo
      exit
   else
      #select the file name
      echo
      echo "${MAG}Select build type:${YEL}"
      #   select FN in `ls -1v *nobtldr.hex` Quit ; do
      select FN in `ls -1v *.hex` Quit ; do
         if [ "${FN}" == "Quit" ] ; then
            echo ${NML}
            exit ;
         else
            echo ${NML}
            break ;
         fi ;
      done ;
      #look for the USB device; either ttyACMx or ttyUSBx
      if [ ${PGMR} == arduino ] || [ ${PGMR} == stk500v2 ]; then
         if [ -c /dev/ttyUSB0 ] || \
         [ -c /dev/ttyUSB1 ]  || \
         [ -c /dev/ttyUSB2 ]  || \
         [ -c /dev/ttyACM0 ]  || \
         [ -c /dev/ttyACM1 ]  || \
         [ -c /dev/ttyACM2 ] ; then
            echo
            echo "${BLU}Select the device (VCP):${NML}"
            if [ -e ${MENUNAME} ] ; then rm -f menu.lst ; fi
            find /dev/ -maxdepth 1 -name "ttyACM*" -or -name "ttyUSB*" | while read DEV ; do
               if [ ! -h $DEV ] ; then
                  #echo ${YEL}${DEV}${WHT}$(udevinfo $DEV)${NML} >> ${MENUNAME} ;
                  echo ${DEV}$(udevinfo $DEV) >> ${MENUNAME} ;
               fi ;
            done
            echo "${YEL}Quit${NML}" >> menu.lst
            select DEVN in `cat ${MENUNAME}` ; do
               if [ "${DEVN}" == "Quit" ] ; then
                  echo ${NML}
                  exit ;
               else
                  #        IDX=`echo ${DEVN} | grep -b -o "-" | awk 'BEGIN {FS=":"}{print $1}'`
                  IDX=`echo ${DEVN} | grep -b -o "-" | awk 'BEGIN {FS=":"}{print $1}'`
                  DEVN=${DEVN:0:${IDX}} ;
                  echo ${NML}
                  break ;
               fi ;
            done
            if [ -e ${MENUNAME} ] ; then rm -f menu.lst ; fi
         else
            echo
            echo "${RED}Error: Communications port not found!${NML}"
            echo
            exit
         fi
      fi
      #
      # Calculate the size of the file specified to be sure it fits
      FN_SIZE=`srec_cat ${FN} -intel -o - -binary | wc -c`
      AVAIL_SIZE=`echo "${ROM_SIZE} - ${BTLD_SIZE}" | bc`
      #echo "ROM_SIZE : $ROM_SIZE"
      #echo "FN_SIZE  : $FN_SIZE"
      #echo "AVAIL_SIZE: $AVAIL_SIZE"
      if [ ${FN_SIZE} -gt ${AVAIL_SIZE} ] ; then
         echo
         echo "${YEL}The HEX file specified will not fit in FLASH!"
         echo " Truncating the file to ${AVAIL_SIZE} bytes.${NML}"
         echo
         TMPFILE=_${RANDOM}.hex ;		#create a temporary file
         # use srec_cat to truncate the file to the correct size
         ${SREC} ${FN} -intel -fill 0xFF 0x0000 0x`echo "obase=16; ${ROM_SIZE}" | bc` \
         -exclude 0x`echo "obase=16; ${AVAIL_SIZE}" | bc` 0x`echo "obase=16; ${ROM_SIZE}" | bc` \
         -o ${TMPFILE} -intel -obs=16
         #
         FN=${TMPFILE} ;
         echo "${WHT}New file is ${BLU}`srec_cat ${FN} -intel -o - -binary | wc -c`${WHT} bytes${NML}"
         echo
      fi
      #
      #test for a connection
      if [ ${PGMR} == arduino ] ; then		#only "arduino" programmer gets a baud
         N=`echo "${#BAUD[@]} - 1" | bc`		#get index pointer to baud rate array
         echo "${BLU}Attempting to communicate with the ARDUINO on ${DEVN} ..."
         for BAUD_N in `seq 0 ${N}` ; do
            ERROR=1 ;		#set a flag
            #            echo "Using: avrdude -p ${PN} -c ${PGMR} -P ${DEVN} -b ${BAUD[${BAUD_N}]}"
            echo -n "${BLU}Testing at ${BAUD[${BAUD_N}]} baud ...${NML} "
            avrdude -p ${PN} -c ${PGMR} -P ${DEVN} -b ${BAUD[${BAUD_N}]} > /dev/null 2>&1
            if [ $? ==  0 ] ; then
               ERROR=0 ;		#set a flag
               echo "${YEL}Success!  Using baud rate of ${BAUD[$BAUD_N]}${NML}"
               break ;				#successful
            else
               echo "${RED}error communicating at ${BAUD[${BAUD_N}]} baud!${NML}"
            fi
         done
         if [ ${ERROR} -gt 0 ] ; then			#cannot communicate with ARDUINO so exit
            echo
            echo "${RED}There was an error communicating with the ARDUINO!${NML}"
            echo
            if [ -f ${TMPFILE} ] ; then rm -f ${TMPFILE} 2>&1>/dev/null ; fi	#delete the temp file if it exists
            exit
         fi
      fi
      #printf "%s\n" "${BAUD[@]}" ## print array
      #
      #connection achieved, programm the device with the selected HEX file
      echo
      echo "${BLU}Programming the AVR with ${FN} ...${NML}"
      if [ ${PGMR} == arduino ] ; then		#only "arduino" programmer gets a baud
         echo "Using: ${CYN}avrdude -p ${PN} -c ${PGMR} -P ${DEVN} -b ${BAUD[${BAUD_N}]} -U flash:w:${FN} ${NML}" ;
         echo
         avrdude -p ${PN} -c ${PGMR} -P ${DEVN} -b ${BAUD[${BAUD_N}]} -U flash:w:${FN} 2>&1 | egrep "Writing|Reading" ;
      elif [ ${PGMR} == stk500v2 ] ; then		#only "arduino" programmer gets a baud
         echo "Using: ${CYN}avrdude -p ${PN} -c ${PGMR} -P ${DEVN}  -U flash:w:${FN} ${NML}" ;
         echo
         avrdude -p ${PN} -c ${PGMR} -P ${DEVN} -U flash:w:${FN} 2>&1 | egrep "Writing|Reading" ;
     elif [ ${PGMR} == DFU ] ; then
         echo "Using: ${CYN}dfu-programmer ${PN} flash --suppress-validation ${FN} ${NML}" ;
         echo
         sudo dfu-programmer ${PN} erase
         if [ $? == 0 ] ; then
#            sudo dfu-programmer ${PN} flash --suppress-validation ${FN}
            sudo dfu-programmer ${PN} flash ${FN}
         else
            echo
            echo "${RED}There was failure to erase the ${PN}!${NML}"
            echo
         fi
      else
         echo "${RED}Invalid programmer!${NORMAL}"
      fi
      echo Error Level: $?
      #avrdude -p ${PN} -c ${PGMR} -P ${DEVN} -b ${BAUD[${BAUD_N}]} -U flash:w:${FN}
      echo
      if [ $? == 0 ] ; then
         echo "${YEL}Successfully programmed the AVR.${NML}"
      else
         echo "${RED}There was a programming error!${NML}"
      fi
      if [ -f ${TMPFILE} ] ; then rm -f ${TMPFILE} 2>&1>/dev/null ; fi	#delete the temp file if it exists
      popd > /dev/null 2>&1 ;
      echo
   fi
else
   FN=`find . -maxdepth 1 -type f -name "$1.hex"`
   if [ ${#FN} == 0 ] ; then
      echo ;
      echo "${RED}Error!  A HEX file with the prefix of ${1} was not found!${NML}" ;
      exit ;
   fi
fi
