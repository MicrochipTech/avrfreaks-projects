#! /bin/bash

. colordefs.sh					# terminal color definitions

LUFADIR="LUFA-110528"				#location of LUFA bootloaders

ORG="ORG"					#remove the "ORG" statement line

BOOTSTRTM168=0x3E00				#start of Bootloader for Mega168
BOOTENDM168=0x3FFF				#end of Bootloader for Mega168

BOOTSTRTM328=0x7E00				#start of Bootloader for Mega328
BOOTENDM328=0x7FFF				#end of Bootloader for Mega328

BOOTSTRTM2560=0x3E000				#start of Bootloader for Mega2560
BOOTENDM2560=0x3FFFF				#end of Bootloader for Mega2560

for DIR in stk500v2 optiboot ; do		#our source directories
   BOOTSRCDIR=images				#location of our HEX files
   BOOTSRCPFX=${DIR}_m				#prefix of each HEX file
   BOOTSRCSFX=mhz.hex				#suffix of each HEX file
   
   BOOTDESTDIR=../../Include			#location of target files
   BOOTDESTPFX=Code_BootloaderCDC-m		#prefix of each target file
   BOOTDESTSFX=mhz.inc				#suffix of each target file
   
   pushd ${DIR} 2>&1 >/dev/null
   source makeall.sh  2>&1 >/dev/null		#make all bootloader flavors
   
   for MCU in 2560 328 168 ; do
      if [ $MCU == 168 ] ; then
         BOOTSTRT=${BOOTSTRTM168} ;
         BOOTEND=${BOOTENDM168} ;
         elif [ $MCU == 328 ] ; then
         BOOTSTRT=${BOOTSTRTM328} ;
         BOOTEND=${BOOTENDM328} ;
         elif [ $MCU == 2560 ] ; then
         BOOTSTRT=${BOOTSTRTM2560} ;
         BOOTEND=${BOOTENDM2560} ;
      fi
      for MCLK in 4 8 16 20 ; do
         if [ ${MCU} == drec ] ; then
            BOOTSRCSFX=mhz-${MCU}.hex	#suffix of each HEX file
            BOOTDESTSFX=mhz-${MCU}.inc ;
            MCU=328 ;
         fi
         if [ -e ${BOOTSRCDIR}/${BOOTSRCPFX}${MCU}-${MCLK}${BOOTSRCSFX} ] ; then
            rm -f ${BOOTDESTDIR}/${BOOTDESTPFX}${MCU}-${MCLK}${BOOTDESTSFX}	#delete existing file
            echo "${BLU}Processing: ${WHT}${BOOTSRCDIR}/${BOOTSRCPFX}${MCU}-${MCLK}${BOOTSRCSFX}${NORMAL}"
            srec_cat ${BOOTSRCDIR}/${BOOTSRCPFX}${MCU}-${MCLK}${BOOTSRCSFX} -intel \
            -fill 0xFF ${BOOTSTRT} ${BOOTEND} \
            -o - -asm -HEXadecimal_STyle | \
            sed '/'"$ORG"'/ d' |
            sed 's/ DB/ .db/' > ${BOOTDESTDIR}/${BOOTDESTPFX}${MCU}-${MCLK}${BOOTDESTSFX}
         fi
      done
   done
   popd 2>&1 > /dev/null
done

# Process the DFU Bootloaders
echo
echo "${BLU}Processing ${WHT}DFU Bootloader ${BLU}file(s)${NORMAL}"
pushd ${LUFADIR}/Bootloaders/DFU/ 2>&1 > /dev/null
source makeall.sh > /dev/null 2>&1
popd 2>&1 > /dev/null				#return to project directory
   