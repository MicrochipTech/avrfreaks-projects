#!/bin/sh
###
# Script to print out a nicely-formatted summary of the object sizes associated
# with the various modules of the kernel
###

FORMAT="console"

if [ $# -lt 3 ]; then
    echo "Usage: size_profile.sh <ARCH> <VARIANT> <TOOLCHAIN> [-d]"
    echo " "
    echo "  Where:"
    echo "    ARCH = architecture (i.e. avr or cm0)"
    echo "    VARIANT = device variant (i.e. atmega328p or samd20)"
    echo "    TOOLCHAIN = compiler toolchain (i.e gcc)"
    echo "    -d (optional) = Format as doxygen output"
    exit
fi

ARCH=$1
VARIANT=$2
TOOLCHAIN=$3

if [ $# -eq 4 ]; then
    if [ "${4}" = "-d" ]; then
        FORMAT="doxygen"
    fi
fi

### Get the specific compiler/toolchain commands most appropriate for the target
SIZE_BIN="size"
COMPILER_BIN="gcc"
ARCH_STRING="none"
case ${ARCH} in
    "avr")
        SIZE_BIN="avr-size"
        COMPILER_BIN="avr-gcc"
        ARCH_STRING="Atmel AVR"
    ;;
    "cm0")
        SIZE_BIN="arm-none-eabi-size"
        COMPILER_BIN="arm-none-eabi-gcc"
        ARCH_STRING="ARM Cortex-M0"
    ;;
    *)
    ;;
esac


### Local variables
TEXT=""
DATA=""
BSS=""
DEC=""
HEX=""
MODULE=""

### Component sizes
KERNEL_SIZE=0
SYNCOBJ_SIZE=0
PORT_SIZE=0
FEATURE_SIZE=0
IFS_CACHE=$IFS

#Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60----5---70----5---80"

if [ "${FORMAT}" = "console" ]; then
    echo    "================================================================="
    echo    "Mark3 Module Size Report"
    echo    "================================================================="
elif [ "${FORMAT}" = "doxygen" ]; then
    echo    "#ifndef __SIZE_REPORT_H__"
    echo    "#define __SIZE_REPORT_H__"
    echo    ""
    echo    "/*!"
    echo    "\page SIZEPROFILE  Code Size Profiling"
    echo    "The following report details the size of each module compiled into the"
    echo    "kernel. The size of each component is dependent on the flags specified"
    echo    "in mark3cfg.h at compile time.  Note that these sizes represent the"
    echo    "maximum size of each module before dead code elimination and any"
    echo    "additional link-time optimization, and represent the maximum possible"
    echo    "size that any module can take."
    echo    ""
    echo    "The results below are for profiling on ${ARCH_STRING} ${VARIANT}-based targets using ${TOOLCHAIN}. "
    echo    "Results are not necessarily indicative of relative or absolute performance"
    echo    "on other platforms or toolchains."
    echo    "\section SIZEPROFILEINFO Information"
    IFS=" "
    SVNROOT=$(svn info | grep -e Root:)
    SVNREV=$(svn info | grep -e Revision:)
    SVNURL=$(svn info | grep -e URL:)
    echo    "Subversion Repository Information:"
    echo    "    - "${SVNROOT}
    echo    "    - "${SVNREV}
    echo    "    - "${SVNURL}
    echo    "    ."
    IFS=${IFS_CACHE}
    echo    ""
    echo    "Date Profiled:"
    DATEINFO=$(date)
    echo    ${DATEINFO}
    echo    ""
    echo    "\section SIZEPROFILEVER  Compiler Version"
    GCCINFO=$(${COMPILER_BIN} --version)
    echo    ${GCCINFO}
    echo    ""
    echo    "\section SIZEPROFILERES  Profiling Results"

    echo    "Mark3 Module Size Report:"
fi


### We parse line-by-line and then field within a line, use the IFS variable for
### selecting what constitutes a delimiter
IFS_CACHE=$IFS
IFS="
"
### Run the avr-size utility to get a listing of all component sizes (code/data)
### data is all 0, which is likely due to the fact that this is a library
MARK3_DATA=$(${SIZE_BIN} ./stage/lib/${ARCH}/${VARIANT}/${TOOLCHAIN}/libmark3.a)


