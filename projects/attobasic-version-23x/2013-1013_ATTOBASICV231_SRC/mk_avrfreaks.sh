#! /bin/bash
# Create the various ZIP files for upload to AVRFREAKS.NET

. colordefs.sh					# add color definitions

TMPFILE=/tmp/$RANDOM.lst
LOGFILE=/tmp/`basename ${TMPFILE} .lst`.log
DESTDIR=../../Backups/CODE
if [ `which 7z 2>/dev/null` ] ; then
   ZIPPER=`which 7z` ;
   ZIPOPT="u -tZIP -xr@${TMPFILE}" ;
   elif [ `which 7za 2>/dev/null` ] ; then
   ZIPPER=`which 7za` ;
   ZIPOPT="u -tZIP -xr@${TMPFILE}" ;
else
   echo "Using zip"
   ZIPPER=`which zip 2>/dev/null` ;
   ZIPOPT="-9uvr -x@${TMPFILE}" ;
fi

pushd ../ > /dev/null ; 			# up one level
TYPE=$(basename $PWD | sed 's/-//g' |  tr '[:lower:]' '[:upper:]')
popd > /dev/null

pushd ../../ > /dev/null ; 			# up two levels
FNBASE=$(basename $PWD | sed 's/-//g' |  tr '[:lower:]' '[:upper:]')
BACKUPS=$PWD/Backups ;				# define backup directory
if [ ! -e $BACKUPS ] ; then mkdir -p ${BACKUPS}/${TYPE} ; fi
popd > /dev/null

REV=$(basename $PWD |  tr '[:lower:]' '[:upper:]') ;

# Clean out previous single build files
find . -maxdepth 1 -type f | while read FN ; do
   TEMP_FN=$(echo ${FN} |  sed 's/.\///g' | tr '[:lower:]' '[:upper:]')
   for EXT in HEX LST LOG MAP OBJ ; do
      if [ ${TEMP_FN} == ${FNBASE}${REV}.${EXT} ] ; then rm -f ${FN} 2>&1 > /dev/null ; fi
   done
   rm -f labels.tmp  2>&1 > /dev/null
done

DATESTRING=$( date +%Y-%m%d )
FNPFX=$(echo ${DATESTRING}_${FNBASE}${REV} | tr '[:lower:]' '[:upper:]') ;
# Define the specific ZIP names
ZIP1=${FNPFX}_HEX.ZIP		# Device Specific HEX files
ZIP2=${FNPFX}_SRC.ZIP		# Source Code
ZIP3=${FNPFX}_USB.ZIP		# Meag32U4/USB1286 USB Bootloader and Serial I/O Support Source

#remove prior build files
for EXT in hex log lst map obj ; do
   rm -f ${FNBASE}.${EXT}  2>&1 > /dev/null
done
rm -f labels.tmp 2>&1 > /dev/null

echo
echo

# Build the HEX files
echo "${YEL}Generating archive ${BLU}${ZIP1}${YEL}... Please wait...${NORMAL}"
echo > ${TMPFILE}		# no files to exclude from this archive
$ZIPPER $ZIPOPT ${DESTDIR}/${ZIP1} AVR_Specific_Builds >> ${LOGFILE} 2>&1 ;
if [ $? == 0 ] ; then
   echo "Completed, no errors."
else
   echo "There was a fatal error!" ;
   less ${LOGFILE} ;
   LOGFILE= ;
fi

#Build Source Files
echo "${YEL}Generating archive ${BLU}${ZIP2}${YEL}... Please wait...${NORMAL}"
# Scan for files to exclude from this archive
for DIR in _Bootloaders _USB_Drivers _USB_Serial AVR_Specific_Builds ; do
   find ${DIR} | sed 's/\.\///g' >> ${TMPFILE} ; 	# exclude specific directories
   #   find ${DIR} -type f >> ${TMPFILE} ; 	# exclude specific directories
done
#less ${TMPFILE}
$ZIPPER $ZIPOPT ${DESTDIR}/${ZIP2} . >> ${LOGFILE} 2>&1 ;
if [ $? == 0 ] ; then
   echo "Completed, no errors."
else
   echo "There was a fatal error!" ;
   less ${LOGFILE} ;
   LOGFILE= ;
fi

#Build USB ad Bootloader files
echo "${YEL}Generating archive ${BLU}${ZIP3}${YEL}... Please wait...${NORMAL}"
echo > ${TMPFILE}		# no files to exclude from this archive
$ZIPPER $ZIPOPT ${DESTDIR}/${ZIP3} _Bootloaders _USB_Serial _USB_Drivers >> ${LOGFILE} 2>&1 ;
if [ $? == 0 ] ; then
   echo "Completed, no errors."
else
   echo "There was a fatal error!" ;
   less ${LOGFILE} ;
   LOGFILE= ;
fi
