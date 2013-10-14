#! /bin/bash
. colordefs.sh					# add color definitions

if [ "$1" == "?" ] ; then
   echo
   echo "Usage: $0 [freak]"
   echo " where [freak] is optional an creates 3 separate files"
   echo " for uploading to AVRFREAKS."
   echo
   exit
fi

INCLFILE=/tmp/$RANDOM.lst
EXCLFILE=/tmp/$RANDOM.lst
LOGFILE=/tmp/`basename ${TMPFILE} .lst`.log
if [ `which 7z 2>/dev/null` ] ; then
   ZIPPER=`which 7z` ;
   ZIPOPT="u -tZIP -ir@${INCLFILE} -xr@${EXCLFILE}" ;
elif [ `which 7za 2>/dev/null` ] ; then
   ZIPPER=`which 7za` ;
   ZIPOPT="u -tZIP -ir@${INCLFILE} -xr@${EXCLFILE}" ;
else
   echo "Using zip"
   ZIPPER=`which zip 2>/dev/null` ;
   ZIPOPT="-9uvr -x@${TMPFILE}" ;
fi

# test for AVRFREAKS files requested
FREAKS=0	# default, no files for AVRFREAKS
if [ "$1" != "${1/[Ff][Rr][Ee][Aa][Kk]/}" ] ; then
   FREAKS=1			# make files for AVRFREAKS
   FRKFN[1]=HEX			# HEX files
   INCLFN[1]="AVR_Specific_Builds"
   EXCLFN[1]=""
   #
   FRKFN[2]=M32U4USB		# Mega32U4 support files
   INCLFN[2]="_LUFA-110528 _USB_Serial"
   EXCLFN[2]=""
   #
   FRKFN[3]=SRC			# Source files
   INCLFN[3]="."
   EXCLFN[3]="_LUFA-110528 _USB_Serial AVR_Specific_Builds"
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

DATESTRING=$( date +%Y-%m%d )
FNPFX=$(echo ${DATESTRING}_${FNBASE}${REV} | tr '[:lower:]' '[:upper:]') ;
SFX=""

clear
if [ -e ${BACKUPS}/${TYPE}/${FNPFX}_${TYPE}.ZIP ] ; then		# if a prior file exists, create a new suffix
   for FNSFX in A B C D E F G H I J ; do
      if [ ! -e ${BACKUPS}/${TYPE}/${FNPFX}${FNSFX}_${TYPE}.ZIP ] ; then break ; fi ;
   done ;
fi ;

ZIPFILE=$( echo ${FNPFX}${FNSFX}_${TYPE}.zip | tr '[:lower:]' '[:upper:]')

#echo "Type    = $TYPE"
#echo "FNBASE  = $FNBASE"
#echo "REV     = $REV"
#echo "FNPFX   = $FNPFX"
#echo "FNSFX   = $FNSFX"
#echo "BACKUPS = $BACKUPS"
#echo "ZIPFILE = $ZIPFILE"
#exit

# Generate list of all files to include
find . -type f | sed 's/\.\///g' >> $INCLFILE ;

# Generate list of all files to exclude
for EXT in "[Tt][Ee][Mm][Pp]*.*" "*,[1-5]" "log" "did" "do" "w" \
"ses" "tmp" "sts" "bak" "[Dd][Bb][Kk]" "*.pipe" ; do
   #   find . -type f -name "*${EXT}" -exec rm -f {} \;		# remove leftover CADENCE job files
   find . -type f -name "*${EXT}" | \
   sed 's/\.\///g' >> ${EXCLFILE} ; 				# exclude leftover CADENCE job files
done

#echo "${YEL}Generating ${BLU}${ZIPFILE}${NORMAL}"
#$ZIPPER $ZIPOPT ${BACKUPS}/${TYPE}/${ZIPFILE} > ${LOGFILE} 2>&1 ;
#if [ $? == 0 ] ; then
#   echo "Completed, no errors."
#   echo
#else
#   echo "There was a fatal error!" ;
#   less ${LOGFILE} ;
#   LOGFILE= ;
#fi

if [ ${FREAKS} == 1 ] ; then
   echo "${MAG}Generating AVRFREAKS archives Please wait...${NORMAL}"
   for N in 1 2 3 ; do
      rm -f ${LOGFILE} ${INCLFILE} ${EXCLFILE} > /dev/null 2>&1 
      ZIPFILE=$( echo ${FNPFX}${FNSFX}_${FRKFN[${N}]}.zip.png | tr '[:lower:]' '[:upper:]')
      echo "${YEL}Generating AVRFREAKS file for ${BLU}${FNPFX}${FNSFX}_${FRKFN[${N}]}${NORMAL}..."
      find ${INCLFN[$N]} -type f | sed 's/\.\///g'  >> ${INCLFILE} ;
      echo "" > ${EXCLFILE} > /dev/null		# over-write old file
      if [ ${#EXCLFN[$N]} -gt 0 ] ; then	# create the list of exclude files
         #         find ${EXCLFN[$N]} -type f | sed 's/\.\///g' | sed '/'${INCLFN[$N]}'/d' >> ${EXCLFILE} ;
         find ${EXCLFN[$N]} -type f | sed 's/\.\///g'  >> ${EXCLFILE} ;
      fi
      $ZIPPER $ZIPOPT ${BACKUPS}/${TYPE}/${ZIPFILE} >> ${LOGFILE} 2>&1 ;
      if [ $? == 0 ] ; then
         echo "Completed, no errors."
         echo
      else
         echo "There was a fatal error!" ;
         less ${LOGFILE} ;
         LOGFILE= ;
      fi
   done
fi
rm -f ${TMPFILE} ${INCLFILE} ${EXCLFILE} ${LOGFILE} > /dev/null 2>&1