### Parse through the reported data, line at a time, and convert the data from the
### default avr-size format into something that's a little nicer looking, with
### better descriptions for each module
LINE_NO=0
for LINE in ${MARK3_DATA}; do
    if [ ${LINE_NO} -eq 0 ]; then
        LINE_NO=$(( LINE_NO+1 ))
        continue
    fi

    IFS=${IFS_CACHE}
    ITER=0
    for TOKEN in ${LINE}; do

        case ${ITER} in
            0)
                TEXT=${TOKEN}
                ;;
            1)
                DATA=${TOKEN}
                ;;
            2)
                BSS=${TOKEN}
                ;;
            3)
                DEC=${TOKEN}
                ;;
            4)
                HEX=${TOKEN}
                ;;
            5)
                MODULE=${TOKEN}
                ;;
            *)
                ;;
        esac

        ITER=$(( ITER+1 ))
        if [ $ITER -eq 6 ]; then
            break;
        fi
    done

    MODNAME=""
    case ${MODULE} in
        "atomic.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Atomic Operations..............................."
            ;;
        "blocking.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Synchronization Objects - Base Class............"
            KERNEL_SIZE=$(( KERNEL_SIZE+${DEC} ))
            ;;
        "driver.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Device Driver Framework (including /dev/null)..."
            FEATURE_SIZE=$(( FEATURE_SIZE+${DEC} ))
            ;;
        "eventflag.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Synchronization Object - Event Flag............."
            SYNCOBJ_SIZE=$(( SYNCOBJ_SIZE+${DEC} ))
            ;;
        "ll.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Fundamental Kernel Linked-List Classes.........."
            KERNEL_SIZE=$(( KERNEL_SIZE+${DEC} ))
            ;;
        "message.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Message-based IPC..............................."
            SYNCOBJ_SIZE=$(( SYNCOBJ_SIZE+${DEC} ))
            ;;
        "mutex.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Mutex (Synchronization Object).................."
            SYNCOBJ_SIZE=$(( SYNCOBJ_SIZE+${DEC} ))
            ;;
        "profile.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Performance-profiling timers...................."
            FEATURE_SIZE=$(( FEATURE_SIZE+${DEC} ))
            ;;
        "quantum.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Round-Robin Scheduling Support.................."
            FEATURE_SIZE=$(( FEATURE_SIZE+${DEC} ))
            ;;
        "scheduler.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Thread Scheduling..............................."
            KERNEL_SIZE=$(( KERNEL_SIZE+${DEC} ))
            ;;
        "ksemaphore.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Semaphore (Synchronization Object).............."
            SYNCOBJ_SIZE=$(( SYNCOBJ_SIZE+${DEC} ))
            ;;
        "thread.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Thread Implementation..........................."
            KERNEL_SIZE=$(( KERNEL_SIZE+${DEC} ))
            ;;
        "threadlist.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Fundamental Kernel Thread-list Data Structures.."
            KERNEL_SIZE=$(( KERNEL_SIZE+${DEC} ))
            ;;
        "kernel.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Mark3 Kernel Base Class........................."
            KERNEL_SIZE=$(( KERNEL_SIZE+${DEC} ))
            ;;
        "timerlist.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Software Timer Implementation..................."
            FEATURE_SIZE=$(( FEATURE_SIZE+${DEC} ))
            ;;
        "transaction.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Kernel Transaction Queues......................."
            KERNEL_SIZE=$(( KERNEL_SIZE+${DEC} ))
            ;;
        "tracebuffer.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Runtime Kernel Trace Implementation............."
            FEATURE_SIZE=$(( FEATURE_SIZE+${DEC} ))
            ;;
        "writebuf16.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="Circular Logging Buffer Base Class.............."
            FEATURE_SIZE=$(( FEATURE_SIZE+${DEC} ))
            ;;
        "threadport.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="${ARCH_STRING} - Basic Threading Support.............."
            PORT_SIZE=$(( PORT_SIZE+${DEC} ))
            ;;
        "kernelswi.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="${ARCH_STRING} - Kernel Interrupt Implemenation......."
            PORT_SIZE=$(( PORT_SIZE+${DEC} ))
            ;;
        "kerneltimer.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="${ARCH_STRING} - Kernel Timer Implementation.........."
            PORT_SIZE=$(( PORT_SIZE+${DEC} ))
            ;;
        "kprofile.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="${ARCH_STRING} - Profiling Timer Implementation......."
            PORT_SIZE=$(( PORT_SIZE+${DEC} ))
            ;;
        "kernel_aware.cpp.o")
            #Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60"
            MODNAME="${ARCH_STRING} - Kernel Aware Simulation Support......"
            PORT_SIZE=$(( PORT_SIZE+${DEC} ))
            ;;
         *)
            MODNAME=${MODULE}
            ;;
    esac

if [ "${FORMAT}" = "console" ]; then
    echo ${MODNAME} ":" ${DEC} "Bytes"
else
    echo "    - " ${MODNAME} ":" ${DEC} "Bytes"
fi

IFS="
"
done

TOTAL_SIZE=$(( FEATURE_SIZE+PORT_SIZE+SYNCOBJ_SIZE+KERNEL_SIZE ))

#Ruler:  ----5---10----5---20----5---30----5---40----5---50----5---60----5---70----5---80"

if [ "${FORMAT}" = "console" ]; then
    echo    "\n"
    echo    "================================================================="
    echo    "Mark3 Kernel Size Summary"
    echo    "================================================================="
    echo    "Kernel                   : " ${KERNEL_SIZE} "Bytes"
    echo    "Synchronization Objects  : " ${SYNCOBJ_SIZE} "Bytes"
    echo    "Port                     : " ${PORT_SIZE} "Bytes"
    echo    "Features                 : " ${FEATURE_SIZE} "Bytes"
    echo    "================================================================="
    echo    "Total Size               : " ${TOTAL_SIZE} "Bytes"
else
    echo    "    ."
    echo    ""
    echo    "Mark3 Kernel Size Summary:"
    echo    "    - Kernel                   : " ${KERNEL_SIZE} "Bytes"
    echo    "    - Synchronization Objects  : " ${SYNCOBJ_SIZE} "Bytes"
    echo    "    - Port                     : " ${PORT_SIZE} "Bytes"
    echo    "    - Features                 : " ${FEATURE_SIZE} "Bytes"
    echo    "    - Total Size               : " ${TOTAL_SIZE} "Bytes"
    echo    "    ."
    echo "*/"
    echo "#endif // __SIZE_REPORT_H__"
fi
