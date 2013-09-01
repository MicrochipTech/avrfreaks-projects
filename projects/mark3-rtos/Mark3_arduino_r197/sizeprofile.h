#ifndef __SIZE_REPORT_H__
#define __SIZE_REPORT_H__

/*!
\page SIZEPROFILE  Code Size Profiling
The following report details the size of each module compiled into the
kernel. The size of each component is dependent on the flags specified
in mark3cfg.h at compile time.  Note that these sizes represent the
maximum size of each module before dead code elimination and any
additional link-time optimization, and represent the maximum possible
size that any module can take.

The results below are for profiling on Atmel AVR atmega328p-based targets using gcc. 
Results are not necessarily indicative of relative or absolute performance
on other platforms or toolchains.
\section SIZEPROFILEINFO Information
Subversion Repository Information:
    - Repository Root: svn+ssh://m0slevin@svn.code.sf.net/p/mark3/source
    - Revision: 188
    - URL: svn+ssh://m0slevin@svn.code.sf.net/p/mark3/source/branch/release/R1/embedded
Relative URL: ^/branch/release/R1/embedded
    .

Date Profiled:
Thu Mar 5 21:04:20 EST 2015

\section SIZEPROFILEVER  Compiler Version
avr-gcc (GCC) 4.8.2 Copyright (C) 2013 Free Software Foundation, Inc. This is free software; see the source for copying conditions. There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

\section SIZEPROFILERES  Profiling Results
Mark3 Module Size Report:
    -  Synchronization Objects - Base Class............ : 84 Bytes
    -  Device Driver Framework (including /dev/null)... : 226 Bytes
    -  Synchronization Object - Event Flag............. : 770 Bytes
    -  Fundamental Kernel Linked-List Classes.......... : 496 Bytes
    -  Message-based IPC............................... : 426 Bytes
    -  Mutex (Synchronization Object).................. : 658 Bytes
    -  Performance-profiling timers.................... : 546 Bytes
    -  Round-Robin Scheduling Support.................. : 252 Bytes
    -  Thread Scheduling............................... : 475 Bytes
    -  Semaphore (Synchronization Object).............. : 544 Bytes
    -  Thread Implementation........................... : 1433 Bytes
    -  Fundamental Kernel Thread-list Data Structures.. : 212 Bytes
    -  Mark3 Kernel Base Class......................... : 80 Bytes
    -  Software Timer Implementation................... : 1015 Bytes
    -  Runtime Kernel Trace Implementation............. : 0 Bytes
    -  Circular Logging Buffer Base Class.............. : 0 Bytes
    -  Atmel AVR - Kernel Aware Simulation Support...... : 287 Bytes
    -  Atmel AVR - Basic Threading Support.............. : 528 Bytes
    -  Atmel AVR - Kernel Interrupt Implemenation....... : 56 Bytes
    -  Atmel AVR - Kernel Timer Implementation.......... : 322 Bytes
    -  Atmel AVR - Profiling Timer Implementation....... : 256 Bytes
    .

Mark3 Kernel Size Summary:
    - Kernel                   :  2780 Bytes
    - Synchronization Objects  :  2398 Bytes
    - Port                     :  1449 Bytes
    - Features                 :  2039 Bytes
    - Total Size               :  8666 Bytes
    .
*/
#endif // __SIZE_REPORT_H__
