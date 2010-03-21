Copyright (c) 2010
Funkenstein Software Consulting. All rights reserved.

FunkOS R3 preview.

//---------------------------------------------------------------------
C++ kernel demo app: 	

 This app uses the new FunkOS++ kernel to implement a simple example 
 application, demonstrating the use of a number of the C++ kernel's 
 features, including: Tasks, semaphores, device drivers (+mutexes), 
 and timers.

 The application task is set up to wait for a timer-triggered semaphore
 to be posted, after which, an NES joypad is read.  The data from the
 joypad is then copied into a local structure, after which the thread
 pends on the semaphore again.

 While the application thread is idle, the idle thread runs, which is
 used to simply increment a counter at this point.
 
 Platform: atmega328p, 16MHz
 Compiler: WinAVR

 //---------------------------------------------------------------------
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
	* Modifications/additions to the source to "port" the software to 
	  processor/compiler targets other than those included in the source 
	  distribution must be submitted to Funkenstein Software Consulting for 
	  inclusion in future revisions of the source.
	* Distribution in binary form as firmware included on a hardware product
	  (whether in a modified form or as provided), must include the following
	  notice within all accompanying product documentation, whether printed
	  or in digital form:
	  "Includes the FunkOS Realtime Operating System by Funkenstein Software 
	  Consulting, available at http://funkos.sourceforge.net"

THIS SOFTWARE IS PROVIDED BY FUNKENSTEIN SOFTWARE CONSULTING ``AS IS'' AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR 
NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL FUNKENSTEIN SOFTWARE 
CONSULTING  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
